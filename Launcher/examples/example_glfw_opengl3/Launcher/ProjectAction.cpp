#include "ProjectAction.h"
#include <imgui.h>
void ProjectAction::Show()
{
	ImVec2 content_region = ImGui::GetContentRegionAvail();

	ImVec2 button_size = { content_region.x * 0.5f , 100 };
	ImGui::Dummy({ content_region.x * 0.25f,0 }); ImGui::SameLine();
	if (ImGui::Button("Create Project", button_size))
	{

	}
	ImGui::Dummy(button_size);
	ImGui::Dummy({ content_region.x * 0.25f,0 }); ImGui::SameLine();
	if (ImGui::Button("Add Project", button_size))
	{

	}
	ImGui::Dummy(button_size);
	ImGui::Dummy({ content_region.x * 0.25f,0 }); ImGui::SameLine();
	if (ImGui::Button("Remove Project", button_size))
	{

	}
}
