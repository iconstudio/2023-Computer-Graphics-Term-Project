#pragma once

class Enemy : public Entity
{
public:
	template<typename Ty, typename ...ArgTy>
		requires EnemyType<Ty, ArgTy...>
	static decltype(auto) Instantiate(ArgTy&& ...args)
	{
		return new Ty{ std::forward<ArgTy>(args)... };
	}

	constexpr Enemy()
		: Entity()
	{}

	Enemy(const glm::vec3& position)
		: Enemy()
	{
		SetPosition(position);
	}

	Enemy(const float& x, const float& y, const float& z)
		: Enemy()
	{
		SetPosition(x, y, z);
	}

	virtual ~Enemy()
	{}

	float moveSpeed;
	float
};

template<typename Ty, typename ...ArgTy>
concept EnemyType = std::derived_from<std::decay_t<Ty>, Enemy> && std::constructible_from<std::decay_t<Ty>, ArgTy...>;
