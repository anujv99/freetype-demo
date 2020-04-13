
/*
 *  Interface to graphics API specific texture
 *  Actual implementation is in the folder external/opengl
 *  Texture object is an array of colors which can be used in the shader
 */

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <cstdint>

#include <utils/stronghandle.h>
#include <utils/handledobject.h>

#include <math/math.h>

namespace engine { namespace graphics {

	enum class TextureFiltering {
		NEAREST,
		LINEAR,
	};

	enum class TextureWrapping {
		REPEAT,
		CLAMP
	};

	enum class InternalTextureFormat {
		R,
		RG,
		RGB,
		RGBA,
		R32F,
		RG32F,
		RGB32F,
		RGBA32F,
	};

	enum class ExternalTextureFormat {
		R,
		RG,
		RGB,
		RGBA
	};

	enum class ExternalDataType {
		FLOAT,
		UNSIGNED_BYTE,
	};

	struct TextureParams {
		TextureFiltering Filtering				= TextureFiltering::LINEAR;
		TextureWrapping Wrapping				= TextureWrapping::CLAMP;
		InternalTextureFormat InternalFormat	= InternalTextureFormat::RGBA;	//format to be used in the shader
		ExternalTextureFormat ExternalFormat	= ExternalTextureFormat::RGBA;	//format of the data used to initialize the texture (number of component per pixel)
		ExternalDataType DataType				= ExternalDataType::UNSIGNED_BYTE;	//data type of the data used to initialize the texture
	};

    class Texture : public utils::HandledObject<Texture> {
        friend class utils::StrongHandle<Texture>;
    protected:
		Texture() {}
		virtual ~Texture() {}
	public:
		virtual void Bind(unsigned int slot) const = 0;
		virtual void UnBind() const = 0;
		virtual void SetData(const unsigned char * pixels, size_t size) = 0;
		virtual Vec2i GetSize() const = 0;
		virtual uintptr_t GetNativeTexture() const = 0; // GLuint for opengl, ID3D11Texture2D for directx11
	public:
		static utils::StrongHandle<Texture> Create(Vec2i size, TextureParams params);
		static utils::StrongHandle<Texture> Create(Vec2i size, const unsigned char * buffer, TextureParams params);
    };

} }

#endif //__TEXTURE_H__
