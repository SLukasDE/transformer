#include <transformer/model/Descriptor.h>
#include <transformer/model/Variant.h>
#include <iostream>

namespace transformer {
namespace model {

namespace {
std::string extractDefineName(const std::string& define) {
	return define.substr(0, define.find('='));
}
}

Variant::Variant(const Descriptor& aDescriptor)
: descriptor(aDescriptor)
{
}

Variant::~Variant() {
}

const Descriptor& Variant::getDescriptor() const {
	return descriptor;
}

bool Variant::hasSourcesMainDir() const {
	return !sourcesMainDir.empty();
}

const std::vector<std::string>& Variant::getSourcesMainDir() const {
	return sourcesMainDir;
}

std::vector<std::string> Variant::getSourcesMainDirEffective() const {
	if(sourcesMainDir.empty()) {
		if(descriptor.isDefaultVariant(*this)) {
			if(descriptor.getBaseDescriptor()) {
				return descriptor.getBaseDescriptor()->getVariant("").getSourcesMainDir();
			}
			else {
				return {"src/main"};
			}
		}
		else {
			return descriptor.getVariant("").getSourcesMainDirEffective();
		}
	}

	return sourcesMainDir;
}

bool Variant::hasSourcesTestDir() const {
	return !sourcesTestDir.empty();
}

const std::vector<std::string>& Variant::getSourcesTestDir() const {
	return sourcesTestDir;
}

std::vector<std::string> Variant::getSourcesTestDirEffective() const {
	if(sourcesTestDir.empty()) {
		if(descriptor.isDefaultVariant(*this)) {
			if(descriptor.getBaseDescriptor()) {
				return descriptor.getBaseDescriptor()->getVariant("").getSourcesTestDir();
			}
			else {
				return {"src/test"};
			}
		}
		else {
			return descriptor.getVariant("").getSourcesTestDirEffective();
		}
	}

	return sourcesTestDir;
}

bool Variant::hasHeadersDir() const {
	return !headersDir.empty();
}

const std::vector<std::string>& Variant::getHeadersDir() const {
	return headersDir;
}

std::vector<std::string> Variant::getHeadersDirEffective() const {
	if(headersDir.empty()) {
		if(descriptor.isDefaultVariant(*this)) {
			if(descriptor.getBaseDescriptor()) {
				return descriptor.getBaseDescriptor()->getVariant("").getHeadersDir();
			}
			else {
				return {"src/main"};
			}
		}
		else {
			return descriptor.getVariant("").getHeadersDirEffective();
		}
	}

	return headersDir;
}

bool Variant::hasSourceFilePatterns() const {
	return !sourceFilePatterns.empty();
}

const std::vector<std::string>& Variant::getSourceFilePatterns() const {
	return sourceFilePatterns;
}

std::vector<std::string> Variant::getSourceFilePatternsEffective() const {
	if(sourceFilePatterns.empty()) {
		if(descriptor.isDefaultVariant(*this)) {
			if(descriptor.getBaseDescriptor()) {
				return descriptor.getBaseDescriptor()->getVariant("").getSourceFilePatterns();
			}
			else {
				return {"*.cpp"};
			}
		}
		else {
			return descriptor.getVariant("").getSourceFilePatternsEffective();
		}
	}

	return sourceFilePatterns;
}

bool Variant::hasHeaderFilePatterns() const {
	return !headerFilePatterns.empty();
}

const std::vector<std::string>& Variant::getHeaderFilePatterns() const {
	return headerFilePatterns;
}

std::vector<std::string> Variant::getHeaderFilePatternsEffective() const {
	if(headerFilePatterns.empty()) {
		if(descriptor.isDefaultVariant(*this)) {
			if(descriptor.getBaseDescriptor()) {
				return descriptor.getBaseDescriptor()->getVariant("").getHeaderFilePatterns();
			}
			else {
				return {"*.h"};
			}
		}
		else {
			return descriptor.getVariant("").getHeaderFilePatternsEffective();
		}
	}

	return headerFilePatterns;
}

const std::vector<std::string>& Variant::getArchitectures() const {
	return architectures;
}

const std::vector<std::string>& Variant::getArchitecturesEffective() const {
	if(architectures.empty() && !descriptor.isDefaultVariant(*this)) {
		return descriptor.getVariant("").getArchitectures();
	}

	return architectures;
}

bool Variant::hasArchitecture(const std::string& architecture) const {
	const std::vector<std::string>& architectures = getArchitecturesEffective();

	for(const auto& architectureFound : architectures) {
		if(architecture == architectureFound) {
			return true;
		}
	}

	return false;
}

const std::map<std::string, Generator>& Variant::getGenerators() const {
	return generators;
}

std::map<std::string, Generator> Variant::getGeneratorsEffective() const {
	if(descriptor.isDefaultVariant(*this)) {
		return getGenerators();
	}

	std::map<std::string, Generator> generatorsEffective = descriptor.getVariant("").getGenerators();
	for(const auto& generator : generators) {
		generatorsEffective[generator.first] = generator.second;
	}

	return generatorsEffective;
}

bool Variant::addDefinePrivate(const std::string& define) {
	std::string defineName = extractDefineName(define);

	if(definesPrivate.find(defineName) != std::end(definesPrivate)) {
		std::cout << "cannot insert private define \"" << define << "\" because it is already defined as private define\n";
		return false;
	}

	if(definesPublic.find(defineName) != std::end(definesPublic)) {
		std::cout << "cannot insert private define \"" << define << "\" because it is already defined as public define\n";
		return false;
	}

	return definesPrivate.insert(std::make_pair(defineName, define)).second;
}

const std::map<std::string, std::string>& Variant::getDefinesPrivate() const {
	return definesPrivate;
}

std::map<std::string, std::string> Variant::getDefinesPrivateEffective() const {
	if(descriptor.isDefaultVariant(*this)) {
		return getDefinesPrivate();
	}

	std::map<std::string, std::string> definesEffective = descriptor.getVariant("").definesPrivate;
	for(const auto& defineEntry : definesPrivate) {
		definesEffective[defineEntry.first] = defineEntry.second;
	}

	return definesEffective;
}

bool Variant::addDefinePublic(const std::string& define) {
	std::string defineName = extractDefineName(define);

	if(definesPrivate.find(defineName) != std::end(definesPrivate)) {
		std::cout << "cannot insert public define \"" << define << "\" because it is already defined as private define\n";
		return false;
	}

	if(definesPublic.find(defineName) != std::end(definesPublic)) {
		std::cout << "cannot insert public define \"" << define << "\" because it is already defined as public define\n";
		return false;
	}

	return definesPublic.insert(std::make_pair(defineName, define)).second;
}

const std::map<std::string, std::string>& Variant::getDefinesPublic() const {
	return definesPublic;
}

std::map<std::string, std::string> Variant::getDefinesPublicEffective() const {
	if(descriptor.isDefaultVariant(*this)) {
		return getDefinesPublic();
	}

	std::map<std::string, std::string> definesEffective = descriptor.getVariant("").definesPublic;
	for(const auto& defineEntry : definesPublic) {
		definesEffective[defineEntry.first] = defineEntry.second;
	}

	return definesEffective;
}

bool Variant::isProvidedSource() const {
	return provideSource;
}

bool Variant::isProvidedSourceEffective() const {
	if(!provideSource && !provideStatic && !provideDynamic && !provideExecutable) {
		if(!descriptor.isDefaultVariant(*this)) {
			return descriptor.getVariant("").isProvidedSource();
		}
		return true;
	}
	return provideSource;
}

bool Variant::isProvidedStatic() const {
	return provideStatic;
}

bool Variant::isProvidedStaticEffective() const {
	if(!provideSource && !provideStatic && !provideDynamic && !provideExecutable) {
		if(!descriptor.isDefaultVariant(*this)) {
			return descriptor.getVariant("").isProvidedStatic();
		}
		return true;
	}
	return provideStatic;
}

bool Variant::isProvidedDynamic() const {
	return provideDynamic;
}

bool Variant::isProvidedDynamicEffective() const {
	if(!provideSource && !provideStatic && !provideDynamic && !provideExecutable) {
		if(!descriptor.isDefaultVariant(*this)) {
			return descriptor.getVariant("").isProvidedDynamic();
		}
		return true;
	}
	return provideDynamic;
}

bool Variant::isProvidedExecutable() const {
	return provideExecutable;
}

bool Variant::isProvidedExecutableEffective() const {
	if(!provideSource && !provideStatic && !provideDynamic && !provideExecutable) {
		if(!descriptor.isDefaultVariant(*this)) {
			return descriptor.getVariant("").isProvidedExecutable();
		}
		return true;
	}
	return provideExecutable;
}

bool Variant::hasDependencies() const {
	return !dependencies.empty();
}

std::vector<Dependency> Variant::getDependencies() const {
	std::vector<Dependency> result;

	std::transform(std::begin(dependencies), std::end(dependencies), std::back_inserter(result), [](const std::pair<std::string, Dependency>& entry) {
		return entry.second;
	});

	return result;
}

std::vector<Dependency> Variant::getDependenciesEffective() const {
	if(descriptor.isDefaultVariant(*this)) {
		return getDependencies();
	}

	std::map<std::string, Dependency> dependenciesEffective = descriptor.getVariant("").dependencies;
	for(const auto& defineEntry : dependencies) {
		dependenciesEffective[defineEntry.first] = defineEntry.second;
	}

	std::vector<Dependency> result;
	std::transform( std::begin(dependenciesEffective), std::end(dependenciesEffective), std::back_inserter(result), [](const std::pair<std::string, Dependency>& entry) {
		return entry.second;
	});

	return result;
}

} /* namespace model */
} /* namespace transformer */
