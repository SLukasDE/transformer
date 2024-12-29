#ifndef TRANSFORMER_REPOSITORY_REPOSITORY_H_
#define TRANSFORMER_REPOSITORY_REPOSITORY_H_

#include <filesystem>
#include <istream>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <transformer/model/Descriptor.h>
#include <transformer/model/VersionRange.h>
#include <transformer/model/VersionNumber.h>

namespace transformer {
namespace repository {


class Repository {
public:
	Repository();
	virtual ~Repository();

	virtual bool hasArtefactVersion(const std::string& name, const std::string& artefactVersion) const = 0;
	virtual bool hasApiVersion(const std::string& name, const std::string& apiVersion) const = 0;

	virtual std::set<std::string> loadArtefactVersions(const std::string& name, const std::string& apiVersion = "") const = 0;
	virtual std::set<std::string> loadApiVersions(const std::string& name) const = 0;

	virtual std::unique_ptr<model::Descriptor> loadDescriptor(const std::string& name, const std::string& version) const = 0;

	virtual void copyArtefactSource(const std::string& artefactId, const std::string& variant, const std::string& version, const std::filesystem::path& toPath) const = 0;
	virtual void copyArtefactHeaders(const std::string& artefactId, const std::string& variant, const std::string& version, const std::filesystem::path& toPath) const = 0;
	virtual void copyArtefactStaticLib(const std::string& artefactId, const std::string& variant, const std::string& version, const std::string& architecture, const std::filesystem::path& toPath) const = 0;
	virtual void copyArtefactDynamicLib(const std::string& artefactId, const std::string& variant, const std::string& version, const std::string& architecture, const std::filesystem::path& toPath) const = 0;
	virtual void copyArtefactExecutable(const std::string& artefactId, const std::string& variant, const std::string& version, const std::string& architecture, const std::filesystem::path& toPath) const = 0;
	virtual void copyArtefactGenerated(const std::string& artefactId, const std::string& artefactVersion, const std::string& variant, const std::string& architecture, const std::string& generatorId, const std::filesystem::path& toPath) const = 0;
};

} /* namespace repository */
} /* namespace transformer */

#endif /* TRANSFORMER_REPOSITORY_REPOSITORY_H_ */
