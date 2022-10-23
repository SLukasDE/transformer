#ifndef TRANSFORMER_MODEL_VERSIONRANGES_H_
#define TRANSFORMER_MODEL_VERSIONRANGES_H_

#include <transformer/model/VersionRange.h>
#include <vector>

namespace transformer {

namespace repository {
class DescriptorReader;
}

namespace model {

class VersionRanges {
	friend class repository::DescriptorReader;
public:
	VersionRanges();
	~VersionRanges();

	bool isEmpty() const;
	void addVersionRange(VersionRange versionRange);

	bool hasVersion(const Version& version) const;
	bool hasCommonVersion(const std::string& commonVersion) const;
	bool hasArtefactVersion(const std::string& artefactVersion) const;
	bool hasApiVersion(const std::string& apiVersion) const;

	const std::vector<VersionRange>& getRanges() const;

private:
	std::vector<VersionRange> ranges;
};

} /* namespace model */
} /* namespace transformer */

#endif /* TRANSFORMER_MODEL_VERSIONRANGES_H_ */
