#include <transformer/Config.h>
#include <transformer/Execute.h>
#include <transformer/files/FileEntries.h>
#include <transformer/files/FileEntry.h>
#include <transformer/repository/RepositoryLocal.h>
#include <transformer/repository/DescriptorReader.h>
#include <transformer/repository/DescriptorWriter.h>
#include <transformer/build/Sources.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace transformer {
namespace repository {

namespace {

std::string extractApiVersionFromFilename(const std::string& filename) {
	return "";
}

} /* anonymous namespace */

/*
RepositoryLocal::RepositoryLocal(std::filesystem::path path)
: Repository()
//  source(path)
{
	source.setPath(path);
}

RepositoryLocal::~RepositoryLocal() {
}
*/

void RepositoryLocal::setPath(const std::filesystem::path path) {
	source.setPath(path);
}

bool RepositoryLocal::hasArtefactVersion(const std::string& artefactId, const std::string& artefactVersion) const {
	std::filesystem::path path = getBasePath(artefactId, artefactVersion);
	path /= "tbuild.cfg";

	return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

bool RepositoryLocal::hasApiVersion(const std::string& artefactId, const std::string& aApiVersion) const {
	for(const files::FileEntry& fileEntryName : source.getFileEntries().getList()) {
		if(!fileEntryName.isDirectory || fileEntryName.name.generic_string() != artefactId) {
			continue;
		}

		for(const files::FileEntry& fileEntryVersion : fileEntryName.entries.getList()) {
			if(fileEntryVersion.isDirectory) {
				continue;
			}

			std::string apiVersion = extractApiVersionFromFilename(fileEntryVersion.name.generic_string());
			if(apiVersion.empty() || apiVersion != aApiVersion) {
				continue;
			}

			return true;
		}
	}

	return false;
}

std::set<std::string> RepositoryLocal::loadArtefactVersions(const std::string& artefactId, const std::string& aApiVersion) const {
	std::set<std::string> result;

	if(aApiVersion.empty()) {
		return loadAllArtefactVersions(artefactId);
	}

	for(const files::FileEntry& fileEntryName : source.getFileEntries().getList()) {
		if(!fileEntryName.isDirectory || fileEntryName.name.generic_string() != artefactId) {
			continue;
		}

		for(const files::FileEntry& fileEntryVersion : fileEntryName.entries.getList()) {
			if(fileEntryVersion.isDirectory) {
				continue;
			}

			std::string apiVersion = extractApiVersionFromFilename(fileEntryVersion.name.generic_string());
			if(apiVersion.empty() || apiVersion != aApiVersion) {
				continue;
			}

			std::filesystem::path path = source.getPath();
			path /= artefactId;
			path /= fileEntryVersion.name;

			std::ifstream istream(path.string());
			std::string line;
			while(std::getline(istream, line))  {
				if(line.empty()) {
					continue;
				}
				result.insert(line);
			}
		}
		break;
	}

	return result;
}

std::set<std::string> RepositoryLocal::loadApiVersions(const std::string& artefactId) const {
	std::set<std::string> result;

	for(const files::FileEntry& fileEntryName : source.getFileEntries().getList()) {
		if(!fileEntryName.isDirectory || fileEntryName.name.generic_string() != artefactId) {
			continue;
		}

		for(const files::FileEntry& fileEntryVersion : fileEntryName.entries.getList()) {
			if(fileEntryVersion.isDirectory) {
				continue;
			}

			std::string apiVersion = extractApiVersionFromFilename(fileEntryVersion.name.generic_string());
			if(apiVersion.empty()) {
				continue;
			}
			result.insert(apiVersion);
		}
	}

	return result;
}

std::unique_ptr<model::Descriptor> RepositoryLocal::loadDescriptor(const std::string& artefactId, const std::string& artefactVersion) const {
	std::unique_ptr<model::Descriptor> descriptor;
	std::filesystem::path path = getBasePath(artefactId, artefactVersion);
	path /= "tbuild.cfg";

	if(std::filesystem::exists(path) && std::filesystem::is_regular_file(path)) {
		descriptor.reset(new model::Descriptor);
		DescriptorReader descriptorReader(*descriptor.get());
		descriptorReader.read(path);
	}

	return descriptor;
}

/* ******************************* *
 * Save descriptor form model data *
 * ******************************* */

void RepositoryLocal::uploadDescriptor(const model::Descriptor& descriptor) const {
	std::filesystem::path path = getBasePath(descriptor.getArtefactId(), descriptor.getArtefactVersion());

	Execute::mkdir(path);
	path /= "tbuild.cfg";
	Execute::remove(path);

	std::ofstream ostream(path.string());
	DescriptorWriter descriptorWriter(descriptor);
	descriptorWriter.write(ostream);
	ostream.close();

	source.invalidateFileEntries();
}

/* *************************************** *
 * Save Artefact-Elements from file system *
 * *************************************** */

void RepositoryLocal::uploadArtefactSource(const std::filesystem::path& fromPath, const model::Descriptor& descriptor, const std::string& variantName) const {
	std::filesystem::path toPath = getBasePath(descriptor.getArtefactId(), descriptor.getArtefactVersion(), variantName);

	Execute::mkdir(toPath);
	toPath /= ("source.tgz");
	Execute::tar(toPath, fromPath);

	source.invalidateFileEntries();
}

void RepositoryLocal::uploadArtefactHeaders(const std::filesystem::path& fromPath, const model::Descriptor& descriptor, const std::string& variantName) const {
	std::filesystem::path toPath = getBasePath(descriptor.getArtefactId(), descriptor.getArtefactVersion(), variantName);

	Execute::mkdir(toPath);
	toPath /= "headers.tgz";
	Execute::tar(toPath, fromPath);

	source.invalidateFileEntries();
}

void RepositoryLocal::uploadArtefactStaticLib(const std::filesystem::path& fromPath, const model::Descriptor& descriptor, const std::string& variantName, const std::string& architecture) const {
	std::filesystem::path toPath = getBasePath(descriptor.getArtefactId(), descriptor.getArtefactVersion(), variantName, architecture);

	Execute::mkdir(toPath);
	toPath /= "static.tgz";
	Execute::tar(toPath, fromPath);

	source.invalidateFileEntries();
}

void RepositoryLocal::uploadArtefactDynamicLib(const std::filesystem::path& fromPath, const model::Descriptor& descriptor, const std::string& variantName, const std::string& architecture) const {
	std::filesystem::path toPath = getBasePath(descriptor.getArtefactId(), descriptor.getArtefactVersion(), variantName, architecture);

	Execute::mkdir(toPath);
	toPath /= "dynamic.tgz";
	Execute::tar(toPath, fromPath);

	source.invalidateFileEntries();
}

void RepositoryLocal::uploadArtefactExecutable(const std::filesystem::path& fromPath, const model::Descriptor& descriptor, const std::string& variantName, const std::string& architecture) const {
	std::filesystem::path toPath = getBasePath(descriptor.getArtefactId(), descriptor.getArtefactVersion(), variantName, architecture);

	Execute::mkdir(toPath);
	toPath /= "executable.tgz";
	Execute::tar(toPath, fromPath);

	source.invalidateFileEntries();
}

void RepositoryLocal::uploadArtefactGenerated(const std::filesystem::path& fromPath, const model::Descriptor& descriptor, const std::string& variantName, const std::string& architecture, const std::string& generatorId) const {
	std::filesystem::path toPath = getBasePath(descriptor.getArtefactId(), descriptor.getArtefactVersion(), variantName, architecture);

	Execute::mkdir(toPath);
	toPath /= "generated";
	toPath /= generatorId;
	toPath /= "generated.tgz";
	Execute::tar(toPath, fromPath);

	source.invalidateFileEntries();
}

/* ******************************************** *
 * Save Artefact-Elements from other repository *
 * ******************************************** */

void RepositoryLocal::saveArtefactSource(const Repository& fromRepository, const std::string& artefactId, const std::string& artefactVersion, const std::string& variant) const {
	std::filesystem::path toPath = getBasePath(artefactId, artefactVersion, variant);
	Execute::mkdir(toPath);

	fromRepository.copyArtefactSource(artefactId, artefactVersion, variant, toPath);

	source.invalidateFileEntries();
}

void RepositoryLocal::saveArtefactHeaders(const Repository& fromRepository, const std::string& artefactId, const std::string& artefactVersion, const std::string& variant) const {
	std::filesystem::path toPath = getBasePath(artefactId, artefactVersion, variant);
	Execute::mkdir(toPath);

	fromRepository.copyArtefactHeaders(artefactId, artefactVersion, variant, toPath);

	source.invalidateFileEntries();
}

void RepositoryLocal::saveArtefactStaticLib(const Repository& fromRepository, const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture) const {
	std::filesystem::path toPath = getBasePath(artefactId, artefactVersion, variant, architecture);
	Execute::mkdir(toPath);

	fromRepository.copyArtefactStaticLib(artefactId, artefactVersion, variant, architecture, toPath);

	source.invalidateFileEntries();
}

void RepositoryLocal::saveArtefactDynamicLib(const Repository& fromRepository, const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture) const {
	std::filesystem::path toPath = getBasePath(artefactId, artefactVersion, variant, architecture);
	Execute::mkdir(toPath);

	fromRepository.copyArtefactDynamicLib(artefactId, artefactVersion, variant, architecture, toPath);

	source.invalidateFileEntries();
}

void RepositoryLocal::saveArtefactExecutable(const Repository& fromRepository, const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture) const {
	std::filesystem::path toPath = getBasePath(artefactId, artefactVersion, variant, architecture);
	Execute::mkdir(toPath);

	fromRepository.copyArtefactExecutable(artefactId, artefactVersion, variant, architecture, toPath);

	source.invalidateFileEntries();
}

void RepositoryLocal::saveArtefactGenerated(const Repository& fromRepository, const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::string& generatorId) const {
	std::filesystem::path toPath = getBasePath(artefactId, artefactVersion, variant, architecture);
	toPath /= "generated";
	toPath /= generatorId;
	Execute::mkdir(toPath);

	fromRepository.copyArtefactGenerated(artefactId, artefactVersion, variant, architecture, generatorId, toPath);

	source.invalidateFileEntries();
}

/* ************************************* *
 * Copy Artefact-Elements to file system *
 * ************************************* */

void RepositoryLocal::copyArtefactSource(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::filesystem::path& toPath) const {
	std::filesystem::path fromPath = getBasePath(artefactId, artefactVersion, variant, true);
	fromPath /= "source.tgz";
	Execute::untar(fromPath, toPath);
}

void RepositoryLocal::copyArtefactHeaders(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::filesystem::path& toPath) const {
	std::filesystem::path fromPath = getBasePath(artefactId, artefactVersion, variant, true);
	fromPath /= "headers.tgz";
	Execute::untar(fromPath, toPath);
}

void RepositoryLocal::copyArtefactStaticLib(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::filesystem::path& toPath) const {
	std::filesystem::path fromPath = getBasePath(artefactId, artefactVersion, variant, architecture);
	fromPath /= "static.tgz";
	Execute::untar(fromPath, toPath);
}

void RepositoryLocal::copyArtefactDynamicLib(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::filesystem::path& toPath) const {
	std::filesystem::path fromPath = getBasePath(artefactId, artefactVersion, variant, architecture);
	fromPath /= "dynamic.tgz";
	Execute::untar(fromPath, toPath);
}

void RepositoryLocal::copyArtefactExecutable(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::filesystem::path& toPath) const {
	std::filesystem::path fromPath = getBasePath(artefactId, artefactVersion, variant, architecture);
	fromPath /= "executable.tgz";
	Execute::untar(fromPath, toPath);
}

void RepositoryLocal::copyArtefactGenerated(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::string& generatorId, const std::filesystem::path& toPath) const {
	std::filesystem::path fromPath = getBasePath(artefactId, artefactVersion, variant, architecture);
	fromPath /= "generated";
	fromPath /= generatorId;
	fromPath /= "generated.tgz";
	Execute::untar(fromPath, toPath);
}


std::set<std::string> RepositoryLocal::loadAllArtefactVersions(const std::string& artefactId) const {
	std::set<std::string> result;

	for(const files::FileEntry& fileEntryName : source.getFileEntries().getList()) {
		if(!fileEntryName.isDirectory || fileEntryName.name.generic_string() != artefactId) {
			continue;
		}

		for(const files::FileEntry& fileEntryVersion : fileEntryName.entries.getList()) {
			if(!fileEntryVersion.isDirectory) {
				continue;
			}

			result.insert(fileEntryVersion.name.generic_string());
		}
		break;
	}

	return result;
}

std::filesystem::path RepositoryLocal::getBasePath(const std::string& artefactId, const std::string& artefactVersion) const {
	std::filesystem::path path = source.getPath();

	path /= artefactId;
	path /= artefactVersion;

	return path;
}

std::filesystem::path RepositoryLocal::getBasePath(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, bool noArchitecture) const {
	std::filesystem::path path = getBasePath(artefactId, artefactVersion);

	if(variant.empty()) {
		path /= "default";
	}
	else {
		path /= "variants";
		path /= variant;
	}
	if(noArchitecture) {
		path /= "common";
	}

	return path;
}

std::filesystem::path RepositoryLocal::getBasePath(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture) const {
	std::filesystem::path path = getBasePath(artefactId, artefactVersion, variant, false);

	path /= "architecture";
	path /= architecture;

	return path;
}
/*
std::filesystem::path RepositoryLocal::createTmpDirectory(const model::Descriptor& descriptor) const {
	std::filesystem::path path = descriptor.getBuildDirEffective();
	path /= "tmp";

	if(std::filesystem::exists(path)) {
		std::filesystem::remove_all(path);
	}
	std::filesystem::create_directory(path);

	return path;
}
*/
} /* namespace repository */
} /* namespace transformer */
