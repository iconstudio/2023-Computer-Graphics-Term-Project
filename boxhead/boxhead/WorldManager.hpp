#pragma once

class WorldManagers
{
public:
	void Awake(Scene* scene)
	{}
};

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
	float x, y;
	GLint textureID;
};
