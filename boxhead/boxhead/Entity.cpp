#include "pch.hpp"
#include "Entity.hpp"
#include "Framework.hpp"
#include "GameScene.hpp"

bool Entity::MoveContact(const glm::vec3& vector)
{
	float len = glm::length(vector);

	const auto bfr_pos = GetPosition();
	const auto now_pos = bfr_pos + vector;

	const float height = GameScene::Instance->GetActualHeight(now_pos.x, now_pos.z);
	const float upper = constants::GROUND_Y + height;

	const bool check = myCollider.CheckGround(upper);

	if (check)
	{
		return false;
	}
	else
	{
		Translate(vector);

		return true;
	}
}

bool Entity::MoveContact(const float& distance, const glm::vec3& direction)
{
	return MoveContact(direction * distance);
}
