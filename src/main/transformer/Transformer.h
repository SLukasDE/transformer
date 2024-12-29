#ifndef TRANSFORMER_TRANSFORMER_H_
#define TRANSFORMER_TRANSFORMER_H_

#include <transformer/architectures/Specifiers.h>
#include <transformer/repository/Database.h>
#include <transformer/build/BuildManager.h>
#include <transformer/build/gcc/BuilderFactory.h>

#include <filesystem>
#include <memory>
#include <vector>
#include <set>
#include <string>

namespace transformer {

class Transformer {
public:
	enum Commands {
		clean,
		dependencies, generateSources, compile, test, link, site, package, install, provide,
		onlyDependencies, onlyGenerateSources, onlyCompile, onlyTest, onlyLink, onlySite, onlyPackage, onlyInstall, onlyProvide
	};

	enum Generators {
		cdtProject, make, cmake, meson
	};

	struct Settings {
		Settings(int argc, char **argv);

		std::vector<Commands> commands;
		std::string buildFile;
		unsigned short parallelCount = 0;
		std::string scriptFile;
		std::string loggerFile;
		std::set<std::string> optionArchitectures;
		std::set<std::string> optionVariants;
		std::set<Generators> generators;
	};

	Transformer(int argc, char **argv);
	~Transformer() = default;

	int run();

	static void printUsage();

private:
	static const std::size_t npos;

	/* settings */
	Settings settings;
	std::filesystem::path localRepoPath;


	/* architectures */
	architectures::Specifiers specifiers;

	transformer::repository::Database database;
	std::unique_ptr<model::Descriptor> descriptor;
	std::unique_ptr<build::BuildManager> buildManager;

	build::gcc::BuilderFactory builderFactoryGCC;

	void loadDescriptor();
	const model::Descriptor& getDescriptor() const;
	build::BuildManager& getBuildManager();
	void printDescriptor() const;
};

} /* namespace transformer */

#endif /* TRANSFORMER_TRANSFORMER_H_ */
