#include <transformer/architectures/solver/Tree.h>
#include <utility>

namespace transformer {
namespace architectures {
namespace solver {

Tree::Node::Node(const Specifier& specifier)
: element(specifier)
{ }

const Specifier& Tree::Node::operator*() const noexcept {
	return element;
}

const Specifier* Tree::Node::operator->() const noexcept {
	return &element;
}

const Tree::Node* Tree::Node::getParent() const noexcept {
	return parent;
}

const std::vector<std::reference_wrapper<const Tree::Node>>& Tree::Node::getChildren() const noexcept {
	return children;
}

Tree::Node& Tree::add(const Specifier& specifier) {
	auto iter = specifiers.find(&specifier);
	if(iter != specifiers.end()) {
		return *iter->second;
	}

	Node* nodePtr = new Node(specifier);
	specifiers.insert(std::make_pair(&specifier, std::unique_ptr<Node>(nodePtr)));
	Node* parentNodePtr = nullptr;

	for(const Specifier* parent = specifier.getParent(); parent; parent = parent->getParent()) {
		if(parent->getType() != Specifier::isArchitecture) {
			continue;
		}

		parentNodePtr = &add(*parent);
		break;
	}

	if(parentNodePtr) {
		parentNodePtr->children.push_back(std::cref(*nodePtr));
		nodePtr->parent = parentNodePtr;
	}
	else {
		children.push_back(std::cref(*nodePtr));
	}

	return *nodePtr;
}

const std::vector<std::reference_wrapper<const Tree::Node>>& Tree::getChildren() const noexcept {
	return children;
}

} /* namespace solver */
} /* namespace architectures */
} /* namespace transformer */
