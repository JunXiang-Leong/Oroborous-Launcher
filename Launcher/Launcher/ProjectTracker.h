#pragma once
#include <vector>
#include <map>
#include <filesystem>
#include <functional>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#define DATA_FILE_PATH "\\Data"
#define DATA_FILE_PROJECT_PATH "\\Data\\Projects.json"
#define DATA_FILE_PATH_TEMPLATE "\\Data\\Template"
#define DATA_FILE_CONFIGFILE_NAME "Config"
class ProjectTracker
{
public:
	ProjectTracker(std::function <void(std::filesystem::path&)> selectProj);
	~ProjectTracker();
	void Show();

	void CreateProject(const std::filesystem::path& p);
	void AddProject(const std::filesystem::path& p);
private:
	void Tracker();
	void Actions();

private://actions
	void RegisterItem(const std::filesystem::path& p);
	bool FileDialogue();
	bool FileDialogue_Generic(const wchar_t* description, const wchar_t* extension, std::function<void(const std::filesystem::path&)> callback = 0);

private:// non file related
	std::string m_search = "";
	std::function <void(std::filesystem::path&)> m_StartProject;
private:
	struct ProjectFileInfo
	{
		std::string dateAdded;
		std::string lastOpened;
		std::string version;
	};
	std::map<std::filesystem::path, ProjectFileInfo> m_project_directories;
	std::filesystem::path m_exepath;
	std::string m_ProjectFileName = "Project";

	rapidjson::Document m_document;
};
