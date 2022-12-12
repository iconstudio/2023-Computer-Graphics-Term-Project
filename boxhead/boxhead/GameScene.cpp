#include "pch.hpp"
#include "GameScene.hpp"
#include "Framework.hpp"

void GameScene::Render()
{
	const auto& matrix_cam = viewManager.GetCameraMatrix();
	const auto& matrix_view = viewManager.GetPerspectiveViewMatrix();

	textureRenderer.PrepareRendering();
	auto tex_uniform_world = textureRenderer.GetUniform("a_WorldMatrix");
	auto tex_uniform_camera = textureRenderer.GetUniform("a_CameraMatrix");
	auto tex_uniform_proj = textureRenderer.GetUniform("a_ProjMatrix");
	auto tex_uniform_sample = textureRenderer.GetUniform("u_Texture");

	auto& floor_texture = Framework::GetTextureBuffer(0);
	auto floor_texture_id = Framework::GetTextureID(0);

	tex_uniform_world.AssignMatrix4x4(ogl::identity);
	tex_uniform_camera.AssignMatrix4x4(matrix_cam);
	tex_uniform_proj.AssignMatrix4x4(matrix_view);
	tex_uniform_sample.ActiveTexture(0);
	tex_uniform_sample.BindTexture(floor_texture_id);

	// x, y, z, s, t
	constexpr GLsizei shade_tex = sizeof(float) * 5;

	auto attr_texpos = textureRenderer.BeginAttribute("a_Position", shade_tex);
	auto attr_texcoord = textureRenderer.BeginAttribute("a_TexCoord", shade_tex);

	// 3: �ؽ��� �ٴ�
	auto model_texfloor = ModelView::GetReference(3);
	model_texfloor.PrepareRendering();
	textureRenderer.ReadBuffer(attr_texpos, 3);
	textureRenderer.ReadBuffer(attr_texcoord, 2);

	model_texfloor.Render();
	textureRenderer.ResetSeekBuffer();

	myRenderer.PrepareRendering();
	auto uniform_mat_world = myRenderer.GetUniform("a_WorldMatrix");
	auto uniform_mat_camera = myRenderer.GetUniform("a_CameraMatrix");
	auto uniform_mat_proj = myRenderer.GetUniform("a_ProjMatrix");

	uniform_mat_world.AssignMatrix4x4(ogl::identity);
	uniform_mat_camera.AssignMatrix4x4(matrix_cam);
	uniform_mat_proj.AssignMatrix4x4(matrix_view);

	// x, y, z, r, g, b, a
	constexpr GLsizei shade_stride = sizeof(float) * 7;

	// x, y, z, r, g, b, a, nx, ny, nz
	constexpr GLsizei normal_stride = sizeof(float) * 10;

	auto attr_pos = myRenderer.BeginAttribute("a_Position", shade_stride);
	auto attr_col = myRenderer.BeginAttribute("a_Colour", shade_stride);

	// 1: ��ǥ��
	auto model_axis = ModelView::GetReference(1);
	model_axis.PrepareRendering();
	myRenderer.ReadBuffer(attr_pos, 3);
	myRenderer.ReadBuffer(attr_col, 4);

	model_axis.Render();
	myRenderer.ResetSeekBuffer();

	// 2: �ٴ�
	auto model_floor = ModelView::GetReference(2);
	model_floor.PrepareRendering();
	myRenderer.ReadBuffer(attr_pos, 3);
	myRenderer.ReadBuffer(attr_col, 4);

	model_floor.Render();
	myRenderer.ResetSeekBuffer();

	for (auto& instance : myInstances)
	{
		//  0: ť��
		instance->PrepareRendering();

		myRenderer.ReadBuffer(attr_pos, 3);
		myRenderer.ReadBuffer(attr_col, 4);

		instance->Render(uniform_mat_world);
		myRenderer.ResetSeekBuffer();
	}

	attr_pos.DisableVertexArray();
	attr_col.DisableVertexArray();
}

