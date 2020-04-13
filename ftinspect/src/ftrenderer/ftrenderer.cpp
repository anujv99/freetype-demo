#include "ftrenderer.h"

#include <imgui.h>

#include <renderer/renderer.h>
#include <renderer/immgfx.h>
#include <utils/assert.h>
#include <math/mvpstack.h>

using namespace engine;
using namespace graphics;
using namespace core;
using namespace utils;
using namespace renderer;
using namespace math;

FTRenderer::FTRenderer() {
	#include "fontshaders.inl"
	#include "distancefieldshaders.inl"

	m_Shader = ShaderProgram::Create(VertexShader::Create(GL_V_FONT_SHADER), FragmentShader::Create(GL_F_FONT_SHADER));
	m_DFShader = ShaderProgram::Create(VertexShader::Create(GL_V_DF_SHADER), FragmentShader::Create(GL_F_DF_SHADER));

	m_DefaultVP = RenderState::Ref().GetViewport();
}

void FTRenderer::StartDrawing(engine::utils::StrongHandle<engine::graphics::Framebuffer> fbo) {
	// Setup drawing area
	Vec2 fboSize = ToVec2(fbo->GetSize());

	Mat4 projection = Mat4::Ortho(-fboSize.x / 2.0f, fboSize.x / 2.0f, -fboSize.y / 2.0f, fboSize.y / 2.0f, -1.0f, 1.0f);

	Viewport vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = fboSize.x;
	vp.Height = fboSize.y;

	RenderState::Ref().SetViewport(vp);

	fbo->Bind();
	fbo->Clear(Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	MVPStack::Ref().Projection().Push(projection);
	Renderer::Ref().BeginScene();
}

void FTRenderer::EndDrawing(engine::utils::StrongHandle<engine::graphics::Framebuffer> fbo, bool isDistanceField) {
	if (isDistanceField)
		Renderer::Ref().EndScene(m_DFShader);
	else
		Renderer::Ref().EndScene(m_Shader);

	ImmGFX::Ref().Render();
	MVPStack::Ref().Projection().Pop();
	fbo->UnBind();

	RenderState::Ref().SetViewport(m_DefaultVP);
}

void FTRenderer::DrawQuad(Vec2 pos, Vec2 size, utils::StrongHandle<graphics::Texture> tex) {
	Renderer::Ref().DrawSprite(pos + m_Offset, size, Vec4(0.0f), Vec2(0.0f), Vec2(1.0f), tex);
}

void FTRenderer::DrawQuad(Vec2 pos, Vec2 size, Vec4 color) {
	Renderer::Ref().DrawSprite(pos + m_Offset, size, color);
}

void FTRenderer::DrawLine(engine::Vec2 start, engine::Vec2 end, engine::Vec4 color) {
	ImmGFX::Ref().Color(color);
	ImmGFX::Ref().DrawLine(start, end);
}

void FTRenderer::SetGamma(float gamma) {
	m_Shader->Bind();
	m_Shader->SetUniformFloat(m_Shader->GetUniformLocation("gamma"), gamma);
}
