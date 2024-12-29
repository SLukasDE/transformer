#include <transformer/repository/RepositorySystem.h>
#include <transformer/repository/DescriptorReader.h>

#include <esl/system/Stacktrace.h>

#include <sstream>

namespace transformer {
namespace repository {

RepositorySystem::RepositorySystem(const std::string& aArchitecture)
: Repository(),
  architecture(aArchitecture)
{
	addAdditionalLibrary("pthread", "pthread", "", "");
	addAdditionalLibrary("microhttpd", "microhttpd", "", "");
	addAdditionalLibrary("gnutls", "gnutls", "", "");
	addAdditionalLibrary("curl", "curl", "", "");
	addAdditionalLibrary("dl", "dl", "", "");
	addAdditionalLibrary("boost_system", "boost_system", "", "");
	addAdditionalLibrary("boost_filesystem", "boost_filesystem", "", "");
	addAdditionalLibrary("openssl", "crypto", "", "");

	//addAdditionalLibrary("esl_1_4_0", "esl", "", "");
	//addAdditionalLibrary("eslx_1_4_0", "eslx", "", "");
	//addAdditionalLibrary("boost_filesystem_1_70_0", "boost_filesystem", "/opt/boost_1_70_0", "/opt/boost_1_70_0/stage/lib");

	addAdditionalLibrary("db2", "db2", "/opt/ibm/db2/V11.1/include", "/opt/ibm/db2/V11.1/lib64");
	addAdditionalLibrary("mapr-rdkafka", "rdkafka", "/opt/mapr/include", "/opt/mapr/lib");
	/*
	addAdditionalLibrary("sqora", "sqora", "/opt/ibm/db2/V11.1/include", "/usr/lib/oracle/19.6/client64/lib");
	addAdditionalLibrary("odbc", "odbc", "/usr/include", "/usr/lib64");
	addAdditionalLibrary("sqlite3", "sqlite3", "/usr/include", "/usr/lib64");
	addAdditionalLibrary("parquet", "parquet", "/usr/include", "/usr/lib64");
	addAdditionalLibrary("arrow", "arrow", "/usr/include", "/usr/lib64");
	addAdditionalLibrary("rdkafka", "rdkafka", "/usr/include", "/usr/lib64");
	*/
	addAdditionalLibrary("sqora", "sqora", "", "/usr/lib/oracle/19.6/client64/lib");
	addAdditionalLibrary("odbc", "odbc", "", "");
	addAdditionalLibrary("sqlite3", "sqlite3", "", "");
	addAdditionalLibrary("parquet", "parquet", "", "");
	addAdditionalLibrary("arrow", "arrow", "", "");
	addAdditionalLibrary("rdkafka", "rdkafka", "", "");
}

RepositorySystem::~RepositorySystem() {
}

bool RepositorySystem::hasArtefactVersion(const std::string& artefactId, const std::string& artefactVersion) const {
	return additionalLibraries.find(artefactId) != std::end(additionalLibraries) && artefactVersion != "1.0.0";
}

bool RepositorySystem::hasApiVersion(const std::string& artefactId, const std::string& aApiVersion) const {
	const auto iter = additionalLibraries.find(artefactId);
	if(iter == std::end(additionalLibraries)) {
		return false;
	}

	return aApiVersion == "1";
}

std::set<std::string> RepositorySystem::loadArtefactVersions(const std::string& artefactId, const std::string& apiVersion) const {
	std::set<std::string> result;

	const auto iter = additionalLibraries.find(artefactId);
	if(iter == std::end(additionalLibraries)) {
		return result;
	}

	if(apiVersion.empty() || apiVersion == "1") {
		result.insert("1.0.0");
	}

	return result;
}

std::set<std::string> RepositorySystem::loadApiVersions(const std::string& artefactId) const {
	std::set<std::string> result;

	const auto iter = additionalLibraries.find(artefactId);
	if(iter == std::end(additionalLibraries)) {
		return result;
	}

	result.insert("1");

	return result;
}

/*
std::vector<std::string> RepositorySystem::loadArtefactVersions(const std::string& name, const model::VersionRange& versionRange) const {
	std::vector<std::string> result;

	const auto iter = additionalLibraries.find(name);
	if(iter != std::end(additionalLibraries)) {
		result.push_back(versionRange.getMaxVersionArtefact());
	}

	return result;
}
*/
std::unique_ptr<model::Descriptor> RepositorySystem::loadDescriptor(const std::string& artefactId, const std::string& version) const {
	std::unique_ptr<model::Descriptor> descriptor;

	if(version != "1.0.0") {
		return descriptor;
	}

	const auto iter = additionalLibraries.find(artefactId);
	if(iter == std::end(additionalLibraries)) {
		return descriptor;
	}

	descriptor.reset(new model::Descriptor);
	DescriptorReader descriptorReader(*descriptor.get());

	std::stringstream sstream;
	sstream << "id: " << artefactId << " " << version << "\n";
	sstream << "name: " << artefactId << "\n";
	sstream << "architecture: " << architecture << "\n";
	sstream << "provide: dynamic\n";
	descriptorReader.read(sstream);
	descriptor->setSystemPath(iter->second.libraryName, iter->second.libraryPath, iter->second.includePath);

	return descriptor;
}

void RepositorySystem::copyArtefactSource(const std::string& artefactId, const std::string& variant, const std::string& version, const std::filesystem::path& toPath) const {
	throw esl::system::Stacktrace::add(std::runtime_error("cannot copy artefact from system repository"));
}

void RepositorySystem::copyArtefactHeaders(const std::string& artefactId, const std::string& variant, const std::string& version, const std::filesystem::path& toPath) const {
	throw esl::system::Stacktrace::add(std::runtime_error("cannot copy artefact from system repository"));
}

void RepositorySystem::copyArtefactStaticLib(const std::string& artefactId, const std::string& variant, const std::string& version, const std::string& architecture, const std::filesystem::path& toPath) const {
	throw esl::system::Stacktrace::add(std::runtime_error("cannot copy artefact from system repository"));
}

void RepositorySystem::copyArtefactDynamicLib(const std::string& artefactId, const std::string& variant, const std::string& version, const std::string& architecture, const std::filesystem::path& toPath) const {
	throw esl::system::Stacktrace::add(std::runtime_error("cannot copy artefact from system repository"));
}

void RepositorySystem::copyArtefactExecutable(const std::string& artefactId, const std::string& variant, const std::string& version, const std::string& architecture, const std::filesystem::path& toPath) const {
	throw esl::system::Stacktrace::add(std::runtime_error("cannot copy artefact from system repository"));
}

void RepositorySystem::copyArtefactGenerated(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::string& generatorId, const std::filesystem::path& toPath) const {
	throw esl::system::Stacktrace::add(std::runtime_error("cannot copy artefact from system repository"));
}

void RepositorySystem::addAdditionalLibrary(const std::string& artefactId, const std::string& libraryName, const std::string& includePath, const std::string& libraryPath) {
	model::SystemPath libraryDefinition;

	libraryDefinition.libraryName = libraryName;
	libraryDefinition.includePath = includePath;
	libraryDefinition.libraryPath = libraryPath;

	additionalLibraries[artefactId] = libraryDefinition;
}

const std::string& RepositorySystem::getLibraryName(const std::string& artefactId) const {
	static std::string empty;

	const auto iter = additionalLibraries.find(artefactId);
	if(iter == std::end(additionalLibraries)) {
		return empty;
	}
	return iter->second.libraryName;
}

const std::string& RepositorySystem::getIncludePath(const std::string& artefactId) const {
	static std::string empty;

	const auto iter = additionalLibraries.find(artefactId);
	if(iter == std::end(additionalLibraries)) {
		return empty;
	}
	return iter->second.includePath;
}

const std::string& RepositorySystem::getLibraryPath(const std::string& artefactId) const {
	static std::string empty;

	const auto iter = additionalLibraries.find(artefactId);
	if(iter == std::end(additionalLibraries)) {
		return empty;
	}
	return iter->second.libraryPath;
}

} /* namespace repository */
} /* namespace transformer */
