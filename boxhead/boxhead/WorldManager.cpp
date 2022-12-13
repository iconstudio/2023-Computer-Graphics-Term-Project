#include "pch.hpp"
#include "WorldManager.hpp"
#include "Framework.hpp"

void WorldManager::Awake()
{
	// 딱 한번만 높이 맵 생성
	std::fstream stage_file{ stageFilepath, std::ios::in };

	if (stage_file)
	{
		terrainData = new int* [boardSizeH];
		for (size_t y = 0; y < boardSizeW; y++)
		{
			terrainData[y] = new int[boardSizeW];
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
					heightMap.emplace_back(i, j, float(terrain_cell));
				}
			}
		}
	}
}

void WorldManager::Start(Scene* scene)
{
	// 모델 가져오기
	auto wall_model_view = ModelView::GetReference<SideCubeModel>();
	
	int test = 0;
	
	// 높이 맵의 내용대로 벽 생성
	for (auto& height_block : heightMap)
	{
		const float cheight = MakeHeight(height_block.myHeight);
		const float cx = boardScaleW * static_cast<float>(height_block.x);
		const float cy = constants::GROUND_Y + 0.5f * cheight;
		const float cz = boardScaleD * static_cast<float>(height_block.y);

		Entity* wall = scene->CreateEntity<Entity>(wall_model_view, cx, cy, cz);
		wall->Scale(boardScaleW, cheight, boardScaleD);
		wall->myName = "Wall";
		wall->isStatic = true;

		auto& collider = wall->GetCollider();
		collider.SetCenter(wall->GetPosition());
		collider.SetExtent(boardScaleW * 0.5f, cheight * 0.5f, boardScaleD * 0.5f);
		
		if (2 < ++test)
		{
			break;
		}
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
			TileCell tile{};

			const float cx = static_cast<float>(j) * boardScaleW * 2;
			const float cy = static_cast<float>(i) * boardScaleD * 2;

			tile.worldMatrix = glm::scale(glm::translate(ogl::identity, { cx, constants::GROUND_Y, cy }), glm::vec3{ 4.0f });
			tile.textureID = ground_textures[(::rand() % 6)];

			tileMap.push_back(tile);
		}
	}
}

void WorldManager::Render(ModelView model, ogl::Uniform& world_uniform, ogl::Uniform& texture_uniform)
{
	for (auto& tile : tileMap)
	{
		world_uniform.AssignMatrix4x4(tile.worldMatrix);

		const GLint& texid = tile.textureID;

		texture_uniform.ActiveTexture(texid - 1);
		texture_uniform.BindTexture(texid);

		model.Render();
	}
}
