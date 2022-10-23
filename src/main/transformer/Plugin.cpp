#include <transformer/Config.h>

#include <transformer/Plugin.h>
#include <transformer/system/ProcessBoost.h>
#include <transformer/system/ProcessSystem.h>

#include <esl/system/Process.h>

#include <memory>

namespace transformer {

void Plugin::install(esl::plugin::Registry& registry, const char* data) {
	esl::plugin::Registry::set(registry);

#ifdef USE_BOOST_PROCESS
	registry.addPlugin<esl::system::Process>(
			"transformer/system/ProcessBoost",
			system::ProcessBoost::create);
#endif
	registry.addPlugin<esl::system::Process>(
			"transformer/system/ProcessSystem",
			system::ProcessSystem::create);
}

} /* namespace transformer */
