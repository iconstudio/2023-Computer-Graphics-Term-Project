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

	// 3: �ؽ��� �ٴ�
	auto model_texfloor = ModelView::GetReference(3);
	model_texfloor.PrepareRendering();
	textureRenderer.ReadBuffer(attr_texpos, 3);
	textureRenderer.ReadBuffer(attr_texcoord, 2);

	worldManager.Render(model_texfloor, tex_uniform_world, tex_uniform_sample);
	textureRenderer.ResetSeekBuffer();

	// 5: �ؽ��� ť��
	auto model_texcube = ModelView::GetReference(5);
	model_texcube.PrepareRendering();
	textureRenderer.ReadBuffer(attr_texpos, 3);
	textureRenderer.ReadBuffer(attr_texcoord, 2);

	tex_uniform_world.AssignMatrix4x4(glm::translate(ogl::identity, { 7.0, 2.0, 7.0f }));

	const GLint& texid = 3;
	glActiveTexture(GL_TEXTURE0);
	tex_uniform_sample.BindTexture(texid);
	model_texcube.Render();
	textureRenderer.ResetSeekBuffer();

	for (auto& objet : everyWall)
	{
		objet->PrepareRendering();
		textureRenderer.ReadBuffer(attr_texpos, 3);
		textureRenderer.ReadBuffer(attr_texcoord, 2);

		glActiveTexture(GL_TEXTURE0);
		tex_uniform_sample.BindTexture(texid);
		objet->Render(tex_uniform_world);
		textureRenderer.ResetSeekBuffer();
	}

	attr_texpos.DisableVertexArray();
	attr_texcoord.DisableVertexArray();
	ogl::ResetProgram();
	glPopMatrix();

	ogl::TurnOnOption(GL_TEXTURE_2D);
	glPushMatrix();

	overTextureRenderer.PrepareRendering();
	auto ov_uniform_world = overTextureRenderer.GetUniform("a_WorldMatrix");
	auto ov_uniform_camera = overTextureRenderer.GetUniform("a_CameraMatrix");
	auto ov_uniform_proj = overTextureRenderer.GetUniform("a_ProjMatrix");
	auto ov_uniform_sample = overTextureRenderer.GetUniform("u_Texture");

	auto attr_ovpos = overTextureRenderer.BeginAttribute("a_Position", shade_tex);
	auto attr_ovcoord = overTextureRenderer.BeginAttribute("a_TexCoord", shade_tex);

	// 12: ȭ�� �ϵ� ȿ��
	auto model_vignette = ModelView::GetReference(4);
	model_vignette.PrepareRendering();
	overTextureRenderer.ReadBuffer(attr_ovpos, 3);
	overTextureRenderer.ReadBuffer(attr_ovcoord, 2);

	ov_uniform_world.AssignMatrix4x4(ogl::identity);
	ov_uniform_camera.AssignMatrix4x4(ogl::identity);
	ov_uniform_proj.AssignMatrix4x4(otho_view);

	ov_uniform_sample.BindTexture(13);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	model_vignette.Render();
	overTextureRenderer.ResetSeekBuffer();

	attr_ovpos.DisableVertexArray();
	attr_ovcoord.DisableVertexArray();
	glPopMatrix();
	ogl::ResetProgram();
}
