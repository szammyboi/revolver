#include "Revolver.h"

#include <iostream>
#include <filesystem>
#include "efsw/efsw.hpp"
#include "Listeners.h"

typedef int(__cdecl* TestFunction)();

Revolver::Revolver()
{
	m_Watcher = new efsw::FileWatcher();
	m_Listener = new ListenerDLL(*this);
	m_FileListener = new ListenerFile(*this);

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

		if (filename.rfind(dll + "_", 0) == 0)
		{
			return LoadDLL(dll, filename.c_str());
		}
	}

	return false;
}

void Revolver::LinkModuleFiles(const std::string& module, const std::string& files)
{
	m_Watcher->addWatch(files, m_FileListener, false);
	std::cout << files << std::endl;
}

Revolver::~Revolver()
{
	for (const auto& [file, module] : m_Modules)
	{
		FreeLibrary(module);
	}
}
