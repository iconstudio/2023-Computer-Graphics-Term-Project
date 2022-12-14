#pragma once
#include "Scene.hpp"

class MainScene : public Scene
{
public:
	MainScene(const size_t& id)
		: Scene(id)
		, textureRenderer(), mainCamera(ogl::up)
		, titleCoords(0.5f, 0.5f, 0.5f)
	{
		SetName("MainScene");
	}

	void Awake() override
	{
		textureRenderer.Awake();
		textureRenderer.LoadVertexShader("..\\Shaders\\TextureV.glsl");
		textureRenderer.LoadFragmentShader("..\\Shaders\\TextureP.glsl");
		textureRenderer.Ready();

		camera::PerspectiveCameraSetting pr_setting{};
		pr_setting.projectionFieldOfView = 40.0f;
		pr_setting.projectionRatio = 16.0f / 9.0f;
		pr_setting.projectionNear = 0.1f;
		pr_setting.projectionFar = 10000.0f;

		camera::OrthodoxCameraSetting ox_setting{};
		ox_setting.projectionLeft = 480.0f;
		ox_setting.projectionRight = 480.0f;
		ox_setting.projectionBottom = -270.0f;
		ox_setting.projectionTop = 270.0f;

		mainCamera.Setup(pr_setting);
		mainCamera.Setup(ox_setting);
		mainCamera.Awake();

		ogl::background_color = { 0.1f, 0.1f, 0.1f, 1.0f };
	}

	void Start() override
	{
		Scene::Start();

		textureRenderer.Start();
	}

	void OnKeyboard(const unsigned char& key, const int& x, const int& y) override
	{
		switch (key)
		{
			case 13: // Enter
			{
				End();
			}
			break;

			case 27: // Escape
			{
				std::quick_exit(0);
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

	void OnMouse(const int& button, const int& state, const int& x, const int& y) override
	{
		if (ogl::IsMouseClicked(state))
		{
			if (ogl::IsLeftMouseButton(button))
			{
				//ogl::background_color.r = RandomizeColour() * 0.5f;
				//ogl::background_color.g = RandomizeColour() * 0.5f;
				//ogl::background_color.b = RandomizeColour() * 0.5f;
			}
		}
	}

	void Render() override;

	ogl::Pipeline textureRenderer;
	camera::Camera mainCamera;
	
	glm::vec3 titleCoords;

	float introFadeTime = 4.0f;
};
