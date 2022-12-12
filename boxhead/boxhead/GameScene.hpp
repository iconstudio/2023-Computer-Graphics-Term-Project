#pragma once
#define NOMINMAX
#include <windows.h>

#include "GameViewManger.hpp"
#include "Player.hpp"
#include "WorldManager.hpp"
#include "AxisModel.hpp"
#include "FloorModel.hpp"
#include "SideCubeModel.hpp"

using namespace camera;

class WorldManager
{
public:
	constexpr WorldManager(float xscale, float yscale, float zscale)
		: heightMap()
		, stageFilepath("Stage.txt")
		, boardScaleW(xscale), boardScaleH(yscale), boardScaleD(zscale)
	{
		heightMap.reserve(boardSizeW * boardSizeH + 1);
	}

	~WorldManager()
	{}

	void Awake()
	{
		// 딱 한번만 높이 맵 생성
		std::fstream stage_file{ stageFilepath, std::ios::in };

		if (stage_file)
		{
			terrainMap = new int* [boardSizeH];
			for (size_t y = 0; y < boardSizeW; y++)
			{
				terrainMap[y] = new int[boardSizeW];
			}

			char piece{};
			size_t x = 0, y = 0;

			while (stage_file >> piece)
			{
				if (' ' == piece) continue;

				SetTerrainAt(x, y, int(piece - '0'));

				x++;
				if (boardSizeW <= x)
				{
					x = 0;
					y++;
				}
			}

			for (size_t i = 0; i < boardSizeW; i++)
			{
				for (size_t j = 0; j < boardSizeH; j++)
				{
					// 열 우선으로 삽입
					auto& terrain_cell = GetTerrainAt(i, j);

					if (0 < terrain_cell)
					{
						float cell_height = 1.0f * float(terrain_cell);
						heightMap.emplace_back(i, j, cell_height);
					}
				}
			}
		}
	}

	void Start(Scene* scene)
	{
		// 모델 가져오기
		auto wall_model_view = ModelView::GetReference<SideCubeModel>();

		// 높이 맵의 내용대로 벽 생성
		for (auto& height_block : heightMap)
		{
			const float cx = boardScaleW * static_cast<float>(height_block.x);
			const float cy = -(1 - height_block.myHeight) * 0.5f;
			const float cz = boardScaleD * static_cast<float>(height_block.y);

			Entity* wall = scene->CreateEntity<Entity>(wall_model_view, cx, cy, cz);
			wall->Scale(boardScaleW, height_block.myHeight, boardScaleD);
		}
	}

	constexpr Block& CellAt(const size_t& x, const size_t& y)
	{
		const auto pos = x * boardSizeH + y;

		return heightMap.at(pos);
	}

	constexpr const Block& CellAt(const size_t& x, const size_t& y) const
	{
		const auto pos = x * boardSizeH + y;

		return heightMap.at(pos);
	}

	constexpr int& SetTerrainAt(const size_t& x, const size_t& y, const int& value)
	{
		auto& cell = GetTerrainAt(x, y);
		cell = value;

		return cell;
	}

	constexpr int& GetTerrainAt(const size_t& x, const size_t& y)
	{
		//return terrainMap.at(y).at(x);
		return terrainMap[y][x];
	}

	constexpr const int& GetTerrainAt(const size_t& x, const size_t& y) const
	{
		//return terrainMap.at(y).at(x);
		return terrainMap[y][x];
	}

	static inline constexpr size_t boardSizeW = 40;
	static inline constexpr size_t boardSizeH = 40;

private:
	float boardScaleW;
	float boardScaleD;
	float boardScaleH;

	//int terrainMap[boardSizeH][boardSizeW];
	int** terrainMap;
	std::string stageFilepath;

	std::vector<Block> heightMap;
};

class GameScene : public Scene
{
public:
	constexpr GameScene(const size_t& id)
		: Scene(id)
		, myRenderer(), textureRenderer()
		, worldManager(tileSizeX, tileSizeY, tileSizeZ), viewManager()
		, windowFocused(false), cursorClicked(false), cursorPosition(), clientRect()
		, playerCharacter(nullptr), playerSpawnPosition(1.0f, 1.0f, 1.0f)
	{
		SetName("GameScene");
	}

	void Awake() override
	{
		Scene::Awake();

		myRenderer.Awake();
		myRenderer.LoadVertexShader("..\\Shaders\\PlainV.glsl");
		myRenderer.LoadFragmentShader("..\\Shaders\\PlainP.glsl");
		myRenderer.Ready();

		textureRenderer.Awake();
		textureRenderer.LoadVertexShader("..\\Shaders\\TextureV.glsl");
		textureRenderer.LoadFragmentShader("..\\Shaders\\TextureP.glsl");
		textureRenderer.Ready();

		viewManager.Awake();

		playerCharacter = new Player{ playerSpawnPosition };
		playerCharacter->myCamera = &viewManager.GetCamera();
		viewManager.SetFollower(playerCharacter);

		worldManager.Awake();
		worldManager.Start(this);
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

		playerCharacter->Update(delta_time);

		viewManager.Update(delta_time);

		const auto focus = GetFocus();
		const auto capture = GetCapture();

		if (focus == WindowManager::windowHandle)
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
		playerCharacter->OnKeyboard(key, x, y);
	}

	void OnKeyboardUp(const unsigned char& key, const int& x, const int& y) override
	{
		playerCharacter->OnKeyboard(key, x, y);
	}

	void OnSpecialKey(const int& key, const int& x, const int& y)
	{
		playerCharacter->OnSpecialKey(key, x, y);
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

	void PrepareRendering() override
	{}

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
	
private:
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

	const float tileSizeX = 1.0f;
	const float tileSizeY = 1.0f;
	const float tileSizeZ = 1.0f;
	WorldManager worldManager;

	float gameStartDelay;
	int waveLevel;
	float waveTime;
	float wavePeriod;
	float waveBetweenDelay;
};
