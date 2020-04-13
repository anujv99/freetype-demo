
#ifndef __FT_RENDERER_H__
#define __FT_RENDERER_H__

#include <graphics/shader.h>
#include <graphics/texture.h>
#include <graphics/renderstate.h>
#include <graphics/framebuffer.h>

#include <math/math.h>

class FTRenderer {
public:
	FTRenderer();
public:
	void StartDrawing(engine::utils::StrongHandle<engine::graphics::Framebuffer> fbo);
	void EndDrawing(engine::utils::StrongHandle<engine::graphics::Framebuffer> fbo, bool isDistanceField = false);
	void DrawQuad(engine::Vec2 pos, engine::Vec2 size, engine::utils::StrongHandle<engine::graphics::Texture> tex);
	void DrawQuad(engine::Vec2 pos, engine::Vec2 size, engine::Vec4 color);
	void DrawLine(engine::Vec2 start, engine::Vec2 end, engine::Vec4 color);

	void SetGamma(float gamma);

	inline engine::utils::StrongHandle<engine::graphics::ShaderProgram> GetDefaultShader() { return m_Shader; }
	inline engine::utils::StrongHandle<engine::graphics::ShaderProgram> GetDFShader() { return m_DFShader; }
private:
	engine::graphics::Viewport m_DefaultVP;

	engine::utils::StrongHandle<engine::graphics::ShaderProgram> m_Shader;
	engine::utils::StrongHandle<engine::graphics::ShaderProgram> m_DFShader;

	engine::Vec2 m_Offset = engine::Vec2(0.0f);
};

#endif //__FT_RENDERER_H__
