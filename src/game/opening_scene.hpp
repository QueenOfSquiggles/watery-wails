#pragma once

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "engine/engine.hpp"
#include "engine/render_system/resource_management/resource_factory.hpp"
#include "engine/audio_system/audio.hpp"
#include "engine/render_system/sky/skybox.hpp"

#include <memory>
#include <iostream>

void load_opening_scene(Engine *engine);