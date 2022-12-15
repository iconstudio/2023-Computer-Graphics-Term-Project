#pragma once

class StaticObject : public GameObject
{
public:
	template<typename Ty, typename ...ArgTy>
		requires StaticObjetType<Ty, ArgTy...>
	static decltype(auto) Instantiate(ArgTy&& ...args)
	{
		return new Ty{ std::forward<ArgTy>(args)... };
	}

	constexpr StaticObject()
		: GameObject()
	{}

	constexpr StaticObject(const ModelView& model_view)
		: StaticObject()
	{
		myModel = model_view;
	}

	StaticObject(const ModelView& model_view, const glm::vec3& position)
		: StaticObject(model_view)
	{
		SetPosition(position);
	}

	StaticObject(const ModelView& model_view, const float& x, const float& y, const float& z)
		: StaticObject(model_view)
	{
		SetPosition(x, y, z);
	}

	StaticObject(const glm::vec3& position)
		: StaticObject()
	{
		SetPosition(position);
	}

	StaticObject(const float& x, const float& y, const float& z)
		: StaticObject()
	{
		SetPosition(x, y, z);
	}

	virtual ~StaticObject()
	{}

	virtual void PrepareRendering() override
	{
		EnumerateTransform();

		if (myModel.IsAvailable())
		{
			myModel.PrepareRendering();
		}
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
			myModel.Render();
		}
	}

	constexpr BoxCollider& GetCollider()
	{
		return myCollider;
	}

	constexpr const BoxCollider& GetCollider() const
	{
		return myCollider;
	}

	ModelView myModel;
	BoxCollider myCollider;
};

template<typename Ty, typename ...ArgTy>
concept StaticObjetType = std::derived_from<std::decay_t<Ty>, StaticObject> && std::constructible_from<std::decay_t<Ty>, ArgTy...>;
