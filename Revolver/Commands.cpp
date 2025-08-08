#include "Commands.h"

#ifdef _WIN32
	#include "Platform/Windows/WindowsCommand.h"
#endif

Shared<Command> Command::Create(const std::string& command, const std::string& working_dir)
{
#ifdef _WIN32
	return CreateShared<WindowsCommand>(command, working_dir);
#else
	#error "ONLY IMPLEMENTED FOR WINDOWS"
#endif
}
