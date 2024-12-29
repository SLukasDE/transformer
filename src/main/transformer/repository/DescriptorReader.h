#ifndef TRANSFORMER_REPOSITORY_DESCRIPTORREADER_H_
#define TRANSFORMER_REPOSITORY_DESCRIPTORREADER_H_

#include <transformer/model/Descriptor.h>
#include <transformer/model/Variant.h>
#include <transformer/model/Version.h>
#include <transformer/model/VersionRange.h>

#include <filesystem>
#include <istream>
#include <set>

namespace transformer {
namespace repository {

class DescriptorReader {
	enum class Context { cxMain, cxVariant, cxSolved };
public:
	DescriptorReader(model::Descriptor& descriptor);
	~DescriptorReader();

	void read(const std::filesystem::path path);
	void read(std::istream& istream);

private:
	static bool readIdAndVariantName(std::string& id, std::string& variantName, std::istringstream& iss);
	void readBaseName(std::istringstream& iss);
	static bool readVersion(model::Version& version, std::istringstream& iss);
	static bool readVersionRange(model::VersionRange& versionRange, bool& isEmpty, std::istringstream& iss);

	void readSourcesMainDir(std::istringstream& iss);
	void readSourcesTestDir(std::istringstream& iss);
	void readHeadersDir(std::istringstream& iss);
	void readBuildDir(std::istringstream& iss);
	void readSourceFilePatterns(std::istringstream& iss);
	void readHeaderFilePatterns(std::istringstream& iss);
	void readArchitectures(std::istringstream& iss);
	void readGenerator(std::istringstream& iss);
	void readDefinePrivate(std::istringstream& iss);
	void readDefinePublic(std::istringstream& iss);
	void readProvide(std::istringstream& iss);
	void readDependency(bool allowedStatic, bool allowedDynamic, bool allowedSystem, std::istringstream& iss);
	void readDependencySolved(bool allowedStatic, bool allowedDynamic, bool allowedSystem, std::istringstream& iss);

	void addVariant(const std::string& name);
	void addSolved(const std::string& name);

	model::Variant& getVariant(const std::string& variantName);
	std::vector<model::DependencySolved>& getSolved(const std::string& variantName);

	model::Descriptor& descriptor;
	Context context;
	// tells which variant is read right now. As default it contains only one entry "" (blank) to tell that default variant is active variant right now.
	// if is is swichted to another variant while parsing the descirptor file, it contains the new variant name.
	// it is possible to enumerat multiple variants when switching to variants. That's why it is a set and not a single value
	std::set<std::string> activeVariantNames;
	std::string solvedName;


};

} /* namespace repository */
} /* namespace transformer */

#endif /* TRANSFORMER_REPOSITORY_DESCRIPTORREADER_H_ */
