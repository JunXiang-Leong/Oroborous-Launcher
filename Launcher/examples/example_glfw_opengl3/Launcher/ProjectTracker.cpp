#include "ProjectTracker.h"
#include "examples/example_glfw_opengl3/Utilities/ImGuiManager.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <filesystem>

#include <windows.h>
#include <shellapi.h>
#include <shobjidl_core.h>

#include <Utilities/FileSystemUtills.h>

#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>

#include <cstdio>
#include <fstream>
#include <streambuf>
ProjectTracker::ProjectTracker()
{
	char temp[1024];
	GetModuleFileNameA(NULL, temp, 1024);
	m_exepath = std::filesystem::path(temp).parent_path();
	//read file
	FILE* fp = fopen((m_exepath.string() + DATA_FILE_PROJECT_PATH).c_str(), "rb"); // non-Windows use "w"
	//pratically nothing since the first item is alr an object
	if (fp == nullptr)
	{
		m_document.SetObject();
		return;
	}
	constexpr size_t sizebuffer = 65500;
	char* readBuffer  = new char[sizebuffer];

	rapidjson::FileReadStream is(fp, readBuffer, sizebuffer);
	m_document.ParseStream(is);

	fclose(fp);
	delete[] readBuffer;

	if (m_document.IsObject() == false || m_document.IsArray())//the first item should be an object
	{
		m_document.SetObject();
		return;
	}

	for (auto iter = m_document.MemberBegin(); iter != m_document.MemberEnd(); ++iter)
	{
		m_project_directories.emplace(std::string(iter->name.GetString()) ,std::filesystem::path(iter->value.GetString()));
	}
}

ProjectTracker::~ProjectTracker()
{
	FILE* fp = fopen((m_exepath.string() + DATA_FILE_PROJECT_PATH).c_str(), "wb"); // non-Windows use "w"

	constexpr size_t sizebuffer = 65500;
	char* writeBuffer = new char[sizebuffer];
	rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	
	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);
	m_document.Accept(writer);

	fclose(fp);
	delete[] writeBuffer;
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
		ImGui::PushID(dir.first.c_str());
		ImVec2 cursor_pos = ImGui::GetCursorPos();
		//name
		ImGui::PushFont(&tempfont);
		ImGui::Text("Name :%s", dir.first.c_str());
		ImGui::PopFont();
		//path
		ImGui::Text("Path :%s", dir.second.string().c_str());

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
		FileDialogue_Generic(L"Config File(*.json)", L"*.json", [this](const std::filesystem::path& p ) {this->AddProject(p); });
	}
	ImGui::Dummy(button_size);
	ImGui::Dummy({ content_region.x * 0.25f,0 }); ImGui::SameLine();
	if (ImGui::Button("Remove Project", button_size))
	{

	}
	ImGui::EndGroup();
}

void ProjectTracker::CreateProject(const std::filesystem::path& p)
{
	if (std::filesystem::exists(p) == false)
		return;//failed

	auto name = FileSystemUtils::DuplicateItem(m_exepath.string() + DATA_FILE_PATH_TEMPLATE , p, m_ProjectFileName);
	if (name.empty() == false)
	{
		RegisterItem(name);
	}
}

void ProjectTracker::AddProject(const std::filesystem::path& p)
{
	if (std::filesystem::exists(p) == false)
		return;//failed
	if (p.stem().string() != DATA_FILE_CONFIGFILE_NAME)
		return;

	RegisterItem(p.parent_path());//move 1 file out, i dont want the config path
}

void ProjectTracker::RegisterItem(const std::filesystem::path& p)
{
	std::string project_title = p.stem().string();
	std::string pathname = p.string();
	m_project_directories.emplace(project_title, pathname);


	rapidjson::Value projectname;
	projectname.SetString(project_title.c_str(), static_cast<rapidjson::SizeType>(project_title.size()), m_document.GetAllocator());

	rapidjson::Value projectpath;
	projectpath.SetString(pathname.c_str(), static_cast<rapidjson::SizeType>(pathname.size()), m_document.GetAllocator());

	m_document.AddMember(projectname, projectpath, m_document.GetAllocator());
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

bool ProjectTracker::FileDialogue_Generic(const wchar_t* description, const wchar_t* extension,std::function<void(const std::filesystem::path&)> callback)
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
		//pFileOpen->SetOptions(FOS_PICKFOLDERS);
		const COMDLG_FILTERSPEC filter[] = {
			{description, extension}
		};
		pFileOpen->SetFileTypes(1, filter);
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
					activated = true;

					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					p = pszFilePath;//this path is e.g (/root/config.exe)
					if (callback)
						callback(p);
					pItem->Release();
					
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
	return activated;
}
