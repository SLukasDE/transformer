/* ******************************************* *
 * Internal class just for class Architectures *
 * ******************************************* */

#ifndef TRANSFORMER_ARCHITECTURES_SPECIFIER_H_
#define TRANSFORMER_ARCHITECTURES_SPECIFIER_H_

#include <transformer/architectures/Architecture.h>

#include <string>
#include <functional>
#include <map>
#include <set>
#include <vector>
#include <memory>

namespace transformer {
namespace architectures {

class Specifiers;

class Specifier {
public:
	enum Type {
		isArchitecture,
		isProfile,
		isUndefined
	};

	enum Availability {
		isStandard,
		isDefault,
		isOptional
	};

	using Architectures = std::map<std::set<std::string>, std::reference_wrapper<const Architecture>>;

	Specifier(Specifiers& specifiers, std::string id, Type type);

	const std::string& getId() const;

	Type getType() const;
	void setType(Type type);

	void setAvailability(Availability availability);
	Availability getAvailability() const;

	/* returns a specifier whose ID is a real substring of the own ID. */
	const Specifier* getParent() const;

	/* returns all specifiers, which contain the own id as a real substring of their id,
	 * but their id is not a substring of one of the other returned specifiers. */
	std::vector<std::reference_wrapper<const Specifier>> getChildren() const;

	void addArchitecture(const Architecture& architecture);
	void removeArchitecture(const std::set<std::string>& ids);
	const Architectures& getArchitectures() const;

	/* Returns an ordered subset of architectures.
	 * Architectures will be part of result set if it supports the given language.
	 * Ordering is done from lowest to most important architecture.
	 * If architecture A has a driver and architecture B has no driver, then A is more important than B.
	 * Else (if both architectures have a driver or have no driver):
	 *   Architecture A is less important than B <=> A.getArchitectureIds() < B.getArchitectureIds()
	 */
	std::vector<std::reference_wrapper<const Architecture>> getOrderedArchitectures(const std::string& language) const;

private:
	Specifiers& specifiers;

	std::string id;
	Type type;

	std::set<Specifier*> children;

	Availability availability = isStandard;
	Architectures architectures;
};

} /* namespace architectures */
} /* namespace transformer */

#endif /* TRANSFORMER_ARCHITECTURES_SPECIFIER_H_ */
