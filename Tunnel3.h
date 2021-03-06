#pragma once
#include <vector>
#include "Box2D\Box2D.h"
#include "GameArea.h"
#include "Script.h"

class Tunnel3 : public GameArea
{
public:
	Tunnel3(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant),
		m_rock(NULL),
		m_script([&]() {
		if (m_rock)
		{
			m_rock->SetType(b2BodyType::b2_dynamicBody);
			m_audio->Play(GameAudio::eRollingStone, 0.1f);
		}
	}, world, pos + b2Vec2(10.0f, 0.0f), 10.0f)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		vector<b2Vec2> ground;
		ground.push_back(b2Vec2_zero);
		ground.emplace_back(15.0f, 0.0f);
		ground.emplace_back(47.0f, 16.0f);
		ground.emplace_back(50.0f, 16.0f);
		m_groundBody = Physics::CreateChain(world, params, ground.data(), ground.size(), pos);
		m_end = ground.back();
		UpdateDimensions();

		ground.clear();
		ground.emplace_back(12.0f, 5.0f);
		ground.emplace_back(15.0f, 5.0f);
		ground.emplace_back(35.0f, 15.0f);
		ground.emplace_back(12.0f, 20.8f);
		m_bodies.push_back(Physics::CreatePolygon(world, params, ground.data(), ground.size(), pos));

		b2Vec2 rockpos(15.0f, 22.0f);
		float radius = 2.0f;
		Physics::Parameters rockparams = { b2BodyType::b2_staticBody, 2.0f, 0.7f, 0.0f, Physics::GroundCategory };
		m_rock = Physics::CreateCircle(world, rockparams, radius, pos + rockpos);
		Graphics::CreateSolidCircle(b2Vec2_zero, radius, Graphics::grey, m_gfx);
		m_gfx.Update(0.0f, m_rock->GetAngle(), m_rock->GetPosition());
	}

	virtual void Draw(Renderer^ renderBatch)
	{
		renderBatch->AddToBatch(&m_gfx);
		GameArea::Draw(renderBatch);
	}

	virtual void Update(float dt)
	{
		const unsigned int interval = 10;
		static unsigned int count = 0;
		count++;
		m_script.Update(dt);
		m_gfx.Update(dt, m_rock->GetAngle(), m_rock->GetPosition());
		if (m_script.ActionTriggered() && count % interval == 0)
		{
			float vol = min(1.0f, 0.1f * abs(m_rock->GetAngularVelocity()));
			m_audio->SetVolume(GameAudio::eRollingStone, vol);
			count = 0;
		}

	}

	virtual void SaveState(Windows::Storage::Streams::DataWriter^ state) const
	{
		GameArea::SaveState(state);
		Physics::Serialize(m_rock, state);
		m_script.SaveState(state);
	}

	virtual void RestoreState(Windows::Storage::Streams::DataReader^ state)
	{
		GameArea::RestoreState(state);
		Physics::DeSerialize(m_rock, state);
		m_script.RestoreState(state);
		m_gfx.Update(0.0f, m_rock->GetAngle(), m_rock->GetPosition());
	}

	virtual bool IsFullScreen() const { return true; }

	virtual ~Tunnel3()
	{
		m_rock->GetWorld()->DestroyBody(m_rock);
		m_audio->Stop(GameAudio::eRollingStone);
	}

private:
	b2Body* m_rock;
	Script m_script;
	VerticeBatch m_gfx;

};
