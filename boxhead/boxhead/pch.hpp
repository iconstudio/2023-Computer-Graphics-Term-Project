#pragma once
#include "stdafx.hpp"

#ifndef __PCH__
#define __PCH__

#include <OpenGL.hpp>
#include <Pipeline.hpp>
#include <Uniform.hpp>

#include "WindowManager.hpp"
#include "Timer.hpp"
#include "Model.hpp"
#include "ModelView.hpp"
#include "Constants.hpp"
#include "Transform.hpp"
#include "GameObject.hpp"
#include "Camera.hpp"
#include "Entity.hpp"
#include "StaticObject.hpp"
#include "Scene.hpp"
#include "Collider.hpp"
#include "GameViewManger.hpp"

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

inline constexpr float catmull_rom_spline(float factor, float p1, float p2, float p3, float p4)
{
	const float __c3 = p1 - p2 * 2.5f + p3 * 2.0f - p4 * 0.5f;
	const float __c4 = (p4 - p1) * 0.5f + (p2 - p3) * 1.5f;

	return (((__c4 * factor + __c3) * factor + (p3 - p1) * 0.5f) * factor + p2);
}

#endif
