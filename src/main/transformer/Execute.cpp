#include <transformer/Config.h>
#include <transformer/Execute.h>
#include <transformer/Process.h>

#include <esl/system/Stacktrace.h>

#include <fstream> // DRY_RUN / parallelCount == 0
#include <iostream>
#include <list>
#include <stdexcept>

namespace transformer {

void Execute::mkdir(const std::filesystem::path& path) {
	addLineToScript("mkdir -p " + path.generic_string());

	if(Config::parallelCount > 0) {
		/* create directory if not exists */
		if(!std::filesystem::exists(path)) {
			std::filesystem::create_directories(path);
		}
		else if(!std::filesystem::is_directory(path)) {
			throw esl::system::Stacktrace::add(std::runtime_error(path.generic_string() + " is no directory"));
		}
	}
}

void Execute::remove(const std::filesystem::path& path) {
	addLineToScript("rm -f " + path.generic_string());

	/* remove file (or directory?) */
	if(Config::parallelCount > 0) {
		std::filesystem::remove(path);
	}
}

void Execute::removeAll(const std::filesystem::path& path) {
	addLineToScript("rm -rf " + path.generic_string());

	if(Config::parallelCount > 0) {
		/* remove file or directory recursively */
		std::filesystem::remove_all(path);
	}
}

void Execute::copy(const std::filesystem::path& fromPath, const std::filesystem::path& toPath) {
	addLineToScript("cp -a " + fromPath.generic_string() + " " + toPath.generic_string());

	if(Config::parallelCount > 0) {
		try {
			std::filesystem::copy(fromPath, toPath);
		}
		catch(const std::exception& e) {
			throw esl::system::Stacktrace::add(std::runtime_error(e.what()));
		}
	}
}

void Execute::copyFile(const std::filesystem::path& fromPath, const std::filesystem::path& toPath) {
	addLineToScript("cp " + fromPath.generic_string() + " " + toPath.generic_string());

	if(Config::parallelCount > 0) {
		try {
			std::filesystem::copy_file(fromPath, toPath);
		}
		catch(const std::exception& e) {
			throw esl::system::Stacktrace::add(std::runtime_error(e.what()));
		}
	}
}

void Execute::run(const std::string& command) {
	int exitCode;
	run(command, exitCode);
	if(exitCode != 0) {
		throw esl::system::Stacktrace::add(std::runtime_error("Command \"" + command + "\" failed with exit code " + std::to_string(exitCode)));
	}
}

void Execute::run(const std::string& command, int& exitCode) {
	std::vector<std::string> textCommandList;
	textCommandList.push_back(command);
	exitCode = run(std::move(textCommandList));
}

int Execute::run(std::vector<std::string> commands) {
	for(const auto& command : commands) {
		addLineToScript(command);
	}

	if(Config::parallelCount > 0) {
		using Processes = std::list<std::unique_ptr<Process>>;
		Processes processes;

		for(auto commandIter = std::begin(commands); commandIter != std::end(commands) || !processes.empty() ;) {
			if(commandIter != std::end(commands) && processes.size() < Config::parallelCount) {
				const auto& command = *commandIter;
				std::cout << command << std::endl;

				std::unique_ptr<Process> process(new Process(command));
				processes.push_back(std::move(process));
				++commandIter;
				continue;
			}

			bool noProgress = true;
			for(Processes::iterator iter = std::begin(processes); iter != std::end(processes);) {
				Process* process = iter->get();

				if(process->isDone()) {
					int exitCode = process->getExitCode();
					if(exitCode != 0) {
						//parallelCount = 0;
						return exitCode;
					}
					iter = processes.erase(iter);
					noProgress = false;
					break;
				}

				++iter;
			}

			if(noProgress && !processes.empty()) {
				Processes::iterator iter = std::begin(processes);
				Process* process = iter->get();

				int exitCode = process->getExitCode();
				if(exitCode != 0) {
					//parallelCount = 0;
					return exitCode;
				}
				iter = processes.erase(iter);
			}
		}
	}

	return 0;
}

int Execute::tar(const std::filesystem::path& tarFile, const std::filesystem::path& fromPath) {
	int exitCode;
	run("tar -czf " + tarFile.string() + " -C " + fromPath.string() + " .", exitCode);
	return exitCode;
}

int Execute::untar(const std::filesystem::path& tarFile, const std::filesystem::path& toPath) {
	int exitCode;
	run("tar -xzf " + tarFile.string() + " -C " + toPath.string(), exitCode);
	return exitCode;
}

void Execute::addLineToScript(const std::string& line) {
	if(!Config::scriptFile.empty()) {
		std::ofstream ofs(Config::scriptFile, std::ofstream::app);
		ofs << line << std::endl;
	}
}

} /* namespace transformer */
