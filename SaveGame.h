#pragma once

class SaveGame
{
public:
	SaveGame();
	void Save();
	void Restore();

private:
	PCWSTR m_saveFile;

};
