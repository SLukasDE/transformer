#include <transformer/config/build/Variant.h>

#include <transformer/config/Exception.h>

namespace transformer {
namespace config {
namespace build {

Variant::Variant(std::string aName, bool aIsPublic)
: name(std::move(aName)),
  isPublicVariant(aIsPublic)
{ }

const std::string& Variant::getName() const {
	return name;
}

bool Variant::isPublic() const {
	return isPublicVariant;
}

void Variant::addVersion(const std::string& version) {
	versions.insert(version);
}

const std::set<std::string>& Variant::getVersions() const {
	return versions;
}

void Variant::addArchitecture(const Architecture& architecture) {
	architectures.push_back(architecture);
}

void Variant::addSourceDirectory(const std::string& sourceDirectory) {
	sourceDirectories.insert(sourceDirectory);
}

void Variant::addFilePattern(const Parser& parser, const std::string& fileType, const std::string& filePattern) {
	auto result = filePatterns.insert(std::make_pair(filePattern, fileType));

	if(result.second == false && result.first->second != fileType) {
		throw Exception(parser, "adding file pattern \"" + filePattern + "\" to file type \"" + fileType + "\" failed because it is already reserved for file type \"" + result.first->second + "\"");
	}
}

void Variant::setProvideSource() {
	provideSource = true;
}

void Variant::setProvideStatic() {
	provideStatic = true;
}

void Variant::setProvideDynamic() {
	provideDynamic = true;
}

void Variant::setProvideExecutable() {
	provideExecutable = true;
}

void Variant::addDependency(const Parser& parser, const Dependency& dependency) {
	auto result = dependencies.insert(std::make_pair(dependency.name, dependency));

	if(result.second == false) {
		throw Exception(parser, "adding dependency \"" + dependency.name + "\" failed because it exists already");
	}
}

void Variant::addDefine(const Parser& parser, const Define& define) {
	auto result = defines.insert(std::make_pair(define.key, define));

	if(result.second == false) {
		throw Exception(parser, "adding define \"" + define.define + "\" failed because \"" + define.key + "\" exists already");
	}
}

} /* namespace build */
} /* namespace config */
} /* namespace transformer */
