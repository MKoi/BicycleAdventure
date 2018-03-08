#pragma once
#include <vector>
#include "Graphics.h"
#include "VerticeBatch.h"

class Theme
{
public:
	enum Type
	{
		e_grass,
		e_desert,
		e_rock,
		e_forest,
		e_forest2,
		e_urban,
		e_undefined,
	};

	Type Style;
	b2Color Color;
	Theme() : Style(e_undefined), Color(Graphics::red), m_decorations() {}
	Theme(Type type, const b2Color& color);
	const VerticeBatch& GetDecoration(size_t hash) const;
	bool HasDecorations() const { return !m_decorations.empty(); }
	void Update(float dt);


private:

	
	std::vector<VerticeBatch> m_decorations;

	void InitGrassTheme();
	void InitDesertTheme();
	void InitRockTheme();
	void InitForestTheme();
	void InitForest2Theme();
	void InitUrbanTheme();
};
