#pragma once

#include "efsw/efsw.hpp"
#include <chrono>
#include <unordered_map>

class Revolver;

class ListenerDLL : public efsw::FileWatchListener
{
public:
	ListenerDLL(Revolver& revolver) : m_Revolver(revolver) {}
	void handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename) override;

private:
	Revolver& m_Revolver;
};

class ListenerFile : public efsw::FileWatchListener
{
public:
	ListenerFile(Revolver& revolver) : m_Revolver(revolver) {}
	void handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename) override;

private:
	Revolver& m_Revolver;
	std::unordered_map<std::string, std::chrono::steady_clock::time_point> m_IgnoreUntil;
};
