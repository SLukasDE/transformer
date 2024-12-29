#include <transformer/Process.h>
#include <transformer/Config.h>

#include <esl/io/Input.h>
#include <esl/plugin/Registry.h>

#include <esl/system/Arguments.h>
#include <esl/system/FileDescriptor.h>

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
