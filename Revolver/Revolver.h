#pragma once

#include <Windows.h>
#include <string>
#include <unordered_map>

#define REVOLVER_EXPOSE extern "C" __declspec(dllexport)

namespace efsw {
	class FileWatcher;
};

class Revolver
{
public:
	friend class Listener_DLL;
	friend class Listener_File;

public:
	Revolver();
	~Revolver();

	bool AddModule(const std::string& module);

	void LinkModuleFiles(const std::string& module, const std::string& files);

	template <typename T>
	T GetFunction(const std::string& dll, const std::string& func)
	{
		return reinterpret_cast<T>(GetProcAddress(m_Modules[dll], func.c_str()));
	}

private:
	bool LoadDLL(const std::string& dll, const char* filename);
	// void ReloadModule(const std::string& module);
private:
	efsw::FileWatcher* m_Watcher;
	Listener_DLL* m_Listener;
	Listener_File* m_FileListener;
	
	std::unordered_map<std::string, HMODULE>
	    m_Modules;
	std::unordered_map<std::string, std::string> m_Paths;
};
