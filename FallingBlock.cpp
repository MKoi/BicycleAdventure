#include "pch.h"
#include "FallingBlock.h"
#include "GameArea.h"


void FallingBlock::BeginContact(b2Contact* c)
{
	GameAudio* audio = GameArea::GetAudio();
	if (audio)
	{
		int i = (std::rand() % 3);
		switch (i)
		{
		case 0:
			audio->Play(GameAudio::eRock1, 0.6f);
			break;
		case 1:
			audio->Play(GameAudio::eRock2, 0.6f);
			break;
		case 2:
			audio->Play(GameAudio::eRock3, 0.6f);
			break;
		default:
			break;
		}
	}
}
