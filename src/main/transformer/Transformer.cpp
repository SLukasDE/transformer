#include <transformer/architectures/Config.h>
#include <transformer/Config.h>
#include <transformer/Execute.h>
#include <transformer/Logger.h>
#include <transformer/repository/DescriptorWriter.h>
#include <transformer/repository/RepositoryLocal.h>
#include <transformer/Transformer.h>

#include <esl/logging/Logging.h>
#include <esl/system/Stacktrace.h>
#include <esl/utility/String.h>

#include <iostream>
#include <limits>
#include <stdexcept>
#include <stdlib.h> // getenv(...)
#include <cstdlib> // getenv(...)

#include <transformer/architectures/solver/Solver.h>
namespace transformer {
namespace {
Logger logger("transformer::Transformer");

void printBuildConfigTest(const architectures::Specifiers& specifiers) {
	architectures::solver::Solver solver(specifiers, {"c++11", "linux", "gcc"});
	std::map<std::set<std::string>, architectures::Architecture> architectures = solver.getBuildArchitectures();
std::cout << "Found " << architectures.size() << " architectures.\n";
	int i=0;
	for(const auto& architectureEntry : architectures) {
		std::cout << "Entry #" << ++i << ":\n";

		std::cout << "  Specifiers (key):\n";
		for(const auto& id : architectureEntry.first) {
			std::cout << "  - \"" << id << "\"\n";
		}

		std::cout << "  Specifiers (architecture):\n";
		for(const auto& id : architectureEntry.second.getArchitectureIds()) {
			std::cout << "  - \"" << id << "\"\n";
		}

		std::cout << "  Languages:\n";
		for(const auto& languageEntry : architectureEntry.second.getLanguages()) {
			std::cout << "  - \"" << languageEntry.second.getLanguage() << "\"\n";
			std::cout << "    Driver: \"" << languageEntry.second.getDriver() << "\"\n";
			for(const auto& setting : languageEntry.second.getDriverSettings()) {
				std::cout << "    - \"" << setting.first << "\": \"" << setting.second << "\"\n";
			}

			std::set<std::string> profiles = languageEntry.second.getProfiles();
			for(const auto& profile : profiles) {
				std::cout << "    Profile: \"" << profile << "\"\n";
				for(const auto& setting : languageEntry.second.getProfileSettings(profile)) {
					std::cout << "    - \"" << setting.first << "\": \"" << setting.second << "\"\n";
				}

			}
		}

	}
}
}

Transformer::Transformer(int argc, char **argv)
{
	for(int i=0; i<argc; ++i) {
		arguments.push_back(argv[i]);
	}
}

void Transformer::loadSettings() {
	const char* envVar = nullptr;

	envVar = std::getenv("TBUILD_HOME");
	if(envVar) {
		localRepoPath = envVar;
	}
	else {
		boost::filesystem::path homePath;
		envVar = std::getenv("HOME");
		if(envVar == nullptr) {
			throw std::runtime_error("No HOME variable found");
		}
		homePath = envVar;

/*
		boost::filesystem::path settingsPath = homePath;
		settingsPath /= ".tbuild";
		settingsPath /= "settings.cfg";
*/
		localRepoPath = homePath;
		localRepoPath /= ".tbuild";
	}

	logger.info << "localRepoPath=\"" << localRepoPath.generic_string() << "\"\n";

	database.setLocalRepositoryPath(localRepoPath);

	loadArchitectures();
/*
	static transformer::repository::RepositoryLocal repositoryCodeHouse;
	repositoryCodeHouse.setPath("/home/a0ca1e4/.xbuild-codehouse");
	database.addRepository("codehouse", repositoryCodeHouse);
*/
}

void Transformer::loadArchitectures() {
	boost::filesystem::path architecturesPath;
	const char* envVar = nullptr;

	envVar = std::getenv("TBUILD_ARCHITECTURES");
	if(envVar) {
		architecturesPath = envVar;
	}
	else {
		architecturesPath = localRepoPath;
		architecturesPath /= "architecture-global.cfg";
	}

	{
		architectures::Config config(specifiers);
		config.load(architecturesPath);
	}
/*
	{
		architectures::Config config(specifiers);
		config.save(std::cout);
	}

	specifiers.print();
*/
	printBuildConfigTest(specifiers);
}

void Transformer::loadDescriptor() {
	descriptor = database.loadDescriptor(buildFile);

	if(descriptor) {
		printDescriptor();
	}
	else {
		std::cerr << "failed to load file \"" << buildFile << "\n";
		buildManager.reset();
	}
}

void Transformer::printUsage() {
	std::cout << "transformer <command> [[<command> [...]] [<option> [<option> [...]]]\n";
	std::cout << "\n";
	std::cout << "commands:\n";
	std::cout << "  clean\n";
	std::cout << "  dependencies\n";
	std::cout << "  generate-sources\n";
	std::cout << "  compile\n";
	std::cout << "  test\n";
	std::cout << "  link\n";
	std::cout << "  site\n";
	std::cout << "  package\n";
	std::cout << "  install\n";
	std::cout << "  provide\n";
	std::cout << "  only-dependencies\n";
	std::cout << "  only-generate-sources\n";
	std::cout << "  only-compile\n";
	std::cout << "  only-test\n";
	std::cout << "  only-link\n";
	std::cout << "  only-site\n";
	std::cout << "  only-package\n";
	std::cout << "  only-install\n";
	std::cout << "  only-provide\n";
	std::cout << "  generate <generate-target>[,<generate-target>[,...]]\n";
	std::cout << "  generate=<generate-target>[,<generate-target>[,...]]\n";
	std::cout << "\n";
	std::cout << "options:\n";
	std::cout << "  variant=<variant>[,<variant>[,...]]\n";
	std::cout << "  architecture=<architecture>[,<architecture>[,...]]\n";
	std::cout << "  logger=<logger.xml>\n";
	std::cout << "  logger <logger.xml>\n";
	std::cout << "  build-file=<tbuild.cfg>\n";
	std::cout << "  build-file <tbuild.cfg>\n";
	std::cout << "  parallel-count=<number>\n";
	std::cout << "  parallel-count <number>\n";
	std::cout << "  bash-script=<file>\n";
	std::cout << "  bash-script <file>\n";
	std::cout << "\n";
	std::cout << "generate-targets:\n";
	std::cout << "  cdt-project\n";
	std::cout << "  make\n";
	std::cout << "  cmake\n";
	std::cout << "  meson\n";
}

std::size_t Transformer::checkArguments() const {
	for(std::size_t index = 0; index < arguments.size(); ++index) {
		if(arguments[index] == "clean"
		|| arguments[index] == "dependencies"
		|| arguments[index] == "generate-sources"
		|| arguments[index] == "compile"
		|| arguments[index] == "test"
		|| arguments[index] == "link"
		|| arguments[index] == "site"
		|| arguments[index] == "package"
		|| arguments[index] == "install"
		|| arguments[index] == "provide"
		|| arguments[index] == "only-dependencies"
		|| arguments[index] == "only-generate-sources"
		|| arguments[index] == "only-compile"
		|| arguments[index] == "only-test"
		|| arguments[index] == "only-link"
		|| arguments[index] == "only-site"
		|| arguments[index] == "only-package"
		|| arguments[index] == "only-install"
		|| arguments[index] == "only-provide") {
			continue;
		}

		std::size_t pos = arguments[index].find('=');
		std::string key = arguments[index].substr(0, pos);

#if 1
		std::vector<std::string> values = readValues(index);
		if(values.empty()) {
			return 0;
		}
#else
		std::pair<std::vector<std::string>,std::size_t> valuesPair = readValues(index);
		std::vector<std::string>& values = valuesPair.first;
		if(valuesPair.second != npos) {
			return valuesPair.second;
		}
#endif
		if(key == "generate") {
			for(const auto& value : values) {
				if(value != "cdt-project"
				&& value != "make"
				&& value != "cmake"
				&& value != "meson") {
					return index;
				}
			}
			continue;
		}

		if(key == "architecture"
		|| key == "variant"
		|| key == "build-file"
		|| key == "logger"
		|| key == "parallel-count"
		|| key == "bash-script") {
			if(values.size() != 1) {
				return index;
			}
			continue;
		}

		return index;
	}

	return npos;
}

int Transformer::run() {
	int exitCode = 0;

	if(arguments.empty()) {
		return 3;
	}


	bool hasBuildFile = false;
	bool hasParallelCount = false;
	std::string loggerFile;
	/* check for build-file and parallel-count option */
	for(std::size_t index = 0; exitCode == 0 && index < arguments.size(); ++index) {
		std::size_t pos = arguments[index].find('=');
		std::string key = arguments[index].substr(0, pos);

		if(key == "build-file") {
			if(hasBuildFile) {
				throw esl::system::Stacktrace::add(std::runtime_error("Multiple definition of parameter 'build-file'"));
			}

			hasBuildFile = true;
			buildFile = readValue(index);

			if(buildFile.empty()) {
				throw esl::system::Stacktrace::add(std::runtime_error("Invalid value \"\" for parameter 'build-file'"));
			}
		}

		else if(key == "parallel-count") {
			if(hasParallelCount) {
				throw esl::system::Stacktrace::add(std::runtime_error("Multiple definition of parameter 'parallel-count'"));
			}
			hasParallelCount = true;

			std::string value = readValue(index);
			int i = std::stoi(value);
			if(i < 0 || i > std::numeric_limits<unsigned short>::max()) {
				throw esl::system::Stacktrace::add(std::runtime_error("Invalid value \"" + value + "\" for parameter 'parallel-count'"));
			}
			Config::parallelCount = static_cast<unsigned short>(i);
		}

		else if(key == "bash-script") {
			if(!Config::scriptFile.empty()) {
				throw esl::system::Stacktrace::add(std::runtime_error("Multiple definition of parameter 'bash-script'"));
			}

			const std::string value = readValue(index);

			if(value.empty()) {
				throw esl::system::Stacktrace::add(std::runtime_error("Invalid value \"\" for parameter 'bash-script'"));
			}
			Execute::run("rm -r " + value);
			Execute::run("touch " + value);
			Execute::run("chmod +x " + value);
			Config::scriptFile = value;
			Execute::addLineToScript("#!/bin/sh");
			Execute::addLineToScript("");
			Execute::addLineToScript("set -x");
		}

		else if(key == "logger") {
			if(!loggerFile.empty()) {
				throw esl::system::Stacktrace::add(std::runtime_error("Multiple definition of parameter 'logger'"));
			}

			loggerFile = readValue(index);

			if(loggerFile.empty()) {
				throw esl::system::Stacktrace::add(std::runtime_error("Invalid value \"\" for parameter 'logger'"));
			}
			esl::logging::Logging::initWithFile(loggerFile);
		}
	}

	loadDescriptor();

	/* first: just read options */
	for(std::size_t index = 0; exitCode == 0 && index < arguments.size(); ++index) {
		std::size_t pos = arguments[index].find('=');
		std::string key = arguments[index].substr(0, pos);

		if(key == "generate") {
		}
		else if(key == "architecture") {
		}
		else if(key == "variant") {
		}
		else {
			continue;
		}

#if 1
		std::vector<std::string> values = readValues(index);
		if(values.empty()) {
#else
		std::pair<std::vector<std::string>,std::size_t> valuesPair = readValues(index);
		std::vector<std::string>& values = valuesPair.first;
		if(valuesPair.second != npos) {
#endif
	        throw esl::system::Stacktrace::add(std::runtime_error("ERROR: missing argument for \"" + key + "\" command at index [" + std::to_string(index) + "]."));
		}

		if(key == "architecture") {
			hasOptionArchitectures = true;
			for(const auto& value : values) {
				optionArchitectures.insert(value);
			}
			continue;
		}

		if(key == "variant") {
			hasOptionVariants = true;
			for(const auto& value : values) {
				if(value.empty()) {
					optionVariants.insert(getDescriptor().getDefaultVariantName());
				}
				else {
					optionVariants.insert(value);
				}
			}
			continue;
		}
	}

	/* not read and execute commands */
	for(std::size_t index = 0; exitCode == 0 && index < arguments.size(); ++index) {
		if(arguments[index] == "clean") {
			getBuildManager().makeClean();
			continue;
		}
		if(arguments[index] == "dependencies") {
			exitCode = getBuildManager().makeDependencies();
			continue;
		}
		if(arguments[index] == "generate-sources") {
			exitCode = getBuildManager().makeGenerateSources();
			continue;
		}
		if(arguments[index] == "compile") {
			exitCode = getBuildManager().makeCompile();
			continue;
		}
		if(arguments[index] == "test") {
			exitCode = getBuildManager().makeTest();
			continue;
		}
		if(arguments[index] == "link") {
			exitCode = getBuildManager().makeLink();
			continue;
		}
		if(arguments[index] == "site") {
			exitCode = getBuildManager().makeSite();
			continue;
		}
		if(arguments[index] == "package") {
			exitCode = getBuildManager().makePackage();
			continue;
		}
		if(arguments[index] == "install") {
			exitCode = getBuildManager().makeInstall();
			continue;
		}
		if(arguments[index] == "provide") {
			exitCode = getBuildManager().makeProvide();
			continue;
		}
		if(arguments[index] == "only-dependencies") {
			exitCode = getBuildManager().onlyDependencies();
			continue;
		}
		if(arguments[index] == "only-generate-sources") {
			exitCode = getBuildManager().onlyGenerateSources();
			continue;
		}
		if(arguments[index] == "only-compile") {
			exitCode = getBuildManager().onlyCompile();
			continue;
		}
		if(arguments[index] == "only-test") {
			exitCode = getBuildManager().onlyTest();
			continue;
		}
		if(arguments[index] == "only-link") {
			exitCode = getBuildManager().onlyLink();
			continue;
		}
		if(arguments[index] == "only-site") {
			exitCode = getBuildManager().onlySite();
			continue;
		}
		if(arguments[index] == "only-package") {
			exitCode = getBuildManager().onlyPackage();
			continue;
		}
		if(arguments[index] == "only-install") {
			exitCode = getBuildManager().onlyInstall();
			continue;
		}
		if(arguments[index] == "only-provide") {
			exitCode = getBuildManager().onlyProvide();
			continue;
		}

		std::size_t pos = arguments[index].find('=');
		std::string key = arguments[index].substr(0, pos);
#if 1
		std::vector<std::string> values = readValues(index);
		if(values.empty()) {
#else
		std::pair<std::vector<std::string>,std::size_t> valuesPair = readValues(index);
		std::vector<std::string>& values = valuesPair.first;
		if(valuesPair.second != npos) {
#endif
	        throw esl::system::Stacktrace::add(std::runtime_error("ERROR: missing argument for \"" + key + "\" command at index [" + std::to_string(index) + "]."));
		}

		if(key == "architecture"
		|| key == "variant"
		|| key == "build-file"
		|| key == "parallel-count"
		|| key == "bash-script"
		|| key == "logger") {
			continue;
		}

		if(key == "generate") {
			for(const auto& value : values) {
				if(value == "cdt-project") {
					if(hasOptionArchitectures == false || optionArchitectures.empty()) {
				        throw esl::system::Stacktrace::add(std::runtime_error("ERROR: missing architecture. An architecture has to be specified if calling \"generate cdt-project\"."));
					}
					if(optionArchitectures.size() > 1) {
				        throw esl::system::Stacktrace::add(std::runtime_error("ERROR: too many architectures. There has been only one architecture be specified if calling \"generate cdt-project\"."));
					}

					if(hasOptionVariants && optionVariants.empty()) {
				        throw esl::system::Stacktrace::add(std::runtime_error("ERROR: This should never happen. Variants have been specified but variants list is empty."));
					}

					std::string architecture = *optionArchitectures.begin();
					getBuildManager().generateCdtProject(optionVariants, architecture);
				}
				else if(value == "make"
				|| value == "cmake"
				|| value == "meson") {
					continue;
				}
				else {
			        throw esl::system::Stacktrace::add(std::runtime_error("ERROR: unknown generator: arguments[" + std::to_string(index) + "] = \"" + arguments[index] + "\"."));
				}
			}
			continue;
		}

		throw esl::system::Stacktrace::add(std::runtime_error("ERROR: unknown argument \"" + key + "\"."));
	}

	return exitCode;
}

const std::size_t Transformer::npos = std::numeric_limits<std::size_t>::max();

const model::Descriptor& Transformer::getDescriptor() const {
	if(!descriptor) {
        throw esl::system::Stacktrace::add(std::runtime_error("ERROR: Descriptor not loaded."));
	}

	return *descriptor.get();
}

void Transformer::printDescriptor() const {
	std::cout << "Descriptor:\n===========\n";
	repository::DescriptorWriter descriptorWriter(getDescriptor());
	descriptorWriter.write(std::cout);
	std::cout << "\n\n";
}

build::BuildManager& Transformer::getBuildManager() {
	if(!buildManager) {
		if(hasOptionVariants && optionVariants.empty()) {
	        throw esl::system::Stacktrace::add(std::runtime_error("ERROR: Variants have been specified but variant-list is empty."));
		}

		if(hasOptionArchitectures && optionArchitectures.empty()) {
	        throw esl::system::Stacktrace::add(std::runtime_error("ERROR: Architectures have been specified but architecture-list is empty."));
		}

		buildManager.reset(new transformer::build::BuildManager(database, getDescriptor(), optionVariants, optionArchitectures));
		buildManager->addBuilderFactory(builderFactoryGCC);
	}

	return *buildManager.get();
}

std::string Transformer::readValue(std::size_t& index) const {
	std::size_t pos = arguments[index].find('=');
	if(pos != std::string::npos) {
		return arguments[index].substr(pos+1);
	}

	if(arguments.size() <= index+1) {
		return "";
	}

	++index;
	return arguments[index];
}
#if 1
std::vector<std::string> Transformer::readValues(std::size_t& index) const {
	return esl::utility::String::split(readValue(index), ',', true);
}
#else
std::pair<std::vector<std::string>,std::size_t> Transformer::readValues(std::size_t& index) const {
	std::pair<std::vector<std::string>,std::size_t> result;

	result.second = npos;
	std::size_t pos = arguments[index].find('=');
	if(pos == std::string::npos) {
		if(arguments.size() <= index+1) {
			result.second = index;
			return result;
		}
		++index;
		result.first  = splitValues(arguments[index]);
	}
	else {
		result.first  = splitValues(arguments[index].substr(pos+1));
	}

	return result;
}

std::vector<std::string> Transformer::splitValues(std::string values) {
	std::vector<std::string> result;

	std::size_t pos = values.find(',');
	result.emplace_back(values.substr(0, pos));

	while(pos != std::string::npos) {
		values = values.substr(pos+1);
		pos = values.find(',');
		result.emplace_back(values.substr(0, pos));
	}

	return result;
}
#endif

} /* namespace transformer */
