#ifndef TRANSFORMER_CONFIG_BUILD_VARIANT_H_
#define TRANSFORMER_CONFIG_BUILD_VARIANT_H_

#include <transformer/config/build/Architecture.h>
#include <transformer/config/build/Dependency.h>
#include <transformer/config/build/Define.h>
#include <transformer/config/Parser.h>

#include <string>
#include <map>
#include <set>
#include <vector>

namespace transformer {
namespace config {
namespace build {

class Variant {
public:
	enum FileType {
		ftHeader,
		ftC,
		ftCpp
	};
	Variant(std::string name, bool isPublic);

	const std::string& getName() const;
	bool isPublic() const;

	void addVersion(const std::string& version);
	const std::set<std::string>& getVersions() const;

	void addArchitecture(const Architecture& architecture);

	void addSourceDirectory(const std::string& sourceDirectory);
	void addFilePattern(const Parser& parser, const std::string& fileType, const std::string& filePattern);

	void setProvideSource();
	void setProvideStatic();
	void setProvideDynamic();
	void setProvideExecutable();

	void addDependency(const Parser& parser, const Dependency& dependency);

	void addDefine(const Parser& parser, const Define& define);

private:
	enum RequireType {
		rtStatic, rtDynamic
	};

	std::string name;
	bool isPublicVariant = true;

	std::set<std::string> versions;

	std::vector<Architecture> architectures;

	std::set<std::string> sourceDirectories;

	/* file pattern to file type */
	std::map<std::string, std::string> filePatterns;

	bool provideSource = false;
	bool provideStatic = false;
	bool provideDynamic = false;
	bool provideExecutable = false;

	/* stores required libraries */
	std::map<std::string, Dependency> dependencies;

	/* stores defines */
	std::map<std::string, Define> defines;
};

} /* namespace build */
} /* namespace config */
} /* namespace transformer */

#endif /* TRANSFORMER_CONFIG_BUILD_VARIANT_H_ */
