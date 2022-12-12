#pragma once
#define NOMINMAX
#include <windows.h>

class Player : public Entity
{
public:
	constexpr Player()
		: Entity()
		, pressForward(0), pressSide(0)
	{}

	Player(const glm::vec3& position)
		: Entity(position)
		, pressForward(0), pressSide(0)
	{}

	Player(const float& x, const float& y, const float& z)
		: Entity(x, y, z)
		, pressForward(0), pressSide(0)
	{}

	virtual ~Player()
	{}

	Entity* Shot()
	{
		return nullptr;
	}

	void Awake()
	{

	}

	void Start() override
	{
		pressForward = 0;
		pressSide = 0;
	}

	void Update(const float& delta_time) override
	{
		Entity::Update(delta_time);

		if (0 != pressSide || 0 != pressForward)
		{
			auto vector = glm::vec3{ pressSide, 0, pressForward };
			glm::vec3 movement;

			if (pressForward < 0)
			{
				vector.z *= 0.6666667f;
			}
			movement = glm::normalize(vector);

			const float speed = 10.0f * delta_time;
			MoveTo(movement, speed);
		}
	}

	void OnMouse(const int& button, const int& state, const int& x, const int& y)
	{

	}

	void OnKeyboard(const unsigned char& key, const int& x, const int& y)
	{
		pressForward = 0;
		pressSide = 0;
		//const float delta_time = Timer::GetDeltaTime();
		//const auto camera_angle = myCamera->GetQuaternion();

		switch (key)
		{
			case 'w':
			case 'W':
			{
				pressForward = 1;
				//const float move_distance_forward = delta_time * 10.0f;
				//MoveForward(move_distance_forward);
			}
			break;

			case 'd':
			case 'D':
			{
				pressSide = -1;
				//const float move_distance_right = delta_time * 10.0f;
				//MoveStrife(-move_distance_right);
			}
			break;

			case 'a':
			case 'A':
			{
				pressSide = 1;
				//const float move_distance_left = delta_time * 10.0f;
				//MoveStrife(move_distance_left);
			}
			break;

			case 's':
			case 'S':
			{
				pressForward = -1;
				//const float move_distance_backward = delta_time * 4.0f;
				//MoveForward(-move_distance_backward);
			}
			break;

			default:
			{}
			break;
		}
	}

	void OnKeyboardUp(const unsigned char& key, const int& x, const int& y)
	{
		switch (key)
		{
			case 'w':
			case 'W':
			{
				//if (pressForward == 1)
					pressForward = 0;
			}
			break;

			case 'd':
			case 'D':
			{
				//if (pressSide == 1)
					pressSide = 0;
			}
			break;

			case 'a':
			case 'A':
			{
				//if (pressSide == -1)
					pressSide = 0;
			}
			break;

			case 's':
			case 'S':
			{
				//if (pressForward == -1)
					pressForward = 0;
			}
			break;

			default:
			{}
			break;
		}
	}

	virtual void OnSpecialKey(const int& key, const int& x, const int& y)
	{

	}

	Entity* Raycast(GameObject obj_list[], const size_t& count, const float& distance, const glm::vec3& direction) const
	{

	}

	GameObject* Raycast(const GameObject& target, const float& distance, const glm::vec3& direction) const
	{


		return nullptr;
	}

	camera::Camera* myCamera;
	int pressForward;
	int pressSide;
};
