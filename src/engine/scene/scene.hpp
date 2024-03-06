#pragma once

#include "engine/data_types/tree.hpp"
#include "engine/game_object/game_object.hpp"
#include "engine/render_system/camera/camera.hpp"
#include "engine/render_system/lights/lights.hpp"
#include <memory>
#include <variant>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

class SceneContainer
{
public:
	std::string name;
	std::optional<
		// this feels less than ideal
		// good enough until an inheritence
		// or component system for storing information
		std::variant<std::shared_ptr<GameObject>, std::shared_ptr<Camera>, std::shared_ptr<PointLight>, std::shared_ptr<DirectionalLight>>>
		data;
	SceneContainer(std::string name, std::optional<std::variant<std::shared_ptr<GameObject>, std::shared_ptr<Camera>, std::shared_ptr<PointLight>, std::shared_ptr<DirectionalLight>>> data) : name(name), data(data) {}
};

class SceneTree
{
	Tree<SceneContainer> tree;
	void process_assimp_node(aiNode *node, const aiScene *scene, std::filesystem::path file, std::shared_ptr<TreeNode<SceneContainer>> parent);

public:
	SceneTree() {}
	SceneTree(std::filesystem::path scene_file);
};