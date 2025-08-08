#include "Listeners.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <filesystem>

#include "Revolver.h"

#include <Windows.h>

bool WaitUntilFileUnlocked(const std::string& path, int maxWaitMs = 5000, int intervalMs = 100)
{
	int waited = 0;

	while (waited < maxWaitMs)
	{
		HANDLE hFile = CreateFileA(
		    path.c_str(),
		    GENERIC_READ,
		    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		    NULL,
		    OPEN_EXISTING,
		    FILE_ATTRIBUTE_NORMAL,
		    NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			return true;
		}

		DWORD err = GetLastError();
		if (err != ERROR_SHARING_VIOLATION && err != ERROR_ACCESS_DENIED)
		{
			std::cerr << "Unexpected file error: " << err << std::endl;
			return false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
		waited += intervalMs;
	}

	std::cerr << "File still locked after waiting: " << path << std::endl;
	return false;
};

void ListenerDLL::handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename)
{
	switch (action)
	{
	case efsw::Actions::Add:
		for (const auto& [dll, module] : m_Revolver.m_Modules)
		{
			std::string prefix = dll + "_";
			std::filesystem::path p(filename);
			if (filename.rfind(prefix, 0) == 0 && p.extension() == ".dll") // starts with "hotreload_"
			{
				WaitUntilFileUnlocked(filename);

				auto oldlib = m_Revolver.m_Modules[dll];
				bool success = m_Revolver.LoadDLL(dll, filename.c_str());

				FreeLibrary(oldlib);
			}
		}
	}
}

void ListenerFile::handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename)
{
	if (action != efsw::Actions::Modified)
		return;

	using namespace std::chrono;
	auto now = steady_clock::now();
	auto key = dir + filename;

	if (m_IgnoreUntil.count(key) > 0 && now < m_IgnoreUntil[key])
		return;

	m_IgnoreUntil[key] = now + milliseconds(500);

	if (m_Revolver.m_BuildCommand)
	{
		std::cout << "-------------------BUILDING-------------------" << std::endl;
		m_Revolver.m_BuildCommand->Execute();
		std::cout << "-------------------RELOADING-------------------" << std::endl;
	}
}
