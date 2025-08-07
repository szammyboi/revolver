#include "Revolver.h"

#include <iostream>
#include <filesystem>
#include <thread>
#include "efsw/efsw.hpp"

typedef int(__cdecl* TestFunction)();

bool RunCommandInDir(const std::string& command, const std::string& directory, DWORD& exitCode) {
    std::string cmd = "cmd.exe /C " + command;
    LPSTR cmdLine = _strdup(cmd.c_str());
    LPSTR workingDir = _strdup(directory.c_str());

    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);

    BOOL success = CreateProcessA(
        NULL,             // Application name
        cmdLine,          // Command line
        NULL,             // Process security
        NULL,             // Thread security
        FALSE,            // Inherit handles
        0,                // Creation flags
        NULL,             // Use parent's environment
        workingDir,       // 🔥 Working directory
        &si,              // Startup info
        &pi               // Process info
    );

    free(cmdLine);
    free(workingDir);

    if (!success) {
        std::cerr << "CreateProcess failed: " << GetLastError() << "\n";
        return false;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}

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

class Listener_DLL : public efsw::FileWatchListener
{
public:
	Listener_DLL(Revolver& revolver) : m_Revolver(revolver) {}
	void handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename) override
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

					TestFunction test = m_Revolver.GetFunction<TestFunction>("HotReload", "test");
					std::cout << "test() -> " << test() << std::endl;
				}
			}
		}
	}

private:
	Revolver& m_Revolver;
};

class Listener_File : public efsw::FileWatchListener
{
public:
	Listener_File(Revolver& revolver) : m_Revolver(revolver) {}
	void handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename) override
	{
		static int i = 0;

		if (filename == m_LastChange && i++ % 2 == 0)
			return;


		DWORD exit;
		RunCommandInDir("xmake build -P .", "c:/dev/cpp/revolver/testing", exit);
		m_LastChange = filename;
		//RunCommandInDir("dir", "c:/dev/cpp/revolver/testing", exit);
	}

private:
	Revolver& m_Revolver;
	std::string m_LastChange = "";
};

Revolver::Revolver()
{
	m_Watcher = new efsw::FileWatcher();
	m_Listener = new Listener_DLL(*this);
	m_FileListener = new Listener_File(*this);

	m_Watcher->addWatch("./", m_Listener, false);
	m_Watcher->watch();
}

bool Revolver::LoadDLL(const std::string& dll, const char* filename)
{
	std::cout << filename << std::endl;
	HMODULE module = LoadLibraryA(filename);
	DWORD err = GetLastError();
	LPVOID msg;
	FormatMessageA(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	    NULL,
	    err,
	    0,
	    (LPSTR)&msg,
	    0,
	    NULL);
	std::cerr << "LoadLibraryA failed: " << err << " - " << (char*)msg << std::endl;
	LocalFree(msg);
	if (module == NULL)
		return false;

	m_Modules[dll] = module;
	m_Paths[dll] = filename;

	return true;
}

bool Revolver::AddModule(const std::string& dll)
{
	const std::filesystem::path sandbox { "./" };

	for (auto const& dir_entry : std::filesystem::directory_iterator { sandbox })
	{
		if (!dir_entry.is_regular_file())
			continue;

		const std::string filename = dir_entry.path().filename().string();

		if (filename.rfind("HotReload_", 0) == 0)
		{
			return LoadDLL(dll, filename.c_str());
		}
	}

	return false;
}

void Revolver::LinkModuleFiles(const std::string& module, const std::string& files)
{
	m_Watcher->addWatch(files, m_FileListener, false);
	
}

Revolver::~Revolver()
{
	for (const auto& [file, module] : m_Modules)
	{
		FreeLibrary(module);
	}
}
