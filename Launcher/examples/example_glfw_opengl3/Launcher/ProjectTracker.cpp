#include "ProjectTracker.h"
#include "examples/example_glfw_opengl3/Utilities/ImGuiManager.h"

ProjectTracker::ProjectTracker()
{
	//read file 
}

void ProjectTracker::Show()
{
	std::string someText = "Test Text";
	ImGui::InputText("Temp Test", &someText);
	for (auto& dir : m_project_directories)
	{
		ImGui::Text(dir.string().c_str());
	}
}
