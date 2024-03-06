#pragma once

#include <memory>
#include "mesh.hpp"

class PrimitiveCube : public MeshSurface
{
public:
	PrimitiveCube(float size);
	virtual void render(RenderContext ctx) override;
};