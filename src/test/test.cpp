//============================================================================
// Name        : transformer.cpp
// Author      : Sven Lukas
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <transformer/Transformer.h>
#include <iostream>
#include <string>
#include <stdexcept>

int main(int argc, char **argv) {
	int exitCode = 0;

	if(argc < 2) {
		std::cerr << "wrong numbers of arguments\n";
		transformer::Transformer::printUsage();
		return 1;
	}

	transformer::Transformer transformer;

	try
	{
		transformer.loadDescriptor();

		for(int i = 1; i<argc; ++i) {
			if(transformer.checkArgument(argv[i]) == false) {
				std::cerr << "wrong argument \"" << argv[i] << "\"\n";
				transformer::Transformer::printUsage();
				return 1;
			}
		}


		for(int i = 1; i<argc; ++i) {
			exitCode = transformer.runArgument(argv[i]);
			if(exitCode != 0) {
				return exitCode;
			}
		}
	}
	catch (const boost::filesystem::filesystem_error& ex) {
		std::cout << "boost::filesystem::filesystem_error exception occured" << std::endl;
		std::cout << ex.what() << std::endl;
		exitCode = 1;
	}
	catch (const std::runtime_error& ex) {
		std::cout << "std::runtime_error occured" << std::endl;
		std::cout << ex.what() << std::endl;;
		exitCode = 1;
	}
	catch (const std::exception& ex) {
		std::cout << "std::exception exception occured" << std::endl;
		std::cout << ex.what() << std::endl;;
		exitCode = 1;
	}
	catch (...)
	{
		std::cout << "exception occured" << std::endl;
		exitCode = 1;
	}


	std::cout << "done." << std::endl; // prints !!!Hello World!!!
	return exitCode;
}
