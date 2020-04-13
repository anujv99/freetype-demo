#include "ftinspect.h"

#include <imgui.h>

#include <renderer/renderer.h>
#include <math/mvpstack.h>
#include <renderer/immgfx.h>

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace engine;
using namespace core;
using namespace graphics;
using namespace renderer;

FT_Outline outline;

FTInspect::FTInspect() : Layer("ftinspect_layer") {
	SetupImGui();
}

FTInspect::~FTInspect() {

}

void FTInspect::OnImGuiUpdate() {
	m_FTGrid.Gui();

}

void FTInspect::OnRender() {
	m_FTGrid.Render();
}

void FTInspect::OnUpdate(TimePoint dt) {
	m_FTGrid.Update();
}

void FTInspect::SetupImGui() {
	ImGuiIO & io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	
	ImGuiStyle & style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
}
