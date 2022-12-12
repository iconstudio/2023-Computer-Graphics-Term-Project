#pragma once

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

	}

	void Update(const float& delta_time) override
	{

	}

	void OnMouse(const int& button, const int& state, const int& x, const int& y)
	{

	}

	virtual void OnKeyboard(const unsigned char& key, const int& x, const int& y)
	{
		const float delta_time = Timer::GetDeltaTime();
		//const auto camera_angle = myCamera->GetQuaternion();

		switch (key)
		{
			case 'w':
			case 'W':
			{
				const float move_distance_forward = delta_time * 10.0f;
				MoveForward(move_distance_forward);
			}
			break;

			case 'd':
			case 'D':
			{
				const float move_distance_right = delta_time * 10.0f;
				MoveStrife(-move_distance_right);
			}
			break;

			case 'a':
			case 'A':
			{
				const float move_distance_left = delta_time * 10.0f;
				MoveStrife(move_distance_left);
			}
			break;

			case 's':
			case 'S':
			{
				const float move_distance_backward = delta_time * 4.0f;
				MoveForward(-move_distance_backward);
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
