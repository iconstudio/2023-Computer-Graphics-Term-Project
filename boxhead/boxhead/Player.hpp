#pragma once
#define NOMINMAX
#include <windows.h>
#include "Enemy.hpp"

class Player : public Entity
{
public:
	constexpr Player()
		: Entity()
		, moveMaxSpeed(4.0f), walkMaxSpeed(1.5f)
		, moveAccel(30.0f), walkAccel(10.0f)
		, moveFriction(16.0f), walkFriction(30.0f)
		, myLife(10), maxLife(10)
		, jumpSpeed(3)
		, startingFlash(false), flashStartTime(0), flashStartPeriod(0.2f)
		, flashCooltime(0), flashCooldown(5.0f), flashJumpSpeed(1.8f), flashAccelSpeed(27.0f)
	{
		maxSpeed = moveMaxSpeed;
		myAccel = moveAccel;
		myFriction = moveFriction;

		myCollider.SetExtent(1.0f, 2.0f, 1.0f);
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

	std::vector<Entity*> FindTargets(Entity* enemylist[], const size_t& count) const
	{
		return {};
	}

	void Attack(const std::vector<Enemy*>& enemylist)
	{

	}

	void GiveDamageTo(Entity* const target)
	{

	}

	inline constexpr void Jump()
	{
		vSpeed = jumpSpeed;
	}

	inline void StartFlashJump()
	{
		startingFlash = true;
		flashStartTime = flashStartPeriod;

		SetSpeed(GetSpeed() * 0.5f);
	}

	inline constexpr void FlashJump()
	{
		flashCooltime = flashCooldown;

		vSpeed = flashJumpSpeed;
		SetSpeed(flashAccelSpeed);
	}

	inline constexpr bool CanJump() const
	{
		return 0 == vSpeed && onGround;
	}

	constexpr bool CanFlash()
	{
		return (CanJump() && !startingFlash && flashCooltime <= 0);
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
		short state_flash = state_f;
		short state_walk = ObtainKeyState(VK_SHIFT);
		short state_confirm = ObtainKeyState(VK_RETURN);

		UpdateKeyState(state_w, checkUp, checkPressedUp);
		UpdateKeyState(state_a, checkLeft, checkPressedLeft);
		UpdateKeyState(state_s, checkDown, checkPressedDown);
		UpdateKeyState(state_d, checkRight, checkPressedRight);
		UpdateKeyState(state_jump, checkJump, checkPressedJump);
		UpdateKeyState(state_flash, checkFlashJump, checkPressedFlashJump);
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

		if (startingFlash)
		{
			if (0 < flashStartTime)
			{
				flashStartTime -= delta_time;
			}
			else
			{
				startingFlash = false;
				FlashJump();
			}
		}

		if (0 < flashCooltime)
		{
			flashCooltime -= delta_time;
		}

		if (CanJump() && checkPressedJump)
		{
			Jump();
		}
		else if (CanFlash() && checkPressedFlashJump)
		{
			StartFlashJump();
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
			auto direction = toward * GetRotation();

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
		if (ogl::IsMouseClicked(state))
		{
			if (ogl::IsLeftMouseButton(button))
			{

			}
		}
	}

	void OnKeyboard(const unsigned char& key, const int& x, const int& y)
	{
		switch (key)
		{
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

	Entity* Raycast(GameObject* (&obj_list)[], const size_t& count, const float& mx, const float& my, const float& radius) const
	{
		std::vector<GameObject*> filter{};
		filter.reserve(count);

		Entity* result = nullptr;

		for (size_t i = 0; i < count; i++)
		{
			auto& obj = obj_list[i];

			auto mid = RaycastOne(obj, mx, my, radius);
			if (mid)
			{
				filter.push_back(mid);
			}
		}

		if (0 < filter.size())
		{
			const auto my_pos = GetPosition();

			std::ranges::sort
			(
				filter,
				[my_pos](const GameObject* lhs, const GameObject* rhs) -> bool {
				const float ldist = glm::distance(lhs->GetPosition(), my_pos);
			const float rdist = glm::distance(rhs->GetPosition(), my_pos);

			return (ldist < rdist);
			});

			result = dynamic_cast<Entity*>(filter[0]);
		}

		return result;
	}

	GameObject* RaycastOne(GameObject* target, const float& mx, const float& my, const float& radius) const
	{
		const auto& obj_mat = target->worldTransform.myMatrix;
		const auto& cam_mat = myCamera->GetCameraMatrix();
		const auto& per_mat = myCamera->GetPerspectiveViewMatrix();

		// 객체를 투영 변환
		const auto& view_mat = per_mat * cam_mat * obj_mat;
		const auto& view_pos = view_mat[3];

		const float dist = glm::distance(view_pos, glm::vec4{ mx, my, 0.0f, 0.0f });
		if (dist < radius)
		{
			return target;
		}

		return nullptr;
	}

	camera::Camera* myCamera;
	int myLife;
	const int maxLife;

	bool checkLeft = false;
	bool checkRight = false;
	bool checkUp = false;
	bool checkDown = false;
	bool checkJump = false;
	bool checkFlashJump = false;
	bool checkWalk = false;
	bool checkConfirm = false;
	bool checkESC = false;
	bool checkPressedLeft = false;
	bool checkPressedRight = false;
	bool checkPressedUp = false;
	bool checkPressedDown = false;
	bool checkPressedJump = false;
	bool checkPressedFlashJump = false;
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
	float jumpSpeed;

	bool startingFlash;
	float flashStartTime;
	const float flashStartPeriod;
	float flashCooltime;
	const float flashCooldown;
	float flashJumpSpeed;
	float flashAccelSpeed;
};
