#ifndef TRANSFORMER_BUILD_GCC_BUILDBASE_H_
#define TRANSFORMER_BUILD_GCC_BUILDBASE_H_

#include <filesystem>
#include <string>
#include <map>
#include <set>
#include <vector>

#include <transformer/build/BuildBase.h>
//#include <transformer/build/gcc/Builder.h>
#include <transformer/files/Source.h>

namespace transformer {
namespace build {
namespace gcc {

class Builder;

class BuildBase : public build::BuildBase {
public:
	BuildBase(const Builder& builder, const files::Source& targetSource);
	~BuildBase();

	const Builder& getBuilder() const;

	void addIncludeFilePattern(const files::Source& source, const std::string& filePattern);
	const std::set<std::string>& getIncludeFilePatterns(const files::Source& source) const;

	void addExcludeFilePattern(const files::Source& source, const std::string& filePattern);
	const std::set<std::string>& getExcludeFilePatterns(const files::Source& source) const;

	virtual void addSource(const files::Source& source);
	const std::vector<std::reference_wrapper<const files::Source>>& getSources() const;

	const Builder& getGccBuilder() const;

private:
	const Builder& builder;

	mutable std::map<std::filesystem::path, std::set<std::string>> includeFilePatterns;
	mutable std::map<std::filesystem::path, std::set<std::string>> excludeFilePatterns;
	std::vector<std::reference_wrapper<const files::Source>> sources;
};

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_GCC_BUILDBASE_H_ */
