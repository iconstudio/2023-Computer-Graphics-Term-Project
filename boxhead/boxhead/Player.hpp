#pragma once
#define NOMINMAX
#include <windows.h>

class Player : public Entity
{
public:
	constexpr Player()
		: Entity()
		, moveMaxSpeed(7.0f), walkMaxSpeed(2.5f)
		, moveAccel(40.0f), walkAccel(20.0f)
		, moveFriction(16.0f), walkFriction(30.0f)
	{
		maxSpeed = moveMaxSpeed;
		myAccel = moveAccel;
		myFriction = moveFriction;
	}

	Player(const glm::vec3& position)
		: Player()
	{
		SetPosition(position);
	}

	Player(const float& x, const float& y, const float& z)
		: Player()
	{
		SetPosition(x, y, z);
	}

	~Player()
	{}

	Entity* Shot()
	{
		return nullptr;
	}

	void Awake()
	{}

	void Start() override
	{}

	void Update(const float& delta_time) override
	{
		Entity::Update(delta_time);

		short state_esc = ObtainKeyState(VK_ESCAPE);
		short state_lt = ObtainKeyState(VK_LEFT);
		short state_rt = ObtainKeyState(VK_RIGHT);
		short state_up = ObtainKeyState(VK_UP);
		short state_dw = ObtainKeyState(VK_DOWN);
		short state_w = ObtainKeyState('W');
		short state_a = ObtainKeyState('A');
		short state_s = ObtainKeyState('S');
		short state_d = ObtainKeyState('D');
		short state_r = ObtainKeyState('R');
		short state_q = ObtainKeyState('Q');
		short state_f = ObtainKeyState('F');
		short state_jump = ObtainKeyState(VK_SPACE);
		short state_walk = ObtainKeyState(VK_SHIFT);
		short state_confirm = ObtainKeyState(VK_RETURN);

		UpdateKeyState(state_w, checkUp, checkPressedUp);
		UpdateKeyState(state_a, checkLeft, checkPressedLeft);
		UpdateKeyState(state_s, checkDown, checkPressedDown);
		UpdateKeyState(state_d, checkRight, checkPressedRight);
		UpdateKeyState(state_jump, checkJump, checkPressedJump);
		UpdateKeyState(state_walk, checkWalk, checkPressedWalk);
		UpdateKeyState(state_esc, checkESC, checkPressedESC);
		UpdateKeyState(state_confirm, checkConfirm, checkPressedConfirm);

		if (checkWalk)
		{
			maxSpeed = walkMaxSpeed;
			myAccel = walkAccel;
			myFriction = walkFriction;
		}
		else
		{
			maxSpeed = moveMaxSpeed;
			myAccel = moveAccel;
			myFriction = moveFriction;
		}

		int press_side = int(checkLeft - checkRight);
		int press_forward = int(checkUp - checkDown);

		if (0 != press_side || 0 != press_forward)
		{
			auto vector = glm::vec3{ press_side, 0, press_forward };
			if (press_forward < 0)
			{
				vector.z *= 0.6666667f;
			}
			
			const auto movement = glm::normalize(vector);
			const auto toward = glm::vec4{ movement, 1.0f };
			const auto direction = toward * GetRotation();

			bool was_overflow = maxSpeed < mySpeed;

			const float aceel = moveAccel * delta_time;
			AddVelocity(aceel, direction);
			
			if (!was_overflow && maxSpeed < mySpeed)
			{
				mySpeed = maxSpeed;
			}
		}
	}

	void OnMouse(const int& button, const int& state, const int& x, const int& y)
	{

	}

	void OnKeyboard(const unsigned char& key, const int& x, const int& y)
	{
		switch (key)
		{
			case 'w':
			case 'W':
			{}
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
			{}
			break;

			default:
			{}
			break;
		}
	}

	virtual void OnSpecialKey(const int& key, const int& x, const int& y)
	{}

	Entity* Raycast(GameObject obj_list[], const size_t& count, const float& distance, const glm::vec3& direction) const
	{

	}

	GameObject* Raycast(const GameObject& target, const float& distance, const glm::vec3& direction) const
	{


		return nullptr;
	}

	camera::Camera* myCamera;

	bool checkLeft = false;
	bool checkRight = false;
	bool checkUp = false;
	bool checkDown = false;
	bool checkJump = false;
	bool checkWalk = false;
	bool checkConfirm = false;
	bool checkESC = false;
	bool checkPressedLeft = false;
	bool checkPressedRight = false;
	bool checkPressedUp = false;
	bool checkPressedDown = false;
	bool checkPressedJump = false;
	bool checkPressedWalk = false;
	bool checkPressedConfirm = false;
	bool checkPressedESC = false;

	float myAccel;
	const float moveMaxSpeed;
	const float walkMaxSpeed;
	const float moveAccel;
	const float walkAccel;
	const float moveFriction;
	const float walkFriction;
};
