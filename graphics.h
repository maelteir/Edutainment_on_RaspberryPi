#pragma once

#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

void InitGraphics();
void ReleaseGraphics();

class GfxShader
{
	GLchar* Src;
	GLuint Id;
	GLuint GlShaderType;

public:

	GfxShader() : Src(NULL), Id(0), GlShaderType(0) {}
	~GfxShader() { if(Src) delete[] Src; }

	bool LoadVertexShader(const char* filename);
	bool LoadFragmentShader(const char* filename);
	GLuint GetId() { return Id; }
};

class GfxProgram
{
	GfxShader* VertexShader;
	GfxShader* FragmentShader;
	GLuint Id;

public:

	GfxProgram() {}
	~GfxProgram() {}

	bool Create(GfxShader* vertex_shader, GfxShader* fragment_shader);
	GLuint GetId() { return Id; }
};

class GfxTexture
{
	int Width;
	int Height;
	GLuint Id;

public:

	GfxTexture() : Width(0), Height(0) {}
	~GfxTexture() {}

	bool Create(int width, int height, int level, const void* data = NULL);
	void SetPixels(const void* data);
	GLuint GetId() { return Id; }
	int GetWidth() {return Width;}
	int GetHeight() {return Height;}
};
void CreatePrograms();
void Diff(int bind ,GfxTexture* texture, GfxTexture* texture1,float x0, float y0, float x1, float y1);
void RGBToGray(int bind , GfxTexture* texture0,float x0, float y0, float x1, float y1);
void Blur(int bind , GfxTexture* texture0,float x0, float y0, float x1, float y1);
void Threshold(int bind , GfxTexture* texture0,float x0, float y0, float x1, float y1);
void Moments(int bind , GfxTexture* texture0,float x0, float y0, float x1, float y1);
void HorizontalReduction(int bind , GfxTexture* texture0,float x0, float y0, float x1, float y1, float textureWidth, float texttureHeight, int stride );
void VerticalReduction(int bind , GfxTexture* texture0,float x0, float y0, float x1, float y1, float textureWidth, float texttureHeight );
void BeginFrame(int bind, GfxTexture* OPtexture, int ReducedVP);
void EndFrame();
