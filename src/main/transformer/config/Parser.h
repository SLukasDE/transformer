#ifndef TRANSFORMER_CONFIG_PARSER_H_
#define TRANSFORMER_CONFIG_PARSER_H_

#include <filesystem>
#include <fstream>
#include <istream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>


namespace transformer {
namespace config {

class Parser {
public:
	Parser(std::filesystem::path path);
	Parser(std::istream& iStream);

	bool nextLine();

	std::size_t getLineNo() const;
	const std::string& getLine() const;

	/* Return value signals if a comment symbol occurred while reading. It's not a hint if no characters occur anymore.
	 *
	 * returns true if a comment symbol occurs while reading, even if it's directly the first symbol (then token is empty).
	 * returns false if no comment symbol occurred while reading, even if it's the last token and there are no more symbols.
	 */
	bool readToken(std::string& token);
	void readLastToken(std::string& token);
	std::vector<std::string> readList();
#if 0
	static bool readToken(std::string& token, std::istringstream& iss);
	static void readLastToken(std::string& token, std::istringstream& iss);
	static std::vector<std::string> readList(std::istringstream& iss);
#endif
	static std::pair<std::string, std::string> splitTokenWithOption(const std::string& str);

private:
	std::filesystem::path path;
	std::ifstream ifStream;

	std::istream& iStream;

	std::size_t lineNo = 0;
	bool isEof = false;
	std::string line;
	std::unique_ptr<std::istringstream> iss;
};

} /* namespace config */
} /* namespace transformer */

#endif /* TRANSFORMER_CONFIG_PARSER_H_ */
