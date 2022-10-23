#ifndef TRANSFORMER_MODEL_VERSIONRANGE_H_
#define TRANSFORMER_MODEL_VERSIONRANGE_H_

#include <transformer/model/Version.h>
#include <string>
#include <vector>

namespace transformer {
namespace model {

class VersionRange {
public:
	VersionRange();
	~VersionRange();

	bool isEmpty() const;

	const std::string& getMinVersionArtefact() const;
	const std::string& getMaxVersionArtefact() const;
	const std::string& getMinVersionApi() const;
	const std::string& getMaxVersionApi() const;

	bool hasVersion(const Version& version) const;
	bool hasCommonVersion(const std::string& commonVersion) const;
//	bool hasVersion(const std::string& artefactVersion, const std::string& apiVersion) const;
	bool hasArtefactVersion(const std::string& artefactVersion) const;
	bool hasApiVersion(const std::string& apiVersion) const;

	bool hasCommonVersion() const;
	bool hasCommonArtefactVersion() const;
	bool hasCommonApiVersion() const;
	bool hasArtefactVersion() const;
	bool hasApiVersion() const;

	void setCommonVersion(const std::string& commonVersion);
	void setArtefactVersion(const std::string& artefactVersion);
	void setArtefactVersion(const std::string& minArtefactVersion, const std::string& maxArtefactVersion);
	void setApiVersion(const std::string& versionApi);
	void setApiVersion(const std::string& minVersionApi, const std::string& maxVersionApi);

private:
	std::string versionBoth; // if NOT empty ignore versionArtefact and versionApi -> assume this value for both of them
	std::string versionArtefact;
	std::string versionApi;
	std::string versionArtefactMax;
	std::string versionApiMax;
};

} /* namespace model */
} /* namespace transformer */

#endif /* TRANSFORMER_MODEL_VERSIONRANGE_H_ */
