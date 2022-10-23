#ifndef TRANSFORMER_ARCHITECTURES_SOLVER_TREE_H_
#define TRANSFORMER_ARCHITECTURES_SOLVER_TREE_H_

#include <transformer/architectures/Specifier.h>

#include <map>
#include <vector>
#include <functional>
#include <memory>

namespace transformer {
namespace architectures {
namespace solver {

class Tree {
public:
	class Node {
	friend class Tree;
	public:
		const Specifier& operator*() const noexcept;
		const Specifier* operator->() const noexcept;

		const Node* getParent() const noexcept;
		const std::vector<std::reference_wrapper<const Node>>& getChildren() const noexcept;

	private:
		Node(const Specifier& specifier);

		const Specifier& element;

		const Node* parent = nullptr;
		std::vector<std::reference_wrapper<const Node>> children;
	};

	Node& add(const Specifier& specifier);
	const std::vector<std::reference_wrapper<const Node>>& getChildren() const noexcept;

private:
	std::vector<std::reference_wrapper<const Node>> children;
	std::map<const Specifier*, std::unique_ptr<Node>> specifiers;
};

} /* namespace solver */
} /* namespace architectures */
} /* namespace transformer */

#endif /* TRANSFORMER_ARCHITECTURES_SOLVER_TREE_H_ */
