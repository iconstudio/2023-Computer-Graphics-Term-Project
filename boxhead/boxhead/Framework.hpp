#pragma once
#include <OpenGL.hpp>
#include <Pipeline.hpp>
#include <VertexStream.hpp>
#include <Blobs.hpp>
#include <Utils.hpp>
#include <BlobUtils.hpp>

#include "SideCubeModel.hpp"
#include "CubeModel.hpp"
#include "AxisModel.hpp"
#include "FloorModel.hpp"
#include "TexturePlaneModel.hpp"
#include "TextureCubeModel.hpp"

class Framework
{
public:
	Framework()
		: modelBuffer(), textureBuffer()
		, myScenes()
		, currentScene(nullptr), reservatedScene(nullptr)
		, sceneProcessFinished(false)
		, gameModels(), gameModelIDs()
		, gameTextures(), gameTextureIDs()
	{
		myScenes.reserve(10);
		gameModels.reserve(20);
		gameModelIDs.reserve(20);
		gameTextureIDs.reserve(20);

		Instance = this;
	}

	void Awake()
	{
		modelBuffer.Reserve(20);
		textureBuffer.Reserve(10);

		CreateModels();
		CreateTextures();

		if (0 < myScenes.size())
		{
			auto first_scene = myScenes.at(0);

			ChangeScene(first_scene);
		}
	}

	void Start()
	{
		if (currentScene && !currentScene->IsEnded())
		{
			sceneProcessFinished = false;
		}
	}

	void Update()
	{
		if (reservatedScene && sceneProcessFinished)
		{
			ChangeScene(reservatedScene);
		}

		if (currentScene)
		{
			sceneProcessFinished = false;

			if (currentScene->IsEnded())
			{
				Scene* next_scene;
				try
				{
					next_scene = myScenes.at(currentScene->GetID() + 1);

					ChangeScene(next_scene);
				}
				catch (...)
				{
					currentScene = nullptr;
				}

				return;
			}

			currentScene->Update();
		}
	}

	void OnUpdateView(const int& w, const int h)
	{
		if (currentScene && !currentScene->IsEnded())
		{
			currentScene->OnUpdateView(w, h);
		}
	}

	void OnKeyboard(const unsigned char& key, const int& x, const int& y)
	{
		if (currentScene && !currentScene->IsEnded())
		{
			currentScene->OnKeyboard(key, x, y);
		}
	}

	void OnKeyboardUp(const unsigned char& key, const int& x, const int& y)
	{
		if (currentScene && !currentScene->IsEnded())
		{
			currentScene->OnKeyboardUp(key, x, y);
		}
	}

	void OnSpecialKey(const int& key, const int& x, const int& y)
	{
		if (currentScene && !currentScene->IsEnded())
		{
			currentScene->OnSpecialKey(key, x, y);
		}
	}

	void OnMouse(const int& button, const int& state, const int& x, const int& y)
	{
		if (currentScene && !currentScene->IsEnded())
		{
			currentScene->OnMouse(button, state, x, y);
		}
	}

	void OnMouseMotion(const int& x, const int& y)
	{
		if (currentScene && !currentScene->IsEnded())
		{
			currentScene->OnMouseMotion(x, y);
		}
	}

	void PrepareRendering()
	{
		if (currentScene && currentScene->IsStarted())
		{
			sceneProcessFinished = false;

			currentScene->PrepareRendering();
		}
	}

	void Render()
	{
		if (currentScene && currentScene->IsStarted())
		{
			currentScene->Render();

			sceneProcessFinished = true;
		}
	}

	Scene* AddScene(Scene* const scene)
	{
		myScenes.push_back(scene);

		return scene;
	}

	template<typename Ty, typename ...ArgTy>
		requires SceneType<Ty, size_t, ArgTy...>
	Ty* AddScene(ArgTy&&... arg)
	{
		const size_t count = myScenes.size();

		Ty* scene = new Ty{ count, std::forward<ArgTy>(arg)... };

		myScenes.push_back(scene);

		return scene;
	}

	Scene* const GetScene(const size_t& index) const
	{
		return myScenes.at(index);
	}

	void ChangeScene(Scene* scene)
	{
		if (!currentScene)
		{
			sceneProcessFinished = true;
		}

		if (sceneProcessFinished)
		{
			ChangeSceneNow(scene);

			reservatedScene = nullptr;
		}
		else
		{
			reservatedScene = scene;
		}
	}

	void ChangeScene(const size_t& index)
	{
		auto target = GetScene(index);

		if (target)
		{
			ChangeScene(target);
		}
	}

	template<ModelType Ty>
	Ty* AddModel(std::string_view name, ogl::VertexStream::Buffer& buffer)
	{
		AddModelID(name, gameModels.size());

		Ty* model = new Ty{ name, buffer };
		gameModels.push_back(model);

		return model;
	}

	Model* const GetModel(const size_t& id)
	{
		return gameModels.at(id);
	}

	const Model* const GetModel(const size_t& id) const
	{
		return gameModels.at(id);
	}

	Model* const GetModel(std::string_view name)
	{
		return gameModels.at(FindModelID(name));
	}

	const Model* const GetModel(std::string_view name) const
	{
		return gameModels.at(FindModelID(name));
	}

	GLint LoadTexture(std::string_view name, std::string_view filepath)
	{
		AddTextureID(name, gameTextureIDs.size());

		GLint texture = InternalLoadTexture(filepath);

		gameTextures.push_back(texture);

		return texture;
	}

	static ogl::VertexStream::Buffer& GetTextureBuffer(const size_t& id)
	{
		return Instance->textureBuffer.At(id);
	}

	static GLint GetTexture(const size_t& index)
	{
		return Instance->gameTextures.at(index);
	}

	static GLint GetTexture(std::string_view name)
	{
		return Instance->gameTextures.at(Instance->FindTextureID(name));
	}

	void AddModelID(std::string_view name, const size_t& id)
	{
		gameModelIDs.insert({ std::string{ name }, id });
	}

	size_t FindModelID(std::string_view name) const
	{
		auto it = gameModelIDs.find(std::string{ name });
		if (gameModelIDs.cend() != it)
		{
			return size_t(-1);
		}

		return it->second;
	}

	void AddTextureID(std::string_view name, const size_t& id)
	{
		gameTextureIDs.insert({ std::string{ name }, id });
	}

	size_t FindTextureID(std::string_view name) const
	{
		auto it = gameTextureIDs.find(std::string{ name });
		if (gameTextureIDs.cend() != it)
		{
			return size_t(-1);
		}

		return it->second;
	}

	static Framework* Instance;

private:
	void ChangeSceneNow(Scene* scene)
	{
		if (currentScene)
		{
			currentScene->Cleanup();
		}

		currentScene = scene;
		sceneProcessFinished = false;

		if (!currentScene->IsAwaken())
		{
			currentScene->Awake();
		}

		if (!currentScene->IsStarted())
		{
			currentScene->Start();
		}
	}

	ogl::VertexStream::Buffer& GetVertexBuffer(const size_t& index)
	{
		return modelBuffer.At(index);
	}

	void CreateModels()
	{
		constexpr auto spatial_c1 = ogl::Colour{ 0.0f, 1.0f, 1.0f, 1.0f };
		constexpr auto spatial_c2 = ogl::Colour{ 1.0f, 0.0f, 1.0f, 1.0f };
		constexpr auto spatial_c3 = ogl::Colour{ 1.0f, 1.0f, 0.0f, 1.0f };
		constexpr auto spatial_c4 = ogl::Colour{ 1.0f, 0.0f, 0.0f, 1.0f };
		constexpr auto spatial_c5 = ogl::Colour{ 0.0f, 1.0f, 0.0f, 1.0f };
		constexpr auto spatial_c6 = ogl::Colour{ 0.0f, 0.0f, 1.0f, 1.0f };
		// °ËÀº»ö
		constexpr auto spatial_c7 = ogl::Colour{ 0.0f, 0.0f, 0.0f, 1.0f };
		// Èò»ö
		constexpr auto spatial_c8 = ogl::Colour{ 1.0f, 1.0f, 1.0f, 1.0f };
		// ±Ý»ö
		constexpr auto spatial_c9 = ogl::Colour{ 1.0f, 0.8f, 0.1f, 1.0f };

		constexpr ogl::Quad pt1 = { -0.5f, +0.5f, -0.5f };
		constexpr ogl::Quad pt2 = { -0.5f, +0.5f, +0.5f };
		constexpr ogl::Quad pt3 = { +0.5f, +0.5f, +0.5f };
		constexpr ogl::Quad pt4 = { +0.5f, +0.5f, -0.5f };
		constexpr ogl::Quad pt5 = { -0.5f, -0.5f, -0.5f };
		constexpr ogl::Quad pt6 = { -0.5f, -0.5f, +0.5f };
		constexpr ogl::Quad pt7 = { +0.5f, -0.5f, +0.5f };
		constexpr ogl::Quad pt8 = { +0.5f, -0.5f, -0.5f };

		constexpr ogl::blob::ColoredPlane each_sides[] =
		{
			ogl::blob::plane::Create(pt1, pt2, pt3, pt4, spatial_c1),
			ogl::blob::plane::Create(pt1, pt5, pt6, pt2, spatial_c2),
			ogl::blob::plane::Create(pt2, pt6, pt7, pt3, spatial_c3),
			ogl::blob::plane::Create(pt1, pt4, pt8, pt5, spatial_c9),
			ogl::blob::plane::Create(pt3, pt7, pt8, pt4, spatial_c5),
			ogl::blob::plane::Create(pt5, pt8, pt7, pt6, spatial_c6)
		};
		const auto raw_cube = ogl::blob::cube::Create(each_sides);

		// Model 0: Å¥ºê
		auto& cube_buffer = modelBuffer.Push(raw_cube);

		// 
		constexpr auto axis_color = ogl::Colour{ 0.0f, 0.0f, 0.0f, 1.0f };
		const ogl::Vertex axis_lines[] =
		{
			{ +400.0f, 0.0f, 0.0f, axis_color },
			{ -400.0f, 0.0f, 0.0f, axis_color },
			{ 0.0f, +300.0f, 0.0f, axis_color },
			{ 0.0f, -300.0f, 0.0f, axis_color },
			{ 0.0f, 0.0f, -400.0f, axis_color },
			{ 0.0f, 0.0f, -400.0f, axis_color }
		};

		// Model 1: ÁÂÇ¥Ãà
		auto& axis_buffer = modelBuffer.PushRaw(axis_lines);

		// 
		constexpr auto floor_c1 = ogl::Colour{ 0.15f, 0.4f, 0.1f, 1.0f };
		constexpr auto floor_c2 = ogl::Colour{ 0.6f, 0.2f, 0.0f, 1.0f };
		constexpr auto floor_c3 = ogl::Colour{ 0.0f, 0.6f, 0.0f, 1.0f };
		constexpr ogl::blob::ColoredPlane floor = ogl::blob::plane::Create
		(
			{ -10.0f, 0.0f, -10.0f, floor_c1 },
			{ -10.0f, 0.0f, +10.0f, floor_c2 },
			{ +10.0f, 0.0f, +10.0f, floor_c3 },
			{ +10.0f, 0.0f, -10.0f, floor_c2 }
		);

		// Model 2: ¹Ù´Ú
		auto& floor_buffer = modelBuffer.Push(floor);

		// 
		const ogl::blob::TexturePlane texfloor = ogl::blob::plane::tex::Create
		(
			0.0f, 0.0f, 0.0f, glm::vec2{ 0.0f, 0.0f },
			0.0f, 0.0f, 1.0f, glm::vec2{ 0.0f, 1.0f },
			1.0f, 0.0f, 1.0f, glm::vec2{ 1.0f, 1.0f },
			1.0f, 0.0f, 0.0f, glm::vec2{ 1.0f, 0.0f }
		);

		// Model 3: ÅØ½ºÃÄ ¹Ù´Ú
		auto& texfloor_buffer = textureBuffer.Push(texfloor);

		// ¸ðµ¨ ÁØºñ
		AddModel<SideCubeModel>("Cube", cube_buffer);
		AddModel<AxisModel>("Axis", axis_buffer);
		AddModel<FloorModel>("Floor", floor_buffer);
		AddModel<TexturePlaneModel>("TextureFloor", texfloor_buffer);
	}

	void CreateTextures()
	{
		auto dirt_tex_0 = LoadTexture("Dirt 0", "textures/TDX0.bmp");
		auto dirt_tex_1 = LoadTexture("Dirt 1", "textures/TDX3.bmp");
		auto dirt_tex_2 = LoadTexture("Dirt 2", "textures/TDF1.bmp");
		auto dirt_tex_3 = LoadTexture("Dirt 3", "textures/TDF2.bmp");
		auto dirt_tex_4 = LoadTexture("Dirt 4", "textures/TDF3.bmp");
		auto dirt_tex_5 = LoadTexture("Dirt 5", "textures/TDF7.bmp");
	}

	GLuint InternalLoadTexture(std::string_view filepath);

	std::vector<Scene*> myScenes;
	Scene* currentScene;
	Scene* reservatedScene;
	bool sceneProcessFinished;

	ogl::VertexStream modelBuffer, textureBuffer;
	std::vector<Model*> gameModels;
	std::unordered_map<std::string, size_t> gameModelIDs;
	std::vector<GLint> gameTextures;
	std::unordered_map<std::string, size_t> gameTextureIDs;

	friend class Model;
	friend class ModelView;
};
