#pragma once
#include "Entity.hpp"

class Wall : public StaticObject
{
public:
	constexpr Wall()
		: StaticObject()
	{}

	constexpr Wall(const ModelView& model_view)
		: Wall()
	{
		myModel = model_view;
	}

	Wall(const ModelView& model_view, const glm::vec3& position)
		: Wall(model_view)
	{
		SetPosition(position);
	}

	Wall(const ModelView& model_view, const float& x, const float& y, const float& z)
		: Wall(model_view)
	{
		SetPosition(x, y, z);
	}

	~Wall()
	{}
};
