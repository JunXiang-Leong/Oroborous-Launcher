#include "ProjectTracker.h"
#include "Utilities/ImGuiManager_Launcher.h"
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <filesystem>

#include <windows.h>
#include <shellapi.h>
#include <shobjidl_core.h>

#include <Utilities/FileSystemUtills_Launcher.h>

#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>

#include <ctime>
#include <cstdio>
#include <fstream>
#include <streambuf>
ProjectTracker::ProjectTracker()
{
	char temp[1024];
	GetModuleFileNameA(NULL, temp, 1024);
	m_exepath = std::filesystem::path(temp).parent_path();
	//read file
	FILE* fp;
	fopen_s(&fp, (m_exepath.string() + DATA_FILE_PROJECT_PATH).c_str(), "rb"); // non-Windows use "w"
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
		auto member = iter->value.GetObj().MemberBegin();
		ProjectFileInfo info;
		info.dateAdded = member->value.GetString(); member++;
		info.lastOpened = member->value.GetString(); member++;
		info.version = member->value.GetString();
		m_project_directories.emplace(std::filesystem::path(iter->name.GetString()) ,info);
	}
}

ProjectTracker::~ProjectTracker()
{
	FILE* fp;
	fopen_s(&fp,(m_exepath.string() + DATA_FILE_PROJECT_PATH).c_str(), "wb"); // non-Windows use "w"

	constexpr size_t sizebuffer = 65500;
	char* writeBuffer = new char[sizebuffer];
	rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	
	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);
	m_document.Accept(writer);

	if(fp)
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
	
	ImGui::InputText("Filter", &m_search);
	ImGui::Separator();

	ImVec2 content_region = ImGui::GetContentRegionAvail();
	ImGui::BeginChild("Trackers", { content_region.x , content_region.y * 0.9f },true);
	ImGui::BeginTable("Tracker table", 1, 0);

	constexpr float height = 100.0f;
	//get copy of font
	ImFont tempfont = *ImGui::GetFont();
	tempfont.Scale *= 1.5f;
	for (auto& dir : m_project_directories)
	{
		std::string temp = dir.first.string();
		auto iter = std::search(temp.begin(), temp.end(), m_search.begin(), m_search.end(), [](char a, char b) {return std::toupper(a) == std::toupper(b); });
		if (iter == temp.end())
			continue;
		ImGui::TableNextColumn();

		ImGui::PushID(dir.first.c_str());
		ImVec2 cursor_pos = ImGui::GetCursorPos();
		//name
		ImGui::PushFont(&tempfont);
		ImGui::Text("Name :%s", dir.first.stem().string().c_str());
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.75f, 0.2f, 0.2f, 1), "%s", std::filesystem::exists(dir.first) ? "" : "[Not Found]");
		ImGui::PopFont();
		//path
		ImGui::Text("Path :%s", dir.first.string().c_str());
		ImGui::Text("Date Added:%s" , dir.second.dateAdded.c_str());
		ImGui::Text("Last Opened:%s", dir.second.lastOpened.c_str());
		ImGui::Text("Editor Version:%s", dir.second.version.c_str());

		ImGui::SetCursorPos(cursor_pos);
		if (ImGui::Selectable("##projecticon", false, ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowItemOverlap, { content_region.x ,height}))
		{
			//some open project code here
		}
		ImGui::Separator();

		ImGui::SetCursorPos(cursor_pos);
		ImGui::Dummy({ content_region.x-120, 0 });
		ImGui::SameLine();
		if (ImGui::Button("Remove"))
		{
			m_document.RemoveMember(dir.first.string().c_str());
			m_project_directories.erase(dir.first);
			ImGui::PopID();
			break;
		}
		ImGui::PopID();
	}
	ImGui::EndTable();
	ImGui::EndChild();

	ImGui::EndGroup();
}

void ProjectTracker::Actions()
{
	ImGui::BeginGroup();
	ImVec2 content_region = ImGui::GetContentRegionAvail();

	ImVec2 button_size = { content_region.x * 0.5f , 100 };

	ImGui::NewLine();
	ImGui::Dummy({ content_region.x * 0.25f,0}); ImGui::SameLine();

	ImGui::BeginGroup();
	ImGui::PushItemWidth(button_size.x);
	if (ImGui::Button("Create Project", button_size))
	{
		FileDialogue();
	}
	ImGui::InputText("##ProjectName", &m_ProjectFileName,ImGuiInputTextFlags_AutoSelectAll);
	ImGui::PopItemWidth();
	ImGui::EndGroup();

	ImGui::Dummy(button_size);
	ImGui::Dummy({ content_region.x * 0.25f,0 }); ImGui::SameLine();
	if (ImGui::Button("Add Project", button_size))
	{
		FileDialogue_Generic(L"Config File(*.json)", L"*.json", [this](const std::filesystem::path& p ) {this->AddProject(p); });
	}
	
	ImGui::EndGroup();
}

void ProjectTracker::CreateProject(const std::filesystem::path& p)
{
	if (std::filesystem::exists(p) == false)
		return;//failed

	auto name = FileSystemUtils_Launcher::DuplicateItem(m_exepath.string() + DATA_FILE_PATH_TEMPLATE , p, m_ProjectFileName);
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
	std::string time_created;
	constexpr const char*  default_ver = "00.00.00";
	{
		time_t rawtime;
		struct tm timeinfo;
		char currentTimeBuffer[64];

		time(&rawtime);
		localtime_s(&timeinfo,&rawtime);

		strftime(currentTimeBuffer, sizeof(currentTimeBuffer), "%d-%m-%Y %H:%M:%S", &timeinfo);
		time_created = (currentTimeBuffer);
	}

	std::string pathname = p.string();
	ProjectFileInfo info;
	info.dateAdded = time_created;
	info.lastOpened = time_created;
	info.version = default_ver;
	m_project_directories.emplace(pathname, info);

	//rapidjson data///////////////////////////
	rapidjson::Value projectInfo;
	projectInfo.SetObject();

	rapidjson::Value date_added;
	date_added.SetString(time_created.c_str(), static_cast<rapidjson::SizeType>(time_created.size()), m_document.GetAllocator());

	rapidjson::Value last_opened;
	last_opened.SetString(time_created.c_str(), static_cast<rapidjson::SizeType>(time_created.size()), m_document.GetAllocator());

	rapidjson::Value version_editor;
	version_editor.SetString(default_ver, static_cast<rapidjson::SizeType>(sizeof(default_ver)), m_document.GetAllocator());//hard coded

	projectInfo.AddMember("Data Added", date_added,m_document.GetAllocator());
	projectInfo.AddMember("Last Opened", last_opened, m_document.GetAllocator());
	projectInfo.AddMember("Version", version_editor, m_document.GetAllocator());

	rapidjson::Value projectpath;
	projectpath.SetString(pathname.c_str(), static_cast<rapidjson::SizeType>(pathname.size()), m_document.GetAllocator());

	m_document.AddMember(projectpath,projectInfo , m_document.GetAllocator());
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
