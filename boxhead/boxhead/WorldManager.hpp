#pragma once
#include "Wall.hpp"

struct HeightBlock
{
	constexpr HeightBlock(size_t ix, size_t iy, const float& height = 1.0f)
		: x(ix), y(iy), myHeight(height)
	{}

	constexpr HeightBlock& operator=(const float& height)
	{
		myHeight = height;

		return *this;
	}

	explicit operator float() const
	{
		return myHeight;
	}

	size_t x, y;
	float myHeight;
};

struct TileCell
{
	glm::mat4 worldMatrix;
	GLint textureID;
};

class WorldManager
{
public:
	constexpr WorldManager(float xscale, float yscale, float zscale)
		: heightMap(), tileMap()
		, stageFilepath("Stage.txt")
		, boardScaleW(xscale), boardScaleH(yscale), boardScaleD(zscale)
		, terrainData()
		, minPositionX(), minPositionZ(), maxPositionX(), maxPositionZ()
	{
		heightMap.reserve(boardSizeW * boardSizeH + 1);
		tileMap.reserve(boardSizeW * boardSizeH + 1);
	}

	~WorldManager()
	{}

	void Awake();

	void Start(Scene* scene);

	void Render(ModelView model, ogl::Pipeline& renderer, ogl::Attribute& position, ogl::Attribute& texcoords, ogl::Uniform& world_uniform, ogl::Uniform& texture_uniform);

	constexpr float GetActualHeight(const float& x, const float& z) const
	{
		const auto area_index = GetIndex(x, z);

		return constants::GROUND_Y + MakeHeight(area_index.first, area_index.second);
	}

	constexpr float GetMinX() const
	{
		return minPositionX;
	}

	constexpr float GetMaxX() const
	{
		return maxPositionX;
	}

	constexpr float GetMinZ() const
	{
		return minPositionZ;
	}

	constexpr float GetMaxZ() const
	{
		return maxPositionZ;
	}

private:
	constexpr float MakeHeight(const float& block_height) const
	{
		return boardScaleH * block_height;
	}

	constexpr float MakeHeight(const size_t& x, const size_t& z) const
	{
		return MakeHeight(static_cast<float>(GetTerrainAt(x, z)));
	}

	constexpr std::pair<size_t, size_t> GetIndex(const float& x, const float& z) const
	{
		float dx, dz;
		if (x < 0)
		{
			dx = 0.0f;
		}
		else
		{
			const float limit = boardScaleW * static_cast<float>(boardSizeW);
			if (limit <= x)
				dx = limit;
			else
				dx = x / boardScaleW;
		}

		if (z < 0)
		{
			dz = 0.0f;
		}
		else
		{
			const float limit = boardScaleD * static_cast<float>(boardSizeH);
			if (limit <= z)
				dz = limit;
			else
				dz = z / boardScaleD;
		}

		return std::make_pair(static_cast<size_t>(dx), static_cast<size_t>(dz));
	}

	constexpr HeightBlock& CellAt(const size_t& x, const size_t& y)
	{
		const auto pos = x * boardSizeH + y;

		return heightMap.at(pos);
	}

	constexpr const HeightBlock& CellAt(const size_t& x, const size_t& y) const
	{
		const auto pos = x * boardSizeH + y;

		return heightMap.at(pos);
	}

	constexpr int& SetTerrainAt(const size_t& x, const size_t& z, const int& value)
	{
		auto& cell = GetTerrainAt(x, z);
		cell = value;

		return cell;
	}

	constexpr int& GetTerrainAt(const size_t& x, const size_t& z)
	{
		return terrainData[z][x];
	}

	constexpr const int& GetTerrainAt(const size_t& x, const size_t& z) const
	{
		return terrainData[z][x];
	}

	static inline constexpr size_t boardSizeW = 40;
	static inline constexpr size_t boardSizeH = 40;
	static inline constexpr size_t tileCountW = 20;
	static inline constexpr size_t tileCountH = 20;

	float boardScaleW;
	float boardScaleD;
	float boardScaleH;

	float minPositionX;
	float minPositionZ;
	float maxPositionX;
	float maxPositionZ;

	std::vector<HeightBlock> heightMap;
	std::vector<TileCell> tileMap;
	int** terrainData;
	std::string stageFilepath;
};
