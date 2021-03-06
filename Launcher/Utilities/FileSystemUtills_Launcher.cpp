#include "FileSystemUtills_Launcher.h"

#include <fstream>//CreateItem()
std::vector<std::filesystem::path> FileSystemUtils_Launcher::ListOfFilesInDirectory(const std::string& path)
{
	std::vector<std::filesystem::path> files;

	auto iter = std::filesystem::directory_iterator(path);
	for (auto& item : iter)
	{
		files.emplace_back(item.path());
	}
	return files;
}
std::filesystem::path FileSystemUtils_Launcher::CreateItem(const std::string& filename, const std::string& ext)
{
	std::string current = filename + ext;
	int count = 0;
	while (std::filesystem::exists(current))
	{
		++count;
		current = filename + "(" + std::to_string(count) + ")" + ext;
	}
	if (ext.empty())
		std::filesystem::create_directory(current);
	else
	{
		std::fstream ifs;
		ifs.open(current, std::fstream::out);
		if (!ifs)
		{
			//ENGINE_ASSERT_MSG(false, "Failed to create file");
		}
		ifs.close();
	}
	return current;
}

bool FileSystemUtils_Launcher::DeleteItem(const std::string& filename)
{
	std::error_code ec;
	std::filesystem::remove_all(filename, ec);
	if (ec.value() != std::error_code().value())//if not default value == error found
	{
		//LOG errors
		//LOG_ENGINE_CRITICAL("Error: {0} , Category: {1}", ec.message(), ec.category().name());
		return false;
	}
	return true;
}

std::string FileSystemUtils_Launcher::DuplicateItem(const std::filesystem::path& filename, const std::filesystem::path & targetfolder, const std::string& name_file)
{
	if (std::filesystem::exists(filename) == false)
		return "";
	std::string parent_path = targetfolder.string();
	std::string name = name_file.empty() ? filename.filename().string(): name_file;
	std::string ext = filename.extension().string();
	int counter = 1;

	std::string copytarget = parent_path + "/" + name + ext;
	while (std::filesystem::exists(copytarget))
	{
		++counter;
		copytarget = parent_path+ "/" + name + " (" + std::to_string(counter) + ")" + ext;
	}
	//if no exist
	if (std::filesystem::is_directory(filename))
	{
		const auto copyOptions = std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive;
		std::filesystem::copy(filename, copytarget,copyOptions);
	}
	else
	{
		std::filesystem::copy(filename, copytarget);
	}
	return copytarget;
}

size_t FileSystemUtils_Launcher::CountFiles_Recursively(const std::filesystem::path& filename)
{
	size_t counter = 0;
	for (auto& file : std::filesystem::recursive_directory_iterator(filename))
	{
		++counter;
	}
	return counter;
}

size_t FileSystemUtils_Launcher::CountFiles_NonRecursive(const std::filesystem::path& filename)
{
	size_t counter = 0;
	for (auto& file : std::filesystem::directory_iterator(filename))
	{
		++counter;
	}
	return counter;
}
