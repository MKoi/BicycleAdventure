#pragma once
#include <string>
#include "VerticeBatch.h"
#include "Renderer.h"
#include "Animation.h"
#include "GameAudio.h"
//#include "Camera.h"
//#include "Game.h"


class Menu
{
public:

	Menu(GameAudio* audio);
	bool OnPressed(Windows::Foundation::Point point);
	void SetScreenDimensions(Renderer^ renderer);
	void Draw(Renderer^ renderBatch);
	void EnablePlay(bool enable = true);
	void EnablePause(bool enable = true);
	void EnableRestart(bool enable = true);
	void EnableTitle(bool enable = true);
	void EnableGameOver(bool enable = true);
	void EnableScore(bool enable = true) { m_score.m_enabled = enable; }
	void EnableBonus(bool enable = true);
	void EnableHiscore(bool enable = true);
	void EnableNewRecord(bool enable = true);
	void EnableHelp(bool enable = true);
	void SetScore(int score);
	void BumpScore();
	void SetBonus(int bonus);
	void SetHiScore(int hiscore);
	bool PausePressed() const { return m_pause.m_pressed; }
	bool RestartPressed() const { return m_restart.m_pressed; }
	bool PlayPressed() const { return m_play.m_pressed; }

	void InputHandled();

	void Update(float dt);

private:
	void CreateGfx();
	void DrawPlay(Renderer^ renderBatch);
	void DrawPause(Renderer^ renderBatch);
	void DrawRestart(Renderer^ renderBatch);
	void DrawTitle(Renderer^ renderBatch);
	void DrawGameOver(Renderer^ renderBatch);
	void DrawHiScore(Renderer^ renderBatch);
	void DrawScore(Renderer^ renderBatch);
	void DrawBonus(Renderer^ renderBatch);
	void DrawNewRecord(Renderer^ renderBatch);
	void DrawHelp(Renderer^ renderBatch);

	struct MenuElement {
		MenuElement(std::vector<MenuElement*>& menus, void (Menu::*drawfunc)(Renderer^ renderer));
		bool m_pressed;
		bool m_enabled;
		VerticeBatch m_gfx;
		Windows::Foundation::Rect m_bounds;
		void (Menu::*m_draw)(Renderer^ renderer);
	};

	struct TextElement {
		TextElement(std::vector<TextElement*>& texts, void (Menu::*drawfunc)(Renderer^ renderer));
		bool m_enabled;
		std::wstring m_text;
		void (Menu::*m_draw)(Renderer^ renderer);
		void Update(float dt);
		AnimatedValue<DirectX::XMFLOAT2> m_scale;
		AnimatedValue<DirectX::XMFLOAT2> m_pos;
		float m_delay;
	};

	float m_transition;
	float m_bigradius;
	float m_smallradius;

	
	std::vector<MenuElement*> m_menuelements;
	std::vector<TextElement*> m_textelements;

	MenuElement m_play;
	MenuElement m_pause;
	MenuElement m_restart;

	TextElement m_title;
	TextElement m_score;
	TextElement m_bonus;
	TextElement m_hiscore;
	TextElement m_gameover;
	TextElement m_newRecord;
	TextElement m_help;

	GameAudio* m_audio;
};
