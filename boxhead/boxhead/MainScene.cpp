#include "pch.hpp"
#include "MainScene.hpp"
#include "Framework.hpp"

void MainScene::Awake()
{
	textureRenderer.Awake();
	textureRenderer.LoadVertexShader("shaders\\TextureV.glsl");
	textureRenderer.LoadFragmentShader("shaders\\TextureP.glsl");
	textureRenderer.Ready();

	ogl::background_color = { 0.1f, 0.1f, 0.1f, 1.0f };
}

void MainScene::Render()
{
	// ��ȯ
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 10);
	glMatrixMode(GL_PROJECTION);

	// ���
	ogl::DrawSetColor(ogl::background_color);
	glRectf(-1, -1, 1, 1);

	// ����
	ogl::DrawSetColor(1.0f, 1.0f, 1.0f, 1.0f);
	ogl::TurnOnOption(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	// �ؽ���
	ogl::TurnOnOption(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 7);
	//glActiveTexture(GL_TEXTURE0);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	constexpr float img_ratio = 64.0f / 256.0f;
	constexpr float himgw = 0.1f;
	constexpr float himgh = himgw / (img_ratio);

	// ���� ��ȯ
	glTranslatef(titleCoords.x, titleCoords.y, 0);
	glRotatef(90, 0, 0, 2);

	// ������ ����

	// Ÿ��Ʋ
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

	float menu_item_alpha = 1.0f;

	if (State::OUTRO == myStatus)
	{
		if (doingAppQuit)
		{
			const float menu_item_fade = std::pow(outroFadeTime / outroFadePeriod, 2.0f);

			menu_item_alpha = catmull_rom_spline(menu_item_fade, 1.0f, 0.9f, 0.8f, 0.0f);
		}
		else
		{
			const float menu_item_fade = (1.0f - outroTitleTime / outroTitlePeriod);

			titleCoords.y = 0.4f * std::pow(1.0f - menu_item_fade, 3.0f);

			menu_item_alpha = catmull_rom_spline(menu_item_fade, 1.0f, 0.6f, 0.55f, 0.0f);
		}
	}
	else if (State::NEXT_ROOM == myStatus || State::EXIT == myStatus)
	{
		menu_item_alpha = 0.0f;
	}

	if (State::HELP != myStatus) // �޴� �׸�
	{
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(titleCoords.x, titleCoords.y - himgh - 0.06f, 0);
		glRotatef(90, 0, 0, 1);

		constexpr float hmenuw = 200.0f / 960.0f;
		constexpr float hmenuh = hmenuw * 0.4f;

		// �޴� �ؽ���
		constexpr GLint item_textures[] = { 8, 9, 10 };
		const ogl::Colour normal_color = { 1.0f, 1.0f, 1.0f, menu_item_alpha };
		const ogl::Colour selected_color = { 0.1f, 1.0f, 0.4f, menu_item_alpha };

		const float client_height = ogl::gl_height;

		// �޴� �׸� ������
		int index = 0;
		for (auto& texid : item_textures)
		{
			glBindTexture(GL_TEXTURE_2D, texid);
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	}
	else // ����
	{
		//const float ratio = static_cast<float>(ogl::gl_width) / static_cast<float>(ogl::gl_height);

		glPushMatrix();
		glLoadIdentity();
		glTranslatef(titleCoords.x, titleCoords.y - himgh - 0.3f, 0);
		glRotatef(90, 0, 0, 1);
		
		glBindTexture(GL_TEXTURE_2D, 11);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

		ogl::DrawSetColor(1.0f, 1.0f, 1.0f, 1.0f);

		ogl::PrimitivesBegin(ogl::PRIMITIVE_TYPES::QUADS);
		glVertex2f(-0.5f, +0.5f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(-0.5f, -0.5f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(+0.5f, -0.5f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(+0.5f, +0.5f);
		glTexCoord2f(0.0f, 0.0f);
		ogl::PrimitivesEnd();
		glPopMatrix();
	}

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
