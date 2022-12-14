#pragma once
#include "BoxCollider.hpp"
#include "GravityHelper.hpp"
#include "MoveHelper.hpp"

class Entity : public GameObject
{
public:
	template<typename Ty, typename ...ArgTy>
		requires EntityType<Ty, ArgTy...>
	static decltype(auto) Instantiate(ArgTy&& ...args)
	{
		return new Ty{ std::forward<ArgTy>(args)... };
	}

	constexpr Entity()
		: GameObject()
		, myName("Entity"), myHealth(), maxHealth()
		, myModel()
		, myCollider()
		, isStatic(false)
		, mySpeed(), vSpeed()
		, myDirection()
		, maxSpeed(8.0f), minVSpeed(-8.0f), maxVSpeed(10.0f)
		, myFriction(), reposeDamping(30.0f)
	{}

	constexpr Entity(const ModelView& model_view)
		: Entity()
	{
		myModel = model_view;
	}

	Entity(const ModelView& model_view, const glm::vec3& position)
		: Entity(model_view)
	{
		SetPosition(position);
	}

	Entity(const ModelView& model_view, const float& x, const float& y, const float& z)
		: Entity(model_view)
	{
		SetPosition(x, y, z);
	}

	Entity(const glm::vec3& position)
		: Entity()
	{
		SetPosition(position);
	}

	Entity(const float& x, const float& y, const float& z)
		: Entity()
	{
		SetPosition(x, y, z);
	}

	virtual ~Entity()
	{}

	virtual void Start()
	{}

	virtual void Update(const float& delta_time)
	{
		myCollider.SetCenter(GetPosition());

		const float dist_ground = myCollider.DistanceGround(constants::GROUND_Y);
		const float gravity = constants::GRAIVTY * delta_time;

		if (0 < dist_ground)
		{
			vSpeed -= gravity;
		}

		if (0 != vSpeed)
		{
			if (dist_ground < 0)
			{
				localTransform.myMatrix[3].y = constants::GROUND_Y + myCollider.GetGroundHeight();
				//Translate(0, -dist_ground, 0);
				vSpeed = 0;
			} 
			else
			{
				Translate(0, vSpeed * delta_time, 0);
			}
		}

		if (0 < mySpeed)
		{
			MoveTo(myDirection, ogl::identity, mySpeed * delta_time);

			if (0 != myFriction)
			{
				mySpeed -= myFriction * delta_time;

				const int sign_dir_y = myDirection.y < 0 ? -1 : 1;
				myDirection.y -= myFriction * delta_time;
				

				if (mySpeed < 0)
				{
					mySpeed = 0;
				}
			}

			if (0 < maxSpeed && maxSpeed < std::abs(mySpeed))
			{
				const float reduction = glm::sign(mySpeed) * reposeDamping * delta_time * 1.5f;
				mySpeed = std::max(maxSpeed, mySpeed - reduction);
			}
		}

		if (0 != minVSpeed && vSpeed < minVSpeed)
		{
			const float addition = reposeDamping * delta_time * 1.5f;
			vSpeed = std::min(minVSpeed, vSpeed + addition);
		}
		if (0 != maxVSpeed && maxVSpeed < vSpeed)
		{
			const float reduction = reposeDamping * delta_time * 1.5f;
			vSpeed = std::max(maxVSpeed, vSpeed - reduction);
		}
	}

	virtual void PrepareRendering()
	{
		GameObject::PrepareRendering();
	}

	virtual void Render(ogl::Uniform& world_uniform)
	{
		if (myChild)
		{
			myChild->Render(world_uniform);
		}

		if (nextSibling)
		{
			nextSibling->Render(world_uniform);
		}

		world_uniform.AssignMatrix4x4(worldTransform.myMatrix);

		if (myModel.IsAvailable())
		{
			myModel.PrepareRendering();
			myModel.Render();
		}
	}

#pragma region 물리
	bool MoveContact(const glm::vec3& vector);
	bool MoveContact(const float& distance, const glm::vec3& direction);

	inline constexpr float SetSpeed(const float& speed)
	{
		return mySpeed = speed;
	}

	inline void SetDirection(const glm::vec3& direction)
	{
		myDirection = glm::normalize(direction);
	}

	inline constexpr float AddSpeed(const float& speed)
	{
		return mySpeed += speed;
	}

	inline void SetVelocity(const glm::vec3& vector)
	{
		SetSpeed(glm::length(vector));
		SetDirection(glm::normalize(vector));
	}

	inline void AddVelocity(const glm::vec3& velocity)
	{
		auto my_velocity = myDirection * mySpeed;
		my_velocity += velocity;

		SetVelocity(my_velocity);
	}

	inline void AddVelocity(const float& speed, const glm::vec3& direction)
	{
		auto my_velocity = myDirection * mySpeed;
		my_velocity += direction * speed;

		SetVelocity(my_velocity);
	}

	inline constexpr float& GetSpeed()
	{
		return mySpeed;
	}

	inline constexpr const float& GetSpeed() const
	{
		return mySpeed;
	}

	inline constexpr glm::vec3& GetDirection(const glm::vec3& direction)
	{
		return myDirection;
	}

	inline constexpr const glm::vec3& GetDirection(const glm::vec3& direction) const
	{
		return myDirection;
	}

	inline constexpr bool CheckGround() const
	{
		return 0 == myDirection.y;
	}
#pragma endregion

#pragma region 충돌
	/// <summary>
	/// 충돌체를 설정합니다.
	/// </summary>
	/// <param name="collider">충돌체</param>
	constexpr void SetCollider(const BoxCollider& collider)
	{
		myCollider = collider;
	}

	/// <summary>
	/// 충돌체를 반환합니다.
	/// </summary>
	/// <returns></returns>
	constexpr BoxCollider& GetCollider()
	{
		return myCollider;
	}

	/// <summary>
	/// 충돌체를 반환합니다.
	/// </summary>
	/// <returns></returns>
	constexpr const BoxCollider& GetCollider() const
	{
		return myCollider;
	}

	/// <summary>
	/// 충돌 검사를 수행합니다.
	/// </summary>
	/// <param name="other"></param>
	/// <returns></returns>
	bool IsCollideWith(const Collider& other) const
	{
		const auto place = WhereCollideWith(other);

		if (wrongCollisionCoord == place)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	/// <summary>
	/// 충돌한 위치를 반환합니다.
	/// </summary>
	/// <param name="other">다른 충돌체</param>
	/// <returns>로컬 좌표계의 충돌 지점</returns>
	glm::vec3 WhereCollideWith(const Collider& other) const
	{
		return wrongCollisionCoord;
	}

protected:
	virtual void EnumerateTransform()
	{
		GameObject::EnumerateTransform();

		myCollider.SetCenter(GetPosition());
	}
#pragma endregion

public:
	Entity(const Entity& other) = default;
	Entity(Entity&& other) = default;
	Entity& operator=(const Entity& other) = default;
	Entity& operator=(Entity&& other) = default;

	std::string myName;
	float myHealth;
	float maxHealth;

	MoveHelper moveHelper;

	bool isStatic;
	float mySpeed;
	float maxSpeed;
	float vSpeed;
	float minVSpeed;
	float maxVSpeed;
	glm::vec3 myDirection;

	float myFriction;
	float reposeDamping;

	bool isAttacking;
	float attackDelay;
	float attackCooltime;
	float attackPreDelay;

	ModelView myModel;
	BoxCollider myCollider;

	static inline constexpr glm::vec3 wrongCollisionCoord = glm::vec3{ std::numeric_limits<float>::min() };
};

template<typename Ty, typename ...ArgTy>
concept EntityType = std::derived_from<std::decay_t<Ty>, Entity> && std::constructible_from<std::decay_t<Ty>, ArgTy...>;
