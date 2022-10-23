#ifndef TRANSFORMER_REPOSITORY_DATABASE_H_
#define TRANSFORMER_REPOSITORY_DATABASE_H_

#include <transformer/model/Descriptor.h>
#include <transformer/model/Variant.h>
#include <transformer/model/VersionRange.h>
#include <transformer/model/VersionRanges.h>
#include <transformer/repository/Repository.h>
#include <transformer/repository/RepositoryLocal.h>
#include <transformer/repository/RepositorySystem.h>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>
//#include <tuple>

namespace transformer {
namespace repository {

// TODO: beim Laden eines Descriptors müssen noch die Base-Discreptoren rekursiv aufgelöst werden und es muss geprüft werden, dass es keinen Zyklus gibt.
class Database {
public:
	Database();
	~Database();

	void setLocalRepositoryPath(const boost::filesystem::path path);
	void addRepository(const std::string& name, Repository& repository);

	std::unique_ptr<model::Descriptor> loadDescriptor(const boost::filesystem::path path);


	const model::Descriptor* getDescriptor(const std::string& artefactId, const std::string& artefactVersion) const;
	const model::Descriptor* getLatestDescriptor(const std::string& artefactId, const model::VersionRange& versionRange, const std::string& variantName) const;

	std::set<std::string> getArtefactVersions(const std::string& artefactId, const model::VersionRange& versionRange) const;
	std::set<std::string> getArtefactVersions(const std::string& artefactId, const model::VersionRanges& versionRanges) const;
	std::set<std::string> getArtefactVersions(const std::string& artefactId) const;
	std::set<std::string> getArtefactVersions(const std::string& artefactId, const std::string& apiVersion) const;
	std::set<std::string> getApiVersions(const std::string& artefactId, const std::string& apiVersionMin, const std::string& apiVersionMax) const;

	void copyArtefactSource(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const boost::filesystem::path& toPath);
	void copyArtefactHeaders(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const boost::filesystem::path& toPath);
	void copyArtefactStaticLib(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const boost::filesystem::path& toPath);
	void copyArtefactDynamicLib(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const boost::filesystem::path& toPath);
	void copyArtefactExecutable(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const boost::filesystem::path& toPath);
	void copyArtefactGenerated(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::string& generatorId, const boost::filesystem::path& toPath);

	// install/provide current built artefact with all parts, like static, dynamic lib etc.
//	void saveArtefact(const model::Descriptor& descriptor) const;
	void uploadArtefact(const model::Descriptor& descriptor) const;
	//private:

private:
	/* Repositories */
	RepositoryLocal repositoryLocal;
	RepositorySystem repositorySystem;
	std::vector<std::pair<std::string, Repository&>> repositories;

	// [repositoryName] -> [repository]
	std::map<std::string, std::reference_wrapper<Repository>> repositoriesByName;

	// [artefactName, artefactVersion] -> [repository]
	mutable std::map<std::pair<std::string, std::string>, std::reference_wrapper<Repository>> repositoryByArtefactVersion;

	// [artefactName] -> [repositoryName] -> [<all versions loaded>, {available artefactVersions}]
	mutable std::map<std::string, std::map<std::string, std::pair<bool, std::set<std::string>>>> repositoryNameArtefactVersionsByArtefactId;

	// [artefactName] -> [repositoryName] -> [<all versions loaded>, {available apiVersions}]
	mutable std::map<std::string, std::map<std::string, std::pair<bool, std::set<std::string>>>> repositoryNameApiVersionsByArtefactId;

	/* APIs */
	// [artefactName, apiVersion] -> [<all repository versions loaded>, {available artefactVersions}]
	mutable std::map<std::pair<std::string, std::string>, std::pair<bool, std::set<std::string>>> artefactVersionsByApiVersion;

	/* Desciptors */
	mutable std::map<std::pair<std::string, std::string>, std::unique_ptr<model::Descriptor>> descriptorsByArtefactVersion;


	Repository* getRepositoryByArtefactVersion(const std::string& artefactId, const std::string& artefactVersion) const;
	void makeLocalAvailable(const std::string& artefactId, const std::string& artefactVersion);

	void uploadArtefact(const model::Descriptor& descriptor, const std::string& variantName) const;
	void uploadArtefactSource(const model::Descriptor& descriptor, const std::string& variantName) const;
	void uploadArtefactHeaders(const model::Descriptor& descriptor, const std::string& variantName) const;

	boost::filesystem::path createTmpDirectory(const model::Descriptor& descriptor) const;
};

} /* namespace repository */
} /* namespace transformer */

#endif /* TRANSFORMER_REPOSITORY_DATABASE_H_ */
