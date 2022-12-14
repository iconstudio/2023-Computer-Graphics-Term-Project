#include "pch.hpp"
#include "MainScene.hpp"
#include "Framework.hpp"

void MainScene::Render()
{
	const GLint& texid = 7;

	// ��ȯ
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 10);
	glMatrixMode(GL_PROJECTION);

	// ����
	ogl::DrawSetColor(1.0f, 1.0f, 1.0f, 1.0f);
	ogl::TurnOnOption(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	// �ؽ���
	ogl::TurnOnOption(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	constexpr float img_ratio = 64.0f / 256.0f;
	constexpr float himgw = 0.1f;
	constexpr float himgh = himgw / (img_ratio);

	// ���� ��ȯ
	glTranslatef(titleCoords.x, titleCoords.y, 0);
	glRotatef(90, 0, 0, 1);

	// ������
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
			fade_alpha = std::pow(introFadeTime / introFadePeriod, 2);
		}
		break;

		case State::MENU:
		{
			fade_alpha = 0.0f;
		}
		break;

		case State::OUTRO:
		{
			fade_alpha = 1.0f - outroFadeTime / introFadePeriod;
		}
		break;
	}

	ogl::DrawSetColor(0.0f, 0.0f, 0.0f, fade_alpha);
	glRectf(-1, -1, 1, 1);

	glPopMatrix();
}
