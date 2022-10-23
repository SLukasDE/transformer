#include <transformer/repository/DescriptorWriter.h>

#include <esl/system/Stacktrace.h>

#include <string>
#include <fstream>
#include <sstream>

namespace transformer {
namespace repository {

DescriptorWriter::DescriptorWriter(const model::Descriptor& aDescriptor)
: descriptor(aDescriptor)
{
}

DescriptorWriter::~DescriptorWriter() {
}

void DescriptorWriter::write(const boost::filesystem::path path) const {
	if(!boost::filesystem::exists(path)) {
		throw esl::system::Stacktrace::add(std::runtime_error("File \"" + path.generic_string() + "\" does not exists."));
	}
	if(!boost::filesystem::is_regular(path)) {
		throw esl::system::Stacktrace::add(std::runtime_error("Path \"" + path.generic_string() + "\" is not a file."));
	}
	std::ofstream ostream(path.string());

	write(ostream);
}

void DescriptorWriter::write(std::ostream& os) const {
	/* write artefact-name and default variant */
	os << "id: " << descriptor.getArtefactId();
	if(!descriptor.getDefaultVariantName().empty()) {
		os << "[" << descriptor.getDefaultVariantName() << "]";
	}

	/* write artefact version */
	{
		model::Version version;
		if(descriptor.hasCommonVersion()) {
			version.setCommonVersion(descriptor.getArtefactVersion());
		}
		else {
			version.setArtefactVersion(descriptor.getArtefactVersion());
			version.setApiVersion(descriptor.getApiVersion());
		}
		os << " ";
		writeVersion(os, version);
		os << "\n";
	}

	os << "name: " << descriptor.getArtefactName() << "\n";

	// write 'base name and version '
	if(!descriptor.getBaseName().empty()) {
		os << "base: " << descriptor.getBaseName() << " [" << descriptor.getBaseVersion() << "]\n";
	}

	// write 'build-dir:'
	if(descriptor.hasBuildDir()) {
		os << "build-dir: " << descriptor.getBuildDir() << "\n";
	}

	/* write default configuration */
	writeVariant(os, descriptor.getVariant(""));

	/* write variants */
	std::vector<std::string> variantNames = descriptor.getSpecialVariantNames();
	for(const auto& variantName : variantNames) {
		os << "\nvariant: " << variantName << "\n";
		writeVariant(os, descriptor.getVariant(variantName));
	}

	/* write solved entries */
	for(const auto& solvedEntry : descriptor.getSolved()) {
		os << "\nsolved: " << solvedEntry.first << "\n";
		writeSolved(os, solvedEntry.second);
	}

}

void DescriptorWriter::writeVersion(std::ostream& os, const model::Version& version) const {
	if(version.hasCommonVersion()) {
		os << version.getVersionArtefact();
	}
	else {
		if(!version.getVersionArtefact().empty()) {
			os << "[" << version.getVersionArtefact() << "]";
		}
		if(!version.getVersionApi().empty()) {
			os << "{" << version.getVersionApi() << "}";
		}
	}
}

void DescriptorWriter::writeVersionRange(std::ostream& os, const model::VersionRange& versionRange) const {
	if(versionRange.hasCommonVersion()) {
		os << versionRange.getMinVersionArtefact();
	}
	else {
		if(!versionRange.getMinVersionArtefact().empty()) {
			if(versionRange.hasCommonArtefactVersion()) {
				os << "[" << versionRange.getMinVersionArtefact() << "]";
			}
			else {
				os << "[" << versionRange.getMinVersionArtefact() << " " << versionRange.getMaxVersionArtefact() << "]";
			}
		}
		if(!versionRange.getMinVersionApi().empty()) {
			if(versionRange.hasCommonApiVersion()) {
				os << "{" << versionRange.getMinVersionApi() << "}";
			}
			else {
				os << "{" << versionRange.getMinVersionApi() << " " << versionRange.getMaxVersionApi() << "}";
			}
		}
	}
}

void DescriptorWriter::writeVersionRanges(std::ostream& os, const model::VersionRanges& versionRanges) const {
	bool isFirst = true;

	for(const auto& versionRange : versionRanges.getRanges()) {
		if(isFirst) {
			isFirst = false;
		}
		else {
			os << " ";
		}
		writeVersionRange(os, versionRange);
	}
}

void DescriptorWriter::writeVariant(std::ostream& os, const model::Variant& variant) const {
	// write 'sources-main-dir:'
	if(variant.hasSourcesMainDir()) {
		os << "sources-main-dir:";
		for(const auto& dir : variant.getSourcesMainDir()) {
			os << " " << dir;
		}
		os << "\n";
	}

	// write 'sources-test-dir:'
	if(variant.hasSourcesTestDir()) {
		os << "sources-test-dir:";
		for(const auto& dir : variant.getSourcesTestDir()) {
			os << " " << dir;
		}
		os << "\n";
	}

	// write 'headers-dir:'
	if(variant.hasHeadersDir()) {
		os << "headers-dir:";
		for(const auto& dir : variant.getHeadersDir()) {
			os << " " << dir;
		}
		os << "\n";
	}

	// write 'sources:'
	if(variant.hasSourceFilePatterns()) {
		os << "source-file-pattern:";
		for(const auto& entries : variant.getSourceFilePatterns()) {
			os << " " << entries;
		}
		os << "\n";
	}

	// write 'headers:'
	if(variant.hasHeaderFilePatterns()) {
		os << "header-file-pattern:";
		for(const auto& entries : variant.getHeaderFilePatterns()) {
			os << " " << entries;
		}
		os << "\n";
	}

	// write 'architecture:'
	if(!variant.getArchitectures().empty()) {
		os << "architecture:";
		for(const auto& entries : variant.getArchitectures()) {
			os << " " << entries;
		}
		os << "\n";
	}

	// write 'provide:'
	if(variant.isProvidedSource() || variant.isProvidedDynamic() || variant.isProvidedStatic() || variant.isProvidedExecutable()) {
		os << "provide:";
		if(variant.isProvidedSource()) {
			os << " source";
		}
		if(variant.isProvidedDynamic()) {
			os << " dynamic";
		}
		if(variant.isProvidedStatic()) {
			os << " static";
		}
		if(variant.isProvidedExecutable()) {
			os << " executable";
		}
		os << "\n";
	}

	// write 'define-public:'
	{
		std::map<std::string, std::string> defines = variant.getDefinesPublic();
		for(const auto& define : defines) {
			os << "define-public: " << define.second << "\n";
		}
	}

	// write 'define-private:'
	{
		std::map<std::string, std::string> defines = variant.getDefinesPrivate();
		for(const auto& define : defines) {
			os << "define-private: " << define.second << "\n";
		}
	}

	// write 'generator:'
	for(const auto& entries : variant.getGenerators()) {
		const model::Generator& generator = entries.second;

		os << "generator: " << generator.id;
		if(!generator.variantName.empty()) {
			os << "[" << generator.variantName << "]";
		}

		if(generator.versionRange.isEmpty()) {
			os << "\n";
			continue;
		}

		os << " ";
		writeVersionRange(os, generator.versionRange);
		os << "\n";
	}

	// write dependencies '[static][,dynamic][,system]:'
	{
		std::vector<model::Dependency> dependencies = variant.getDependencies();
		for(const auto& dependency : dependencies) {
			writeDependency(os, dependency);
		}
	}
}

void DescriptorWriter::writeSolved(std::ostream& os, const std::vector<model::DependencySolved>& solved) const {
	for(const auto& dependencySolved: solved) {
		writeDependencySolved(os, dependencySolved);
	}
}

void DescriptorWriter::writeDependency(std::ostream& os, const model::Dependency& dependency) const {
	if(!dependency.allowedTypeRepositoryStatic && !dependency.allowedTypeRepositoryDynamic && !dependency.allowedTypeSystemDynamic) {
		return;
	}

	bool isFirst = true;

	if(dependency.allowedTypeRepositoryStatic) {
		if(isFirst) {
			isFirst = false;
		}
		else {
			os << ",";
		}
		os << "static";
	}

	if(dependency.allowedTypeRepositoryDynamic) {
		if(isFirst) {
			isFirst = false;
		}
		else {
			os << ",";
		}
		os << "dynamic";
	}

	if(dependency.allowedTypeSystemDynamic) {
		if(isFirst) {
			isFirst = false;
		}
		else {
			os << ",";
		}
		os << "system";
	}

	os << ": " << dependency.id;
	if(!dependency.variantName.empty()) {
		os << "[" << dependency.variantName << "]";
	}
	os << " ";
	writeVersionRanges(os, dependency.versionRanges);
	os << "\n";
}

void DescriptorWriter::writeDependencySolved(std::ostream& os, const model::DependencySolved& dependencySolved) const {
	if(!dependencySolved.allowedTypeRepositoryStatic && !dependencySolved.allowedTypeRepositoryDynamic && !dependencySolved.allowedTypeSystemDynamic) {
		return;
	}

	bool isFirst = true;

	if(dependencySolved.allowedTypeRepositoryStatic) {
		if(isFirst) {
			isFirst = false;
		}
		else {
			os << ",";
		}
		os << "static";
	}

	if(dependencySolved.allowedTypeRepositoryDynamic) {
		if(isFirst) {
			isFirst = false;
		}
		else {
			os << ",";
		}
		os << "dynamic";
	}

	if(dependencySolved.allowedTypeSystemDynamic) {
		if(isFirst) {
			isFirst = false;
		}
		else {
			os << ",";
		}
		os << "system";
	}

	os << ": " << dependencySolved.name;
	if(!dependencySolved.variantName.empty()) {
		os << "[" << dependencySolved.variantName << "]";
	}

	os << " {" << dependencySolved.versionApi << "}\n";
}

} /* namespace repository */
} /* namespace transformer */
