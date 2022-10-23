#include <transformer/architectures/Specifier.h>
#include <transformer/architectures/Specifiers.h>
#include <transformer/architectures/Language.h>
#include <transformer/Logger.h>

#include <utility>
#include <list>
#include <stdexcept>
#include <algorithm>

namespace transformer {
namespace architectures {

namespace {
Logger logger("transformer::architectures::Specifier");

struct ArchitectureCompare {
	ArchitectureCompare(std::string aLanguage)
	: language(std::move(aLanguage))
	{ }

	bool operator()(const std::reference_wrapper<const Architecture>& a, const std::reference_wrapper<const Architecture>& b) {
		bool hasDriverA = (a.get().getLanguage(language).getDriver().empty() == false);
		bool hasDriverB = (b.get().getLanguage(language).getDriver().empty() == false);

		if(hasDriverA == hasDriverB) {
			return a.get().getArchitectureIds() < b.get().getArchitectureIds();
		}
		return hasDriverB;
	}

	std::string language;
};
}

Specifier::Specifier(Specifiers& aSpecifiers, std::string aId, Type aType)
: specifiers(aSpecifiers),
  id(std::move(aId)),
  type(aType)
{ }

const std::string& Specifier::getId() const {
	return id;
}

Specifier::Type Specifier::getType() const {
	return type;
}

void Specifier::setType(Type aType) {
	if(type == aType) {
		return;
	}

	switch(type) {
	case isProfile:
		throw std::runtime_error("cannot change level of specifier \"" + id + "\" from profile to architecture.");
	case isArchitecture:
		throw std::runtime_error("cannot change level of specifier \"" + id + "\" from architecture to profile.");
	default:
		type = aType;
	}
}

void Specifier::setAvailability(Availability aAvailability) {
	availability = aAvailability;
}

Specifier::Availability Specifier::getAvailability() const {
	return availability;
}

const Specifier* Specifier::getParent() const {
	if(!id.empty()) {
		for(std::size_t i = id.size()-1; true; --i) {
			std::string subId(id.substr(0, i));
			const Specifier* parentSpecifier = specifiers.getSpecifier(subId);

			if(parentSpecifier && parentSpecifier != this) {
				return parentSpecifier;
			}

			if(i == 0) {
				break;
			}
		}
	}

	return nullptr;
}

std::vector<std::reference_wrapper<const Specifier>> Specifier::getChildren() const {
	std::vector<std::reference_wrapper<const Specifier>> rv;

	for(auto& specifier : specifiers.getSpecifiers()) {
		if(specifier->getParent() == this) {
			rv.push_back(*specifier);
		}
	}

	return rv;
}

void Specifier::addArchitecture(const Architecture& architecture) {
	architectures.insert(std::make_pair(architecture.getArchitectureIds(), std::cref(architecture)));
}

void Specifier::removeArchitecture(const std::set<std::string>& ids) {
	architectures.erase(ids);
}

const Specifier::Architectures& Specifier::getArchitectures() const {
	return architectures;
}

std::vector<std::reference_wrapper<const Architecture>> Specifier::getOrderedArchitectures(const std::string& language) const {
	std::vector<std::reference_wrapper<const Architecture>> rv;

	for(const auto& architectureEntry : architectures) {
		const Architecture& architecture = architectureEntry.second.get();

		if(architecture.getLanguages().find(language) == architecture.getLanguages().end()) {
			continue;
		}

		rv.push_back(std::cref(architecture));
	}

	ArchitectureCompare architectureCompare(language);
	std::sort(rv.begin(), rv.end(), architectureCompare);
	return rv;
}

} /* namespace architectures */
} /* namespace transformer */
