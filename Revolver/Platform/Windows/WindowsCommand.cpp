#include "WindowsCommand.h"

#include <Windows.h>

WindowsCommand::WindowsCommand(const std::string& command, const std::string& working_dir)
    : m_Directory(working_dir), m_Command(command)
{
	std::string cmd = "cmd.exe /C " + m_Command;
	m_Data.cmdline = _strdup(cmd.c_str());
	m_Data.dir = _strdup(m_Directory.c_str());

	m_Data.startup_info = { 0 };
	m_Data.process_info = { 0 };
	m_Data.startup_info.cb = sizeof(m_Data.startup_info);
}

void WindowsCommand::Execute()
{
	BOOL success = CreateProcessA(
	    NULL,
	    m_Data.cmdline,
	    NULL,
	    NULL,
	    FALSE,
	    0,
	    NULL,
	    m_Data.dir,
	    &m_Data.startup_info,
	    &m_Data.process_info);

	if (!success)
	{
		//		std::cerr << "CreateProcess Failed: " << GetLastError() << "\n";
		return;
	}

	DWORD error;
	WaitForSingleObject(m_Data.process_info.hProcess, INFINITE);
	GetExitCodeProcess(m_Data.process_info.hProcess, &error);

	CloseHandle(m_Data.process_info.hProcess);
	CloseHandle(m_Data.process_info.hThread);
}
