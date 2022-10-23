#include <transformer/system/ProcessBoost.h>

#ifdef USE_BOOST_PROCESS
#include <esl/system/Stacktrace.h>

#include <boost/process.hpp>

#include <stdexcept>

namespace transformer {
namespace system {

std::unique_ptr<esl::system::Process> ProcessBoost::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<esl::system::Process>(new ProcessBoost(settings));
}

ProcessBoost::ProcessBoost(const std::vector<std::pair<std::string, std::string>>& settings)
{
    for(const auto& setting : settings) {
        throw esl::system::Stacktrace::add(std::runtime_error("ProcessBoost: unknown attribute '\"" + setting.first + "\"'."));
    }
}

esl::system::Transceiver& ProcessBoost::operator[](const esl::system::FileDescriptor& fd) {
	return dummyTransceiver;
}

void ProcessBoost::setWorkingDir(std::string workingDir) {
}

void ProcessBoost::setEnvironment(std::unique_ptr<esl::system::Environment> environment) {
}

const esl::system::Environment* ProcessBoost::getEnvironment() const {
	return nullptr;
}

void ProcessBoost::addFeature(esl::object::Object& feature) {
}

int ProcessBoost::execute(esl::system::Arguments arguments) const {
	boost::process::child child(arguments.getArgs());
	child.wait();
	return child.exit_code();
}

void ProcessBoost::sendSignal(const esl::utility::Signal& signal) const {
}

const void* ProcessBoost::getNativeHandle() const {
	return nullptr;
}

} /* namespace system */
} /* namespace transformer */
#endif
