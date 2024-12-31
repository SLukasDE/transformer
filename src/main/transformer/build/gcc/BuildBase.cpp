#include <transformer/build/gcc/BuildBase.h>

namespace transformer {
namespace build {
namespace gcc {

BuildBase::BuildBase(const Builder& aBuilder, const files::Source& targetSource)
: build::BuildBase(targetSource),
  builder(aBuilder)
{
}

BuildBase::~BuildBase() {
}

const Builder& BuildBase::getBuilder() const {
	return builder;
}

void BuildBase::addIncludeFilePattern(const files::Source& source, const std::string& filePattern) {
	includeFilePatterns[source.getPath()].insert(filePattern);
}

const std::set<std::string>& BuildBase::getIncludeFilePatterns(const files::Source& source) const {
	return includeFilePatterns[source.getPath()];
}

void BuildBase::addExcludeFilePattern(const files::Source& source, const std::string& filePattern) {
	excludeFilePatterns[source.getPath()].insert(filePattern);
}

const std::set<std::string>& BuildBase::getExcludeFilePatterns(const files::Source& source) const {
	return excludeFilePatterns[source.getPath()];
}

void BuildBase::addSource(const files::Source& source) {
	sources.emplace_back(source);
}

const std::vector<std::reference_wrapper<const files::Source>>& BuildBase::getSources() const {
	return sources;
}

const Builder& BuildBase::getGccBuilder() const {
	return builder;
}

} /* namespace gcc */
} /* namespace build */
} /* namespace transformer */
