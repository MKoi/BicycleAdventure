#pragma once
#include <vector>
#include <Audio.h>
#include "gamesounds.h"

using std::unique_ptr;

class GameAudio
{
public:
	enum SoundId
	{
		eRolling = XACT_WAVEBANK_GAMESOUNDS_ROLLING,
		eCheering = XACT_WAVEBANK_GAMESOUNDS_CHEERING,
		eMenuClick = XACT_WAVEBANK_GAMESOUNDS_MENU_CLICK,
		ePedalUp = XACT_WAVEBANK_GAMESOUNDS_PEDAL1,
		ePedalDown = XACT_WAVEBANK_GAMESOUNDS_PEDAL2,
		eOuch1 = XACT_WAVEBANK_GAMESOUNDS_AUTS1,
		eOuch2 = XACT_WAVEBANK_GAMESOUNDS_AUTS2,
		eOuch3 = XACT_WAVEBANK_GAMESOUNDS_AUTS3,
		eJahuu1 = XACT_WAVEBANK_GAMESOUNDS_JEE1,
		eJahuu2 = XACT_WAVEBANK_GAMESOUNDS_JEE2,
		eJahuu3 = XACT_WAVEBANK_GAMESOUNDS_JEE3,
		eSplash = XACT_WAVEBANK_GAMESOUNDS_SPLASH,
		eWheel = XACT_WAVEBANK_GAMESOUNDS_PEDALING,
		eBrakeShort = XACT_WAVEBANK_GAMESOUNDS_BRAKE_SHORT,
		eBrakeLong = XACT_WAVEBANK_GAMESOUNDS_BRAKE_LONG,
		eRock1 = XACT_WAVEBANK_GAMESOUNDS_ROCK1,
		eRock2 = XACT_WAVEBANK_GAMESOUNDS_ROCK2,
		eRock3 = XACT_WAVEBANK_GAMESOUNDS_ROCK3,
		eStart = XACT_WAVEBANK_GAMESOUNDS_THEME_START,
		eGameOver = XACT_WAVEBANK_GAMESOUNDS_THEME_SAD,
		eNewRecord = XACT_WAVEBANK_GAMESOUNDS_THEME_HAPPY,
		eRollingStone = XACT_WAVEBANK_GAMESOUNDS_BIG_STONE,
		eGravel = XACT_WAVEBANK_GAMESOUNDS_GRAVEL,
		eMaxSounds = XACT_WAVEBANK_GAMESOUNDS_ENTRY_COUNT
	};

	GameAudio();
	void LoadResources();
	void Update(float dt);
	void Play(SoundId id, float vol = 1.0f);
	void PlayImmediately(SoundId id, float vol = 1.0f);
	void Pause(SoundId id);
	void Pause();
	void SaveState();
	void Resume(SoundId id);
	void Resume();
	void RestoreState();
	void Stop(SoundId id);
	void Stop();
	void Suspend();
	void ResumeSuspend();

	void SetPitch(SoundId id, float pitch = 0.0f);
	void SetVolume(SoundId id, float vol = 1.0f);

private:
	struct SoundData
	{
		void SetState(DirectX::SoundState s);
		void SetVolume(float vol);
		unique_ptr<DirectX::SoundEffectInstance> m_effect;
		DirectX::SoundState m_state;
		DirectX::SoundState m_savedState;
		float m_vol;
		bool m_stateModified;
		bool m_volModified;
	};
	bool CheckId(SoundId id) const;
	unique_ptr<DirectX::WaveBank> m_wavebank;
	SoundData m_sounds[eMaxSounds];
	/*
	unique_ptr<DirectX::SoundEffectInstance> m_soundInstances[eMaxSounds];
	std::vector<bool> m_stateModified;
	std::vector<DirectX::SoundState> m_soundState;
	std::vector<DirectX::SoundState> m_savedSoundState;
	*/
	std::unique_ptr<DirectX::AudioEngine> m_audio;
	unsigned int m_apiCallsPerFrame;
	const float m_updateInterval;
	float m_dt;
	bool m_retryAudio;
};