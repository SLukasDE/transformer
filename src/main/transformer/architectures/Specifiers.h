#ifndef TRANSFORMER_ARCHITECTURES_SPECIFIERS_H_
#define TRANSFORMER_ARCHITECTURES_SPECIFIERS_H_

#include <transformer/architectures/Specifier.h>
#include <transformer/architectures/Architecture.h>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <memory>

namespace transformer {
namespace architectures {

// TODO: umbenennen in "class Dictionary"
class Specifiers {
public:
	void dumpTree() const;
	void dumpTree(const Specifier& specifier, std::size_t depth) const;
	void print() const;

	std::set<const Specifier*> getSpecifiers() const;

	/* only lookup in primarySpecifiers, not in secondarySpecifiers */
	const Specifier* getSpecifier(const std::string& id) const;

	/* only lookup in primarySpecifiers, not in secondarySpecifiers */
	const Specifier& operator[](const std::string& id) const;

	void addBranch(const std::set<std::string>& ids);
	const std::set<const Specifier*>& getBranches(const std::string& id) const;
	const std::set<const Specifier*>& getBranches(const Specifier& specifier) const;
	std::map<const Specifier*, std::vector<std::reference_wrapper<const Specifier>>> getBranches() const;

	void addArchitecture(Architecture architecture);
	std::vector<std::reference_wrapper<const Architecture>> getArchitectures() const;

	void setOptional(const std::string& id);
	void setDefault(const std::string& id);

private:
	Specifier& addSpecifier(const std::string& id, Specifier::Type type);

	/* Contains all origin Specifiers added with addSpecifier(...)
	 * by it's original id.
	 * It contains as well generated Specifiers by addSpecifier(...) that are
	 * branching the path to other specifiers.
	 */
	// TODO: umbenennen in originalSpecifier
	std::map<std::string, std::unique_ptr<Specifier>> primarySpecifiers;

	/* Contains references of Specifiers added with addSpecifier(...)
	 * by it's original id and as well by all of it subIds
	 *
	 * See Readme.txt
	 */
	// TODO: umbenennen in shortcutSpecifier
	std::map<std::string, std::reference_wrapper<Specifier>> secondarySpecifiers;

	std::map<const Specifier*, std::set<const Specifier*>> branchesBySpecifier;

	std::map<std::set<std::string>, std::unique_ptr<Architecture>> architectureByIds;
};

} /* namespace architectures */
} /* namespace transformer */

#endif /* TRANSFORMER_ARCHITECTURES_SPECIFIERS_H_ */
