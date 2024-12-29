#include <transformer/Config.h>
#include <transformer/config/Exception.h>
#include <transformer/Logger.h>
#include <transformer/Transformer.h>
#include <transformer/Plugin.h>

#include <esl/monitoring/Logging.h>
#include <esl/plugin/Registry.h>
#include <esl/plugin/exception/PluginNotFound.h>
#include <esl/system/Stacktrace.h>

#ifndef NO_OPENESL
#include <openesl/Plugin.h>
#include <esl/monitoring/LogbookLogging.h>
#endif

#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

transformer::Logger logger("::");

int main(int argc, char **argv) {
	int rc = 1;

	if(argc < 2) {
		std::cerr << "wrong numbers of arguments\n";
		transformer::Transformer::printUsage();
		return 1;
	}


    struct RegistryGuard {
        ~RegistryGuard() {
            esl::plugin::Registry::cleanup();
        }
    } registryGuard;
    esl::plugin::Registry& registry(esl::plugin::Registry::get());

	try {
#ifndef NO_OPENESL
        openesl::Plugin::install(registry, nullptr);

        auto logging = esl::monitoring::LogbookLogging::createNative();
        registry.setObject(std::move(logging));
#endif
		transformer::Plugin::install(registry, nullptr);

		transformer::Transformer transformer(argc-1, &argv[1]);
#if 0
		std::size_t checkResult = transformer.checkArguments();
		if(checkResult != transformer::Transformer::npos) {
			std::cerr << "wrong argument \"" << argv[checkResult+1] << "\" or missing next argument\n";
			transformer::Transformer::printUsage();
		}
		else {
		}
#else
		rc = transformer.run();
#endif
	}
    catch(const esl::plugin::exception::PluginNotFound& e) {
        std::cerr << "Plugin not found exception occurred: " << e.what() << "\n";
        const esl::plugin::Registry::BasePlugins& basePlugins = esl::plugin::Registry::get().getPlugins(e.getTypeIndex());
        if(basePlugins.empty()) {
            std::cerr << "No implementations available.\n";
        }
        else {
            std::cerr << "Implementations available:\n";
            for(const auto& basePlugin : basePlugins) {
                std::cerr << "- " << basePlugin.first << "\n";
            }
        }
    }
	catch (const std::invalid_argument& e) {
		std::cerr << e.what() << "\n";
		transformer::Transformer::printUsage();
	}
	catch (const std::exception& e) {
    	std::cerr << "std::exception exception occurred\n";
    	std::cerr << e.what() << std::endl;;

		const esl::system::Stacktrace* stacktrace = esl::system::Stacktrace::get(e);
    	if(stacktrace) {
    		stacktrace->dump(std::cerr);
    	}
	}
	catch (...) {
		std::cerr << "Unknown exception occurred\n";
	}

    esl::monitoring::Logging* logging = registry.findObject<esl::monitoring::Logging>();
	if(rc != 0 && logging) {
		std::stringstream sstream;
		logging->flush(&sstream);
		if(!sstream.str().empty()) {
			std::cerr << "Replay logger:\n";
			std::cerr << sstream.str() << "\n";
		}
	}

	return rc;
}
