#include "pch.h"
#include "GameAudio.h"
#include "gamesounds.h"
#include <ppltasks.h>


using namespace DirectX;
using namespace concurrency;

#ifdef _DEBUG
AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default | AudioEngine_Debug | AudioEngine_UseMasteringLimiter | AudioEngine_DisableVoiceReuse;
#else
AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default | AudioEngine_UseMasteringLimiter | AudioEngine_DisableVoiceReuse;
#endif

GameAudio::GameAudio() : m_apiCallsPerFrame(0), m_updateInterval(0.03f), m_dt(m_updateInterval), m_retryAudio(false)
{
	for (SoundData& s : m_sounds)
	{
		s.m_state = SoundState::STOPPED;
		s.m_savedState = SoundState::STOPPED;
		s.m_vol = 1.0f;
		s.m_stateModified = false;
		s.m_volModified = false;
	}
	m_audio.reset(new AudioEngine(eflags));
}

void GameAudio::LoadResources()
{
	create_task([&]()->void
	{
		m_wavebank.reset(new WaveBank(m_audio.get(), L"assets\\gamesounds.xwb"));
		m_sounds[eRolling].m_effect = m_wavebank->CreateInstance(eRolling);
		m_sounds[eWheel].m_effect = m_wavebank->CreateInstance(eWheel);
		m_sounds[eRollingStone].m_effect = m_wavebank->CreateInstance(eRollingStone);
	}).then([&]()
	{
		try
		{
			m_wavebank->Play(eStart, 1.0f, 0.0f, 0.0f);
#ifdef _DEBUG
			OutputDebugStringA("GameAudio::LoadResources done\n");
#endif
		}
		catch (...)
		{
#ifdef _DEBUG
			OutputDebugStringA("GameAudio::LoadResources failed\n");
#endif
		}
	});
}

void GameAudio::Update(float dt)
{
	m_dt -= dt;
	if (m_dt < 0.0f)
	{
		size_t i = 0;
		for (SoundData& s : m_sounds)
		{
			if (s.m_stateModified)
			{
				try
				{
					if (s.m_effect)
					{
						SoundState current = s.m_effect->GetState();
						if (current == SoundState::STOPPED && s.m_state == SoundState::PLAYING)
						{
							s.m_effect->Play(true);
						}
						else if (current == SoundState::PAUSED && s.m_state == SoundState::PLAYING)
						{
							s.m_effect->Resume();
						}
						else if (current == SoundState::PLAYING && s.m_state == SoundState::STOPPED)
						{
							s.m_effect->Pause(); // Stop()
						}
						else if (current == SoundState::PAUSED && s.m_state == SoundState::STOPPED)
						{
							s.m_effect->Stop();
						}
						else if (current == SoundState::PLAYING && s.m_state == SoundState::PAUSED)
						{
							s.m_effect->Pause();
						}
					}
					else if (s.m_state == PLAYING)
					{
						//float vol = (s.m_volModified) ? s.m_vol : 1.0f;
						float pitch = 0.0f;
						float pan = 0.0f;
						m_wavebank->Play(i, s.m_vol, pitch, pan);
						s.m_state = STOPPED;
					}
					else
					{
						s.m_state = STOPPED;
					}
				}
				catch (std::exception&)
				{
#ifdef _DEBUG
					OutputDebugStringA("GameAudio::Update failed\n");
#endif
				}			
			}
			if (s.m_volModified && s.m_effect)
			{
				s.m_effect->SetVolume(s.m_vol);				
			}
			s.m_volModified = false;
			s.m_stateModified = false;
			++i;
		}
		m_apiCallsPerFrame = 0;
		m_dt += m_updateInterval;
	}
	if (m_retryAudio)
	{
		m_retryAudio = false;
		if (m_audio->Reset())
		{
			// TODO: restart any looped sounds here
		}
	}
	else if (!m_audio->Update())
	{
		if (m_audio->IsCriticalError())
		{
			m_retryAudio = true;
		}
	}

}


void GameAudio::Play(SoundId id, float vol)
{
	if (CheckId(id))
	{
		if (m_sounds[id].m_effect == nullptr || m_sounds[id].m_state != SoundState::PLAYING)
		{
			m_sounds[id].m_state = SoundState::PLAYING;
			m_sounds[id].m_stateModified = true;
			m_sounds[id].SetVolume(vol);
		}
	}
}

void GameAudio::PlayImmediately(SoundId id, float vol)
{
	if (CheckId(id))
	{
		m_wavebank->Play(id, vol, 0.0f, 0.0f);
	}
}


void GameAudio::Pause(SoundId id)
{
	if (CheckId(id))
	{
		m_sounds[id].SetState(SoundState::PAUSED);
	}
}

void GameAudio::Pause()
{
	for (SoundData& s : m_sounds)
	{
		s.SetState(SoundState::PAUSED);
	}
}

void GameAudio::SaveState()
{
	for (SoundData& s : m_sounds)
	{
		s.m_savedState = s.m_state;
	}
}

void GameAudio::Resume(SoundId id)
{
	if (CheckId(id))
	{
		m_sounds[id].SetState(SoundState::PLAYING);
	}
}

void GameAudio::Resume()
{
	for (SoundData& s : m_sounds)
	{
		s.SetState(SoundState::PLAYING);
	}
}

void GameAudio::RestoreState()
{
	for (SoundData& s : m_sounds)
	{
		s.SetState(s.m_savedState);
	}
}


void GameAudio::Stop(SoundId id)
{
	if (CheckId(id))
	{
		m_sounds[id].SetState(SoundState::STOPPED);
	}
}

void GameAudio::Stop()
{
	for (SoundData& s : m_sounds)
	{
		s.SetState(SoundState::STOPPED);
	}
}

void GameAudio::Suspend()
{
	SaveState();
	if (m_audio)
	{
		m_audio->Suspend();
	}
}


void GameAudio::ResumeSuspend()
{
	if (m_audio)
	{
		m_audio->Resume();
	}
	RestoreState();
}

void GameAudio::SetPitch(SoundId id, float pitch)
{
	if (CheckId(id) && m_sounds[id].m_effect)
	{
		try
		{
			m_sounds[id].m_effect->SetPitch(pitch);
		}
		catch (std::exception&)
		{
#if _DEBUG
			OutputDebugStringA("GameAudio::SetPitch failed\n");
#endif
		}
	}
}

void GameAudio::SetVolume(SoundId id, float vol)
{
	if (CheckId(id))
	{
		m_sounds[id].SetVolume(vol);
	}
}

bool GameAudio::CheckId(SoundId id) const
{
	return (id >= 0 && id < eMaxSounds);
}

void GameAudio::SoundData::SetState(DirectX::SoundState s)
{
	if (m_state != s)
	{
		m_state = s;
		m_stateModified = true;
	}
}

void GameAudio::SoundData::SetVolume(float vol)
{
	if (m_vol != vol)
	{
		m_vol = vol;
		m_volModified = true;
	}
}


