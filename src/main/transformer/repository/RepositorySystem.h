#ifndef TRANSFORMER_REPOSITORY_REPOSITORYSYSTEM_H_
#define TRANSFORMER_REPOSITORY_REPOSITORYSYSTEM_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <transformer/repository/Repository.h>
#include <transformer/model/SystemPath.h>

namespace transformer {
namespace repository {

class RepositorySystem: public Repository {
public:
	RepositorySystem(const std::string& architecture);
	~RepositorySystem();

	bool hasArtefactVersion(const std::string& artefactId, const std::string& artefactVersion) const override;
	bool hasApiVersion(const std::string& artefactId, const std::string& apiVersion) const override;

	std::set<std::string> loadArtefactVersions(const std::string& artefactId, const std::string& apiVersion) const override;
	std::set<std::string> loadApiVersions(const std::string& artefactId) const override;

	std::unique_ptr<model::Descriptor> loadDescriptor(const std::string& artefactId, const std::string& artefactVersion) const override;

	void copyArtefactSource(const std::string& artefactId, const std::string& variant, const std::string& version, const boost::filesystem::path& toPath) const override;
	void copyArtefactHeaders(const std::string& artefactId, const std::string& variant, const std::string& version, const boost::filesystem::path& toPath) const override;
	void copyArtefactStaticLib(const std::string& artefactId, const std::string& variant, const std::string& version, const std::string& architecture, const boost::filesystem::path& toPath) const override;
	void copyArtefactDynamicLib(const std::string& artefactId, const std::string& variant, const std::string& version, const std::string& architecture, const boost::filesystem::path& toPath) const override;
	void copyArtefactExecutable(const std::string& artefactId, const std::string& variant, const std::string& version, const std::string& architecture, const boost::filesystem::path& toPath) const override;
	void copyArtefactGenerated(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::string& generatorId, const boost::filesystem::path& toPath) const override;

	void addAdditionalLibrary(const std::string& artefactId, const std::string& libraryName, const std::string& includePath, const std::string& libraryPath);
	const std::string& getLibraryName(const std::string& artefactId) const;
	const std::string& getIncludePath(const std::string& artefactId) const;
	const std::string& getLibraryPath(const std::string& artefactId) const;

private:
	const std::string architecture;
	std::map<std::string, model::SystemPath> additionalLibraries;
};

} /* namespace repository */
} /* namespace transformer */

#endif /* TRANSFORMER_REPOSITORY_REPOSITORYSYSTEM_H_ */
