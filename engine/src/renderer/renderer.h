
/*
 *	Simple 2D renderer
 *	Handles the state and batching of sprites
 */

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <math/math.h>
#include <math/mvpstack.h>

#include <utils/singleton.h>

#include <graphics/buffer.h>
#include <graphics/bufferlayout.h>
#include <graphics/renderstate.h>
#include <graphics/shader.h>
#include <graphics/vertexarray.h>
#include <graphics/texture.h>
#include <graphics/framebuffer.h>

namespace engine { namespace renderer {

	class Renderer : public utils::Singleton<Renderer> {
		friend class utils::Singleton<Renderer>;
	private:
		Renderer();
		~Renderer();
	public:
		void BeginScene();
		void EndScene();
		void EndScene(utils::StrongHandle<graphics::ShaderProgram> shader);

		void DrawSprite(Vec2 pos, Vec2 size, Vec4 color);
		void DrawSprite(Vec2 pos, Vec2 size, Vec4 color, Vec2 texCoordTopLeft, Vec2 texCoordBottomRight, utils::StrongHandle<graphics::Texture> texture);

		// from framebuffer cannot be the default framebuffer
		// to pass a framebuffer to the default framebuffer pass to = nullptr
		void PassFramebuffer(utils::StrongHandle<graphics::Framebuffer> from, utils::StrongHandle<graphics::Framebuffer> to);
	private:
		void CreateRendererObjects();
		void CreateFramebufferObjects();
		int SubmitTexture(utils::StrongHandle<graphics::Texture> texture);
	private:
		struct RendererVertex {
			Vec2 Position;
			Vec2 TexCoord;
			pvint TexIndex;
			unsigned char col[4];
		};
	private:
		utils::StrongHandle<graphics::VertexBuffer> m_RendererBuffer;
		utils::StrongHandle<graphics::VertexArray> m_RendererVertexArray;
		utils::StrongHandle<graphics::ShaderProgram> m_RendererShader;
		utils::StrongHandle<graphics::UniformBuffer> m_MVPBuffer;

		utils::StrongHandle<graphics::VertexArray> m_FramebufferRendererVertexArray;
		utils::StrongHandle<graphics::ShaderProgram> m_FramebufferRendererShader;

		RendererVertex * m_Vertices = nullptr;
		std::vector<utils::StrongHandle<graphics::Texture>> m_Textures;

		size_t m_VertexIndex = 0u;

		static constexpr const unsigned int MAX_VERTICES = 1024u;
		static constexpr const unsigned int MAX_TEXTURES = 8u;
	private:
		friend class ImmGFX;
		utils::StrongHandle<graphics::UniformBuffer> GetMVPBuffer() { return m_MVPBuffer; }
	};

} }

#endif //__RENDERER_H__
