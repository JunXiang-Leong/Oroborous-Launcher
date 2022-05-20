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
	ProjectTracker();
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
private:
	std::map<std::string , std::filesystem::path> m_project_directories;
	std::filesystem::path m_exepath;
	std::string m_ProjectFileName = "Project";

	rapidjson::Document m_document;
};
