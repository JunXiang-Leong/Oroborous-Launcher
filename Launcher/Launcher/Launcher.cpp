#include "Launcher.h"
#include "Utilities/ImGuiManager_Launcher.h"
Launcher::Launcher()
{
	InitAllUI();
}

void Launcher::InitAllUI()
{
	ImGuiManager_Launcher::Create("ProjectTracker", true, ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration, [this]() {m_tracker.Show(); });
}
