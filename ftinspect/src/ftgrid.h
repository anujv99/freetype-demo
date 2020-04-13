
#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <string>
#include <vector>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H
#include FT_GLYPH_H
#include FT_BBOX_H
#include FT_OUTLINE_H

#include <utils/stronghandle.h>
#include <graphics/texture.h>
#include <graphics/framebuffer.h>

#include "ftrenderer/ftrenderer.h"

class FTGrid {
public:
	FTGrid();
public:
	void LoadFont(const std::string & fontPath);
	void DrawBitmap(unsigned int fontIndex);
	void DrawPoints(unsigned int fontIndex);
	void DrawOutline(unsigned int fontIndex);
	void DrawDistanceFields(unsigned int fontIndex);
	void UpdateBitmap(unsigned int fontIndex, unsigned int glyphIndex, unsigned int ptSize /*in pixels*/);
	void UpdateDistanceFieldTexture(FT_Bitmap * bmp, FT_Outline * outline);

	inline FTRenderer & GetRenderer() { return m_Renderer; }

	void Update();
	void Gui();
	void Render();

	// Function to update width and egde uniform variable of the distance field shader
	void UpdateWidthEdge();
private:
	std::vector<FT_Face> m_FontFaces;

	FT_Library m_Lib;
	FTRenderer m_Renderer;

	FT_Int32 m_Flags;

	engine::utils::StrongHandle<engine::graphics::Texture> m_TextureCache;
	engine::utils::StrongHandle<engine::graphics::Texture> m_DFTextureCache; //Distance Field Texture

	engine::utils::StrongHandle<engine::graphics::Framebuffer> m_DefaultFBO;
	engine::utils::StrongHandle<engine::graphics::Framebuffer> m_DFFBO; //Distance Field framebuffer


	bool m_IsChanged			= true;
	bool m_ShowBitmap			= true;
	bool m_ShowPoints			= false;
	bool m_ShowOutline			= false;
	bool m_ShowDistanceField	= false;
	bool m_ShowRendererDF		= false;
	bool m_OutlineDistanceField = false; //Use DF generated from outline if true otherwise generate from bitmap

	int m_GlyphIndex			= 0;
	int m_FontSize				= 72;
	int m_FontIndex				= 0;
	int m_OutlineThickness		= 1;

	float m_Zoom				= 1.0f;
	float m_Gamma				= 1.5f;

	// parameters to render distance field
	float m_DFWidth				= 0.23f;
	float m_DFEdge				= 0.01f;

	engine::Vec2 m_Offset		= engine::Vec2(0.0f);
};

#endif //__ENGINE_H__
