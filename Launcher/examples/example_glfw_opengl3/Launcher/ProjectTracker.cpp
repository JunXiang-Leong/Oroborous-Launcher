#include "ProjectTracker.h"
#include "examples/example_glfw_opengl3/Utilities/ImGuiManager.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <filesystem>

#include <windows.h>
#include <shellapi.h>
#include <shobjidl_core.h>

#include <Utilities/FileSystemUtills.h>
ProjectTracker::ProjectTracker()
{
	//read file 
}

void ProjectTracker::Show()
{
	ImGui::BeginTable("table", 2,ImGuiTableFlags_::ImGuiTableFlags_Resizable);
	ImGui::TableNextColumn();
	Tracker();
	ImGui::TableNextColumn();
	Actions();
	ImGui::EndTable();
}

void ProjectTracker::Tracker()
{
	ImGui::BeginGroup();
	std::string someText = "Test Text";
	ImGui::InputText("Temp Test", &someText);

	ImGui::BeginTable("Tracker table",1);
	float width = ImGui::GetContentRegionAvail().x;
	constexpr float height = 100.0f;
	//get copy of font
	ImFont tempfont = *ImGui::GetFont();
	tempfont.Scale *= 1.5f;
	for (auto& dir : m_project_directories)
	{
		ImGui::TableNextColumn();
		ImGui::PushID(dir.string().c_str());
		ImVec2 cursor_pos = ImGui::GetCursorPos();
		//name
		ImGui::PushFont(&tempfont);
		ImGui::Text("Name :%s", dir.stem().string().c_str());
		ImGui::PopFont();
		//path
		ImGui::Text("Path :%s", dir.string().c_str());

		ImGui::SetCursorPos(cursor_pos);
		if (ImGui::Selectable("##projecticon", false, 0, {width ,height}))
		{

		}
		ImGui::PopID();
	}
	ImGui::EndTable();
	ImGui::EndGroup();
}

void ProjectTracker::Actions()
{
	ImGui::BeginGroup();
	ImVec2 content_region = ImGui::GetContentRegionAvail();

	ImVec2 button_size = { content_region.x * 0.5f , 100 };
	if (ImGui::InputText("Project Name", &m_ProjectFileName))
	{

	}

	ImGui::Dummy({ content_region.x * 0.25f,0 }); ImGui::SameLine();
	if (ImGui::Button("Create Project", button_size))
	{
		FileDialogue();
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
	ImGui::EndGroup();
}

void ProjectTracker::CreateProject(std::filesystem::path p)
{
	if (std::filesystem::exists(p) == false)
		return;//failed
	char temp[1024];
	GetModuleFileNameA(NULL, temp, 1024);
	std::filesystem::path exepath = temp;
	auto name = FileSystemUtils::DuplicateItem(exepath.parent_path().string() + "/DoNotTouch", p, m_ProjectFileName);
	if(name.empty() == false)
		m_project_directories.push_back(name);
}

bool ProjectTracker::FileDialogue()
{
	bool activated = false;
	std::filesystem::path p;//this folder path
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
		pFileOpen->SetOptions(FOS_PICKFOLDERS);

		if (SUCCEEDED(hr))
		{
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);
			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					p = pszFilePath;//this path is /rootpath
					//give root path
					CreateProject(p);
					pItem->Release();
					activated = true;
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
	return activated;
}
