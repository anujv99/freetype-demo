#include "opengltexture.h"

#ifdef __OPENGL__

namespace engine {

	namespace graphics {
		utils::StrongHandle<Texture> Texture::Create(Vec2i size, const unsigned char * buffer, TextureParams params) {
			return dynamic_cast<Texture *>(new engine::external::opengl::OpenGLTexture(size, buffer, params));
		}

		utils::StrongHandle<Texture> Texture::Create(Vec2i size, TextureParams params) {
			return dynamic_cast<Texture *>(new engine::external::opengl::OpenGLTexture(size, params));
		}
	}

	namespace external { namespace opengl {

		OpenGLTexture::OpenGLTexture(Vec2i size, graphics::TextureParams params) :
			m_ID(0u), m_Width(0u), m_Height(0u), m_BytesPerPixel(0u), m_LastSlot(-1),
			m_Format(GL_INVALID_ENUM), m_InternalFormat(GL_INVALID_ENUM), m_ExternalDataType(GL_INVALID_ENUM) {

			ASSERTM(size.x > 0 && size.y > 0, "[OpenGL] Invalid texture size");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

			ASSERTM(m_ID != 0u, "[OpenGL] Failed to create texture");

			glBindTexture(GL_TEXTURE_2D, m_ID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OpenGLHelper::GetOpenGLType(params.Wrapping));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OpenGLHelper::GetOpenGLType(params.Wrapping));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OpenGLHelper::GetOpenGLType(params.Filtering));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OpenGLHelper::GetOpenGLType(params.Filtering));

			m_InternalFormat = OpenGLHelper::GetOpenGLType(params.InternalFormat);
			m_Format = OpenGLHelper::GetOpenGLType(params.ExternalFormat);
			m_BytesPerPixel = OpenGLHelper::GetNumBytes(params.ExternalFormat);
			m_ExternalDataType = OpenGLHelper::GetOpenGLType(params.ExternalDataType);

			m_Width = (GLuint)size.x;
			m_Height = (GLuint)size.y;

			glTextureStorage2D(m_ID, 1, m_InternalFormat, (GLsizei)size.x, (GLsizei)size.y);
			glBindTexture(GL_TEXTURE_2D, 0u);
		}

		OpenGLTexture::OpenGLTexture(Vec2i size, const unsigned char * buffer, graphics::TextureParams params) :
			m_ID(0u), m_Width(0u), m_Height(0u), m_BytesPerPixel(0u), m_LastSlot(-1),
			m_Format(GL_INVALID_ENUM), m_InternalFormat(GL_INVALID_ENUM), m_ExternalDataType(GL_INVALID_ENUM) {

			ASSERTM(size.x > 0 && size.y > 0, "[OpenGL] Invalid texture size");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

			ASSERTM(m_ID != 0u, "[OpenGL] Failed to create texture");

			glBindTexture(GL_TEXTURE_2D, m_ID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OpenGLHelper::GetOpenGLType(params.Wrapping));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OpenGLHelper::GetOpenGLType(params.Wrapping));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OpenGLHelper::GetOpenGLType(params.Filtering));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OpenGLHelper::GetOpenGLType(params.Filtering));

			m_InternalFormat = OpenGLHelper::GetOpenGLType(params.InternalFormat);
			m_Format = OpenGLHelper::GetOpenGLType(params.ExternalFormat);
			m_BytesPerPixel = OpenGLHelper::GetNumBytes(params.ExternalFormat);
			m_ExternalDataType = OpenGLHelper::GetOpenGLType(params.ExternalDataType);

			m_Width = (GLuint)size.x;
			m_Height = (GLuint)size.y;

			if (m_BytesPerPixel != 4) {
				glPixelStorei(GL_UNPACK_ALIGNMENT, GL_TRUE); // Disable byte-alignment restriction
			} else {
				glPixelStorei(GL_UNPACK_ALIGNMENT, GL_FALSE);
			}
			
			glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_Format, m_ExternalDataType, buffer);
			glBindTexture(GL_TEXTURE_2D, 0u);
		}

		OpenGLTexture::~OpenGLTexture() {
			glDeleteTextures(1, &m_ID);
		}

		void OpenGLTexture::Bind(unsigned int slot) const {
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, m_ID);
			m_LastSlot = slot;
		}

		void OpenGLTexture::UnBind() const {
			ASSERTM(m_LastSlot >= 0, "[OpenGL] Texture not already bound to a valid slot. Unbind a texture without binding will cause undefined behaviour");
			glActiveTexture(GL_TEXTURE0 + m_LastSlot);
			glBindTexture(GL_TEXTURE_2D, 0u);
			m_LastSlot = -1;
		}

		void OpenGLTexture::SetData(const unsigned char * pixels, size_t size) {
			ASSERTM(pixels, "[OpenGL] Invalid data to set texture");
			ASSERTM(size > 0u, "[OpenGL] Invalid size to set texture");
			if (size != (size_t)m_BytesPerPixel * (size_t)m_Width * (size_t)m_Height) {
				LOG_WARN("[OpenGL] Texture2D::SetData expects size to match the size of complete texture."
					"The size given is not equal to the size of the texture and may cause undefined behaviour");
			}
			glBindTexture(GL_TEXTURE_2D, m_ID);

			if (m_BytesPerPixel != 4) {
				glPixelStorei(GL_UNPACK_ALIGNMENT, GL_TRUE); // Disable byte-alignment restriction
			} else {
				glPixelStorei(GL_UNPACK_ALIGNMENT, GL_FALSE);
			}

			glTexSubImage2D(GL_TEXTURE_2D, 0u, 0u, 0u, m_Width, m_Height, m_Format, m_ExternalDataType, pixels);
		}

	} }

}

#endif //__OPENGL__

