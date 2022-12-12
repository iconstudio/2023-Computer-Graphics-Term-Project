#pragma once
#include "GameObject.hpp"
#include "ModelView.hpp"
#include "BoxCollider.hpp"

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
		, myName(), myHealth(), maxHealth()
		, myModel()
		, myCollider(nullptr)
		, mySpeed(), myDirection()
		, maxSpeed(8.0f), myFriction(), airDamping(30.0f)
	{}

	constexpr Entity(const ModelView& model_view)
		: GameObject()
		, myName(), myHealth(), maxHealth()
		, myModel(model_view)
		, myCollider(nullptr)
		, mySpeed(), myDirection()
		, maxSpeed(8.0f), myFriction(), airDamping(30.0f)
	{}

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
		if (0 < mySpeed)
		{
			MoveTo(myDirection, ogl::identity, mySpeed * delta_time);

			if (0 != myFriction)
			{
				mySpeed -= myFriction * delta_time;

				if (mySpeed < 0)
				{
					mySpeed = 0;
				}
			}

			if (0 < maxSpeed && maxSpeed < std::abs(mySpeed))
			{
				mySpeed = std::max(maxSpeed, mySpeed - airDamping * delta_time * 1.5f);
			}
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
#pragma endregion

#pragma region 충돌
	/// <summary>
	/// 충돌체를 설정합니다.
	/// </summary>
	/// <param name="collider">충돌체</param>
	constexpr void SetCollider(BoxCollider* const collider)
	{
		if (!collider) return;

		myCollider = collider;
	}

	/// <summary>
	/// 충돌체를 해제합니다.
	/// </summary>
	constexpr void DetachCollider()
	{
		if (myCollider)
		{
			myCollider = nullptr;
		}
	}

	/// <summary>
	/// 충돌체를 반환합니다.
	/// </summary>
	/// <returns></returns>
	constexpr BoxCollider* GetCollider()
	{
		return myCollider;
	}

	/// <summary>
	/// 충돌체를 반환합니다.
	/// </summary>
	/// <returns></returns>
	constexpr const BoxCollider* GetCollider() const
	{
		return myCollider;
	}

	/// <summary>
	/// 충돌 검사를 수행합니다.
	/// </summary>
	/// <param name="other"></param>
	/// <returns></returns>
	bool IsCollideWith(const Collider* const other) const
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
	glm::vec3 WhereCollideWith(const Collider* const other) const
	{
		return wrongCollisionCoord;
	}
#pragma endregion

	Entity(const Entity& other) = default;
	Entity(Entity&& other) = default;
	Entity& operator=(const Entity& other) = default;
	Entity& operator=(Entity&& other) = default;

	std::string myName;
	float myHealth;
	float maxHealth;
	float mySpeed;
	glm::vec3 myDirection;
	float maxSpeed;
	// 마찰력
	float myFriction;
	// 공기 저항력
	float airDamping;

	bool isAttacking;
	float attackDelay;
	float attackCooltime;
	float attackPreDelay;

	ModelView myModel;
	BoxCollider* myCollider;
};

template<typename Ty, typename ...ArgTy>
concept EntityType = std::derived_from<std::decay_t<Ty>, Entity> && std::constructible_from<std::decay_t<Ty>, ArgTy...>;
