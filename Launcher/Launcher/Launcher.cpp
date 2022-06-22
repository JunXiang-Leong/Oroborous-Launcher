#include "Launcher.h"
#include "examples/example_glfw_opengl3/Utilities/ImGuiManager.h"
Launcher::Launcher()
{
	InitAllUI();
}

void Launcher::InitAllUI()
{
	ImGuiManager::Create("ProjectTracker", true, ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration, [this]() {m_tracker.Show(); });
}
