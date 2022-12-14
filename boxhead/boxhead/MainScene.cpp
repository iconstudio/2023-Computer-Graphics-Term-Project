#include "pch.hpp"
#include "MainScene.hpp"
#include "Framework.hpp"

void MainScene::Render()
{
	const GLint& texid = 7;

	// 변환
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 10);
	glMatrixMode(GL_PROJECTION);

	// 알파
	ogl::DrawSetColor(1.0f, 1.0f, 1.0f, 1.0f);
	ogl::TurnOnOption(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 텍스쳐
	ogl::TurnOnOption(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	constexpr float img_ratio = 64.0f / 256.0f;
	constexpr float himgw = 0.1f;
	constexpr float himgh = himgw / (img_ratio);

	// 기하 변환
	glTranslatef(titleCoords.x, titleCoords.y, titleCoords.z);
	glRotatef(90, 0, 0, 1);

	// 렌더링
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
	ogl::ResetProgram();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	fadeRenderer.PrepareRendering();
	auto uniform_ratio = fadeRenderer.GetUniform("u_Ratio");
	auto uniform_percentage = fadeRenderer.GetUniform("u_Percentage");

	constexpr float asepct_ratio = 9.0f / 16.0f;
	uniform_ratio.Assign(asepct_ratio);

	// x, y, r, g, b, a
	constexpr GLsizei shade_tex = sizeof(float) * 6;

	fadeBuffer.PrepareRendering();
	auto attr_coord = fadeRenderer.BeginAttribute("a_Position", shade_tex);
	auto attr_color = fadeRenderer.BeginAttribute("a_Colour", shade_tex);

	switch (myStatus)
	{
		case State::INTRO:
		{
			uniform_percentage.Assign(introFadeTime / introFadePeriod);

			//ogl::DrawSetColor(0.0f, 0.0f, 0.0f);
			//glRectf(-1, -1, 1, 1);
		}
		break;

		case State::MENU:
		{
			uniform_percentage.Assign(1.0f);
		}
		break;

		case State::OUTRO:
		{
			uniform_percentage.Assign(outroFadeTime / introFadePeriod);
		}
		break;

		default:
		{
			uniform_percentage.Assign(0.0f);

		}
		break;
	}
	
	fadeRenderer.ReadBuffer(attr_coord, 3);
	fadeRenderer.ReadBuffer(attr_color, 4);
	ogl::Render(ogl::PRIMITIVE_TYPES::TRIANGLE_FAN, 4);
	fadeRenderer.ResetSeekBuffer();

	ogl::ResetProgram();

}
