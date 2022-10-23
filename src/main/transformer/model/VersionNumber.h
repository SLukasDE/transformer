#ifndef TRANSFORMER_MODEL_VERSIONNUMBER_H_
#define TRANSFORMER_MODEL_VERSIONNUMBER_H_

#include <string>
#include <vector>
#include <tuple>

namespace transformer {
namespace model {

class VersionNumber {
public:
	using Component = std::tuple<bool, std::string, unsigned int>; // [isNumber] , [original string of this component] , [converted to number if isNumber==true or <undefined> if isNumber == false]

	VersionNumber(std::string versionString);
	~VersionNumber();

	const std::string& getString() const;

	bool operator<(const VersionNumber&) const;
	bool operator>(const VersionNumber&) const;
	bool operator<=(const VersionNumber&) const;
	bool operator>=(const VersionNumber&) const;
	bool operator==(const VersionNumber&) const;

	static int cmp(const VersionNumber&, const VersionNumber&);
	static std::vector<std::string> splitVersion(const std::string& version);

private:
	std::string versionString;
	std::vector<Component> components;
};

} /* namespace model */
} /* namespace transformer */

#endif /* TRANSFORMER_MODEL_VERSIONNUMBER_H_ */
