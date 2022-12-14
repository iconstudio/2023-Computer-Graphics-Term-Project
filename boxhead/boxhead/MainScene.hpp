#pragma once
#include <Blobs.hpp>
#include <BlobUtils.hpp>

#include "Scene.hpp"

class MainScene : public Scene
{
private:
	enum class State
	{
		INTRO, MENU, HELP, OUTRO, EXIT, NEXT_ROOM
	};

public:
	MainScene(const size_t& id)
		: Scene(id)
		, fadeRenderer(), fadeBuffer()
		, textureRenderer()
		, titleCoords(0.0f, 0.2f, 0.0f)
		, myStatus(State::INTRO), doingAppQuit(false)
		, introFadeTime(introFadePeriod), outroFadeTime(outroFadePeriod)
	{
		SetName("MainScene");
	}

	void Awake() override
	{
		textureRenderer.Awake();
		textureRenderer.LoadVertexShader("..\\Shaders\\TextureV.glsl");
		textureRenderer.LoadFragmentShader("..\\Shaders\\TextureP.glsl");
		textureRenderer.Ready();

		fadeRenderer.Awake();
		fadeRenderer.LoadVertexShader("..\\Shaders\\CircleV.glsl");
		fadeRenderer.LoadFragmentShader("..\\Shaders\\CircleP.glsl");
		fadeRenderer.Ready();

		fadeBuffer.Reserve(20);
		
		constexpr auto fade_color = ogl::Colour{ 1.0f, 1.0f, 1.0f, 1.0f };
		const ogl::Vertex2D rect[] =
		{
			{ -1.0f, -1.0f, fade_color },
			{ +1.0f, -1.0f, fade_color },
			{ +1.0f, +1.0f, fade_color },
			{ -1.0f, +1.0f, fade_color }
		};
		
		auto& floor_buffer = fadeBuffer.Push(rect);

		ogl::background_color = { 0.1f, 0.1f, 0.1f, 1.0f };
	}

	void Start() override
	{
		Scene::Start();

		textureRenderer.Start();
		fadeRenderer.Start();

		myStatus = State::INTRO;
		introFadeTime = introFadePeriod;
		outroFadeTime = outroFadePeriod;
	}

	void Update() override
	{
		const auto delta_time = Timer::GetDeltaTime();

		switch (myStatus)
		{
			case State::INTRO:
			{
				if (0 < introFadeTime)
				{
					introFadeTime -= delta_time;
				}
				else
				{
					myStatus = State::MENU;
					introFadeTime = 0;
				}
			}
			break;

			case State::MENU:
			{

			}
			break;

			case State::OUTRO:
			{
				if (0 < outroFadeTime)
				{
					outroFadeTime -= delta_time;
				}
				else
				{
					if (doingAppQuit)
					{
						myStatus = State::EXIT;
					}
					else
					{
						myStatus = State::NEXT_ROOM;
					}

					outroFadeTime = 0;
				}
			}
			break;

			case State::NEXT_ROOM:
			{
				End();
			}
			break;

			case State::EXIT:
			{
				std::quick_exit(0);
			}
			return;
		}
	}
	
	void OnKeyboard(const unsigned char& key, const int& x, const int& y) override
	{
		switch (key)
		{
			case 13: // Enter
			{
				if (State::HELP == myStatus)
				{
					SetState(State::MENU);
				}
				else if (State::MENU == myStatus)
				{
					SetState(State::OUTRO);
				}
				else
				{
					
				}
			}
			break;

			case 27: // Escape
			{
				SetState(State::OUTRO);
				doingAppQuit = true;
			}
			break;
		}
	}

	void OnSpecialKey(const int& key, const int& x, const int& y) override
	{
		switch (key)
		{
			case GLUT_KEY_LEFT:
			{

			}
			break;

			case GLUT_KEY_RIGHT:
			{

			}
			break;

			case GLUT_KEY_UP:
			{

			}
			break;

			case GLUT_KEY_DOWN:
			{

			}
			break;

			case GLUT_KEY_F4:
			{
				std::quick_exit(0);
			}
			break;
		}
	}
	
	void Render() override;

	constexpr void SetState(const State& state)
	{
		myStatus = state;
	}

	ogl::Pipeline fadeRenderer;
	ogl::Pipeline textureRenderer;
	ogl::VertexStream fadeBuffer;
	
	glm::vec3 titleCoords;

	State myStatus;
	bool doingAppQuit;
	float introFadeTime;
	const float introFadePeriod = 0.8f;
	float outroFadeTime;
	const float outroFadePeriod = 1.0f;
};
