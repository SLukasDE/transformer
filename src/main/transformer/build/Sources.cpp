#include <transformer/build/Sources.h>
#include <transformer/model/Variant.h>

namespace transformer {
namespace build {

Sources::Sources(const model::Descriptor& aDescriptor)
: descriptor(aDescriptor)
{
}

Sources::~Sources() {
}

/* ******* *
 * getPath *
 * ******* */

std::vector<std::filesystem::path> Sources::getPathesSourceMain(const std::string& variantName) const {
	std::vector<std::filesystem::path> result;
	const model::Variant& variant = descriptor.getVariant(variantName);

	for(std::size_t i=0; i<variant.getSourcesMainDirEffective().size(); ++i) {
		result.push_back(variant.getSourcesMainDirEffective()[i]);
	}

	return result;
}

std::vector<std::filesystem::path> Sources::getPathesSourceTest(const std::string& variantName) const {
	std::vector<std::filesystem::path> result;
	const model::Variant& variant = descriptor.getVariant(variantName);

	for(std::size_t i=0; i<variant.getSourcesTestDirEffective().size(); ++i) {
		result.push_back(variant.getSourcesTestDirEffective()[i]);
	}

	return result;
}

std::vector<std::filesystem::path> Sources::getPathesHeader(const std::string& variantName) const {
	std::vector<std::filesystem::path> result;
	const model::Variant& variant = descriptor.getVariant(variantName);

	for(std::size_t i=0; i<variant.getHeadersDirEffective().size(); ++i) {
		result.push_back(variant.getHeadersDirEffective()[i]);
	}

	return result;
}

std::filesystem::path Sources::getPathBuild(const std::string& artefactId, const std::string& artefactVersion) const {
	std::filesystem::path path = descriptor.getBuildDirEffective();

	path /= artefactId;
	path /= artefactVersion;

	return path;
}

std::filesystem::path Sources::getPathBuildSource(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName) const {
	std::filesystem::path path = getPathBuild(artefactId, artefactVersion, variantName);
	path /= "source";
	return path;
}

std::filesystem::path Sources::getPathBuildHeaders(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName) const {
	std::filesystem::path path = getPathBuild(artefactId, artefactVersion, variantName);
	path /= "headers";
	return path;
}

std::filesystem::path Sources::getPathBuildCompile(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const {
	std::filesystem::path path = getPathBuild(artefactId, artefactVersion, variantName, architecture);
	path /= "compile";
	return path;
}

std::filesystem::path Sources::getPathBuildCompileTest(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const {
	std::filesystem::path path = getPathBuild(artefactId, artefactVersion, variantName, architecture);
	path /= "compile-test";
	return path;
}

std::filesystem::path Sources::getPathBuildLinkStatic(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const {
	std::filesystem::path path = getPathBuild(artefactId, artefactVersion, variantName, architecture);
	path /= "link-static";
	return path;
}

std::filesystem::path Sources::getPathBuildLinkDynamic(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const {
	std::filesystem::path path = getPathBuild(artefactId, artefactVersion, variantName, architecture);
	path /= "link-dynamic";
	return path;
}

std::filesystem::path Sources::getPathBuildLinkExecutable(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const {
	std::filesystem::path path = getPathBuild(artefactId, artefactVersion, variantName, architecture);
	path /= "link-executable";
	return path;
}

std::filesystem::path Sources::getPathBuildLinkExecutableTest(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const {
	std::filesystem::path path = getPathBuild(artefactId, artefactVersion, variantName, architecture);
	path /= "link-executable-test";
	return path;
}

std::filesystem::path Sources::getPathBuildGenerated(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture, const std::string& generatorId) const {
//	std::filesystem::path path = getPathBuild(descriptor.getArtefactId(), descriptor.getArtefactVersion(), variantName, architecture);
	std::filesystem::path path = getPathBuild(artefactId, artefactVersion, variantName, architecture);

	path /= "generated";
	path /= generatorId;

	return path;
}








/* ********* *
 * getSource *
 * ********* */

std::vector<std::reference_wrapper<const files::Source>> Sources::getSourcesSourceMain(const std::string& variantName) const {
	std::vector<std::reference_wrapper<const files::Source>> result;
	std::vector<std::filesystem::path> pathes = getPathesSourceMain(variantName);
	std::string sourceName;

	if(variantName.empty()) {
		sourceName = "source/default/main";
	}
	else {
		sourceName = "source/variant/" + variantName + "/main";
	}

	for(std::size_t i=0; i<pathes.size(); ++i) {
		result.push_back(std::ref(addOrGetSource(sourceName+std::to_string(i), pathes[i])));
	}

	return result;
}

std::vector<std::reference_wrapper<const files::Source>> Sources::getSourcesSourceTest(const std::string& variantName) const {
	std::vector<std::reference_wrapper<const files::Source>> result;
	std::vector<std::filesystem::path> pathes = getPathesSourceTest(variantName);
	std::string sourceName;

	if(variantName.empty()) {
		sourceName = "source/default/test";
	}
	else {
		sourceName = "source/variant/" + variantName + "/test";
	}

	for(std::size_t i=0; i<pathes.size(); ++i) {
		result.push_back(std::ref(addOrGetSource(sourceName+std::to_string(i), pathes[i])));
	}

	return result;
}

std::vector<std::reference_wrapper<const files::Source>> Sources::getSourcesHeader(const std::string& variantName) const {
	std::vector<std::reference_wrapper<const files::Source>> result;
	std::vector<std::filesystem::path> pathes = getPathesHeader(variantName);
	std::string sourceName;

	if(variantName.empty()) {
		sourceName = "source/default/header";
	}
	else {
		sourceName = "source/variant/" + variantName + "/header";
	}

	for(std::size_t i=0; i<pathes.size(); ++i) {
		result.push_back(std::ref(addOrGetSource(sourceName+std::to_string(i), pathes[i])));
	}

	return result;
}

const files::Source& Sources::getSourceBuildSource(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName) const {
	return addOrGetSource(getPathBuildSource(artefactId, artefactVersion, variantName));
}

const files::Source& Sources::getSourceBuildHeaders(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName) const {
	return addOrGetSource(getPathBuildHeaders(artefactId, artefactVersion, variantName));
}

const files::Source& Sources::getSourceBuildCompile(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const {
	return addOrGetSource(getPathBuildCompile(artefactId, artefactVersion, variantName, architecture));
}

const files::Source& Sources::getSourceBuildCompileTest(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const {
	return addOrGetSource(getPathBuildCompileTest(artefactId, artefactVersion, variantName, architecture));
}

const files::Source& Sources::getSourceBuildLinkStatic(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const {
	return addOrGetSource(getPathBuildLinkStatic(artefactId, artefactVersion, variantName, architecture));
}

const files::Source& Sources::getSourceBuildLinkDynamic(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const {
	return addOrGetSource(getPathBuildLinkDynamic(artefactId, artefactVersion, variantName, architecture));
}

const files::Source& Sources::getSourceBuildLinkExecutable(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const {
	return addOrGetSource(getPathBuildLinkExecutable(artefactId, artefactVersion, variantName, architecture));
}

const files::Source& Sources::getSourceBuildLinkExecutableTest(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const {
	return addOrGetSource(getPathBuildLinkExecutableTest(artefactId, artefactVersion, variantName, architecture));
}

const files::Source& Sources::getSourceBuildGenerated(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture, const std::string& generatorId) const {
	return addOrGetSource(getPathBuildGenerated(artefactId, artefactVersion, variantName, architecture, generatorId));
}


const files::Source& Sources::addOrGetSource(const std::filesystem::path& path) const {
	return addOrGetSource(path.generic_string(), path);
}

const files::Source& Sources::addOrGetSource(const std::string& name, const std::filesystem::path& path) const {
	std::map<std::string, files::Source>::const_iterator iter = sourcesByName.find(name);

	if(iter == std::end(sourcesByName)) {
		sourceNames.push_back(name);
		iter = sourcesByName.emplace(name, files::Source(path)).first;
	}

	return iter->second;
}
/*
std::filesystem::path Sources::getPathBuild(const std::string& variantName, const std::string& architecture) const {
	std::filesystem::path path = descriptor.getBuildDirEffective();

	if(variantName.empty()) {
		path /= "default";
	}
	else {
		path /= "variants";
		path /= variantName;
	}
	if(architecture.empty()) {
		path /= "common";
	}
	else {
		path /= "architecture";
		path /= architecture;
	}

	return path;
}
*/
std::filesystem::path Sources::getPathBuild(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const std::string& architecture) const {
	std::filesystem::path path = getPathBuild(artefactId, artefactVersion);

	if(variantName.empty()) {
		path /= "default";
	}
	else {
		path /= "variants";
		path /= variantName;
	}

	if(architecture.empty()) {
		path /= "common";
	}
	else {
		path /= "architecture";
		path /= architecture;
	}

	return path;
}

} /* namespace build */
} /* namespace transformer */
