#include "pch.hpp"
#include "GameScene.hpp"
#include "Framework.hpp"

void GameScene::Render()
{
	ogl::TurnOnOption(GL_TEXTURE_2D);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	const auto& matrix_cam = viewManager.GetCameraMatrix();
	const auto& matrix_view = viewManager.GetPerspectiveViewMatrix();
	const auto& otho_view = viewManager.GetOrthodoxViewMatrix();

	textureRenderer.PrepareRendering();
	auto tex_uniform_world = textureRenderer.GetUniform("a_WorldMatrix");
	auto tex_uniform_camera = textureRenderer.GetUniform("a_CameraMatrix");
	auto tex_uniform_proj = textureRenderer.GetUniform("a_ProjMatrix");
	auto tex_uniform_sample = textureRenderer.GetUniform("u_Texture");

	tex_uniform_camera.AssignMatrix4x4(matrix_cam);
	tex_uniform_proj.AssignMatrix4x4(matrix_view);

	// x, y, z, s, t
	constexpr GLsizei shade_tex = sizeof(float) * 5;

	auto attr_texpos = textureRenderer.BeginAttribute("a_Position", shade_tex);
	auto attr_texcoord = textureRenderer.BeginAttribute("a_TexCoord", shade_tex);

	// 3: 텍스쳐 바닥
	auto model_texfloor = ModelView::GetReference(3);
	model_texfloor.PrepareRendering();
	textureRenderer.ReadBuffer(attr_texpos, 3);
	textureRenderer.ReadBuffer(attr_texcoord, 2);

	worldManager.Render(model_texfloor, tex_uniform_world, tex_uniform_sample);
	textureRenderer.ResetSeekBuffer();

	// 5: 텍스쳐 큐브
	auto model_texcube = ModelView::GetReference(5);
	model_texcube.PrepareRendering();
	textureRenderer.ReadBuffer(attr_texpos, 3);
	textureRenderer.ReadBuffer(attr_texcoord, 2);

	// 테스트 큐브
	const GLint& texid = 14;
	glActiveTexture(GL_TEXTURE0);
	tex_uniform_world.AssignMatrix4x4(glm::translate(ogl::identity, { 7.0, 2.0, 7.0f }));
	tex_uniform_sample.BindTexture(texid);
	model_texcube.Render();
	textureRenderer.ResetSeekBuffer();

	for (auto& objet : everyWall)
	{
		const GLint& obj_texid = 15;

		objet->PrepareRendering();
		textureRenderer.ReadBuffer(attr_texpos, 3);
		textureRenderer.ReadBuffer(attr_texcoord, 2);

		glActiveTexture(GL_TEXTURE0);
		tex_uniform_sample.BindTexture(obj_texid);
		objet->Render(tex_uniform_world);
		textureRenderer.ResetSeekBuffer();
	}

	attr_texpos.DisableVertexArray();
	attr_texcoord.DisableVertexArray();
	ogl::ResetProgram();
	glPopMatrix();

	ogl::TurnOffOption(GL_TEXTURE_2D);
}
