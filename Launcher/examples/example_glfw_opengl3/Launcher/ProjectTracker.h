#pragma once
#include <vector>
#include <filesystem>
class ProjectTracker
{
public:
	ProjectTracker();
	void Show();
private:
	std::vector<std::filesystem::path> m_project_directories;
};
