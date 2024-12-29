#include <transformer/architectures/Config.h>
#include <transformer/Config.h>
#include <transformer/Execute.h>
#include <transformer/Logger.h>
#include <transformer/repository/DescriptorWriter.h>
#include <transformer/repository/RepositoryLocal.h>
#include <transformer/Transformer.h>

#include <esl/monitoring/Logging.h>
#include <esl/plugin/Registry.h>
#include <esl/system/Stacktrace.h>
#include <esl/utility/String.h>

#include <iostream>
#include <limits>
#include <stdexcept>
#include <cstdlib> // getenv(...)

//#include <transformer/architectures/solver/Solver.h>
namespace transformer {
namespace {
Logger logger("transformer::Transformer");
/*
void printBuildConfigTest(const architectures::Specifiers& specifiers) {
	architectures::solver::Solver solver(specifiers, {"c++11", "linux", "gcc"});
	std::map<std::set<std::string>, architectures::Architecture> architectures = solver.getBuildArchitectures();

    std::cout << architectures.size() << " architectures found:\n";
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
*/

std::string readValue(std::size_t& index, const std::vector<std::string>& arguments) {
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

std::vector<std::string> readValues(const std::string& value) {
	return esl::utility::String::split(value, ',', true);
}

void setValue(std::string& dst, const std::string& src, const std::string& key) {
	if(!dst.empty()) {
		throw esl::system::Stacktrace::add(std::runtime_error("Multiple definition of parameter '" + key + "'"));
	}

	dst = src;

	if(dst.empty()) {
		throw esl::system::Stacktrace::add(std::runtime_error("Invalid value \"\" for parameter '" + key + "'"));
	}
}

void setValue(unsigned short& dst, const std::string& src, const std::string& key) {
	if(dst == 0) {
		throw esl::system::Stacktrace::add(std::runtime_error("Multiple definition of parameter '" + key + "'"));
	}

	int i = std::stoi(src);
	if(i < 0 || i > std::numeric_limits<unsigned short>::max()) {
		throw esl::system::Stacktrace::add(std::runtime_error("Invalid value \"" + src + "\" for parameter '" + key + "'"));
	}
	dst = static_cast<unsigned short>(i);
}

void setValue(std::set<std::string>& dst, const std::string& src, const std::string& key) {
	if(!dst.empty()) {
		throw esl::system::Stacktrace::add(std::runtime_error("Multiple definition of parameter '" + key + "'"));
	}

	std::vector<std::string> values = readValues(src);
	for(const auto& value : values) {
		if(value.empty()) {
			continue;
		}
		dst.insert(value);
	}
	if(dst.empty()) {
        throw esl::system::Stacktrace::add(std::runtime_error("Missing argument for parameter '" + key + "'"));
	}
}

void setValue(std::set<Transformer::Generators>& dst, const std::string& src, const std::string& key) {
	if(!dst.empty()) {
		throw esl::system::Stacktrace::add(std::runtime_error("Multiple definition of parameter '" + key + "'"));
	}

	std::vector<std::string> values = readValues(src);
	for(const auto& value : values) {
		if(value == "cdt-project") {
			dst.insert(Transformer::Generators::cdtProject);
		}
		else if(value == "make") {
			dst.insert(Transformer::Generators::make);
		}
		else if(value == "cmake") {
			dst.insert(Transformer::Generators::cmake);
		}
		else if(value == "meson") {
			dst.insert(Transformer::Generators::meson);
		}
		else if(!value.empty()) {
			throw esl::system::Stacktrace::add(std::runtime_error("Invalid value \"" + src + "\" for parameter '" + key + "'"));
		}
	}
	if(dst.empty()) {
        throw esl::system::Stacktrace::add(std::runtime_error("Missing argument for parameter '" + key + "'"));
	}
}
} /* anonymous namespace */


Transformer::Settings::Settings(int argc, char **argv) {
	std::vector<std::string> arguments;
	for(int i=0; i<argc; ++i) {
		arguments.push_back(argv[i]);
	}

	for(std::size_t index = 0; index < arguments.size(); ++index) {
		if(arguments[index] == "clean") {
			commands.push_back(Transformer::Commands::clean);
		}
		else if(arguments[index] == "dependencies") {
			commands.push_back(Transformer::Commands::dependencies);
		}
		else if(arguments[index] == "generate-sources") {
			commands.push_back(Transformer::Commands::generateSources);
		}
		else if(arguments[index] == "compile") {
			commands.push_back(Transformer::Commands::compile);
		}
		else if(arguments[index] == "test") {
			commands.push_back(Transformer::Commands::test);
		}
		else if(arguments[index] == "link") {
			commands.push_back(Transformer::Commands::link);
		}
		else if(arguments[index] == "site") {
			commands.push_back(Transformer::Commands::site);
		}
		else if(arguments[index] == "package") {
			commands.push_back(Transformer::Commands::package);
		}
		else if(arguments[index] == "install") {
			commands.push_back(Transformer::Commands::install);
		}
		else if(arguments[index] == "provide") {
			commands.push_back(Transformer::Commands::provide);
		}


		else if(arguments[index] == "only-generate-sources") {
			commands.push_back(Transformer::Commands::onlyGenerateSources);
		}
		else if(arguments[index] == "only-compile") {
			commands.push_back(Transformer::Commands::onlyCompile);
		}
		else if(arguments[index] == "only-test") {
			commands.push_back(Transformer::Commands::onlyTest);
		}
		else if(arguments[index] == "only-link") {
			commands.push_back(Transformer::Commands::onlyLink);
		}
		else if(arguments[index] == "only-site") {
			commands.push_back(Transformer::Commands::onlySite);
		}
		else if(arguments[index] == "only-package") {
			commands.push_back(Transformer::Commands::onlyPackage);
		}
		else if(arguments[index] == "only-install") {
			commands.push_back(Transformer::Commands::onlyInstall);
		}
		else if(arguments[index] == "only-provide") {
			commands.push_back(Transformer::Commands::onlyProvide);
		}
		else {
			std::size_t pos = arguments[index].find('=');
			std::string key = arguments[index].substr(0, pos);

			/* if arguments[index] == "<key>=<more>"
			 *    return '<more>'
			 * if arguments[index] == "<key>"
			 *    return arguments[index+1] and increment index-reference
			 */
			const std::string value = readValue(index, arguments);


			if(key == "build-file") {
				setValue(buildFile, value, key);
			}
			else if(key == "bash-script") {
				setValue(scriptFile, value, key);
			}
			else if(key == "logger") {
				setValue(loggerFile, value, key);
			}
			else if(key == "architecture") {
				setValue(optionArchitectures, value, key);
			}
			else if(key == "variant") {
				setValue(optionVariants, value, key);
			}
			else if(key == "parallel-count") {
				setValue(parallelCount, value, key);
			}
			else if(key == "generate") {
				setValue(generators, value, key);
			}
			else {
				throw std::invalid_argument("unknown argument \"" + arguments[index] + "\"");
			}
		}
	}

	if(buildFile.empty()) {
		buildFile = "tbuild.cfg";
	}

	if(parallelCount == 0) {
		parallelCount = 20;
	}

	if(!loggerFile.empty()) {
	    esl::monitoring::Logging* logging = esl::plugin::Registry::get().findObject<esl::monitoring::Logging>();
		if(logging) {
			logging->addFile(loggerFile);
		}
	}
}

Transformer::Transformer(int argc, char **argv)
: settings(argc, argv)
{
	const char* envVar = nullptr;

	envVar = std::getenv("TBUILD_HOME");
	if(envVar) {
		localRepoPath = envVar;
	}
	else {
		envVar = std::getenv("HOME");
		if(envVar == nullptr) {
			throw std::runtime_error("No HOME variable found");
		}
		localRepoPath = std::filesystem::path(envVar) / ".tbuild";
	}
	logger.info << "localRepoPath=\"" << localRepoPath.generic_string() << "\"\n";

	std::filesystem::path architecturesPath;
	envVar = std::getenv("TBUILD_ARCHITECTURES");
	if(envVar) {
		architecturesPath = envVar;
	}
	else {
		architecturesPath = localRepoPath / "architecture-global.cfg";
	}
	logger.info << "architecturesPath=\"" << architecturesPath.generic_string() << "\"\n";

	/* define local and remote repositories */
	database.setLocalRepositoryPath(localRepoPath);
	/*
		static transformer::repository::RepositoryLocal repositoryCodeHouse;
		repositoryCodeHouse.setPath("/home/user/.xbuild-codehouse");
		database.addRepository("codehouse", repositoryCodeHouse);
	*/

	/* load architectures */
	{
		architectures::Config config(specifiers);
		config.load(architecturesPath);
		//config.save(std::cout);
	}
	//specifiers.print();
	//printBuildConfigTest(specifiers);
}

int Transformer::run() {
	int exitCode = 0;

	loadDescriptor();

	if(!settings.scriptFile.empty()) {
		Execute::run("rm -r " + settings.scriptFile);
		Execute::run("touch " + settings.scriptFile);
		Execute::run("chmod +x " + settings.scriptFile);
		Config::scriptFile = settings.scriptFile;
		Execute::addLineToScript("#!/bin/sh");
		Execute::addLineToScript("");
		Execute::addLineToScript("set -x");
	}

	if(settings.generators.count(Transformer::Generators::cdtProject) > 0) {
		if(settings.optionArchitectures.empty()) {
	        throw esl::system::Stacktrace::add(std::runtime_error("Missing architecture. An architecture has to be specified if calling \"generate cdt-project\"."));
		}
		if(settings.optionArchitectures.size() > 1) {
	        throw esl::system::Stacktrace::add(std::runtime_error("Too many architectures. There has been only one architecture be specified if calling \"generate cdt-project\"."));
		}

		std::string architecture = *settings.optionArchitectures.begin();
		getBuildManager().generateCdtProject(settings.optionVariants, architecture);
	}

	/* not read and execute commands */
	for(auto command : settings.commands) {
		switch(command) {
		case Commands::clean:
			getBuildManager().makeClean();
			break;
		case Commands::dependencies:
			exitCode = getBuildManager().makeDependencies();
			break;
		case Commands::generateSources:
			exitCode = getBuildManager().makeGenerateSources();
			break;
		case Commands::compile:
			exitCode = getBuildManager().makeCompile();
			break;
		case Commands::test:
			exitCode = getBuildManager().makeTest();
			break;
		case Commands::link:
			exitCode = getBuildManager().makeLink();
			break;
		case Commands::site:
			exitCode = getBuildManager().makeSite();
			break;
		case Commands::package:
			exitCode = getBuildManager().makePackage();
			break;
		case Commands::install:
			exitCode = getBuildManager().makeInstall();
			break;
		case Commands::provide:
			exitCode = getBuildManager().makeProvide();
			break;
		case Commands::onlyDependencies:
			exitCode = getBuildManager().onlyDependencies();
			break;
		case Commands::onlyGenerateSources:
			exitCode = getBuildManager().onlyGenerateSources();
			break;
		case Commands::onlyCompile:
			exitCode = getBuildManager().onlyCompile();
			break;
		case Commands::onlyTest:
			exitCode = getBuildManager().onlyTest();
			break;
		case Commands::onlyLink:
			exitCode = getBuildManager().onlyLink();
			break;
		case Commands::onlySite:
			exitCode = getBuildManager().onlySite();
			break;
		case Commands::onlyPackage:
			exitCode = getBuildManager().onlyPackage();
			break;
		case Commands::onlyInstall:
			exitCode = getBuildManager().onlyInstall();
			break;
		case Commands::onlyProvide:
			exitCode = getBuildManager().onlyProvide();
			break;
		}
	}

	return exitCode;
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
	std::cout << "\n";
	std::cout << "environment variables:\n";
	std::cout << "  TBUILD_HOME            Defines directory for local repository. Default is \"${HOME}/.tbuild/\" .\n";
	std::cout << "  TBUILD_ARCHITECTURES   Defines config file for architecture definitions. Default is \"${TBUILD_HOME}/architecture-global.cfg\"\n";
	std::cout << "\n";
}

const std::size_t Transformer::npos = std::numeric_limits<std::size_t>::max();

void Transformer::loadDescriptor() {
	descriptor = database.loadDescriptor(settings.buildFile);

	if(descriptor) {
		printDescriptor();
	}
	else {
		std::cerr << "failed to load file \"" << settings.buildFile << "\n";
		buildManager.reset();
	}
}

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
		buildManager.reset(new transformer::build::BuildManager(database, getDescriptor(), settings.optionVariants, settings.optionArchitectures));
		buildManager->addBuilderFactory(builderFactoryGCC);
	}

	return *buildManager.get();
}

} /* namespace transformer */
