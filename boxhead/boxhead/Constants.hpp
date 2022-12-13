#pragma once

namespace constants
{
	constexpr int CLIENT_W{ 800 }, CLIENT_H{ 600 };
	constexpr float CLIENT_RATIO = static_cast<float>(CLIENT_H) / static_cast<float>(CLIENT_W);

	constexpr float GRAIVTY = 9.8f;
	constexpr float GROUND_Y = 0.0f;
};
