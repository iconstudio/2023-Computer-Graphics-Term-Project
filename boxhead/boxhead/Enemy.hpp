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

	constexpr Enemy(const ModelView& model_view)
		: Entity()
	{}

	Enemy(const ModelView& model_view, const glm::vec3& position)
		: Enemy(model_view)
	{
		SetPosition(position);
	}

	Enemy(const ModelView& model_view, const float& x, const float& y, const float& z)
		: Enemy(model_view)
	{
		SetPosition(x, y, z);
	}

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
};

template<typename Ty, typename ...ArgTy>
concept EnemyType = std::derived_from<std::decay_t<Ty>, Enemy> && std::constructible_from<std::decay_t<Ty>, ArgTy...>;
