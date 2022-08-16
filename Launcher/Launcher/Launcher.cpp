#include "Launcher.h"
#include "Utilities/ImGuiManager_Launcher.h"
Launcher::Launcher(std::function <void(std::filesystem::path&)> selectProj)
	:m_tracker(selectProj)
{
	InitAllUI();
}

void Launcher::InitAllUI()
{
	ImGuiManager_Launcher::Create("ProjectTracker", true, ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration, [this]() {m_tracker.Show(); });
}
