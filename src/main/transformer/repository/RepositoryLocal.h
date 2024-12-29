#ifndef TRANSFORMER_REPOSITORY_REPOSITORYLOCAL_H_
#define TRANSFORMER_REPOSITORY_REPOSITORYLOCAL_H_

#include <vector>
#include <string>
#include <vector>
#include <set>

#include <transformer/files/Source.h>
#include <transformer/repository/Repository.h>
#include <transformer/model/Descriptor.h>
#include <transformer/model/VersionRange.h>

namespace transformer {
namespace repository {

class RepositoryLocal : public Repository {
public:
//	RepositoryLocal(std::filesystem::path path);
	RepositoryLocal() = default;
	~RepositoryLocal() = default;

	void setPath(const std::filesystem::path path);

	bool hasArtefactVersion(const std::string& artefactId, const std::string& artefactVersion) const override;
	bool hasApiVersion(const std::string& artefactId, const std::string& apiVersion) const override;

	std::set<std::string> loadArtefactVersions(const std::string& artefactId, const std::string& apiVersion) const override;
	std::set<std::string> loadApiVersions(const std::string& artefactId) const override;

	std::unique_ptr<model::Descriptor> loadDescriptor(const std::string& artefactId, const std::string& artefactVersion) const override;

	void uploadDescriptor(const model::Descriptor& descriptor) const;

	void uploadArtefactSource(const std::filesystem::path& fromPath, const model::Descriptor& descriptor, const std::string& variantName) const;
	void uploadArtefactHeaders(const std::filesystem::path& fromPath, const model::Descriptor& descriptor, const std::string& variantName) const;
	void uploadArtefactStaticLib(const std::filesystem::path& fromPath, const model::Descriptor& descriptor, const std::string& variantName, const std::string& architecture) const;
	void uploadArtefactDynamicLib(const std::filesystem::path& fromPath, const model::Descriptor& descriptor, const std::string& variantName, const std::string& architecture) const;
	void uploadArtefactExecutable(const std::filesystem::path& fromPath, const model::Descriptor& descriptor, const std::string& variantName, const std::string& architecture) const;
	void uploadArtefactGenerated(const std::filesystem::path& fromPath, const model::Descriptor& descriptor, const std::string& variantName, const std::string& architecture, const std::string& generatorId) const;

	void saveArtefactSource(const Repository& fromRepository, const std::string& artefactId, const std::string& artefactVersion, const std::string& variant) const;
	void saveArtefactHeaders(const Repository& fromRepository, const std::string& artefactId, const std::string& artefactVersion, const std::string& variant) const;
	void saveArtefactStaticLib(const Repository& fromRepository, const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture) const;
	void saveArtefactDynamicLib(const Repository& fromRepository, const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture) const;
	void saveArtefactExecutable(const Repository& fromRepository, const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture) const;
	void saveArtefactGenerated(const Repository& fromRepository, const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::string& generatorId) const;

	void copyArtefactSource(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::filesystem::path& toPath) const override;
	void copyArtefactHeaders(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::filesystem::path& toPath) const override;
	void copyArtefactStaticLib(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::filesystem::path& toPath) const override;
	void copyArtefactDynamicLib(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::filesystem::path& toPath) const override;
	void copyArtefactExecutable(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::filesystem::path& toPath) const override;
	void copyArtefactGenerated(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::string& generatorId, const std::filesystem::path& toPath) const override;

private:
	files::Source source;

	std::set<std::string> loadAllArtefactVersions(const std::string& artefactId) const;
	std::filesystem::path getBasePath(const std::string& artefactId, const std::string& artefactVersion) const;
	std::filesystem::path getBasePath(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, bool noArchitecture = true) const;
	std::filesystem::path getBasePath(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture) const;

//	std::filesystem::path createTmpDirectory(const model::Descriptor& descriptor) const;
};

} /* namespace repository */
} /* namespace transformer */

#endif /* TRANSFORMER_REPOSITORY_REPOSITORYLOCAL_H_ */
