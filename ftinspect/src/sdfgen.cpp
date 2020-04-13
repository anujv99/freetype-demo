#include "sdfgen.h"

#include <iostream>

// Bitmap Conversion

struct Point {
	int dx, dy;

	int DistSq() const { return dx * dx + dy * dy; }
};

struct Grid {
	Grid(unsigned int wid, unsigned int hei) : width(wid), height(hei) {
		grid = new Point * [height];
		for (unsigned int i = 0; i < height; i++) {
			grid[i] = new Point[width];
		}
	}
	~Grid() {
		for (unsigned int i = 0; i < height; i++) {
			delete[] grid[i];
		}
		delete[] grid;
	}

	Point ** grid = nullptr;
	unsigned int height = 0u, width = 0u;
};

Point inside = { 0, 0 };
Point empty = { 9999, 9999 };

Point Get(Grid & g, int x, int y) {
	// OPTIMIZATION: you can skip the edge check code if you make your grid 
	// have a 1-pixel gutter.
	if (x >= 0 && y >= 0 && x < g.width && y < g.height)
		return g.grid[y][x];
	else
		return empty;
}

void Put(Grid & g, int x, int y, const Point & p) {
	g.grid[y][x] = p;
}

void Compare(Grid & g, Point & p, int x, int y, int offsetx, int offsety) {
	Point other = Get(g, x + offsetx, y + offsety);
	other.dx += offsetx;
	other.dy += offsety;

	if (other.DistSq() < p.DistSq())
		p = other;
}

void GenerateSDF(Grid & g) {
	// Pass 0
	for (int y = 0; y < g.height; y++) {
		for (int x = 0; x < g.width; x++) {
			Point p = Get(g, x, y);
			Compare(g, p, x, y, -1, 0);
			Compare(g, p, x, y, 0, -1);
			Compare(g, p, x, y, -1, -1);
			Compare(g, p, x, y, 1, -1);
			Put(g, x, y, p);
		}

		for (int x = g.width - 1; x >= 0; x--) {
			Point p = Get(g, x, y);
			Compare(g, p, x, y, 1, 0);
			Put(g, x, y, p);
		}
	}

	// Pass 1
	for (int y = g.height - 1; y >= 0; y--) {
		for (int x = g.width - 1; x >= 0; x--) {
			Point p = Get(g, x, y);
			Compare(g, p, x, y, 1, 0);
			Compare(g, p, x, y, 0, 1);
			Compare(g, p, x, y, -1, 1);
			Compare(g, p, x, y, 1, 1);
			Put(g, x, y, p);
		}

		for (int x = 0; x < g.width; x++) {
			Point p = Get(g, x, y);
			Compare(g, p, x, y, -1, 0);
			Put(g, x, y, p);
		}
	}
}

SDFData SDFGen::GenerateSDFFromBitmap(const unsigned char * buff, unsigned int sizeX, unsigned int sizeY) {

	// SDF is usually larger than the original image
	// So to contain the entire spread area some padding is added
	int PADDING = 16;

	Grid grid1(sizeX + PADDING * 2, sizeY + PADDING * 2);
	Grid grid2(sizeX + PADDING * 2, sizeY + PADDING * 2);

	for (int x = PADDING; x < sizeX + PADDING; x++) {
		for (int y = PADDING; y < sizeY + PADDING; y++) {
			// Points inside get marked with a dx/dy of zero.
			// Points outside get marked with an infinitely large distance.

			unsigned char r = buff[((y - PADDING) * sizeX) + (x - PADDING)];
			if (r < 128) {
				Put(grid1, x, y, inside);
				Put(grid2, x, y, empty);
			} else {
				Put(grid2, x, y, inside);
				Put(grid1, x, y, empty);
			}
		}
	}

	for (int x = 0; x < sizeX + PADDING * 2; x++) {
		for (int y = 0; y < sizeY + PADDING * 2; y++) {
			if (x < PADDING || x > sizeX + PADDING || y < PADDING || y > sizeY + PADDING) {
				Put(grid1, x, y, inside);
				Put(grid2, x, y, empty);
			}
		}
	}

	::GenerateSDF(grid1);
	::GenerateSDF(grid2);

	SDFData data;
	data.Width = sizeX + PADDING * 2;
	data.Height = sizeY + PADDING * 2;
	data.Buffer.reserve(data.Width * data.Height);

	for (int y = 0; y < sizeY + PADDING * 2; y++) {
		for (int x = 0; x < sizeX + PADDING * 2; x++) {
			// Calculate the actual distance from the dx/dy
			int dist1 = (int)(sqrt((double)Get(grid1, x, y).DistSq()));
			int dist2 = (int)(sqrt((double)Get(grid2, x, y).DistSq()));
			int dist = dist1 - dist2;

			float color = (float)dist / 20.0f;
			color = (color + 1.0f) / 2.0f;

			data.Buffer.push_back(color);
		}
	}

	return data;
}

// Generate DF from outline. Taken directly from msdfgen/ext/import-font.cpp

#include <msdfgen.h>
#include <msdfgen-ext.h>

using namespace msdfgen;

#define F26DOT6_TO_DOUBLE(x) (1/64.*double(x))

struct FtContext {
	Point2 position;
	Shape * shape;
	Contour * contour;
};

static Point2 ftPoint2(const FT_Vector & vector) {
	return Point2(F26DOT6_TO_DOUBLE(vector.x), F26DOT6_TO_DOUBLE(vector.y));
}

static int ftMoveTo(const FT_Vector * to, void * user) {
	FtContext * context = reinterpret_cast<FtContext *>(user);
	if (!(context->contour && context->contour->edges.empty()))
		context->contour = &context->shape->addContour();
	context->position = ftPoint2(*to);
	return 0;
}

static int ftLineTo(const FT_Vector * to, void * user) {
	FtContext * context = reinterpret_cast<FtContext *>(user);
	Point2 endpoint = ftPoint2(*to);
	if (endpoint != context->position) {
		context->contour->addEdge(new LinearSegment(context->position, endpoint));
		context->position = endpoint;
	}
	return 0;
}

static int ftConicTo(const FT_Vector * control, const FT_Vector * to, void * user) {
	FtContext * context = reinterpret_cast<FtContext *>(user);
	context->contour->addEdge(new QuadraticSegment(context->position, ftPoint2(*control), ftPoint2(*to)));
	context->position = ftPoint2(*to);
	return 0;
}

static int ftCubicTo(const FT_Vector * control1, const FT_Vector * control2, const FT_Vector * to, void * user) {
	FtContext * context = reinterpret_cast<FtContext *>(user);
	context->contour->addEdge(new CubicSegment(context->position, ftPoint2(*control1), ftPoint2(*control2), ftPoint2(*to)));
	context->position = ftPoint2(*to);
	return 0;
}

static bool loadGlyph(Shape & output, FT_Outline * outline) {
	output.contours.clear();
	output.inverseYAxis = true;

	FtContext context = { };
	context.shape = &output;
	FT_Outline_Funcs ftFunctions;
	ftFunctions.move_to = &ftMoveTo;
	ftFunctions.line_to = &ftLineTo;
	ftFunctions.conic_to = &ftConicTo;
	ftFunctions.cubic_to = &ftCubicTo;
	ftFunctions.shift = 0;
	ftFunctions.delta = 0;
	FT_Error error = FT_Outline_Decompose(outline, &ftFunctions, &context);
	if (error)
		return false;
	if (!output.contours.empty() && output.contours.back().edges.empty())
		output.contours.pop_back();
	return true;
}

SDFData SDFGen::GenerateSDFFromOutline(FT_Outline * outline, unsigned int sizeX, unsigned int sizeY) {
	SDFData data;
	data.Buffer.reserve(sizeX * sizeY);

	FT_BBox bBox;
	FT_Outline_Get_BBox(outline, &bBox);
	double bBoxCenterX = bBox.xMin + ((bBox.xMax - bBox.xMin) / 2.0);
	double bBoxCenterY = bBox.yMin + ((bBox.yMax - bBox.yMin) / 2.0);
	bBoxCenterX /= 64.0;
	bBoxCenterY /= 64.0;

	Shape shape;
	if (::loadGlyph(shape, outline)) {
		shape.normalize();
		Bitmap<float, 1> df(sizeX, sizeY);
		generateSDF(df, shape, 4.0, Vector2(1.0), Vector2(-bBoxCenterX + sizeX / 2.0, -bBoxCenterY + sizeY / 2.0));
		
		{
			// Convert float bitmap to unsigned char
			for (unsigned int y = 0; y < df.height(); y++) {
				for (unsigned int x = 0; x < df.width(); x++) {
					float pixelColor = (*df(x, y) + 1.0f) / 2.0f;
					data.Buffer.push_back(pixelColor);
				}
			}

			data.Width = df.width();
			data.Height = df.height();
		}
		
	}
	
	return data;
}
