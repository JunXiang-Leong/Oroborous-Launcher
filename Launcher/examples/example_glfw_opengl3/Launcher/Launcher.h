#pragma once
#include "ProjectTracker.h"
#include "ProjectAction.h"
class Launcher
{
public:
	Launcher();
	void InitAllUI();
private:
	ProjectTracker m_tracker;
	ProjectAction m_action;
};
