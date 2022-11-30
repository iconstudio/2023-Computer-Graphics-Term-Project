#pragma once
#include "Shader.hpp"
#include "Utils.hpp"
#include "Attribute.hpp"
#include "Uniform.hpp"

namespace ogl
{
	class Pipeline
	{
	public:
		constexpr Pipeline() : Pipeline(NULL)
		{}

		constexpr Pipeline(const GLuint& program)
			: myID(program)
			, shVertex(nullptr), shFragment(nullptr)
		{}

		~Pipeline()
		{
			if (NULL != myID)
			{
				glDeleteProgram(myID);
				myID = NULL;
			}
		}

		Pipeline(const Pipeline& other) = delete;
		Pipeline& operator=(const Pipeline& other) = delete;

		inline void Awake()
		{
			myID = glCreateProgram();
		}

		inline void Start()
		{
			GLint Success = 0;

			// Attach �Ϸ�� shaderProgram�� ��ŷ��
			glLinkProgram(myID);

			// ��ũ�� �����ߴ��� Ȯ��
			glGetProgramiv(myID, GL_LINK_STATUS, &Success);

			const auto& vpath = shVertex->shaderFile;
			const auto& fpath = shFragment->shaderFile;

			if (!Success)
			{
				std::stringstream error{};
				error << " ���̴� " << std::hex << this << "���� ���̴� ���α׷��� ��ũ ����!\n[���̴� ����]\n���� ���̴�: " << vpath << "���� ���̴�: " << fpath << '\n';

				throw std::runtime_error(error.str());
				return;
			}

			glValidateProgram(myID);
			glGetProgramiv(myID, GL_VALIDATE_STATUS, &Success);
			if (!Success)
			{
				std::stringstream error{};
				error << " ���̴� " << std::hex << this << "���� ���̴� ���α׷��� ���� ����!\n[���̴� ����]\n���� ���̴�: " << vpath << "���� ���̴�: " << fpath << '\n';

				throw std::runtime_error(error.str());
				return;
			}

			glUseProgram(myID);

			auto vcaption = CutExtensions(vpath.filename().string());
			auto fcaption = CutExtensions(fpath.filename().string());

			auto common_mismatch = std::mismatch(vcaption.cbegin(), vcaption.cend(), fcaption.cbegin(), fcaption.cend());
			auto common_caption = std::string{ vcaption.cbegin(), common_mismatch.first};

			std::cout << common_caption << " �� ������ �Ϸ�.\n";
		}

		inline void PrepareRendering()
		{
			glUseProgram(myID);

			writePtr = 0;
		}

		inline void ResetSeekBuffer()
		{
			writePtr = 0;
		}

		inline void SeekBuffer(const size_t pos)
		{
			writePtr = reinterpret_cast<GLvoid*>(pos);
		}

		inline void ForwardBuffer(const size_t size)
		{
			writePtr = reinterpret_cast<GLvoid*>(reinterpret_cast<size_t>(writePtr) + size);
		}

		inline void ReadBuffer(Attribute& attr, const GLint length)
		{
			attr.Assign(attr.writeType, length, attr.writeStride, writePtr);

			if (GL_FLOAT == attr.writeType)
			{
				ForwardBuffer(sizeof(float) * length);
			}
			else
			{
				ForwardBuffer(sizeof(int) * length);
			}
		}

		inline Uniform GetUniform(std::string_view name) const
		{
			return Uniform(glGetUniformLocation(myID, name.data()));
		}

		inline Attribute GetAttribute(std::string_view name, const GLsizei stride = 0, const GLenum type = GL_FLOAT) const
		{
			return Attribute(glGetAttribLocation(myID, name.data()), stride, type);
		}

		inline Attribute BeginAttribute(std::string_view name, const GLsizei stride = 0, const GLenum type = GL_FLOAT) const
		{
			auto result = Attribute(glGetAttribLocation(myID, name.data()), stride, type);
			result.EnableVertexArray();

			return result;
		}

		inline std::shared_ptr<Shader> LoadShader(const GLenum type, const Path& shader_file)
		{
			auto fn_shader = shader_file.filename();
			auto sh = std::make_shared<Shader>(type, shader_file);

			std::cout << "���̴� �ҷ����� ��... ";
			sh->Load();
			std::cout << "�Ϸ�.\n";

			std::cout << "���̴� ������ ��... ";
			if (!sh->Compile())
			{
				std::cout << "�������ϴ� �߿� ���� �߻�!\n";
				return sh;
			}
			std::cout << "�Ϸ�.\n";

			return sh;
		}

		inline std::shared_ptr<Shader> LoadVertexShader(const Path& shader_file)
		{
			std::cout << "���� ���̴�: " << shader_file.filename() << '\n';
			shVertex = LoadShader(GL_VERTEX_SHADER, shader_file);

			AttachShader(*shVertex);

			return shVertex;
		}

		inline std::shared_ptr<Shader> LoadFragmentShader(const Path& shader_file)
		{
			std::cout << "���� ���̴�: " << shader_file.filename() << '\n';
			shFragment = LoadShader(GL_FRAGMENT_SHADER, shader_file);

			AttachShader(*shFragment);

			return shFragment;
		}

		inline std::pair<std::shared_ptr<Shader>, std::shared_ptr<Shader>>
			LoadShaders(const Path& vs, const Path& fs)
		{
			return { LoadVertexShader(vs), LoadFragmentShader(fs) };
		}

		inline void AttachShader(const GLuint shader)
		{
			glAttachShader(myID, shader);
		}

		inline void AttachShader(Shader& shader)
		{
			shader.Attach(myID);
		}

	private:
		GLuint myID; // ���α׷�
		std::shared_ptr<Shader> shVertex, shFragment;

		GLvoid* writePtr = 0;
	};
}