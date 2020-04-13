#include "ftgrid.h"

#include <imgui.h>
#include "imguifilebrowser.h"

#include <math/math.h>
#include <utils/stringutils.h>
#include <utils/assert.h>
#include <core/input.h>
#include <graphics/renderstate.h>

#include "sdfgen.h"

using namespace engine;
using namespace utils;
using namespace graphics;
using namespace core;

FTGrid::FTGrid() : m_Renderer() {
	FT_Init_FreeType(&m_Lib);
	m_Flags = FT_LOAD_RENDER;

	m_Renderer.SetGamma(m_Gamma);
	UpdateWidthEdge();

	m_DefaultFBO = Framebuffer::Create(Vec2i(500), InternalTextureFormat::RGBA);
	m_DFFBO = Framebuffer::Create(Vec2i(500), InternalTextureFormat::RGBA);
}

void FTGrid::LoadFont(const std::string & fontPath) {
	FT_Face face;
	FT_New_Face(m_Lib, fontPath.c_str(), 0, &face);
	m_FontFaces.push_back(face);
	m_IsChanged = true;
}

void FTGrid::DrawBitmap(unsigned int fontIndex) {
	m_Renderer.DrawQuad(m_Offset, ToVec2(m_TextureCache->GetSize()) * m_Zoom, m_TextureCache);
}

void FTGrid::DrawPoints(unsigned int fontIndex) {
	FT_Face & face = m_FontFaces[fontIndex];
	FT_Outline & outline = face->glyph->outline;

	FT_BBox bBox;
	FT_Outline_Get_BBox(&outline, &bBox);
	
	Vec2 bBoxCenter = Vec2(bBox.xMin + ((bBox.xMax - bBox.xMin) / 2.0f), bBox.yMin + ((bBox.yMax - bBox.yMin) / 2.0f)) / 64.0f;

	for (unsigned int i = 0; i < outline.n_points; i++) {
		Vec2 pos = Vec2(outline.points[i].x, outline.points[i].y);
		m_Renderer.DrawQuad(m_Offset + ((pos / 64.0f) - bBoxCenter) * m_Zoom, Vec2(6.0f), Vec4(1.0f, 0.0f, 1.0f, 1.0f));
	}
}

void FTGrid::DrawOutline(unsigned int fontIndex) {
	FT_Outline & outline = m_FontFaces[fontIndex]->glyph->outline;

	struct UserData {
		std::vector<Vec2> Positions;
		Vec2 CurrentPos;
	};

	FT_Outline_Funcs funcs;
	funcs.shift = 0;
	funcs.delta = 0;

	funcs.move_to = [](const FT_Vector * to, void * user)->int {
		UserData * data = (UserData *)user;
		data->CurrentPos = Vec2(to->x, to->y) / 64.0f;
		return 0;
	};

	funcs.line_to = [](const FT_Vector * to, void * user)->int {
		UserData * data = (UserData *)user;
		data->Positions.push_back(data->CurrentPos);
		data->Positions.push_back(Vec2(to->x, to->y) / 64.0f);
		data->CurrentPos = Vec2(to->x, to->y) / 64.0f;
		return 0;
	};

	funcs.conic_to = [](const FT_Vector * control, const FT_Vector * to, void * user)->int {
		UserData * data = (UserData *)user;

		Vec2 startPos = data->CurrentPos;
		Vec2 endPos = Vec2(to->x, to->y) / 64.0f;
		Vec2 controlPos = Vec2(control->x, control->y) / 64.0f;

		Vec2 prevPos = data->CurrentPos;

		for (float i = 0.1f; i < 1.1f; i += 0.1f) {
			Vec2 nextPos = Vec2::QuadraticBezier(startPos, endPos, controlPos, i);
			data->Positions.push_back(prevPos);
			data->Positions.push_back(nextPos);
			prevPos = nextPos;
		}

		data->CurrentPos = endPos;

		return 0;
	};

	funcs.cubic_to = [](const FT_Vector * control1, const FT_Vector * control2, const FT_Vector * to, void * user)->int {
		UserData * data = (UserData *)user;

		Vec2 startPos = data->CurrentPos;
		Vec2 endPos = Vec2(to->x, to->y) / 64.0f;
		Vec2 control1Pos = Vec2(control1->x, control1->y) / 64.0f;
		Vec2 control2Pos = Vec2(control2->x, control2->y) / 64.0f;

		Vec2 prevPos = data->CurrentPos;

		for (float i = 0.1f; i < 1.1f; i += 0.1f) {
			Vec2 nextPos = Vec2::CubicBezier(startPos, endPos, control1Pos, control2Pos, i);
			data->Positions.push_back(prevPos);
			data->Positions.push_back(nextPos);
			prevPos = nextPos;
		}

		data->CurrentPos = endPos;

		return 0;
	};

	UserData data;
	FT_Outline_Decompose(&outline, &funcs, &data);

	if (data.Positions.size() <= 0) { return; }

	FT_BBox bBox;
	FT_Outline_Get_BBox(&outline, &bBox);
	Vec2 bBoxCenter = Vec2(bBox.xMin + ((bBox.xMax - bBox.xMin) / 2.0f), bBox.yMin + ((bBox.yMax - bBox.yMin) / 2.0f)) / 64.0f;

	ASSERT(data.Positions.size() % 2 == 0);

	for (std::size_t i = 0; i < data.Positions.size(); i += 2) {
		m_Renderer.DrawLine(
			m_Offset + (data.Positions[i] - bBoxCenter) * m_Zoom,
			m_Offset + (data.Positions[i + 1] - bBoxCenter) * m_Zoom,
			Vec4(0.0f, 1.0f, 1.0f, 1.0f));
	}

	int x = 0;
}

void FTGrid::DrawDistanceFields(unsigned int fontIndex) {
	m_Renderer.DrawQuad(m_Offset, ToVec2(m_DFTextureCache->GetSize()) * m_Zoom, m_DFTextureCache);
}

void FTGrid::UpdateBitmap(unsigned int fontIndex, unsigned int glyphIndex, unsigned int ptSize /*in pixels*/) {
	if (m_IsChanged) {
		FT_Face & face = m_FontFaces[fontIndex];
		FT_Set_Pixel_Sizes(face, 0, ptSize);
		FT_Load_Glyph(face, glyphIndex, m_Flags);

		Vec2i size = Vec2i(face->glyph->bitmap.width, face->glyph->bitmap.rows);

		if (size.x <= 0 || size.y <= 0) return;

		TextureParams params;
		params.Filtering = TextureFiltering::NEAREST;
		params.InternalFormat = InternalTextureFormat::R;
		params.ExternalFormat = ExternalTextureFormat::R;
		params.Wrapping = TextureWrapping::CLAMP;

		if (m_Flags & FT_LOAD_MONOCHROME) {
			FT_Bitmap bmp;
			FT_Bitmap_Init(&bmp);

			FT_Bitmap_Convert(m_Lib, &face->glyph->bitmap, &bmp, 1);

			for (unsigned int i = 0; i < bmp.width * bmp.rows; i++) {
				if (bmp.buffer[i] > 0) bmp.buffer[i] = 255;
			}

			m_TextureCache = Texture::Create(size, bmp.buffer, params);

			UpdateDistanceFieldTexture(&bmp, &face->glyph->outline);

			FT_Bitmap_Done(m_Lib, &bmp);
		} else {
			m_TextureCache = Texture::Create(size, face->glyph->bitmap.buffer, params);
			UpdateDistanceFieldTexture(&face->glyph->bitmap, &face->glyph->outline);
		}

		m_IsChanged = false;
	}
}

void FTGrid::UpdateDistanceFieldTexture(FT_Bitmap * bmp, FT_Outline * outline) {
	if (!m_ShowDistanceField) return;

	TextureParams params;
	params.Filtering = TextureFiltering::LINEAR;
	params.InternalFormat = InternalTextureFormat::R32F;
	params.ExternalFormat = ExternalTextureFormat::R;
	params.Wrapping = TextureWrapping::CLAMP;
	params.ExternalDataType = ExternalDataType::FLOAT;

	if (m_OutlineDistanceField) {
		auto df = SDFGen::GenerateSDFFromOutline(outline, bmp->width, bmp->rows);
		m_DFTextureCache = Texture::Create(Vec2i(df.Width, df.Height), (unsigned char *)df.Buffer.data(), params);

		m_DFWidth = 0.23f;
		m_DFEdge = 0.01f;
		UpdateWidthEdge();
	} else {
		auto df = SDFGen::GenerateSDFFromBitmap(bmp->buffer, bmp->width, bmp->rows);
		m_DFTextureCache = Texture::Create(Vec2i(df.Width, df.Height), (unsigned char *)df.Buffer.data(), params);

		m_DFWidth = 0.5f;
		m_DFEdge = 0.01f;
		UpdateWidthEdge();
	}
}

void FTGrid::Update() {
	if (m_FontFaces.size() <= m_FontIndex) return;
	UpdateBitmap(0, m_GlyphIndex, m_FontSize);
}

void FTGrid::Gui() {
	{
		// If no font loaded
		if (m_FontFaces.size() == 0) {

			static ImGui::FileBrowser fileDialog;

			ImGui::Begin("Font Selector", (bool *)0, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("No Font Selected. Please select a font file!");
			if (ImGui::Button("Select Font File")) {
				fileDialog.Open();
			}
			ImGui::End();

			fileDialog.Display();

			if (fileDialog.HasSelected()) {
				LoadFont(fileDialog.GetSelected().string());
			}

			return;
		}
	}

	if (m_ShowDistanceField) {
		// Distance Field Viewport
		ImGui::Begin("Distance Field", (bool *)0, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Scroll to zoom");
		ImGui::Text("Current Glyph Size : %d, %d", m_DFTextureCache->GetSize().x, m_DFTextureCache->GetSize().y);

		if (ImGui::IsWindowHovered()) {
			float delta = Input::GetMouseScrollDelta().y;
			m_Zoom += delta;
			if (m_Zoom < 0.5f) m_Zoom = 0.5f;
		}

		ImGui::Image((void *)m_DFFBO->GetTexture()->GetNativeTexture(),
			ImVec2(m_DFFBO->GetSize().x, m_DFFBO->GetSize().y), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
	}

	{
		// Default Viewport
		ImGui::Begin("Window", (bool *)0, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Scroll to zoom");
		ImGui::Text("Current Glyph Size : %d, %d", m_TextureCache->GetSize().x, m_TextureCache->GetSize().y);

		if (ImGui::IsWindowHovered()) {
			float delta = Input::GetMouseScrollDelta().y;
			m_Zoom += delta;
			if (m_Zoom < 0.5f) m_Zoom = 0.5f;
		}

		ImGui::Image((void *)m_DefaultFBO->GetTexture()->GetNativeTexture(),
			ImVec2(m_DefaultFBO->GetSize().x, m_DefaultFBO->GetSize().y), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
	}

	{
		// Properties Window
		ImGui::Begin("Properties", (bool *)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::Text("ftinspect demo");
		if (ImGui::SliderFloat("Gamma", &m_Gamma, 0.05f, 3.0f)) {
			m_Renderer.SetGamma(m_Gamma);
		}
		ImGui::DragFloat2("Position", &m_Offset[0]);

		ImGui::Separator();

		if (ImGui::SliderInt("Size", &m_FontSize, 8, 512)) {
			m_IsChanged = true;
		}
		if (ImGui::SliderInt("Glyph Index", &m_GlyphIndex, 0, m_FontFaces[m_FontIndex]->num_glyphs)) {
			m_IsChanged = true;
		}

		ImGui::Separator();
		ImGui::Text("Render Mode");

		static int radio = 0;
		if (ImGui::RadioButton("Default", radio == 0)) {
			radio = 0;
			m_Flags = FT_LOAD_RENDER;
			m_IsChanged = true;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("No Antialiasing", radio == 1)) {
			radio = 1;
			m_Flags = FT_LOAD_RENDER | FT_LOAD_MONOCHROME;
			m_IsChanged = true;
		}

		ImGui::Checkbox("Show Bitmap", &m_ShowBitmap);
		ImGui::Checkbox("Show Points", &m_ShowPoints);
		ImGui::Checkbox("Show Outline", &m_ShowOutline);

		if (m_ShowOutline) {
			ImGui::Indent();
			if (ImGui::SliderInt("Outline Thickness", &m_OutlineThickness, 1, 5)) {
				RenderState::Ref().SetLineThickness((float)m_OutlineThickness);
			}
			ImGui::Unindent();
		}

		ImGui::Separator();

		if (ImGui::Checkbox("Show Distance Field", &m_ShowDistanceField)) {
			m_IsChanged = true;
		}

		if (m_ShowDistanceField) {
			ImGui::Indent();
			ImGui::Checkbox("Show Rendered", &m_ShowRendererDF);

			static int dfradio = 0;

			if (ImGui::RadioButton("Bitmap", dfradio == 0)) {
				dfradio = 0;
				m_OutlineDistanceField = false;
				m_IsChanged = true;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Outline", dfradio == 1)) {
				dfradio = 1;
				m_OutlineDistanceField = true;
				m_IsChanged = true;
			}

			if (m_ShowRendererDF) {
				if (ImGui::SliderFloat("Width", &m_DFWidth, 0.01f, 1.0f)) {
					UpdateWidthEdge();
				}
				if (ImGui::SliderFloat("Edge", &m_DFEdge, 0.01f, 1.0f)) {
					UpdateWidthEdge();
				}
			}

			ImGui::Unindent();
		}

		ImGui::End();
	}

}

void FTGrid::Render() {
	if (m_FontFaces.size() <= m_FontIndex) return;

	m_Renderer.StartDrawing(m_DefaultFBO);

	if (m_ShowBitmap)
		DrawBitmap(m_FontIndex);

	if (m_ShowPoints)
		DrawPoints(m_FontIndex);

	if (m_ShowOutline)
		DrawOutline(m_FontIndex);

	m_Renderer.EndDrawing(m_DefaultFBO);

	if (m_ShowDistanceField) {
		m_Renderer.StartDrawing(m_DFFBO);

		DrawDistanceFields(m_FontIndex);

		m_Renderer.EndDrawing(m_DFFBO, m_ShowRendererDF);
	}
}

void FTGrid::UpdateWidthEdge() {
	m_Renderer.GetDFShader()->Bind();
	m_Renderer.GetDFShader()->SetUniformFloat(m_Renderer.GetDFShader()->GetUniformLocation("width"), m_DFWidth);
	m_Renderer.GetDFShader()->SetUniformFloat(m_Renderer.GetDFShader()->GetUniformLocation("edge"), m_DFEdge);
}
