#ifndef TRANSFORMER_LOGGER_H_
#define TRANSFORMER_LOGGER_H_

#include <esl/monitoring/Streams.h>
#include <esl/monitoring/Logger.h>

namespace transformer {

using Logger = esl::monitoring::Logger<esl::monitoring::Streams::Level::trace>;

//extern Logger logger;

} /* namespace transformer */

#endif /* TRANSFORMER_LOGGER_H_ */
