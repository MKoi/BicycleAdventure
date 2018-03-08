#include "pch.h"
#include "Game.h"
#include "Menu.h"
#include <ppltasks.h>
#include <string>

using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Concurrency;
using namespace DirectX;

const Physics::Parameters bikeparams = { b2BodyType::b2_dynamicBody, 1.0f, 1.0f, 0.1f, Physics::PlayerCategory };
const Physics::Parameters cyclistparams = { b2BodyType::b2_dynamicBody, 0.1f, 0.7f, 0.5f, Physics::PlayerCategory };

Game::Game(Renderer^ renderer) :
m_audio(),
m_renderer(renderer),
m_world(b2Vec2(0.0f, -10.0f)),
m_levels(&m_world),
m_background(),
m_cycle(&m_world, b2Vec2(3.0f, 2.0f), bikeparams, &m_audio),
m_cyclist(&m_world, &m_cycle, cyclistparams, &m_audio),
m_menu(&m_audio),
m_saveFile(L"save"),
m_ready(false),
m_hiScore(0),
m_score(0),
m_bonus(0),
m_dt(0.0f),
m_freezeCamera(false),
m_pauseRequested(false)
{
//	m_renderer = renderer;
	//m_debugDraw.SetFlags(b2Draw::e_jointBit | b2Draw::e_shapeBit);
	//m_world.SetDebugDraw(&m_debugDraw);
	m_audio.LoadResources();
	GameArea::SetStartZoneWidth(m_cycle.GetWidth());
	GameArea::SetAudio(&m_audio);


	m_states.resize(static_cast<size_t>(eMaxStates));
	m_states.at(ePlay).SetAction(&Game::DoPlay);
	m_states.at(ePlay).SetNext(&m_states.at(ePause), 
		[&](){	return m_menu.PausePressed(); },
		[&]()
			{
				m_audio.SaveState();
				m_audio.Pause();
				m_audio.Play(GameAudio::eMenuClick);
				m_menu.EnablePlay(); 
				m_menu.EnablePause(false);
			}
	);

	m_states.at(ePlay).SetNext(&m_states.at(eStart),
		[&](){	return m_menu.RestartPressed(); },
		[&]()
			{	
				DoRestart();
				m_audio.Play(GameAudio::eMenuClick);
			}
	);
	m_states.at(ePlay).SetNext(&m_states.at(eGameOver),
		[&](){	return m_cyclist.IsDead() || CyclistOutOfBounds(); },
		[&]()
			{	
				m_states.at(eGameOver).SetDelay(3.0f);
				if (!CyclistOutOfBounds())
				{
					m_renderer->SetDeadZone(0.0f);
					m_renderer->SetCameraTarget(m_cyclist.GetPosition().x, m_cyclist.GetPosition().y, 0.0f);
					m_renderer->SetTransitionTime(1.0f);
				}
				else
				{
					m_freezeCamera = true;
				}
				if (CheckHiScore())
				{
					m_audio.Play(GameAudio::eCheering, 0.4f);
					m_audio.Play(GameAudio::eNewRecord, 1.5f);
				}
				else
				{
					m_audio.Play(GameAudio::eGameOver, 1.5f);
				}
				m_menu.EnableGameOver();
				m_menu.EnableBonus(false);
				m_menu.EnablePause(false);
				m_menu.EnableHelp(false);
			}
	);

	m_states.at(eGameOver).SetAction(&Game::DoGameOver);
	m_states.at(eGameOver).SetNext(&m_states.at(eStart),
		[&]()
			{
				if (m_menu.RestartPressed())
				{
					m_states.at(eGameOver).SetDelay(0.0f);
				}
				return true;
			},
		[&]()
			{
				m_audio.Stop();
				m_menu.EnableGameOver(false); 
				m_menu.EnableRestart(false);
				m_menu.EnableScore(false);
				ResetLevels();
				m_menu.EnableTitle();
				m_menu.EnablePlay();
			}
	);

	m_states.at(eStart).SetNext(&m_states.at(ePlay),
		[&](){	return m_menu.PlayPressed(); },
		[&]()
			{
				m_audio.Play(GameAudio::eMenuClick);
				m_menu.EnableHelp();
				m_menu.EnableTitle(false);
				m_menu.EnablePlay(false);
				m_menu.EnableNewRecord(false);
				m_menu.EnableHiscore(false);
				m_menu.EnableScore(); 
				m_menu.EnablePause(); 
				m_menu.EnableRestart(); 
			}
	);

	m_states.at(ePause).SetNext(&m_states.at(ePlay),
		[&](){	return m_menu.PlayPressed(); },
		[&]()
			{
				m_audio.RestoreState();
				m_audio.Play(GameAudio::eMenuClick);
				m_menu.EnablePlay(false); 
				m_menu.EnablePause(); 
			}
	);
	m_states.at(ePause).SetNext(&m_states.at(eStart),
		[&](){	return m_menu.RestartPressed(); },
		[&]()
			{
				DoRestart();
				m_audio.Play(GameAudio::eMenuClick);
			}
	);

	m_currentState = &m_states.at(eStart);
	SetInitialMenuState();
	Restore();
	m_world.SetContactListener(this);
}


void Game::Save(Windows::ApplicationModel::SuspendingDeferral^ deferral)
{
	m_ready = false;
	Pause();
	m_audio.Suspend();
	auto folder = ApplicationData::Current->LocalFolder;
	task<StorageFile^> getFileTask(folder->CreateFileAsync(
		ref new Platform::String(m_saveFile), 
		CreationCollisionOption::ReplaceExisting));

	// Create a local to allow the DataReader to be passed between lambdas.
	auto writer = std::make_shared<Streams::DataWriter^>(nullptr);

	getFileTask.then([](StorageFile^ file)
	{
		return file->OpenAsync(FileAccessMode::ReadWrite);
	}).then([this, writer](Streams::IRandomAccessStream^ stream)
	{
		Streams::DataWriter^ datawriter = ref new Streams::DataWriter(stream);
		*writer = datawriter;
		std::lock_guard<std::mutex> lock(m_worldLock);
		datawriter->WriteInt32(SaveGameVersion);
		datawriter->WriteInt32(static_cast<int>(GetCurrentState()));
		m_levels.Save(datawriter);
		m_cycle.SaveState(datawriter);
		m_cyclist.SaveState(datawriter);
		m_renderer->SaveState(datawriter);
		datawriter->WriteSingle(m_score);
		datawriter->WriteInt32(m_hiScore);
		return datawriter->StoreAsync();
	}).then([writer](uint32 count)
	{
		return (*writer)->FlushAsync();
	}).then([this, deferral, writer](bool flushed)
	{
		delete (*writer);
		if (deferral)
		{
			deferral->Complete();
		}
	});
}


void Game::Restore()
{
	auto folder = ApplicationData::Current->LocalFolder;
	task<StorageFile^> getFileTask(folder->GetFileAsync(
		ref new Platform::String(m_saveFile)));

	// Create a local to allow the DataReader to be passed between lambdas.
	auto reader = std::make_shared<Streams::DataReader^>(nullptr);
	getFileTask.then([this, reader](task<StorageFile^> fileTask)
	{
		try
		{
			StorageFile^ file = fileTask.get();

			task<Streams::IRandomAccessStreamWithContentType^> (file->OpenReadAsync()).then([reader](Streams::IRandomAccessStreamWithContentType^ stream)
			{
				*reader = ref new Streams::DataReader(stream);
				return (*reader)->LoadAsync(static_cast<uint32>(stream->Size));
			}).then([this, reader](uint32 bytesRead)
			{
				Streams::DataReader^ state = (*reader);

				try
				{
					int version = state->ReadInt32();
					if (version == Game::SaveGameVersion)
					{
						StateId stateid = static_cast<StateId>(state->ReadInt32());
						m_currentState = &m_states.at(stateid);
						m_levels.Restore(state);
						m_cycle.RestoreState(state);
						m_cyclist.RestoreState(state);
						m_renderer->RestoreState(state);
						m_score = state->ReadSingle();
						m_hiScore = state->ReadInt32();
						m_cycle.Update(0.0f);
						m_cyclist.Update(0.0f);
						m_background.Clear();
						m_background.SetTheme(m_levels.PreviousLevel()->GetTheme(), m_levels.PreviousLevel()->EndPoint().x);
						m_background.SetTheme(m_levels.CurrentLevel()->GetTheme(), m_levels.CurrentLevel()->EndPoint().x);
						m_background.SetTheme(m_levels.NextLevel()->GetTheme(), m_levels.NextLevel()->EndPoint().x);
						m_background.SetTheme(m_levels.NextNextLevel()->GetTheme(), m_levels.NextNextLevel()->EndPoint().x);
						SetInitialMenuState();
					}
					else
					{
#ifdef _DEBUG
						OutputDebugStringA("SaveGameVersion does not match!");
#endif
						ResetLevels();
					}
				}
				catch (Platform::Exception^ e) 
				{
					ResetLevels();
				}
				catch (const std::exception&)
				{
					ResetLevels();
				}

				m_ready = true;
			});
		}
		catch (Platform::Exception^ e)
		{
			ResetLevels();
			m_ready = true;
		}
	});
}

void Game::UpdateScreenDimensions()
{
	m_menu.SetScreenDimensions(m_renderer);
}

void Game::OnPressed(Windows::Foundation::Point point)
{
	if (m_menu.OnPressed(point))
	{
		m_cycle.GetControls().ControlPressed(false);
	}
	else
	{
		m_cycle.GetControls().ControlPressed(true);
		m_menu.EnableHelp(false);
	}
}


void Game::OnReleased()
{
	m_cycle.GetControls().ControlPressed(false);
	
}

void Game::ResetLevels()
{
	std::lock_guard<std::mutex> lock(m_worldLock);
	m_levels.Reset();
	m_background.Clear();
	m_background.SetTheme(m_levels.PreviousLevel()->GetTheme(), m_levels.PreviousLevel()->EndPoint().x);
	m_background.SetTheme(m_levels.CurrentLevel()->GetTheme(), m_levels.CurrentLevel()->EndPoint().x);
	m_background.SetTheme(m_levels.NextLevel()->GetTheme(), m_levels.NextLevel()->EndPoint().x);
	m_background.SetTheme(m_levels.NextNextLevel()->GetTheme(), m_levels.NextNextLevel()->EndPoint().x);
	m_cycle.Reset(m_levels.CurrentLevel()->StartPoint());
	m_cyclist.Reset();
	m_renderer->SetDeadZone();
	UpdateCameraView(m_levels.CurrentLevel()->IsFullScreen());
	m_renderer->SetCameraToTarget();
	m_score = 0;
	m_bonus = 0;
	m_freezeCamera = false;
	m_pauseRequested = false;
	// m_worldLock freed when lock goes out of scope
}

void Game::ResumeSuspend()
{
	m_ready = true;
	m_audio.ResumeSuspend();
	m_audio.Pause();
}

void Game::Pause()
{
	if (GetCurrentState() == ePlay)
	{
		m_pauseRequested = true;
	}
}

void Game::Draw()
{	
	//m_renderer->SetText(std::to_wstring((int)m_distance));
	m_background.Draw(m_renderer);
	m_levels.Draw(m_renderer);
	m_cyclist.DrawLeft(m_renderer);
	m_cycle.Draw(m_renderer);
	m_cyclist.DrawRight(m_renderer);
	m_menu.Draw(m_renderer);
	
//	m_world.DrawDebugData();

	m_renderer->Render();
//	m_debugDraw.Render(m_renderer);
}

void Game::Update(float dt)
{
	m_dt = dt;
	if (m_pauseRequested)
	{
		m_currentState = &m_states[ePause];
		m_menu.EnablePlay();
		m_menu.EnablePause(false);
		m_menu.EnableRestart();
		m_pauseRequested = false;
	}
	m_currentState->Update(dt);
	m_currentState->DoAction(this);
	m_currentState = m_currentState->GetNext(this);
	m_audio.Update(dt);
	m_menu.Update(dt);
	m_menu.InputHandled();
}


Game::StateId Game::GetCurrentState()
{
	size_t index = m_currentState - &m_states[0];
	return m_pauseRequested ? ePause : static_cast<StateId>(index);
}

void Game::SetInitialMenuState()
{
	StateId current = GetCurrentState();
	m_menu.EnableGameOver(current == eGameOver);
	m_menu.EnablePause(current == ePlay);
	m_menu.EnablePlay(current == ePause || current == eStart);
	m_menu.EnableRestart(current == ePause || current == ePlay);
	m_menu.EnableScore(current == ePlay || current == ePause);
	m_menu.EnableTitle(current == eStart);
	m_menu.EnableBonus(m_cyclist.IsAir() && current == ePlay);
	m_bonus = static_cast<int>(m_cyclist.AirDistanceTravelled());
	m_menu.SetScore(static_cast<int>(m_score));
	m_menu.SetBonus(m_bonus);
	m_menu.SetHiScore(m_hiScore);
}

void Game::BeginContact(b2Contact* c)
{
	void* bodyUserData = c->GetFixtureA()->GetBody()->GetUserData();
	if (bodyUserData)
	{
		static_cast<GameObject*>(bodyUserData)->BeginContact(c);
	}
	bodyUserData = c->GetFixtureB()->GetBody()->GetUserData();
	if (bodyUserData)
	{
		static_cast<GameObject*>(bodyUserData)->BeginContact(c);
	}
}

void Game::EndContact(b2Contact* c)
{
	void* bodyUserData = c->GetFixtureA()->GetBody()->GetUserData();
	if (bodyUserData)
	{
		static_cast<GameObject*>(bodyUserData)->EndContact(c);
	}
	bodyUserData = c->GetFixtureB()->GetBody()->GetUserData();
	if (bodyUserData)
	{
		static_cast<GameObject*>(bodyUserData)->EndContact(c);
	}
}


void Game::UpdateCameraView(bool fullscreen)
{
	float x = fullscreen ? m_levels.CurrentLevel()->CenterPoint().x : m_cyclist.GetPosition().x + 0.5f*m_cyclist.GetSpeed().x;
	float y = fullscreen ? m_levels.CurrentLevel()->CenterPoint().y : m_cyclist.GetPosition().y + 0.5f*m_cyclist.GetSpeed().y;
	float width = fullscreen ? m_levels.CurrentLevel()->Width() + m_cycle.GetWidth() : fabs(m_cyclist.GetSpeed().x) + m_cycle.GetWidth();
	if (x - 0.5f * width < m_levels.PreviousLevel()->StartPoint().x)
	{
#ifdef _DEBUG
		char buf[32];
		sprintf_s(buf, "camera: %x < %x\n", x - 0.5f * width, m_levels.PreviousLevel()->StartPoint().x);
		OutputDebugStringA(buf);
#endif
		x = m_levels.PreviousLevel()->StartPoint().x + 0.5f * width;
		y = m_levels.PreviousLevel()->StartPoint().y;
	}
	m_renderer->SetCameraTarget(x, y, width);
}


void Game::DoPlay()
{
	UpdateWorld();
	m_cycle.Update(m_dt);
	m_cyclist.Update(m_dt);
	m_levels.Update(m_dt);
	if (!m_levels.CurrentLevel()->IsFullScreen())
	{
		UpdateCameraView();
	}
	m_score += m_cyclist.DistanceDelta();
	m_menu.SetScore(static_cast<int>(m_score));
	if (m_cyclist.IsAir())
	{
		m_bonus = static_cast<int>(m_cyclist.AirDistanceTravelled());
		m_menu.SetBonus(m_bonus);
		m_menu.EnableBonus();
	}
	else
	{
		if (m_bonus > 0)
		{
			m_score += m_bonus;
			m_menu.BumpScore();
			m_bonus = 0;
		}
		m_menu.EnableBonus(false);
		
	}
	if (m_cyclist.GetPosition().x > m_levels.NextLevel()->StartPoint().x)
	{
		std::lock_guard<std::mutex> lock(m_worldLock);
		m_levels.GenerateNextLevel();
		m_background.SetTheme(m_levels.NextNextLevel()->GetTheme(), m_levels.NextNextLevel()->EndPoint().x);
		UpdateCameraView(m_levels.CurrentLevel()->IsFullScreen());
		if (m_levels.PreviousLevel()->IsFullScreen() || m_levels.CurrentLevel()->IsFullScreen())
		{
			m_renderer->SetTransitionTime(0.3f);
		}
		if (!m_levels.PreviousLevel()->IsFullScreen() && m_levels.CurrentLevel()->IsFullScreen())
		{
			m_renderer->SetDeadZone(0.0f);
		}
		if (m_levels.PreviousLevel()->IsFullScreen() && !m_levels.CurrentLevel()->IsFullScreen())
		{
			m_renderer->SetDeadZone();
		}

		// m_worldLock freed when lock goes out of scope
	}

}


void Game::DoGameOver()
{
	UpdateWorld();
	m_cycle.Update(m_dt);
	m_cyclist.Update(m_dt);
	m_levels.Update(m_dt);
	if (!m_freezeCamera)
	{
		UpdateCameraView(false);
	}

}

void Game::DoRestart()
{
	m_audio.Stop();
	m_menu.EnableHelp(false);
	m_menu.EnablePause(false);
	m_menu.EnableRestart(false);
	CheckHiScore();
	m_menu.EnableScore(false);
	m_menu.EnableBonus(false);
	ResetLevels();
	m_menu.EnableTitle();
	m_menu.EnablePlay();
}

bool Game::CheckHiScore()
{
	bool newRecord = false;
	int score = static_cast<int>(m_score);
	if (score > m_hiScore)
	{
		newRecord = true;
		m_hiScore = score;
		m_menu.SetHiScore(score);
		m_menu.EnableHiscore();
		m_menu.EnableNewRecord();
		m_menu.EnableScore(false);
	}
	return newRecord;
}

bool Game::CyclistOutOfBounds()
{
	if (m_cyclist.GetPosition().x < m_levels.CurrentLevel()->StartPoint().x)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Game::UpdateWorld()
{
	const float maxTimeStep = 1.0f / 30.0f;
	float timeStep = min(maxTimeStep, m_dt);
	std::lock_guard<std::mutex> lock(m_worldLock);
	m_world.Step(timeStep, 8, 3);
	// m_worldLock freed when lock goes out of scope
}