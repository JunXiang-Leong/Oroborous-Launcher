#pragma once
#include <vector>
#include <filesystem>
class ProjectTracker
{
public:
	ProjectTracker();
	void Show();
private:
	void Tracker();
	void Actions();

private://actions
	void CreateProject(std::filesystem::path p);
	bool FileDialogue();
private:
	std::vector<std::filesystem::path> m_project_directories;
	std::string m_ProjectFileName = "Project";
};
