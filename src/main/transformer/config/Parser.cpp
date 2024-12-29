#include <transformer/config/Parser.h>
#include <transformer/config/Exception.h>

#include <esl/utility/String.h>

#include <stdexcept>

namespace transformer {
namespace config {

namespace {

//enum Context { cxMain, cxVariant, cxSolved };
bool readCheckIfEmpty(std::istringstream& iss) {
	std::string token;

	iss >> token;
	token = esl::utility::String::trim(token);

	return token.empty() || (token.at(0) == '#');
}
/*
bool isComment(const std::string& str) {
	if(str.empty()) {
		return false;
	}
	// is comment
	return (str.at(0) == '#');
}
*/
}

Parser::Parser(std::filesystem::path aPath)
: path(std::move(aPath)),
  ifStream(path.string()),
  iStream(ifStream)
{
	if(!std::filesystem::exists(path)) {
		throw std::runtime_error("File \"" + path.generic_string() + "\" does not exists.");
	}
	if(!std::filesystem::is_regular_file(path)) {
		throw std::runtime_error("Path \"" + path.generic_string() + "\" is not a file.");
	}
}

Parser::Parser(std::istream& aIStream)
: iStream(aIStream)
{ }

bool Parser::nextLine() {
	if(isEof) {
		return false;
	}

	if(!std::getline(iStream, line))  {
		isEof = true;
		return false;
	}

	++lineNo;
	iss = std::unique_ptr<std::istringstream>(new std::istringstream(line));

	return true;
/*
	while(std::getline(iStream, line))  {
		++lineNo;

		std::istringstream iss(line);
		std::string token;

		iss >> token;

		// is empty line
		if(token.empty()) {
			continue;
		}

		// is comment
		if(token.at(0) == '#') {
			continue;
		}
	}
*/
}

std::size_t Parser::getLineNo() const {
	return lineNo;
}

const std::string& Parser::getLine() const {
	return line;
}

bool Parser::readToken(std::string& token) {
	if(isEof) {
		throw Exception(*this, "EOF");
	}

	std::string tmpToken;
	*iss >> tmpToken;

	token.clear();
	for(std::size_t index = 0; index < tmpToken.size(); ++index) {
		if(tmpToken.at(index) == '#') {
			return true;
		}

		token += tmpToken.at(index);
	}

	return false;
}

void Parser::readLastToken(std::string& token) {
	bool hasComment = readToken(token);
	if(!hasComment && !token.empty() && !readCheckIfEmpty(*iss)) {
		throw std::runtime_error("unknown text after '" + token + "'");
	}
}

std::vector<std::string> Parser::readList() {
	std::vector<std::string> result;
	std::string token;
	bool hasComment = false;

	while(!hasComment) {
		hasComment = readToken(token);
		if(token.empty()) {
			break;
		}
		result.push_back(token);
	}

	return result;
}








#if 0
bool Parser::readToken(std::string& token, std::istringstream& iss) {
	std::string tmpToken;
	iss >> tmpToken;

	token.clear();
	for(std::size_t index = 0; index < tmpToken.size(); ++index) {
		if(tmpToken.at(index) == '#') {
			return true;
		}

		token += tmpToken.at(index);
	}

	return false;
}

void Parser::readLastToken(std::string& token, std::istringstream& iss) {
	bool hasComment = readToken(token, iss);
	if(!hasComment && !token.empty() && !readCheckIfEmpty(iss)) {
		throw std::runtime_error("unknown text after '" + token + "'");
	}
}

std::vector<std::string> Parser::readList(std::istringstream& iss) {
	std::vector<std::string> result;
	std::string token;
	bool hasComment = false;

	while(!hasComment) {
		hasComment = readToken(token, iss);
		if(token.empty()) {
			break;
		}
		result.push_back(token);
	}

	return result;
}
#endif

std::pair<std::string, std::string> Parser::splitTokenWithOption(const std::string& str) {
	std::pair<std::string, std::string> rv;

	bool hasOption = false;

	for(std::size_t index = 0; index < str.size(); ++index) {
		if(str.at(index) == '[') {
			if(hasOption) {
				throw std::runtime_error("multiple definition of option");
			}
			hasOption = true;

			while(true) {
				++index;
				if(index >= str.size()) {
					throw std::runtime_error("incomplete definition of option");
				}
				if(str.at(index) == ']') {
					break;
				}
				rv.second += str.at(index);
			}
			continue;
		}

		if(hasOption) {
			throw std::runtime_error("invalid character after definition of option");
		}

		rv.first += str.at(index);
	}

	return rv;
}

} /* namespace config */
} /* namespace transformer */
