#include <transformer/architectures/Architecture.h>

#include <stdexcept>
#include <utility>

namespace transformer {
namespace architectures {

const std::set<std::string>& Architecture::getArchitectureIds() const noexcept {
	return ids;
}

void Architecture::addArchitectureId(const std::string& id) {
	ids.insert(id);
}

Language& Architecture::getLanguage(const std::string& language) {
	auto iter = languages.find(language);
	if(iter != languages.end()) {
		return iter->second;
	}
	languages.insert(std::make_pair(language, Language(language)));

	return languages.find(language)->second;
}

const Language& Architecture::getLanguage(const std::string& language) const {
	auto iter = languages.find(language);

	if(iter == languages.end()) {
		throw std::runtime_error("Cannot get language \"" + language + "\"");
	}

	return iter->second;
}

const std::map<std::string, Language>& Architecture::getLanguages() const noexcept {
	return languages;
}

} /* namespace architectures */
} /* namespace transformer */
