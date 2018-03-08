#include "pch.h"
#include "SaveGame.h"
#include <ppltasks.h>

using namespace Windows::Storage;
using namespace Concurrency;

SaveGame::SaveGame(): m_saveFile(L"save")
{
    
    
}

void SaveGame::Save()
{
    auto folder = ApplicationData::Current->LocalFolder;
    task<StorageFile^> getFileTask(folder->CreateFileAsync(
        ref new Platform::String(m_saveFile), 
        CreationCollisionOption::ReplaceExisting));

    // Create a local to allow the DataReader to be passed between lambdas.
    auto writer = std::make_shared<Streams::DataWriter^>(nullptr);
}