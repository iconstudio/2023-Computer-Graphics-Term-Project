#pragma once
#include "stdafx.hpp"

#ifndef __PCH__
#define __PCH__

#include <OpenGL.hpp>
#include <Pipeline.hpp>
#include <Uniform.hpp>

#include "WindowManager.hpp"
#include "Timer.hpp"
#include "Constants.hpp"
#include "Transform.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "Collider.hpp"
#include "GameObject.hpp"
#include "Entity.hpp"
#include "Model.hpp"
#include "ModelView.hpp"
#include "GameViewManger.hpp"

namespace map
{
	class WorldManager;
}

float RandomizeColour();

inline constexpr bool CheckKey(const short state)
{
	return !(0 == state || 1 == state);
}

inline constexpr bool CheckPressedKey(const short state)
{
	return (state & 0x8000) != 0;
}

inline constexpr bool CheckPressingKey(const short state)
{
	return (state & 0x80) != 0;
}

inline short ObtainKeyState(const int key)
{
	return GetAsyncKeyState(key);
}

inline constexpr void UpdateKeyState(const short state, bool& pressing, bool& pressed)
{
	if (CheckKey(state))
	{
		if (!pressing)
		{
			pressed = true;
		}
		else
		{
			pressed = false;
		}

		pressing = true;
	}
	else
	{
		pressing = false;
		pressed = false;
	}
}

#endif
