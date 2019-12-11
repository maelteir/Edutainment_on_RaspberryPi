#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include "bcm_host.h"
#include "graphics.h"

#define check() assert(glGetError() == 0)

uint32_t GScreenWidth;
uint32_t GScreenHeight;
EGLDisplay GDisplay;
EGLSurface GSurface;
EGLContext GContext;

GfxShader GSimpleVS;
GfxShader GSimpleFS;
GfxProgram GSimpleProg;
GLuint GQuadVertexBuffer;
GLuint GFrameBuffer;

GfxShader GGrayFS;
GfxProgram GGrayProg;

GfxShader GBlurFS;
GfxProgram GBlurProg;

GfxShader GThresFS;
GfxProgram GThresProg;

GfxShader GMomFS;
GfxProgram GMomProg;

GfxShader GHRedFS;
GfxProgram GHRedProg;

GfxShader GHRedFS2;
GfxProgram GHRedProg2;

GfxShader GVRedFS;
GfxProgram GVRedProg;

void InitGraphics()
{
	bcm_host_init();
	int32_t success = 0;
	EGLBoolean result;
	EGLint num_config;

	static EGL_DISPMANX_WINDOW_T nativewindow;

	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;

	static const EGLint attribute_list[] =
	{
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};

	static const EGLint context_attributes[] = 
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	EGLConfig config;

	// get an EGL display connection
	GDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	assert(GDisplay!=EGL_NO_DISPLAY);
	check();

	// initialize the EGL display connection
	result = eglInitialize(GDisplay, NULL, NULL);
	assert(EGL_FALSE != result);
	check();

	// get an appropriate EGL frame buffer configuration
	result = eglChooseConfig(GDisplay, attribute_list, &config, 1, &num_config);
	assert(EGL_FALSE != result);
	check();

	// get an appropriate EGL frame buffer configuration
	result = eglBindAPI(EGL_OPENGL_ES_API);
	assert(EGL_FALSE != result);
	check();

	// create an EGL rendering context
	GContext = eglCreateContext(GDisplay, config, EGL_NO_CONTEXT, context_attributes);
	assert(GContext!=EGL_NO_CONTEXT);
	check();

	// create an EGL window surface
	success = graphics_get_display_size(0 /* LCD */, &GScreenWidth, &GScreenHeight);
	assert( success >= 0 );

	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.width = GScreenWidth;
	dst_rect.height = GScreenHeight;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = GScreenWidth << 16;
	src_rect.height = GScreenHeight << 16;        

	dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
	dispman_update = vc_dispmanx_update_start( 0 );

	dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
		-127/*layer*/, &dst_rect, 0/*src*/,
		&src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, (DISPMANX_TRANSFORM_T)0/*transform*/);

	static const EGLint pbufferAttributes[] =
	{
		EGL_WIDTH,
		GScreenWidth,
		EGL_HEIGHT,
		GScreenHeight,
		EGL_NONE
	};


	nativewindow.element = dispman_element;
	nativewindow.width = GScreenWidth;
 	nativewindow.height = GScreenHeight;
	vc_dispmanx_update_submit_sync( dispman_update );

	printf("GScreenWidth: %i GScreenHeight: %i",GScreenWidth, GScreenHeight);

	check();

	//GSurface = eglCreateWindowSurface( GDisplay, config, &nativewindow, NULL );
	GSurface = eglCreatePbufferSurface( GDisplay, config, pbufferAttributes);//, NULL );
	assert(GSurface != EGL_NO_SURFACE);
	check();

	// connect the context to the surface
	result = eglMakeCurrent(GDisplay, GSurface, GSurface, GContext);
	assert(EGL_FALSE != result);
	check();

	// Set background color and clear buffers
	glClearColor(0.15f, 0.25f, 0.35f, 1.0f);
	glClear( GL_COLOR_BUFFER_BIT );

	
	//create an ickle vertex buffer
/*	static const GLfloat quad_vertex_positions[] = {
		0.0f, 0.0f,	1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};
	
*/
	//load the shaders
	CreatePrograms();

	static const GLfloat quad_vertex_positions[] = {
		0.0f, 0.0f,	1.0f, 1.0f,
		-1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, -1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 1.0f
	};	glGenBuffers(1, &GQuadVertexBuffer);
	
	check();
	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertex_positions), quad_vertex_positions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	check();
}

void CreatePrograms()
{
	//load the test shaders
	GSimpleVS.LoadVertexShader("simplevertshader.glsl");
	GSimpleFS.LoadFragmentShader("simplefragshader.glsl");
	GSimpleProg.Create(&GSimpleVS,&GSimpleFS);
	check();
	glUseProgram(GSimpleProg.GetId());
	check();

	//Create RGB shader and program
	GGrayFS.LoadFragmentShader("grayfragshader.glsl");
	GGrayProg.Create(&GSimpleVS,&GGrayFS);
	check();
	glUseProgram(GGrayProg.GetId());
	check();

	//Create Blur shader and program
	GBlurFS.LoadFragmentShader("blurfragshader.glsl");
	GBlurProg.Create(&GSimpleVS,&GBlurFS);
	check();
	glUseProgram(GBlurProg.GetId());
	check();
	
	//Create Threshold shader and program
	GThresFS.LoadFragmentShader("thresfragshader.glsl");
	GThresProg.Create(&GSimpleVS,&GThresFS);
	check();
	glUseProgram(GThresProg.GetId());
	check();

	//Create Moments shader and program
	GMomFS.LoadFragmentShader("momfragshader.glsl");
	GMomProg.Create(&GSimpleVS,&GMomFS);
	check();
	glUseProgram(GMomProg.GetId());
	check();

	//Create Horizontal reduction shader and program
	GHRedFS.LoadFragmentShader("horizRedfragshader.glsl");
	GHRedProg.Create(&GSimpleVS,&GHRedFS);
	check();
	glUseProgram(GHRedProg.GetId());
	check();

	//Create Horizontal reduction shader and program with stride 2
	GHRedFS2.LoadFragmentShader("horizRedfragshader2.glsl");
	GHRedProg2.Create(&GSimpleVS,&GHRedFS2);
	check();
	glUseProgram(GHRedProg2.GetId());
	check();

	//Create Vertical reduction shader and program 
	GVRedFS.LoadFragmentShader("vertRedfragshader.glsl");
	GVRedProg.Create(&GSimpleVS,&GVRedFS);
	check();
	glUseProgram(GVRedProg.GetId());
	check();

}

/*void BeginFrame()
{
	// Prepare viewport
	glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	check();

	// Clear the background
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	check();
}
*/
// bind = 1 when the render buffer must be binded to the passed texture
void BeginFrame(int bind, GfxTexture* OPtexture, int ReducedVP)
{
        if ( bind == 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
	{

		glGenFramebuffers(1,&GFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, GFrameBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, OPtexture->GetId(), 0 );	
	}
	check();

	//printf("Display width=%i Display height= %i\n",GScreenWidth,GScreenHeight);
	// Prepare viewport
	glViewport ( 0, 0, 1024, 1024); //GScreenWidth*2, GScreenHeight*2 );
	check();

	if (ReducedVP == 1){
		glViewport ( 0, 0, 64, 64); //GScreenWidth*2, GScreenHeight*2 );
		check();
	}


	// Clear the background
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	check();
}
void EndFrame()
{
	eglSwapBuffers(GDisplay,GSurface);
	check();
}

void ReleaseGraphics()
{

}

// printShaderInfoLog
// From OpenGL Shading Language 3rd Edition, p215-216
// Display (hopefully) useful error messages if shader fails to compile
void printShaderInfoLog(GLint shader)
{
	int infoLogLen = 0;
	int charsWritten = 0;
	GLchar *infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

	if (infoLogLen > 0)
	{
		infoLog = new GLchar[infoLogLen];
		// error check for fail to allocate memory omitted
		glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
		std::cout << "InfoLog : " << std::endl << infoLog << std::endl;
		delete [] infoLog;
	}
}

bool GfxShader::LoadVertexShader(const char* filename)
{
	//cheeky bit of code to read the whole file into memory
	assert(!Src);
	FILE* f = fopen(filename, "rb");
	assert(f);
	fseek(f,0,SEEK_END);
	int sz = ftell(f);
	fseek(f,0,SEEK_SET);
	Src = new GLchar[sz+1];
	fread(Src,1,sz,f);
	Src[sz] = 0; //null terminate it!
	fclose(f);

	//now create and compile the shader
	GlShaderType = GL_VERTEX_SHADER;
	Id = glCreateShader(GlShaderType);
	glShaderSource(Id, 1, (const GLchar**)&Src, 0);
	glCompileShader(Id);
	check();

	//compilation check
	GLint compiled;
	glGetShaderiv(Id, GL_COMPILE_STATUS, &compiled);
	if(compiled==0)
	{
		printf("Failed to compile vertex shader %s:\n%s\n", filename, Src);
		printShaderInfoLog(Id);
		glDeleteShader(Id);
		return false;
	}
	else
	{
		printf("Compiled vertex shader %s:\n%s\n", filename, Src);
	}

	return true;
}

bool GfxShader::LoadFragmentShader(const char* filename)
{
	//cheeky bit of code to read the whole file into memory
	assert(!Src);
	FILE* f = fopen(filename, "rb");
	assert(f);
	fseek(f,0,SEEK_END);
	int sz = ftell(f);
	fseek(f,0,SEEK_SET);
	Src = new GLchar[sz+1];
	fread(Src,1,sz,f);
	Src[sz] = 0; //null terminate it!
	fclose(f);

	//now create and compile the shader
	GlShaderType = GL_FRAGMENT_SHADER;
	Id = glCreateShader(GlShaderType);
	glShaderSource(Id, 1, (const GLchar**)&Src, 0);
	glCompileShader(Id);
	check();

	//compilation check
	GLint compiled;
	glGetShaderiv(Id, GL_COMPILE_STATUS, &compiled);
	if(compiled==0)
	{
		printf("Failed to compile fragment shader %s:\n%s\n", filename, Src);
		printShaderInfoLog(Id);
		glDeleteShader(Id);
		return false;
	}
	else
	{
		printf("Compiled fragment shader %s:\n%s\n", filename, Src);
	}

	return true;
}

bool GfxProgram::Create(GfxShader* vertex_shader, GfxShader* fragment_shader)
{
	VertexShader = vertex_shader;
	FragmentShader = fragment_shader;
	Id = glCreateProgram();
	glAttachShader(Id, VertexShader->GetId());
	glAttachShader(Id, FragmentShader->GetId());
	glLinkProgram(Id);
	check();
	printf("Created program id %d from vs %d and fs %d\n", GetId(), VertexShader->GetId(), FragmentShader->GetId());

	// Prints the information log for a program object
	char log[1024];
	glGetProgramInfoLog(Id,sizeof log,NULL,log);
	printf("%d:program:\n%s\n", Id, log);

	return true;	
}

void Diff(int bind ,GfxTexture* texture, GfxTexture* texture1,float x0, float y0, float x1, float y1)
{
	glUseProgram(GSimpleProg.GetId());
	check();
        
        glUniform2f(glGetUniformLocation(GSimpleProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GSimpleProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GSimpleProg.GetId(),"tex"), 0);
	glUniform1i(glGetUniformLocation(GSimpleProg.GetId(),"tex1"), 1);

	check();
       
        if (bind == 1){
		glBindFramebuffer(GL_FRAMEBUFFER, GFrameBuffer);
		check();
	}

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);
	check();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture->GetId());
	check();
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,texture1->GetId());
	check();
	
	GLuint loc = glGetAttribLocation(GSimpleProg.GetId(),"vertex");
	check();

	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 16, 0);
	check();

	glEnableVertexAttribArray(loc);
	check();

	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 );
	check();

	glFinish();
	check();

	glFlush();
	check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void RGBToGray(int bind , GfxTexture* texture0,float x0, float y0, float x1, float y1)
{

	glUseProgram(GGrayProg.GetId());
	check();

        glUniform2f(glGetUniformLocation(GGrayProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GGrayProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GGrayProg.GetId(),"tex"), 0);
   	check();

	if (bind == 1){
		glBindFramebuffer(GL_FRAMEBUFFER, GFrameBuffer);
		check();
	}

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);
	check();
       
        glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture0->GetId());
	check();

	GLuint loc = glGetAttribLocation(GGrayProg.GetId(),"vertex");
	check();

	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 0, 0);
	check();

	glEnableVertexAttribArray(loc);
	check();

	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 );
	check();

	glFinish();
	check();

	glFlush();
	check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Blur(int bind , GfxTexture* texture0,float x0, float y0, float x1, float y1)
{

	glUseProgram(GBlurProg.GetId());
	check();

        glUniform2f(glGetUniformLocation(GBlurProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GBlurProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GBlurProg.GetId(),"tex"), 0);
   	glUniform2f(glGetUniformLocation(GBlurProg.GetId(),"texelsize"),1.f/texture0->GetWidth(),1.f/texture0->GetHeight());
	check();


	if (bind == 1){
		glBindFramebuffer(GL_FRAMEBUFFER, GFrameBuffer);
		check();
	}

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);
	check();
       
        glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture0->GetId());
	check();

	GLuint loc = glGetAttribLocation(GBlurProg.GetId(),"vertex");
	check();

	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 0, 0);
	check();

	glEnableVertexAttribArray(loc);
	check();

	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 );
	check();

	glFinish();
	check();

	glFlush();
	check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Threshold(int bind , GfxTexture* texture0,float x0, float y0, float x1, float y1)
{

	glUseProgram(GThresProg.GetId());
	check();

        glUniform2f(glGetUniformLocation(GThresProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GThresProg.GetId(),"scale"),x1-x0,y1-y0);
	glUniform1i(glGetUniformLocation(GThresProg.GetId(),"tex"), 0);
	glUniform4f(glGetUniformLocation(GThresProg.GetId(),"col"),0.2f,0.2f,0.2f,1.0f);
	check();

	if (bind == 1){
		glBindFramebuffer(GL_FRAMEBUFFER, GFrameBuffer);
		check();
	}

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);
	check();
       
        glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture0->GetId());
	check();

	GLuint loc = glGetAttribLocation(GThresProg.GetId(),"vertex");
	check();

	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 0, 0);
	check();

	glEnableVertexAttribArray(loc);
	check();

	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 );
	check();

	glFinish();
	check();

	glFlush();
	check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Moments(int bind , GfxTexture* texture0,float x0, float y0, float x1, float y1)
{

	glUseProgram(GMomProg.GetId());
	check();

	glUniform2f(glGetUniformLocation(GMomProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GMomProg.GetId(),"scale"),x1-x0,y1-y0);
        glUniform1i(glGetUniformLocation(GMomProg.GetId(),"tex"), 0);
	printf("GScreenWidth %i GScreenHeight %i",GScreenWidth,GScreenHeight);
        glUniform1f(glGetUniformLocation(GMomProg.GetId(),"screenWidth"),float(GScreenWidth));
        glUniform1f(glGetUniformLocation(GMomProg.GetId(),"screenHeight"),float(GScreenHeight));
   	check();

	if (bind == 1){
		glBindFramebuffer(GL_FRAMEBUFFER, GFrameBuffer);
		check();
	}

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);
	check();
       
        glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture0->GetId());
	check();

	GLuint loc = glGetAttribLocation(GMomProg.GetId(),"vertex");
	check();

	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 0, 0);
	check();

	glEnableVertexAttribArray(loc);
	check();

	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 );
	check();

	glFinish();
	check();

	glFlush();
	check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void HorizontalReduction(int bind , GfxTexture* texture0,float x0, float y0, float x1, float y1,float textureWidth, float textureHeight, int stride)
{

	
	if (stride == 2)
	{
		glUseProgram(GHRedProg2.GetId());
		check();
		glUniform2f(glGetUniformLocation(GHRedProg2.GetId(),"offset"),x0,y0);
		glUniform2f(glGetUniformLocation(GHRedProg2.GetId(),"scale"),x1-x0,y1-y0);
        	glUniform1i(glGetUniformLocation(GHRedProg2.GetId(),"tex"), 0);
       		glUniform1f(glGetUniformLocation(GHRedProg2.GetId(),"screenWidth"),float(GScreenWidth));
        	glUniform1f(glGetUniformLocation(GHRedProg2.GetId(),"screenHeight"),float(GScreenHeight));
	}
	else
	{
		glUseProgram(GHRedProg.GetId());
		check();
		glUniform2f(glGetUniformLocation(GHRedProg.GetId(),"offset"),x0,y0);
		glUniform2f(glGetUniformLocation(GHRedProg.GetId(),"scale"),x1-x0,y1-y0);
        	glUniform1i(glGetUniformLocation(GHRedProg.GetId(),"tex"), 0);
	        glUniform1f(glGetUniformLocation(GHRedProg.GetId(),"screenWidth"),float(GScreenWidth));
        	glUniform1f(glGetUniformLocation(GHRedProg.GetId(),"screenHeight"),float(GScreenHeight));
        }

	check();

	if (bind == 1){
		glBindFramebuffer(GL_FRAMEBUFFER, GFrameBuffer);
		check();
	}

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);
	check();
       
        glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture0->GetId());
	check();

	GLuint loc = glGetAttribLocation((stride == 2)?GHRedProg2.GetId():GHRedProg.GetId(),"vertex");
	check();

	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 0, 0);
	check();

	glEnableVertexAttribArray(loc);
	check();

	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 );
	check();

	glFinish();
	check();

	glFlush();
	check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void VerticalReduction(int bind , GfxTexture* texture0,float x0, float y0, float x1, float y1,float textureWidth, float texttureHeight)
{

	
	glUseProgram(GVRedProg.GetId());
	check();
	glUniform2f(glGetUniformLocation(GVRedProg.GetId(),"offset"),x0,y0);
	glUniform2f(glGetUniformLocation(GVRedProg.GetId(),"scale"),x1-x0,y1-y0);
       	glUniform1i(glGetUniformLocation(GVRedProg.GetId(),"tex"), 0);
        glUniform1f(glGetUniformLocation(GVRedProg.GetId(),"texWidth"),float(textureWidth));
       	glUniform1f(glGetUniformLocation(GVRedProg.GetId(),"texHeight"),float(texttureHeight));
	check();

	if (bind == 1){
		glBindFramebuffer(GL_FRAMEBUFFER, GFrameBuffer);
		check();
	}

	glBindBuffer(GL_ARRAY_BUFFER, GQuadVertexBuffer);
	check();
       
        glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture0->GetId());
	check();

	GLuint loc = glGetAttribLocation(GVRedProg.GetId(),"vertex");
	check();

	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 0, 0);
	check();

	glEnableVertexAttribArray(loc);
	check();

	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 );
	check();

	glFinish();
	check();

	glFlush();
	check();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

bool GfxTexture::Create(int width, int height, int level, const void* data)
{
	Width = width;
	Height = height;
	glGenTextures(1, &Id);
	check();
	glBindTexture(GL_TEXTURE_2D, Id);
	check();
	glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	check();
	glGenerateMipmap(GL_TEXTURE_2D);
	check();

	//if (level == 0)
	//{
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
	//}
	//else
	//{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_LINEAR_MIPMAP_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
	//}
	
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLfloat)GL_MIRRORED_REPEAT);//CLAMP_TO_EDGE);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLfloat)GL_MIRRORED_REPEAT);//CLAMP_TO_EDGE);
	check();

	glGenerateMipmap(GL_TEXTURE_2D);
	check();
	
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

void GfxTexture::SetPixels(const void* data)
{
	glBindTexture(GL_TEXTURE_2D, Id);
	check();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	check();
	glBindTexture(GL_TEXTURE_2D, 0);
	check();
}
