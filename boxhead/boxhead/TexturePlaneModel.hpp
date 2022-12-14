#pragma once

class TexturePlaneModel : public Model
{
public:
	constexpr TexturePlaneModel(std::string_view name, ogl::VertexStream::Buffer& buffer)
		: Model(name, buffer)
		, textureID(0)
	{}

	void PrepareRendering(ogl::Uniform& tex_uniform) const
	{
		Model::PrepareRendering();

		//tex_uniform.BindTexture(textureID);
		//tex_uniform.Assign(textureID);
	}

	virtual void Render() const override
	{
		ogl::Render(ogl::PRIMITIVE_TYPES::TRIANGLE_FAN, 4);
	}

	static constexpr size_t GetID()
	{
		return ReferenceIndex;
	}

	GLint textureID;

	constinit inline static size_t ReferenceIndex = 3;
};

