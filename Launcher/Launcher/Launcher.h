#pragma once
#include "ProjectTracker.h"
class Launcher
{
public:
	Launcher(std::function <void(std::filesystem::path&)> selectProj);
	void InitAllUI();
private:
	ProjectTracker m_tracker;

};
