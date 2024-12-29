#ifndef TRANSFORMER_SYSTEM_PROCESSSYSTEM_H_
#define TRANSFORMER_SYSTEM_PROCESSSYSTEM_H_

#include <esl/object/Object.h>

#include <esl/system/Arguments.h>
#include <esl/system/Process.h>
#include <esl/system/Transceiver.h>
#include <esl/system/FileDescriptor.h>
#include <esl/system/Environment.h>
#include <esl/system/Signal.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace transformer {
namespace system {

class ProcessSystem : public esl::system::Process {
public:
	static std::unique_ptr<esl::system::Process> create(const std::vector<std::pair<std::string, std::string>>& settings);

	ProcessSystem(const std::vector<std::pair<std::string, std::string>>& settings);

	esl::system::Transceiver& operator[](const esl::system::FileDescriptor& fd) override;

	void setWorkingDir(std::string workingDir) override;
	void setEnvironment(std::unique_ptr<esl::system::Environment> environment) override;
	const esl::system::Environment* getEnvironment() const override;

	void addFeature(esl::object::Object& feature) override;

	int execute(esl::system::Arguments arguments) const override;

	void sendSignal(const esl::system::Signal& signal) const override;
	const void* getNativeHandle() const override;

private:
	esl::system::Transceiver dummyTransceiver;
};


} /* namespace system */
} /* namespace transformer */

#endif /* TRANSFORMER_SYSTEM_PROCESSSYSTEM_H_ */
