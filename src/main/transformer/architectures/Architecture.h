#ifndef TRANSFORMER_ARCHITECTURES_ARCHITECTURE_H_
#define TRANSFORMER_ARCHITECTURES_ARCHITECTURE_H_

#include <transformer/architectures/Language.h>

#include <string>
#include <set>

namespace transformer {
namespace architectures {

class Architecture {
public:
	const std::set<std::string>& getArchitectureIds() const noexcept;
	void addArchitectureId(const std::string& id);

	Language& getLanguage(const std::string& language);
	const Language& getLanguage(const std::string& language) const;

	const std::map<std::string, Language>& getLanguages() const noexcept;

private:
	std::set<std::string> ids;

	std::map<std::string, Language> languages;
};

} /* namespace architectures */
} /* namespace transformer */

#endif /* TRANSFORMER_ARCHITECTURES_ARCHITECTURE_H_ */
