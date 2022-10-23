#include <transformer/config/Exception.h>

namespace transformer {
namespace config {

namespace {
static constexpr const char* defaultMessage = "parser error";
}

Exception::Exception(const Parser& parser)
: std::runtime_error(defaultMessage),
  lineNo(parser.getLineNo()),
  line(parser.getLine())
{ }

Exception::Exception(const Parser& parser, const std::string& message)
: std::runtime_error(message),
  lineNo(parser.getLineNo()),
  line(parser.getLine())
{ }

Exception::Exception(const Parser& parser, const char* message)
: std::runtime_error(message),
  lineNo(parser.getLineNo()),
  line(parser.getLine())
{ }

std::size_t Exception::getLineNo() const {
	return lineNo;
}

const std::string& Exception::getLine() const {
	return line;
}

} /* namespace config */
} /* namespace transformer */
