#pragma once

#include "Memory.h"

#include <string>

class Command
{
public:
	virtual ~Command() = default;

	virtual void Execute() = 0;

	static Shared<Command> Create(const std::string& command, const std::string& working_dir = ".");
};
