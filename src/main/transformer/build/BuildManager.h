#ifndef TRANSFORMER_BUILD_BUILDMANAGER_H_
#define TRANSFORMER_BUILD_BUILDMANAGER_H_

#include <vector>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <ostream>
#include <transformer/Logger.h>
#include <transformer/repository/Database.h>
#include <transformer/model/Descriptor.h>
#include <transformer/model/Generator.h>
#include <transformer/build/Sources.h>
#include <transformer/build/SolvedStatus.h>
#include <transformer/build/BuilderFactory.h>

namespace transformer {
namespace build {

class BuildManagerVariant;
class Builder;

class BuildManager {
friend class BuildManagerVariant;
public:
	static Logger logger;
	BuildManager(repository::Database& database, const model::Descriptor& descriptor, const std::set<std::string>& optionVariantName, const std::set<std::string>& optionArchitectures);
	~BuildManager();

	void addBuilderFactory(BuilderFactory& builderFactory);

	void initializeDependencies();

	void makeClean();
	int makeDependencies();
	int makeGenerateSources();
	int makeCompile();
	int makeTest();
	int makeLink();
	int makeSite();
	int makePackage();
	int makeInstall();
	int makeProvide();

	void generateCdtProject(const std::set<std::string>& variantNames,const std::string& architecture);

	int onlyDependencies();
	int onlyGenerateSources();
	int onlyCompile();
	int onlyTest();
	int onlyLink();
	int onlySite(); // only documentation
	int onlyPackage();
	int onlyInstall();
	int onlyProvide();

	void addGeneratorDescriptor(const model::Generator& generator, const model::Descriptor& generatorDescriptor);
	const model::Descriptor& getGeneratorDescriptor(const model::Generator& generator) const;

	const SolvedDescriptors& getSolvedDescriptors(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName) const;
	bool addSolvedDescriptors(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName, const SolvedDescriptors& solvedDescriptors);

	SolvedStatus& getSolvedStatus(const std::string& artefactId, const std::string& artefactVersion, const std::string& variantName);

	repository::Database& getDatabase() const;
	const model::Descriptor& getDescriptor() const;
	const Sources& getSources() const;

private:
	bool isInitializedDependencies = false;
	repository::Database& database;
	const model::Descriptor& descriptor;
	std::set<std::string> optionVariantNames;
	std::set<std::string> optionArchitectures;
	Sources sources;
	std::vector<std::reference_wrapper<BuilderFactory>> builderFactories;
	std::map<std::string, std::unique_ptr<Builder>> buildersByArchitecture;
	std::map<std::string, std::unique_ptr<BuildManagerVariant>> buildManagerVariantByVariantName;

	/* Stored solved descriptors when solving dependencies for a descriptor. The key of this root descriptor is stored as key.
	 * So, the map only stores entries, for solving dependencies of a descriptor.
	 * [artefactId, artefactVersion, variantName] -> { <SolvedDescriptor> } // = dependencies
	 */
	// enthält als Key die Koordinaten des Descritpros, der ursächtlich für das Auflösen der Dependencies ist.
	std::map<std::tuple<std::string, std::string, std::string>, SolvedDescriptors> solvedDescriptorsByArtefactIdVersionVariant;

	/* Stored EVERY dependency that has been loaded/processed so far.
	 * if there is an entry, then this artefact (key: id+version+variant) has been processed already.
	 * The value tells if it has been processed with "binary needed".
	 * If value is false but now loadDependency is called again with true, then it has to be continues just for binary stuff.
	 */
	std::map<std::tuple<std::string, std::string, std::string>, SolvedStatus> solvedStatusByArtefactIdVersionVariant;

	std::map<std::tuple<std::string, std::string, std::string, std::string>, const model::Descriptor&> generatorDescriptorByGenerator;

};

} /* namespace build */
} /* namespace transformer */

#endif /* TRANSFORMER_BUILD_BUILDMANAGER_H_ */
