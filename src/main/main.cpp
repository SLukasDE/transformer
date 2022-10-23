#include <transformer/Config.h>
#include <transformer/config/Exception.h>
#include <transformer/Logger.h>
#include <transformer/Transformer.h>
#include <transformer/Plugin.h>

#include <esl/plugin/Registry.h>
#include <esl/system/Stacktrace.h>

#ifndef NO_ESLX
#include <eslx/Plugin.h>
#endif

#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

transformer::Logger logger("::");

int main(int argc, char **argv) {
	if(argc < 2) {
		std::cerr << "wrong numbers of arguments\n";
		transformer::Transformer::printUsage();
		return 1;
	}


	int rc = 1;

	try {
#ifndef NO_ESLX
        //esl::plugin::Registry::get().loadPlugin("/usr/lib64/libeslx.so.1.5", nullptr);
		eslx::Plugin::install(esl::plugin::Registry::get(), nullptr);
	    esl::system::Stacktrace::init("eslx/system/Stacktrace", {});
#endif
		transformer::Plugin::install(esl::plugin::Registry::get(), nullptr);

		transformer::Transformer transformer(argc-1, &argv[1]);
		transformer.loadSettings();

		std::size_t checkResult = transformer.checkArguments();
		if(checkResult != transformer::Transformer::npos) {
			std::cerr << "wrong argument \"" << argv[checkResult+1] << "\" or missing next argument\n";
			transformer::Transformer::printUsage();
		}
		else {
			rc = transformer.run();
		}
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

	if(rc != 0 && esl::logging::Logging::get()) {
		std::stringstream sstream;
		esl::logging::Logging::get()->flush(&sstream);
		if(!sstream.str().empty()) {
			std::cerr << "Replay logger:\n";
			std::cerr << sstream.str() << "\n";
		}
	}
	esl::plugin::Registry::cleanup();

	return rc;
}
