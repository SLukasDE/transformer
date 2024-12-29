#include <transformer/model/Descriptor.h>

#include <esl/system/Stacktrace.h>

#include <map>

namespace transformer {
namespace model {

Descriptor::Descriptor()
: variant(*this)
{
}

const std::string& Descriptor::getArtefactId() const {
	return artefactId;
}

const std::string& Descriptor::getArtefactName() const {
	return artefactName;
}

const bool Descriptor::hasCommonVersion() const {
	return commonVersion;
}

const std::string& Descriptor::getArtefactVersion() const {
	return artefactVersion;
}

const std::string& Descriptor::getApiVersion() const {
	if(hasCommonVersion()) {
		return artefactVersion;
	}
	return apiVersion;
}

const std::string& Descriptor::getBaseName() const {
	return baseName;
}

const std::string& Descriptor::getBaseVersion() const {
	return baseArtefactVersion;
}

const Descriptor* Descriptor::getBaseDescriptor() const {
	return baseDescriptor;
}

bool Descriptor::hasBuildDir() const {
	return !buildDir.empty();
}

const std::string& Descriptor::getBuildDir() const {
	return buildDir;
}

std::string Descriptor::getBuildDirEffective() const {
	if(buildDir.empty()) {
		if(getBaseDescriptor()) {
			return getBaseDescriptor()->getBuildDir();
		}
		else {
			return "build";
		}
	}

	return buildDir;
}

std::set<std::string> Descriptor::getAllArchitectures() const {
	std::set<std::string> result;

	for(const auto& architecture: variant.getArchitectures()) {
		result.insert(architecture);
	}
	for(const auto& variant: variants) {
		for(const auto& architecture: variant.second.get()->getArchitectures()) {
			result.insert(architecture);
		}
	}

	return result;
}

bool Descriptor::hasSpecialVariants() const {
	return !variants.empty();
}

const std::string& Descriptor::getDefaultVariantName() const {
	return variantName;
}

std::vector<std::string> Descriptor::getSpecialVariantNames() const {
	std::vector<std::string> result;

	for(const auto& entry : variants) {
		result.push_back(entry.first);
	}
	return result;
}

bool Descriptor::hasSpecialVariant(const std::string& variantName) const {
	return variants.find(variantName) != std::end(variants);
}

void Descriptor::addSpecialVariant(const std::string& variantName) {
	if(hasSpecialVariant(variantName)) {
		throw esl::system::Stacktrace::add(std::runtime_error("variant \"" + variantName + "\" exists already"));
	}
	variants.emplace(variantName, std::unique_ptr<Variant>(new Variant(*this)));
}

const Variant& Descriptor::getVariant(const std::string& aVariantName) const {
	if(aVariantName.empty()) {
		return variant;
	}

	auto iter = variants.find(aVariantName);
	if(iter == std::end(variants)) {
		if(aVariantName == variantName) {
			return variant;
		}
		throw esl::system::Stacktrace::add(std::runtime_error("request for unknown variant \"" + variantName + "\""));
	}

	return *iter->second.get();
}
/*
const std::map<std::string, std::unique_ptr<Variant>>& Descriptor::getSpecialVariants() const {
	return variants;
}
*/
bool Descriptor::isDefaultVariant(const Variant& aVariant) const {
	return &aVariant == &variant;
}

bool Descriptor::hasSolved(const std::string& aVariantName) const {
	return solved.find(aVariantName) != std::end(solved);
}

void Descriptor::addSolved(const std::string& aVariantName) {
	if(hasSolved(aVariantName)) {
		throw esl::system::Stacktrace::add(std::runtime_error("variant \"" + aVariantName + "\" exists already as solved"));
	}
	solved.emplace(aVariantName, std::vector<DependencySolved>());
}

const std::map<std::string, std::vector<DependencySolved>>& Descriptor::getSolved() const {
	return solved;
}

void Descriptor::setSystemPath(std::string libraryName, std::string libraryPath, std::string includePath) {
	SystemPath aSystemPath;

	aSystemPath.libraryName = libraryName;
	aSystemPath.libraryPath = libraryPath;
	aSystemPath.includePath = includePath;

	systemPaths = aSystemPath;
}

const std::optional<SystemPath>& Descriptor::getSystemPath() const {
	return systemPaths;
}

} /* namespace model */
} /* namespace transformer */
