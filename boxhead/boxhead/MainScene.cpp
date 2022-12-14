#include "pch.hpp"
#include "MainScene.hpp"
#include "Framework.hpp"

void MainScene::Awake()
{
	textureRenderer.Awake();
	textureRenderer.LoadVertexShader("..\\Shaders\\TextureV.glsl");
	textureRenderer.LoadFragmentShader("..\\Shaders\\TextureP.glsl");
	textureRenderer.Ready();

	ogl::background_color = { 0.1f, 0.1f, 0.1f, 1.0f };
}

void MainScene::Render()
{
	// 변환
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 10);
	glMatrixMode(GL_PROJECTION);

	// 배경
	ogl::DrawSetColor(ogl::background_color);
	glRectf(-1, -1, 1, 1);

	// 알파
	ogl::DrawSetColor(1.0f, 1.0f, 1.0f, 1.0f);
	ogl::TurnOnOption(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	// 텍스쳐
	ogl::TurnOnOption(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 7);
	//glActiveTexture(GL_TEXTURE0);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	constexpr float img_ratio = 64.0f / 256.0f;
	constexpr float himgw = 0.1f;
	constexpr float himgh = himgw / (img_ratio);

	// 기하 변환
	glTranslatef(titleCoords.x, titleCoords.y, 0);
	glRotatef(90, 0, 0, 2);

	// 렌더링 시작

	// 타이틀
	ogl::PrimitivesBegin(ogl::PRIMITIVE_TYPES::QUADS);
	glVertex2f(-himgw, +himgh);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(-himgw, -himgh);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(+himgw, -himgh);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(+himgw, +himgh);
	glTexCoord2f(0.0f, 0.0f);
	ogl::PrimitivesEnd();
	glPopMatrix();

	// 메뉴 항목
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(titleCoords.x, titleCoords.y - himgh - 0.06f, 0);
	glRotatef(90, 0, 0, 1);

	constexpr float hmenuw = 200.0f / 960.0f;
	constexpr float hmenuh = hmenuw * 0.4f;

	// 메뉴 텍스쳐
	constexpr GLint item_textures[] = { 8, 9, 10 };
	constexpr ogl::Colour normal_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr ogl::Colour selected_color = { 0.1f, 1.0f, 0.4f, 1.0f };

	const float client_height = ogl::gl_height;

	// 메뉴 항목 렌더링
	int index = 0;
	for (auto& texid : item_textures)
	{
		glBindTexture(GL_TEXTURE_2D, texid);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		if (menuSelected == index)
		{
			ogl::DrawSetColor(selected_color);
		}
		else
		{
			ogl::DrawSetColor(normal_color);
		}

		ogl::PrimitivesBegin(ogl::PRIMITIVE_TYPES::QUADS);
		glVertex2f(-hmenuh, +hmenuw);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(-hmenuh, -hmenuw);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(+hmenuh, -hmenuw);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(+hmenuh, +hmenuw);
		glTexCoord2f(0.0f, 0.0f);
		ogl::PrimitivesEnd();

		glTranslatef(-0.2f, 0, 0);
		index++;
	}

	glPopMatrix();

	//
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 10);
	glMatrixMode(GL_PROJECTION);

	//
	ogl::TurnOffOption(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTranslatef(0, 0, 1);

	float fade_alpha = 0.0f;
	switch (myStatus)
	{
		case State::INTRO:
		{
			fade_alpha = std::pow(introFadeTime / introFadePeriod, 3.0f);
		}
		break;

		case State::MENU:
		{
			fade_alpha = 0.0f;
		}
		break;

		case State::OUTRO:
		{
			fade_alpha = 1.0f - std::pow(outroFadeTime / introFadePeriod, 2.0f);
		}
		break;

		case State::EXIT:
		case State::NEXT_ROOM:
		{
			fade_alpha = 1.0f;
		}
		break;
	}

	ogl::DrawSetColor(0.0f, 0.0f, 0.0f, fade_alpha);
	glRectf(-1, -1, 1, 1);

	glPopMatrix();
}
