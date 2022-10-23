#include <transformer/files/FileEntries.h>
#include <transformer/files/FileEntry.h>
#include <transformer/Execute.h>

#include <esl/system/Stacktrace.h>

#include <iostream>

namespace transformer {
namespace files {

FileEntries::FileEntries() {
}

FileEntries::~FileEntries() {
}

void FileEntries::loadDirectory(const boost::filesystem::path& path) {
	entries.clear();
	if(boost::filesystem::exists(path) == false) {
		return;
	}
	if(boost::filesystem::is_directory(path) == false) {
		throw esl::system::Stacktrace::add(std::runtime_error("Path " + path.generic_string() + " exists but it is no directory"));
	}
	boost::filesystem::directory_iterator endIter;
	for( boost::filesystem::directory_iterator iter(path); iter != endIter; ++iter) {
	//for (boost::filesystem::directory_entry& dirEntry : boost::filesystem::directory_iterator(path)) {
		if(boost::filesystem::is_directory(iter->status())) {
		//if (boost::filesystem::is_directory(dirEntry)) {
			FileEntry entry;
			entry.isDirectory = true;

			entry.name = iter->path().filename().generic_string();
			entry.entries.loadDirectory(iter->path());
			//entry.name = dirEntry.path().filename().generic_string();
			//entry.entries.loadDirectory(dirEntry.path());

			entries.push_back(std::move(entry));
			continue;
		}
		if (boost::filesystem::is_regular(iter->status())) {
		//if (boost::filesystem::is_regular(dirEntry)) {
			FileEntry entry;
			entry.isDirectory = false;

			entry.name = iter->path().filename().generic_string();
			//entry.name = dirEntry.path().filename().generic_string();

			entries.push_back(std::move(entry));
			continue;
		}
	}
}

const std::vector<FileEntry>& FileEntries::getList() const {
	return entries;
}

FileEntries& FileEntries::operator+=(const FileEntries& rootEntry2) {
	for(const auto& subEntry2 : rootEntry2.entries) {
		bool found = false;

		for(auto& subEntry1 : entries) {
			if(subEntry2.name == subEntry1.name) {
				subEntry1.entries += subEntry2.entries;
				found = true;
				break;
			}
		}
		if(found == false) {
			entries.push_back(subEntry2);
		}
	}

	return *this;
}

FileEntries FileEntries::operator+(const FileEntries& rootEntry2) const {
	FileEntries result = *this;
	result += rootEntry2;
	return result;
}

FileEntries FileEntries::operator- (const FileEntries& entries2) const {
	FileEntries result;

	for(auto entry1 : entries) {
		bool found = false;

		for(const auto& entry2 : entries2.getList()) {
			if(entry1.name != entry2.name) {
				continue;
			}

			found = true;
			entry1.entries = (entry1.entries - entry2.entries);
		}

		if(found && entry1.entries.entries.empty()) {
			continue;
		}

		result.entries.push_back(std::move(entry1));
	}

	return result;
}

FileEntries FileEntries::makeFlat() const {
	FileEntries result;

	for(const FileEntry& entry : entries) {
		{
			FileEntry newEntry;
			newEntry.name = entry.name;
			newEntry.isDirectory = entry.isDirectory;
			newEntry.lastWriteTime = entry.lastWriteTime;
			result.entries.push_back(newEntry);
		}

		FileEntries subEntries = entry.entries.makeFlat();
		for(FileEntry& subEntry : subEntries.entries) {
			FileEntry newEntry;
			newEntry.name = entry.name;
			newEntry.name /= subEntry.name;
			newEntry.isDirectory = subEntry.isDirectory;
			newEntry.lastWriteTime = subEntry.lastWriteTime;
			result.entries.push_back(newEntry);
		}
	}

	return result;
}

FileEntries FileEntries::makePrefix(const boost::filesystem::path& path) const {
	FileEntry tmpEntry;
	tmpEntry.name = path;
	tmpEntry.isDirectory = true;
	tmpEntry.entries = *this;

	FileEntries result;
	result.entries.push_back(std::move(tmpEntry));
	return result;
}

FileEntries FileEntries::filter(const std::set<std::string>& includePatterns, const std::set<std::string>& excludePatterns) const {
	files::FileEntries entries = filter([includePatterns, excludePatterns](const files::FileEntry& entry) {
		for(const auto& pattern : excludePatterns) {
			if(pattern.size() > 0 && pattern.at(0) == '*') {
				if(entry.name.extension() == pattern.substr(1, pattern.size()-1)) {
					return false;
				}
			}
			else {
				if(entry.name.generic_string() == pattern) {
					return false;
				}
			}
		}

		for(const auto& pattern : includePatterns) {
			if(pattern.size() > 0 && pattern.at(0) == '*') {
				if(entry.name.extension() == pattern.substr(1, pattern.size()-1)) {
					return true;
				}
			}
			else {
				if(entry.name.generic_string() == pattern) {
					return true;
				}
			}
		}

		return false;
	});

	return entries;
}

FileEntries FileEntries::filter(const std::vector<std::string>& includePatterns, const std::vector<std::string>& excludePatterns) const {
	files::FileEntries entries = filter([includePatterns, excludePatterns](const files::FileEntry& entry) {
		for(const auto& pattern : excludePatterns) {
			if(pattern.size() > 0 && pattern.at(0) == '*') {
				if(entry.name.extension() == pattern.substr(1, pattern.size()-1)) {
					return false;
				}
			}
			else {
				if(entry.name.generic_string() == pattern) {
					return false;
				}
			}
		}

		for(const auto& pattern : includePatterns) {
			if(pattern.size() > 0 && pattern.at(0) == '*') {
				if(entry.name.extension() == pattern.substr(1, pattern.size()-1)) {
					return true;
				}
			}
			else {
				if(entry.name.generic_string() == pattern) {
					return true;
				}
			}
		}

		return false;
	});

	return entries;
}
/*
FileEntries FileEntries::filterByExtension(std::string extension) const {
	return filter([extension](const FileEntry& entry) {
		return entry.name.extension() == extension;
	});
}

FileEntries FileEntries::filterByExtensions(const std::vector<boost::filesystem::path>& extensions) const {
	return filter([&extensions](const FileEntry& entry) {
		for(const auto& extension : extensions) {
			if(entry.name.extension() == extension) {
				return true;
			}
		}
		return false;
	});
}
*/
void FileEntries::print(const std::string& prefix) const {
	for(const auto& entry: getList()) {
		if(entry.isDirectory) {
			std::cout << prefix << entry.name.generic_string() << "/\n";
		}
		else {
			std::cout << prefix << entry.name.generic_string() << "\n";
		}
		entry.entries.print(prefix + entry.name.generic_string() + "/");
	}
}

void FileEntries::syncDirectoryStructure(const FileEntries& entriesActual, const FileEntries& entriesTarget, const boost::filesystem::path& path, bool withDelete) {
	FileEntries difference;
	FileEntries entriesActualDirs = entriesActual.filter([](const FileEntry& entry) { return entry.isDirectory; });
	FileEntries entriesTargetDirs = entriesTarget.filter([](const FileEntry& entry) { return entry.isDirectory; });

	if(withDelete) {
		difference = entriesTargetDirs - entriesActualDirs;
		deleteDirectories(path, difference);
	}

	difference = entriesActualDirs - entriesTargetDirs;
	if(boost::filesystem::exists(path) == false) {
		Execute::mkdir(path);
	} else if(boost::filesystem::is_directory(path) == false) {
		throw esl::system::Stacktrace::add(std::runtime_error("Path " + path.generic_string() + " exists but it is no directory"));
	}

	createDirectories(path, difference);
}

void FileEntries::createDirectories(const boost::filesystem::path& path, const FileEntries& entries) {
	for(const auto& entry : entries.getList()) {
		if(!entry.isDirectory) {
			continue;
		}

		boost::filesystem::path tmpPath(path);
		tmpPath /= entry.name;
		Execute::mkdir(tmpPath);
		createDirectories(tmpPath, entry.entries);
	}
}

void FileEntries::deleteDirectories(const boost::filesystem::path& path, const FileEntries& entries) {
	for(const auto& entry : entries.getList()) {
		if(!entry.isDirectory) {
			continue;
		}

		boost::filesystem::path tmpPath(path);
		tmpPath /= entry.name;
		deleteDirectories(tmpPath, entry.entries);
		Execute::removeAll(tmpPath);
	}
}

} /* namespace files */
} /* namespace transformer */
