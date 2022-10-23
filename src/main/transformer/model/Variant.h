#ifndef TRANSFORMER_MODEL_VARIANT_H_
#define TRANSFORMER_MODEL_VARIANT_H_

#include <string>
#include <vector>
#include <map>
#include <transformer/model/Dependency.h>
#include <transformer/model/Generator.h>

namespace transformer {

namespace repository {
class DescriptorReader;
}

namespace model {

class Descriptor;

class Variant {
	friend class repository::DescriptorReader;
public:
	Variant(const Descriptor& descriptor);
	~Variant();

	const Descriptor& getDescriptor() const;

	bool hasSourcesMainDir() const;
	const std::vector<std::string>& getSourcesMainDir() const;
	std::vector<std::string> getSourcesMainDirEffective() const;

	bool hasSourcesTestDir() const;
	const std::vector<std::string>& getSourcesTestDir() const;
	std::vector<std::string> getSourcesTestDirEffective() const;

	bool hasHeadersDir() const;
	const std::vector<std::string>& getHeadersDir() const;
	std::vector<std::string> getHeadersDirEffective() const;

	bool hasSourceFilePatterns() const;
	const std::vector<std::string>& getSourceFilePatterns() const;
	std::vector<std::string> getSourceFilePatternsEffective() const;

	bool hasHeaderFilePatterns() const;
	const std::vector<std::string>& getHeaderFilePatterns() const;
	std::vector<std::string> getHeaderFilePatternsEffective() const;

	const std::vector<std::string>& getArchitectures() const;
	const std::vector<std::string>& getArchitecturesEffective() const;
	bool hasArchitecture(const std::string& architecture) const;

	const std::map<std::string, Generator>& getGenerators() const;
	std::map<std::string, Generator> getGeneratorsEffective() const;

	/* returns true on success, false if define-key exists already */
	bool addDefinePrivate(const std::string& define);
	const std::map<std::string, std::string>& getDefinesPrivate() const;
	std::map<std::string, std::string> getDefinesPrivateEffective() const;

	/* returns true on success, false if define-key exists already */
	bool addDefinePublic(const std::string& define);
	const std::map<std::string, std::string>& getDefinesPublic() const;
	std::map<std::string, std::string> getDefinesPublicEffective() const;

	bool isProvidedSource() const;
	bool isProvidedSourceEffective() const;
	bool isProvidedStatic() const;
	bool isProvidedStaticEffective() const;
	bool isProvidedDynamic() const;
	bool isProvidedDynamicEffective() const;
	bool isProvidedExecutable() const;
	bool isProvidedExecutableEffective() const;

	bool hasDependencies() const;
	std::vector<Dependency> getDependencies() const;
	std::vector<Dependency> getDependenciesEffective() const;

private:
	const Descriptor& descriptor;

	std::vector<std::string> sourcesMainDir;
	std::vector<std::string> sourcesTestDir;
	std::vector<std::string> headersDir;
	// file pattern for source files, e.g. .cpp, .cxx
	std::vector<std::string> sourceFilePatterns;
	// file pattern for header files, e.g. .h, .hpp
	std::vector<std::string> headerFilePatterns;

	std::vector<std::string> architectures;

	std::map<std::string, Generator> generators;

	std::map<std::string, std::string> definesPrivate;

	std::map<std::string, std::string> definesPublic;

	/* if one of them is true, then packageSource is enabled
	 * packageSource := packageSourceStatic || packageSourceDynamic
	 *
	 * Die Aufteilung in 2 Attribute dient lediglich zur Verarbeitung im Solver.
	 * Bei der Interpretation gibt es schließlich nur die Frage ob ein Source-Package gebaut werden soll.
	 * Und dies dies dann der Fall, wenn einer der beiden Attribute true ist.
	 *
	 * Umgekehrt werden beim Einlesen einer Descriptor-Datei stets beide Attribute entwerder auf False oder True gesetzt.
	 */
	bool provideSource = false;
	bool provideStatic = false;
	bool provideDynamic = false;
	bool provideExecutable = false;

	std::map<std::string, Dependency> dependencies;
};

} /* namespace model */
} /* namespace transformer */

#endif /* TRANSFORMER_MODEL_VARIANT_H_ */
