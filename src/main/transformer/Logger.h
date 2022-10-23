#ifndef TRANSFORMER_LOGGER_H_
#define TRANSFORMER_LOGGER_H_

#include <esl/logging/Logger.h>
#include <esl/logging/Level.h>

namespace transformer {

using Logger = esl::logging::Logger<esl::logging::Level::TRACE>;

//extern Logger logger;

} /* namespace transformer */

#endif /* TRANSFORMER_LOGGER_H_ */
