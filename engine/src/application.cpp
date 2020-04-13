#include "application.h"

#include "core/window.h"
#include "core/graphicscontext.h"
#include "core/input.h"

#include "math/mvpstack.h"
#include "renderer/renderer.h"
#include "renderer/immgfx.h"

#include "utils/fileutils.h"

#include "graphics/renderstate.h"
#include "graphics/framebuffer.h"

#include "imgui/imguimanager.h"

namespace engine {

	Application::Application() : m_IsRunning(true) {
		core::Window::CreateInst();
		core::Window::Ref().Create(1280, 720, "ftinspect");

		core::Window::Ref().SetEventFunction(BIND_EVENT_FN(Application::OnEvent));

		core::GraphicsContext::CreateInst();
		core::GraphicsContext::Ref().CreateContext(core::Window::Ref().GetRawWindow());

		graphics::RenderState::CreateInst();

		math::MVPStack::CreateInst();

		renderer::Renderer::CreateInst();
		renderer::ImmGFX::CreateInst();

		ImGuiManager::CreateInst();

		graphics::BlendFunction func;
		func.SrcColor = graphics::BlendColorOption::SRC_ALPHA;
		func.DestColor = graphics::BlendColorOption::INV_SRC_ALPHA;
		func.ColorOperation = graphics::BlendOperation::ADD;

		func.SrcAlpha = graphics::BlendAlphaOption::ONE;
		func.DestAlpha = graphics::BlendAlphaOption::ONE;
		func.AlphaOperation = graphics::BlendOperation::ADD;
		graphics::RenderState::Ref().SetBlendFunction(func);

		auto winSize = Vec2(core:: Window::Ref().GetWidth(), core::Window::Ref().GetHeight());
		winSize /= 2.0f;

		Mat4 projection = Mat4::Ortho(-winSize.x, winSize.x, -winSize.y, winSize.y, -1.0f, 1.0f);
		math::MVPStack::Ref().Projection().Push(projection);

		graphics::Viewport v;
		v.TopLeftX = 0.0f;
		v.TopLeftY = 0.0f;
		v.Width = winSize.x * 2.0f;
		v.Height = winSize.y * 2.0f;
		graphics::RenderState::Ref().SetViewport(v);
	}

	Application::~Application() {
		math::MVPStack::Ref().Projection().Pop();

		ImGuiManager::DestroyInst();

		graphics::RenderState::DestroyInst();
		renderer::Renderer::DestroyInst();
		renderer::ImmGFX::DestroyInst();
		math::MVPStack::DestroyInst();
		core::GraphicsContext::DestroyInst();
		core::Window::DestroyInst();
	}

	void Application::Run() {
		
		while (m_IsRunning) {
			core::Timer::Update();

			core::GraphicsContext::Ref().BeginFrame();

			graphics::RenderState::Ref().SetTopology(graphics::Topology::TRIANGLE);

			Update();

			Render();

			Gui();

			core::GraphicsContext::Ref().EndFrame();
			core::Input::Update();
			core::Window::Ref().Update();
		}

	}

	void Application::Update() {
		for (auto & l : m_LayerStack) {
			l->OnUpdate(core::Timer::GetDeltaTime());
		}
	}

	void Application::Render() {
		for (auto & l : m_LayerStack) {
			l->OnRender();
		}
		renderer::ImmGFX::Ref().Render();
	}


	void Application::Gui() {
		ImGuiManager::Ref().BeginFrame();

		for (auto & l : m_LayerStack) {
			l->OnImGuiUpdate();
		}

		ImGuiManager::Ref().EndFrame();
	}

	void Application::OnEvent(core::events::Event & e) {

		core::Input::OnEvent(e);

		core::events::EventDispatcher dispatcher(e);
		 
		dispatcher.Dispatch<core::events::WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<core::events::WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		for (auto & l : m_LayerStack) {
			l->OnEvent(e);
		}
	}
	 
	bool Application::OnWindowClose(core::events::WindowCloseEvent & e) {
		m_IsRunning = false;
		return true;
	}

	bool Application::OnWindowResize(core::events::WindowResizeEvent & e) {
		graphics::Viewport v;
		v.TopLeftX = 0.0f;
		v.TopLeftY = 0.0f;
		v.Width = e.GetWindowSizeX();
		v.Height = e.GetWindowSizeY();
		graphics::RenderState::Ref().SetViewport(v);

		Mat4 projection = Mat4::Ortho(-v.Width / 2.0f, v.Width / 2.0f, -v.Height / 2.0f, v.Height / 2.0f, -1.0f, 1.0f);
		math::MVPStack::Ref().Projection().Pop();
		math::MVPStack::Ref().Projection().Push(projection);

		return false;
	}

}
