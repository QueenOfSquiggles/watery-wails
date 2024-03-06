#include "scene.hpp"
#include <iostream>

// template <>
// std::shared_ptr<TreeNode<SceneContainer>> TreeNode<SceneContainer>::add_child(std::optional<SceneContainer> data)
// {
// 	auto node = std::shared_ptr<TreeNode<SceneContainer>>(new TreeNode(data));
// 	children.push_back(node);
// 	return node;
// }

SceneTree::SceneTree(std::filesystem::path scene_file)
{
	Assimp::Importer importer;
	auto scene = importer.ReadFile(std::filesystem::absolute(scene_file).string(), aiProcessPreset_TargetRealtime_Fast | aiProcess_CalcTangentSpace);
	auto err = std::string(importer.GetErrorString());
	if (!err.empty())
	{
		std::cerr << "Error loading scene file: " << err << std::endl;
		return;
	}
	if (scene->HasCameras())
	{
		auto cam_root = tree.root.add_child(SceneContainer{
			"camera_root",
			std::nullopt,
		});
		for (unsigned int i = 0; i < scene->mNumCameras; i++)
		{
			auto cam_data = scene->mCameras[i];
			auto cam = std::shared_ptr<Camera>(new Camera());
			cam->position = glm::vec3{
				cam_data->mPosition.x,
				cam_data->mPosition.y,
				cam_data->mPosition.z,
			};
			auto up = glm::vec3{
				cam_data->mUp.x,
				cam_data->mUp.y,
				cam_data->mUp.z,
			};
			auto front = glm::vec3{
				cam_data->mLookAt.x,
				cam_data->mLookAt.y,
				cam_data->mLookAt.z,
			};
			cam->create_perspective(cam_data->mHorizontalFOV, cam_data->mAspect, cam_data->mClipPlaneNear, cam_data->mClipPlaneFar);
			// TODO: convert matrix data into rotation data
			//			cam->update_transform();
			cam_root->add_child(SceneContainer{
				cam_data->mName.C_Str(),
				cam,
			});
		}
	}
	if (scene->HasLights())
	{
		auto lights_root = tree.root.add_child(SceneContainer{
			"lights_root",
			std::nullopt,
		});
		for (unsigned int i = 0; i < scene->mNumLights; i++)
		{
			auto light = scene->mLights[i];
			switch (light->mType)
			{
			case aiLightSourceType::aiLightSource_DIRECTIONAL:
			{
				auto dirlight = std::shared_ptr<DirectionalLight>(new DirectionalLight());
				dirlight->direction = glm::vec3{light->mDirection.x, light->mDirection.y, light->mDirection.z};
				dirlight->colour = glm::vec3{light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b};
				lights_root->add_child(SceneContainer{
					std::string(light->mName.C_Str()),
					dirlight,
				});
			}
			break;
			case aiLightSourceType::aiLightSource_POINT:
			{
				auto pointlight = std::shared_ptr<PointLight>(new PointLight());
				pointlight->position = {light->mPosition.x, light->mPosition.y, light->mPosition.z};
				pointlight->falloff_components = {light->mAttenuationConstant, light->mAttenuationLinear, light->mAttenuationQuadratic};
				pointlight->colour = {light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b};
				lights_root->add_child(SceneContainer{
					std::string(light->mName.C_Str()),
					pointlight,
				});
			}
			break;
			default:
				break;
			}
		}
	}
	process_assimp_node(scene->mRootNode, scene, scene_file, std::shared_ptr<TreeNode<SceneContainer>>(&tree.root));
}

void SceneTree::process_assimp_node(aiNode *node, const aiScene *scene, std::filesystem::path file, std::shared_ptr<TreeNode<SceneContainer>> parent)
{
	auto mesh = std::shared_ptr<Mesh>(new Mesh());
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		auto mesh_data = scene->mMeshes[node->mMeshes[i]];
		auto surf_data = mesh->load_surf_data_container(mesh_data, scene, file);
		auto surf = new MeshSurface{
			surf_data.vertices,
			surf_data.indices,
			surf_data.attributes,
			surf_data.material,
		};
		mesh->surfaces.push_back(std::shared_ptr<MeshSurface>(surf));
	}
	auto current_tree_node = parent->add_child(SceneContainer{
		std::string(node->mName.C_Str()),
		std::shared_ptr<GameObject>(new GameObject(mesh)),
	});
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		auto child = node->mChildren[i];
		process_assimp_node(child, scene, file, current_tree_node);
	}
}