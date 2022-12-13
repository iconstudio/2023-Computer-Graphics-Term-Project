#pragma once

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
		, terrainData(), tileData()
	{
		heightMap.reserve(boardSizeW * boardSizeH + 1);
	}

	~WorldManager()
	{}

	void Awake();

	void Start(Scene* scene);

	void Render(ModelView model, ogl::Uniform& world_uniform, ogl::Uniform& texture_uniform);

private:
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

	constexpr int& SetTerrainAt(const size_t& x, const size_t& y, const int& value)
	{
		auto& cell = GetTerrainAt(x, y);
		cell = value;

		return cell;
	}

	constexpr int& GetTerrainAt(const size_t& x, const size_t& y)
	{
		return terrainData[y][x];
	}

	constexpr const int& GetTerrainAt(const size_t& x, const size_t& y) const
	{
		return terrainData[y][x];
	}

	static inline constexpr size_t boardSizeW = 40;
	static inline constexpr size_t boardSizeH = 40;
	static inline constexpr size_t tileCountW = 20;
	static inline constexpr size_t tileCountH = 20;

	float boardScaleW;
	float boardScaleD;
	float boardScaleH;

	std::vector<HeightBlock> heightMap;
	std::vector<TileCell> tileMap;
	int** terrainData;
	std::string stageFilepath;

	TileCell tileData[tileCountH][tileCountW];
};
