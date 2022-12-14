#include "pch.hpp"
#include "MainScene.hpp"
#include "Framework.hpp"

void MainScene::Render()
{
	ogl::ResetProgram();

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
	constexpr float himgw = 0.1;
	constexpr float himgh = himgw / (img_ratio);

	constexpr float dx = 0.0f;
	constexpr float dy = 0.2f;

	// 기하 변환
	glTranslatef(dx, dy, 0.0f);
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
}
