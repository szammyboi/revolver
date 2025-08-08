#pragma once

#include "Revolver/Commands.h"

#include <string>
#include <Windows.h>

struct WindowsCommandData
{
	LPSTR cmdline;
	LPSTR dir;
	STARTUPINFOA startup_info;
	PROCESS_INFORMATION process_info;
};

class WindowsCommand : public Command
{
public:
	WindowsCommand(const std::string& command, const std::string& working_dir);
	virtual ~WindowsCommand() = default;

	virtual void Execute() override;

private:
	WindowsCommandData m_Data;
	std::string m_Directory;
	std::string m_Command;
};
