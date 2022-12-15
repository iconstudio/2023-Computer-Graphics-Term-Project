#pragma once
#define NOMINMAX
#include <windows.h>

#include "GameViewManger.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "WorldManager.hpp"
#include "AxisModel.hpp"
#include "FloorModel.hpp"
#include "SideCubeModel.hpp"

using namespace camera;

class GameScene : public Scene
{
public:
	constexpr GameScene(const size_t& id)
		: Scene(id)
		, myRenderer(), textureRenderer()
		, worldManager(tileSizeX, tileSizeY, tileSizeZ), viewManager()
		, windowFocused(false), cursorClicked(false), cursorPosition(), clientRect()
		, playerCharacter(nullptr), playerSpawnPosition(3.0f, 1.0f, 3.0f)
		, everyEnemy()
	{
		SetName("GameScene");
		everyEnemy.reserve(50);

		Instance = this;
	}

	void Awake() override
	{
		Scene::Awake();

		myRenderer.Awake();
		myRenderer.LoadVertexShader("shaders\\PlainV.glsl");
		myRenderer.LoadFragmentShader("shaders\\PlainP.glsl");
		myRenderer.Ready();

		textureRenderer.Awake();
		textureRenderer.LoadVertexShader("shaders\\TextureV.glsl");
		textureRenderer.LoadFragmentShader("shaders\\TextureP.glsl");
		textureRenderer.Ready();

		viewManager.Awake();

		playerCharacter = new Player{ playerSpawnPosition };
		playerCharacter->myCamera = &viewManager.GetCamera();
		viewManager.SetFollower(playerCharacter);

		AddEntity(playerCharacter);

		worldManager.Awake();
		worldManager.Start(this);

		ogl::background_color = { 0.0f, 0.5f, 1.0f, 1.0f };
	}

	void Start() override
	{
		Scene::Start();

		viewManager.Start();
		playerCharacter->Start();

		windowFocused = true;
		UpdateClientRect();
		ResetCursorPosNow();
		FocusCursor();

		myRenderer.Start();
		textureRenderer.Start();
	}

	void Update() override
	{
		const auto delta_time = Timer::GetDeltaTime();

		Scene::Update();
		
		auto pl_position = playerCharacter->GetPosition();
		const auto pl_bounds = playerCharacter->GetBounds();
		const auto bound_x = pl_bounds.x / 2;
		const auto bound_z = pl_bounds.z / 2;

		if (pl_position.x - bound_x < worldManager.GetMinX())
		{
			pl_position.x = worldManager.GetMinX() + bound_x;
		}
		else if (worldManager.GetMaxX() < pl_position.x + bound_x)
		{
			pl_position.x = worldManager.GetMaxX() - bound_x;
		}

		if (pl_position.z - bound_x < worldManager.GetMinZ())
		{
			pl_position.z = worldManager.GetMinZ() + bound_z;
		}
		else if (worldManager.GetMaxZ() < pl_position.z + bound_z)
		{
			pl_position.z = worldManager.GetMaxZ() - bound_z;
		}

		playerCharacter->SetPosition(pl_position);

		UpdateEnemyList();

		viewManager.Update(delta_time);

		const auto focus = GetFocus();
		const auto capture = GetCapture();

		if (focus == WindowManager::windowHandle && capture == WindowManager::windowHandle)
		{
			if (windowFocused)
			{
				HideCursor();

				POINT mouse{};
				GetCursorPos(&mouse);

				const int dx = mouse.x - cursorPosition.x;
				const int dy = mouse.y - cursorPosition.y;

				viewManager.MouseTrack(dx, dy, delta_time);

				playerCharacter->SetRotation(0.0f, viewManager.cameraYaw, 0.0f);

				const int tx = clientRect.left + int(clientRect.right - clientRect.left) / 2;
				const int ty = clientRect.top + int(clientRect.bottom - clientRect.top) / 2;

				SetCursorPos(tx, ty);
				cursorPosition = { tx, ty };
			}
		}
		else
		{
			const auto capture = GetCapture();

			if (capture == WindowManager::windowHandle)
			{
				windowFocused = false;
				ReleaseCapture();
			}
		}
	}

	void OnUpdateView(const int& w, const int& h) override
	{
		UpdateClientRect();

		FocusCursor();
	}

	void OnKeyboard(const unsigned char& key, const int& x, const int& y) override
	{
		switch (key)
		{
			case 't':
			case 'T':
			{
				const auto capture = GetCapture();

				if (capture == WindowManager::windowHandle)
				{
					windowFocused = false;
					ReleaseCapture();
				}
			}
			break;
		}

		playerCharacter->OnKeyboard(key, x, y);
	}

	void OnKeyboardUp(const unsigned char& key, const int& x, const int& y) override
	{
		playerCharacter->OnKeyboard(key, x, y);
	}

	void OnSpecialKey(const int& key, const int& x, const int& y)
	{
		playerCharacter->OnSpecialKey(key, x, y);

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
			if (!cursorClicked)
			{
				ResetCursorPosNow();

				cursorClicked = true;
			}
		}
		else if (ogl::IsMouseReleased(state))
		{
			cursorClicked = false;

			const auto capture = GetCapture();

			if (capture != WindowManager::windowHandle)
			{
				windowFocused = true;
				SetCapture(WindowManager::windowHandle);
			}
		}

		playerCharacter->OnMouse(button, state, x, y);
	}

	void OnMouseMotion(const int& x, const int& y) override
	{
		if (cursorClicked)
		{
			//SetCapture(WindowManager::windowHandle);
		}
	}

	void Render() override;

	void Cleanup() override
	{
		ShowCursor();

		const auto capture = GetCapture();
		if (capture == WindowManager::windowHandle)
		{
			ReleaseCapture();
		}

		ClipCursor(NULL);
	}

	constexpr float GetActualHeight(const float& x, const float& z) const
	{
		return worldManager.GetActualHeight(x, z);
	}

	static GameScene* Instance;

private:
	template<typename Ty, typename... Args>
		requires EnemyType<Ty, Args...>
	Ty* CreateEnemy(Args... args)
	{
		auto* result = Enemy::Instantiate<Ty>(std::forward<Args>(args)...);
		everyEnemy.push_back(result);

		return result;
	}

	void UpdateEnemyList()
	{
		for (auto it = everyEnemy.begin(); it != everyEnemy.end();)
		{
			auto inst = *it;

			if (inst->myHealth)
			{
				it = everyEnemy.erase(it);
			}
			else
			{
				it++;
			}
		}
	}

	void UpdateClientRect()
	{
		GetClientRect(WindowManager::windowHandle, &clientRect);
	}

	void ResetCamera()
	{
		viewManager.Reset();
	}

	void ShowCursor()
	{
		::ShowCursor(TRUE);
	}

	void HideCursor()
	{
		::ShowCursor(FALSE);
	}

	void FocusCursor()
	{
		RECT temp_rect{};
		GetClientRect(WindowManager::windowHandle, &clientRect);

		POINT pt1{}, pt2{};
		pt1.x = clientRect.left;
		pt1.y = clientRect.top;
		pt2.x = clientRect.right;
		pt2.y = clientRect.bottom;

		ClientToScreen(WindowManager::windowHandle, &pt1);
		ClientToScreen(WindowManager::windowHandle, &pt2);
		clientRect.left = pt1.x;
		clientRect.top = pt1.y;
		clientRect.right = pt2.x;
		clientRect.bottom = pt2.y;

		ClipCursor(&clientRect);
	}

	void ResetCursorPosNow()
	{
		POINT mouse{};
		GetCursorPos(&mouse);

		cursorPosition = { mouse.x, mouse.y };
	}

	ogl::Pipeline myRenderer;
	ogl::Pipeline textureRenderer;

	bool windowFocused;
	bool cursorClicked;
	POINT cursorPosition;
	RECT clientRect;

	GameViewManger viewManager;

	Player* playerCharacter;
	const glm::vec3 playerSpawnPosition;
	std::vector<Enemy*> everyEnemy;

	const float tileSizeX = 2.0f;
	const float tileSizeY = 1.0f;
	const float tileSizeZ = 2.0f;
	WorldManager worldManager;

	float gameStartDelay;
	int waveLevel;
	float waveTime;
	float wavePeriod;
	float waveBetweenDelay;

	friend class WorldManager;
	friend class Scene;
};
