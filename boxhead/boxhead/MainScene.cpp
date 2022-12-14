#include "pch.hpp"
#include "MainScene.hpp"
#include "Framework.hpp"

void MainScene::Render()
{
	ogl::ResetProgram();

	const GLint& texid = 6;

	glPushMatrix();
	glOrtho(-1, 1, -1, 1, -1, 10);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	ogl::DrawSetColor(1.0f, 1.0f, 1.0f, 1.0f);

	ogl::TurnOnOption(GL_BLEND);
	ogl::TurnOnOption(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0 + texid - 1);
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	glRotatef(90, 0, 0, 1);
	ogl::PrimitivesBegin(ogl::PRIMITIVE_TYPES::QUADS);

	glVertex2f(-0.2f, +0.2f);
	glTexCoord2f(0.0f, 0.0f);

	glVertex2f(-0.2f, -0.2f);
	glTexCoord2f(0.0f, 1.0f);

	glVertex2f(+0.2f, -0.2f);
	glTexCoord2f(1.0f, 1.0f);

	glVertex2f(+0.2f, +0.2f);
	glTexCoord2f(1.0f, 0.0f);

	ogl::PrimitivesEnd();

	glPopMatrix();
}
