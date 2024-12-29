#include <transformer/system/ProcessSystem.h>

#include <esl/system/Stacktrace.h>

#include <cstdlib>
#include <stdexcept>

namespace transformer {
namespace system {

std::unique_ptr<esl::system::Process> ProcessSystem::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<esl::system::Process>(new ProcessSystem(settings));
}

ProcessSystem::ProcessSystem(const std::vector<std::pair<std::string, std::string>>& settings)
{
    for(const auto& setting : settings) {
        throw esl::system::Stacktrace::add(std::runtime_error("ProcessSystem: unknown attribute '\"" + setting.first + "\"'."));
    }
}

esl::system::Transceiver& ProcessSystem::operator[](const esl::system::FileDescriptor& fd) {
	return dummyTransceiver;
}

void ProcessSystem::setWorkingDir(std::string workingDir) {
}

void ProcessSystem::setEnvironment(std::unique_ptr<esl::system::Environment> environment) {
}

const esl::system::Environment* ProcessSystem::getEnvironment() const {
	return nullptr;
}

void ProcessSystem::addFeature(esl::object::Object& feature) {
}

int ProcessSystem::execute(esl::system::Arguments arguments) const {
	return std::system(arguments.getArgs().c_str());
}

void ProcessSystem::sendSignal(const esl::system::Signal& signal) const {
}

const void* ProcessSystem::getNativeHandle() const {
	return nullptr;
}


} /* namespace system */
} /* namespace transformer */
