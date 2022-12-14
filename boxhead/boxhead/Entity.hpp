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
		, myHealth(), maxHealth()
		, myModel()
		, myCollider()
		, isStatic(false)
		, mySpeed(), vSpeed()
		, myDirection()
		, maxSpeed(8.0f), minVSpeed(-8.0f), maxVSpeed(10.0f)
		, myFriction(20.0f), airDamping(5.0f), reposeDamping(30.0f)
	{
		myName = "Entity";
	}

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

				if (myDirection.y != 0)
				{
					const int sign_dir_y = myDirection.y < 0 ? -1 : 1;
					myDirection.y -= sign_dir_y * myFriction * delta_time;
					const int sign_now_dir_y = myDirection.y < 0 ? -1 : 1;

					if (0 != sign_dir_y && sign_dir_y != sign_now_dir_y)
					{
						myDirection.y = 0;
					}
				}

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
		return myCollider.CheckGround(constants::GROUND_Y);
	}
#pragma endregion

#pragma region 충돌
	constexpr void SetCollider(const BoxCollider& collider)
	{
		myCollider = collider;
	}
	
	constexpr BoxCollider& GetCollider()
	{
		return myCollider;
	}

	constexpr const BoxCollider& GetCollider() const
	{
		return myCollider;
	}
	
	bool IsCollideWith(const BoxCollider& other) const
	{
		if (myCollider.Check(other))
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	glm::vec3 GetBounds() const
	{
		return myCollider.colliderExtent;
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
	float airDamping;
	float reposeDamping;

	bool isAttacking;
	float attackDelay;
	float attackCooltime;
	float attackPreDelay;

	ModelView myModel;
	BoxCollider myCollider;
};

template<typename Ty, typename ...ArgTy>
concept EntityType = std::derived_from<std::decay_t<Ty>, Entity> && std::constructible_from<std::decay_t<Ty>, ArgTy...>;
