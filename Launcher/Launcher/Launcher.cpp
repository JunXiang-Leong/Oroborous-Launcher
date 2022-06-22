#include "Launcher.h"
#include "Utilities/ImGuiManager.h"
Launcher::Launcher()
{
	InitAllUI();
}

void Launcher::InitAllUI()
{
	ImGuiManager::Create("ProjectTracker", true, ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration, [this]() {m_tracker.Show(); });
}
