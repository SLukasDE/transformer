#include <transformer/architectures/solver/Languages.h>
#include <transformer/architectures/Architecture.h>
#include <transformer/architectures/Language.h>

#include <vector>

namespace transformer {
namespace architectures {
namespace solver {

Languages::Languages(const Specifiers& specifiers)
{
	/* iterate over all available architectures */
	std::vector<std::reference_wrapper<const Architecture>> architectures = specifiers.getArchitectures();
	for(auto architecture : architectures) {
		for(const auto& language : architecture.get().getLanguages()) {
			if(language.second.getDriver().empty() == false) {
				languages.insert(language.first);
			}
		}
	}
}

Languages::Languages(const std::vector<std::reference_wrapper<const Specifier>>& specifiers)
{
	for(auto specifier : specifiers) {
		const auto& architectures = specifier.get().getArchitectures();

		for(auto architecture : architectures) {
			for(const auto& language : architecture.second.get().getLanguages()) {
				if(language.second.getDriver().empty() == false) {
					languages.insert(language.first);
				}
			}
		}
	}
}

const std::set<std::string>& Languages::getLanguages() const noexcept {
	return languages;
}

} /* namespace solver */
} /* namespace architectures */
} /* namespace transformer */
