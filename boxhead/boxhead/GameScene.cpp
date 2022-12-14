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

	auto& floor_texture = Framework::GetTextureBuffer(0);

	tex_uniform_camera.AssignMatrix4x4(matrix_cam);
	tex_uniform_proj.AssignMatrix4x4(matrix_view);

	// x, y, z, s, t
	constexpr GLsizei shade_tex = sizeof(float) * 5;

	auto attr_texpos = textureRenderer.BeginAttribute("a_Position", shade_tex);
	auto attr_texcoord = textureRenderer.BeginAttribute("a_TexCoord", shade_tex);

	// 3: 텍스쳐 바닥
	auto model_texfloor = ModelView::GetReference(3);
	model_texfloor.PrepareRendering();
	
	worldManager.Render(model_texfloor, textureRenderer, attr_texpos, attr_texcoord, tex_uniform_world, tex_uniform_sample);

	// 12: 화면 암등 효과
	auto model_vignette = ModelView::GetReference(4);
	model_vignette.PrepareRendering();
	textureRenderer.ReadBuffer(attr_texpos, 3);
	textureRenderer.ReadBuffer(attr_texcoord, 2);

	tex_uniform_world.AssignMatrix4x4(ogl::identity);
	tex_uniform_camera.AssignMatrix4x4(ogl::identity);
	tex_uniform_proj.AssignMatrix4x4(otho_view);

	tex_uniform_sample.ActiveTexture(12);
	tex_uniform_sample.BindTexture(13);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	model_vignette.Render();
	ogl::Render(ogl::PRIMITIVE_TYPES::QUADS, 4);
	textureRenderer.ResetSeekBuffer();

	attr_texpos.DisableVertexArray();
	attr_texcoord.DisableVertexArray();
	ogl::ResetProgram();
	glPopMatrix();

	ogl::TurnOffOption(GL_TEXTURE_2D);
	glPushMatrix();
	myRenderer.PrepareRendering();
	auto uniform_mat_world = myRenderer.GetUniform("a_WorldMatrix");
	auto uniform_mat_camera = myRenderer.GetUniform("a_CameraMatrix");
	auto uniform_mat_proj = myRenderer.GetUniform("a_ProjMatrix");

	uniform_mat_world.AssignMatrix4x4(ogl::identity);
	uniform_mat_camera.AssignMatrix4x4(matrix_cam);
	uniform_mat_proj.AssignMatrix4x4(matrix_view);

	// x, y, z, r, g, b, a
	constexpr GLsizei shade_stride = sizeof(float) * 7;

	auto attr_pos = myRenderer.BeginAttribute("a_Position", shade_stride);
	auto attr_col = myRenderer.BeginAttribute("a_Colour", shade_stride);

	// 버그 있음
	myRenderer.ReadBuffer(attr_pos, 3);
	myRenderer.ReadBuffer(attr_col, 2);
	ogl::Render(ogl::PRIMITIVE_TYPES::POINTS, 8);

	for (auto& objet : everyWall)
	{
		objet->PrepareRendering();
		myRenderer.ReadBuffer(attr_pos, 3);
		myRenderer.ReadBuffer(attr_col, 4);

		objet->Render(uniform_mat_world);
		myRenderer.ResetSeekBuffer();
	}

	for (auto it = myInstances.begin(); it != myInstances.end(); it++)
	{
		auto& instance = *it;

		//  0: 큐브
		instance->PrepareRendering();

		myRenderer.ReadBuffer(attr_pos, 3);
		myRenderer.ReadBuffer(attr_col, 4);

		instance->Render(uniform_mat_world);
		myRenderer.ResetSeekBuffer();
	}

	attr_pos.DisableVertexArray();
	attr_col.DisableVertexArray();
	ogl::ResetProgram();
	glPopMatrix();
}
