#ifndef TRANSFORMER_PROCESS_H_
#define TRANSFORMER_PROCESS_H_

#include <transformer/Process.h>

#include <esl/system/Process.h>

#include <atomic>
#include <memory>
#include <string>
#include <thread>

namespace transformer {

class Process {
public:
	Process(const std::string& command);
	~Process();

	bool isDone() const;
	int getExitCode() const;
	const std::string& getCommand() const noexcept;

private:
	void run();

	std::string command;
	std::unique_ptr<esl::system::Process> processPtr;
	mutable std::thread processThread;
	int exitCode = 0;
	std::atomic<bool> done{false};
};

} /* namespace transformer */

#endif /* TRANSFORMER_PROCESS_H_ */
