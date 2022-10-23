#include <transformer/Process.h>
#include <transformer/Config.h>

#include <esl/io/Input.h>
#include <esl/plugin/Registry.h>

#include <iostream>
#include <ostream>

namespace transformer {

Process::Process(const std::string& aCommand)
: command(aCommand),
  processPtr(esl::plugin::Registry::get().create<esl::system::Process>(Config::eslProcessImplementation, {})),
  processThread(std::thread([this]() { this->run(); }))
{ }

Process::~Process() {
	if(processThread.joinable()) {
		processThread.join();
	}
}

bool Process::isDone() const {
	return done;
}

int Process::getExitCode() const {
	if(!done && processThread.joinable()) {
		processThread.join();
	}
	return exitCode;
}

const std::string& Process::getCommand() const noexcept {
	return command;
}

void Process::run() {
	esl::system::Process& process = *processPtr;
	process[esl::system::FileDescriptor::getOut()] >> esl::io::Input();
	process[esl::system::FileDescriptor::getErr()] >> esl::io::Input();
	try {
		exitCode = process.execute(esl::system::Arguments(command));
	}
	catch(...) {
		exitCode = 1;
	}
	done.store(true);
}

} /* namespace transformer */

#ifdef USE_BOOST_PROCESS

#include <transformer/Process.h>

#include <boost/process.hpp>

#include <iostream>
#include <string>
#include <utility>
#include <vector>


namespace transformer {

class ProcessBoost : public Process {
public:
	ProcessBoost(const std::string& aCommand, bool aShowExecuteCommand);
	~ProcessBoost();

	bool process() override;
	int getExitCode() const override;

private:
	bool processPipe(boost::process::ipstream& pipe, bool& isFirstLine, std::string& line, bool isStdError);

	bool showExecuteCommand;

	std::string command;
	boost::process::ipstream pipeStdOutStream;
	boost::process::ipstream pipeStdErrStream;
//	boost::process::group group;
	boost::process::child child;

	bool isRunning = true;
	bool hasCommandPrinted = false;
	bool isFirstStdOutLine = true;
	bool isFirstStdErrLine = true;
	std::string lineStdOut;
	std::string lineStdErr;
	std::vector<std::pair<std::string, bool>> lines;
	int exitCode = 0;
};

ProcessBoost::ProcessBoost(const std::string& aCommand, bool aShowExecuteCommand)
: showExecuteCommand(aShowExecuteCommand),
  command(aCommand),
  child(aCommand/*, group*/, boost::process::std_out > pipeStdOutStream, boost::process::std_err > pipeStdErrStream)
{
	if(showExecuteCommand) {
		std::cout << command << std::endl;
	}
}

ProcessBoost::~ProcessBoost() {
	if(isRunning) {
//		group.terminate();
		child.wait();
	}
}

bool ProcessBoost::process() {
	bool hasProcessed = false;
	hasProcessed |= processPipe(pipeStdOutStream, isFirstStdOutLine, lineStdOut, false);
	hasProcessed |= processPipe(pipeStdErrStream, isFirstStdErrLine, lineStdErr, true);
	if(hasProcessed || child.running()) {
		return false;
	}
	processPipe(pipeStdOutStream, isFirstStdOutLine, lineStdOut, false);
	processPipe(pipeStdErrStream, isFirstStdErrLine, lineStdErr, true);

	if(!lines.empty()) {
		if(showExecuteCommand == false) {
			std::cout << command << std::endl;
		}
		for(const auto& line : lines) {
			if(line.second) {
				std::cerr << line.first << std::endl;
			}
			else {
				std::cout << line.first << std::endl;
			}
		}
	}
	if(isRunning) {
		isRunning = false;
//		group.terminate();
		child.wait();
		exitCode = child.exit_code();
	}
	return true;
}

int ProcessBoost::getExitCode() const {
	return exitCode;
}

/* return true if there was something to process. otherwise false is returned */
bool ProcessBoost::processPipe(boost::process::ipstream& pipe, bool& isFirstLine, std::string& line, bool isStdError) {
	if(pipe) {
		if(isFirstLine) {
			isFirstLine = false;
		}
		else {
			lines.push_back(std::make_pair(line, isStdError));
		}
		std::getline(pipe, line);
		return !line.empty();
	}
	else if(!line.empty()) {
		lines.push_back(std::make_pair(line, isStdError));
		line.clear();
		return true;
	}
	return false;
}

} /* namespace transformer */

#endif
