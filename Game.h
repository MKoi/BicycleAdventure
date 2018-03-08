#pragma once
#include <mutex>
#include "Box2D\Box2D.h"
#include "GameArea.h"
#include "LevelManager.h"
#include "Bicycle.h"
#include "Graphics.h"
#include "Cyclist.h"
#include "Physics.h"
#include "GameAudio.h"
#include "Menu.h"
#include "Background.h"
#include "State.h"

class Game: public b2ContactListener
{
public:
	enum StateId
	{
		ePause,
		ePlay,
		eStart,
		eGameOver,
		eMaxStates
	};
	Game(Renderer^ renderer);

	bool Ready() const { return m_ready && m_renderer->Ready(); }
	void Save(Windows::ApplicationModel::SuspendingDeferral^ deferral = nullptr);
	void Restore();
	void UpdateScreenDimensions();
	void OnPressed(Windows::Foundation::Point point);
	void OnReleased();
	void ResetLevels();
	void BeginContact(b2Contact* c);
	void EndContact(b2Contact* c);
	void Pause();
	void Draw();
	void ResumeSuspend();
	void Update(float dt);

private:
	void UpdateCameraView(bool fullscreen = false);
	void DoPlay();
	void DoGameOver();
	void DoRestart();
	bool CheckHiScore();
	bool CyclistOutOfBounds();
	StateId GetCurrentState();
	void SetInitialMenuState();
	void UpdateWorld();

	GameAudio m_audio;
	Renderer^ m_renderer;
	//Graphics m_debugDraw;
	b2World m_world;
	LevelManager m_levels;
	Background m_background;
	Bicycle m_cycle;
	Cyclist m_cyclist;
	Menu m_menu;
	PCWSTR m_saveFile;
	bool m_ready;
	int m_hiScore;
	float m_score;
	int m_bonus;
	float m_dt;
	bool m_freezeCamera;
	bool m_pauseRequested;
	std::mutex m_worldLock;

	std::vector<State<Game> > m_states;
	State<Game>* m_currentState;
	static const int SaveGameVersion = 1;
};
