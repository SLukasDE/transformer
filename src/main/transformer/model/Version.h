#ifndef TRANSFORMER_MODEL_VERSION_H_
#define TRANSFORMER_MODEL_VERSION_H_

#include <string>
#include <vector>

namespace transformer {
namespace model {

class Version {
	friend class VersionRange;

public:
	Version();
	~Version();

	bool isEmpty() const;

	const std::string& getVersionArtefact() const;
	const std::string& getVersionApi() const;

	bool hasVersion(const Version& version) const;
	bool hasCommonVersion(const std::string& versionBoth) const;
	bool hasArtefactVersion(const std::string& artefactVersion) const;
	bool hasApiVersion(const std::string& apiVersion) const;
//	bool hasVersion(const std::string& versionArtefact, const std::string& versionApi) const;

	bool hasCommonVersion() const;
	bool hasArtefactVersion() const;
	bool hasApiVersion() const;

	void setCommonVersion(const std::string& commonVersion);
	void setArtefactVersion(const std::string& artefactVersion);
	void setApiVersion(const std::string& versionApi);

protected:
	std::string versionBoth; // if NOT empty ignore versionArtefact and versionApi -> assume this value for both of them
	std::string versionArtefact;
	std::string versionApi;
};

} /* namespace model */
} /* namespace transformer */

#endif /* TRANSFORMER_MODEL_VERSION_H_ */
