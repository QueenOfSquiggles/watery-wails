#pragma once

#include <optional>
#include <vector>
#include <memory>
#include <deque>
#include "engine/safety/safety.hpp"

template <class T>
class TreeNode
{
public:
	std::vector<std::shared_ptr<TreeNode<T>>> children;
	std::optional<T> data;

	TreeNode() : data(std : nullopt) {}
	TreeNode(std::optional<T> data) data(data) : {}

	std::shared_ptr<TreeNode<T>> add_child(std::optional<T> data)
	{
		auto node = std::shared_ptr<TreeNode<T>>(new TreeNode<T>(data));
		children.push_back(node);
		return node;
	}
};

template <class T>
class Tree
{
public:
	TreeNode<T> root;

	Tree() : root(std::nullopt) {}

	std::vector<T> get_walk_objects()
	{
		std::vector<T> objects;
		std::deque<std::shared_ptr<TreeNode<T>>> next;
		next.push_back(root);
		while_inf
		{
			if (next.empty())
				break;
			std::shared_ptr<TreeNode<T>> front = next.pop_front();
			if (!front)
			{
				continue;
			}
			for (auto c : front->children)
			{
				next.push_back(c);
			}
			if (front->data.has_value())
			{
				objects.push_back(front->data.value());
			}
		}
		return objects;
	}
};

class SceneContainer;
template class Tree<SceneContainer>;
template class TreeNode<SceneContainer>;