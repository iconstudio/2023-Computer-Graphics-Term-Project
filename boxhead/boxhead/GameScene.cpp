#include "pch.hpp"
#include "GameScene.hpp"
#include "Framework.hpp"

void WorldManager::Awake()

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

void WorldManager::Start(Scene* scene)
{
	// 모델 가져오기
	auto wall_model_view = ModelView::GetReference<SideCubeModel>();

	// 높이 맵의 내용대로 벽 생성
	for (auto& height_block : heightMap)
	{
		const float cheight = boardScaleH * static_cast<float>(height_block.myHeight);
		const float cx = boardScaleW * static_cast<float>(height_block.x);
		const float cy = 0.5f * cheight;
		const float cz = boardScaleD * static_cast<float>(height_block.y);

		Entity* wall = scene->CreateEntity<Entity>(wall_model_view, cx, cy, cz);
		wall->Scale(boardScaleW, cheight, boardScaleD);
	}

	// 타일 텍스쳐 가져오기
	GLint ground_textures[6]{};

	ground_textures[0] = Framework::GetTexture(0); // ("Dirt 0");
	ground_textures[1] = Framework::GetTexture(1); // ("Dirt 0");
	ground_textures[2] = Framework::GetTexture(2); // ("Dirt 0");
	ground_textures[3] = Framework::GetTexture(3); // ("Dirt 0");
	ground_textures[4] = Framework::GetTexture(4); // ("Dirt 0");
	ground_textures[5] = Framework::GetTexture(5); // ("Dirt 0");

	for (size_t i = 0; i < tileCountH; i++)
	{
		for (size_t j = 0; j < tileCountW; j++)
		{
			auto& tile = tileMap[i][j];

			tile.x = static_cast<float>(j) * boardScaleW;
			tile.y = static_cast<float>(i) * boardScaleD;
			tile.textureID = ground_textures[(::rand() % 6)];
		}
	}
}

void GameScene::Render()
{
	const auto& matrix_cam = viewManager.GetCameraMatrix();
	const auto& matrix_view = viewManager.GetPerspectiveViewMatrix();

	textureRenderer.PrepareRendering();
	auto tex_uniform_world = textureRenderer.GetUniform("a_WorldMatrix");
	auto tex_uniform_camera = textureRenderer.GetUniform("a_CameraMatrix");
	auto tex_uniform_proj = textureRenderer.GetUniform("a_ProjMatrix");
	auto tex_uniform_sample = textureRenderer.GetUniform("u_Texture");

	auto& floor_texture = Framework::GetTextureBuffer(0);

	tex_uniform_camera.AssignMatrix4x4(matrix_cam);
	tex_uniform_proj.AssignMatrix4x4(matrix_view);

	// x, y, z, s, t
	constexpr GLsizei shade_tex = sizeof(float) * 5;

	auto attr_texpos = textureRenderer.BeginAttribute("a_Position", shade_tex);
	auto attr_texcoord = textureRenderer.BeginAttribute("a_TexCoord", shade_tex);

	// 3: 텍스쳐 바닥
	auto model_texfloor = ModelView::GetReference(3);
	model_texfloor.PrepareRendering();
	textureRenderer.ReadBuffer(attr_texpos, 3);
	textureRenderer.ReadBuffer(attr_texcoord, 2);

	auto tile_matrix = ogl::identity;
	tex_uniform_world.AssignMatrix4x4(tile_matrix);
	worldManager.Render(model_texfloor, tex_uniform_world, tex_uniform_sample);

	//model_texfloor.Render();
	textureRenderer.ResetSeekBuffer();

	myRenderer.PrepareRendering();
	auto uniform_mat_world = myRenderer.GetUniform("a_WorldMatrix");
	auto uniform_mat_camera = myRenderer.GetUniform("a_CameraMatrix");
	auto uniform_mat_proj = myRenderer.GetUniform("a_ProjMatrix");

	uniform_mat_world.AssignMatrix4x4(ogl::identity);
	uniform_mat_camera.AssignMatrix4x4(matrix_cam);
	uniform_mat_proj.AssignMatrix4x4(matrix_view);

	// x, y, z, r, g, b, a
	constexpr GLsizei shade_stride = sizeof(float) * 7;

	// x, y, z, r, g, b, a, nx, ny, nz
	constexpr GLsizei normal_stride = sizeof(float) * 10;

	auto attr_pos = myRenderer.BeginAttribute("a_Position", shade_stride);
	auto attr_col = myRenderer.BeginAttribute("a_Colour", shade_stride);

	// 1: 좌표축
	auto model_axis = ModelView::GetReference(1);
	model_axis.PrepareRendering();
	myRenderer.ReadBuffer(attr_pos, 3);
	myRenderer.ReadBuffer(attr_col, 4);

	model_axis.Render();
	myRenderer.ResetSeekBuffer();

	for (auto& instance : myInstances)
	{
		//  0: 큐브
		instance->PrepareRendering();

		myRenderer.ReadBuffer(attr_pos, 3);
		myRenderer.ReadBuffer(attr_col, 4);

		instance->Render(uniform_mat_world);
		myRenderer.ResetSeekBuffer();
	}

	attr_pos.DisableVertexArray();
	attr_col.DisableVertexArray();
}
