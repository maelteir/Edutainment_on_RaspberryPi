// OpenCVTest.cpp : Defines the entry point for the console application.
//

#ifdef WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <strings.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <iostream>
#include <ctime>
#include <fstream>
#include <sys/time.h>

//new includes
#include <sys/stat.h>
#include <fcntl.h>
#include "camera.h"
#include "graphics.h"
#include <math.h>

using namespace std;

#define MAIN_TEXTURE_WIDTH 512
#define MAIN_TEXTURE_HEIGHT 512
#define check() assert(glGetError() == 0)

void PrintFrameBuffer();
int GetPosition();
char tmpbuff[MAIN_TEXTURE_WIDTH*MAIN_TEXTURE_HEIGHT*4];

struct timeval timeStart, timeEnd;
void timerStart()
{
	gettimeofday(&timeStart, NULL);
}

void timerEnd()
{
	gettimeofday(&timeEnd, NULL);
}

double elapsedTime()
{
	double deltaTime;
	deltaTime = (timeEnd.tv_sec-timeStart.tv_sec)*1000.0+ (timeEnd.tv_usec-timeStart.tv_usec)/1000.0;
	return deltaTime;
}

//entry point
int main(int argc, const char **argv)
{
	timerStart();
	#ifdef __WIN32__
	WORD versionWanted = MAKEWORD(1, 1);
	WSADATA wsaData;
	WSAStartup(versionWanted, &wsaData);
	#endif
/*
	ofstream timeFile; //sent times
	timeFile.open("Tracker-pipe2-log.txt");
	static const char *outFile = "java -jar ./MFGame-pipe-2.jar";

	FILE *fp_out = popen(outFile,"w");
*/

        char posArray[2];//create posArray

	int sock_descriptor;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	sock_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_descriptor < 0)
		printf("Failed creating socket\n");

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	server = gethostbyname("127.0.0.1");

	if (server == NULL) {
		printf("Failed finding server name\n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	memcpy((char *) &(serv_addr.sin_addr.s_addr), (char *) (server->h_addr), server->h_length);
	serv_addr.sin_port = htons(65533);
	if (connect(sock_descriptor, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{
		printf("Failed to connect to server\n");
		return -1;
	}

	else
		printf("Connected successfully\n");

	//timerStart();
	//should the camera convert frame data from yuv to argb automatically?
	bool do_argb_conversion = true;

	//init graphics and the camera
	InitGraphics();
	CCamera* cam = StartCamera(MAIN_TEXTURE_WIDTH, MAIN_TEXTURE_HEIGHT,30,1,do_argb_conversion);

	//create 7 texture buffers
	int num_textures = 7;
	GfxTexture textures[num_textures];
	
	for(int texidx = 0; texidx <5; texidx++)
		textures[texidx].Create(MAIN_TEXTURE_WIDTH, MAIN_TEXTURE_HEIGHT,0); 
	
	for(int texidx = 5; texidx <7; texidx++)
		textures[texidx].Create(MAIN_TEXTURE_WIDTH, MAIN_TEXTURE_HEIGHT,0); 
	  
        
	float aspect_ratio, screen_aspect_ratio ;
	aspect_ratio = float(MAIN_TEXTURE_WIDTH)/float(MAIN_TEXTURE_HEIGHT);
	screen_aspect_ratio = 1920.f/1080.f;
		
	//timerEnd();
       // printf("Initialization takes %.3f ms \n",elapsedTime());

	//Get initial camera input into texture[1] after converting to Gray
	//-----------------------------------------------------------------
	//timerStart();
	for(int i = 0; i < 10; i++)
	{
		//lock the chosen frame buffer, and copy it directly into the corresponding open gl texture
		const void* frame_data; int frame_sz;
		if(cam->BeginReadFrame(0,frame_data,frame_sz))
		{
			if(do_argb_conversion)
			{
				//if doing argb conversion the frame data will be exactly the right size so just set directly
				textures[0].SetPixels(frame_data);
			}
			else
			{
				//if not converting argb the data will be the wrong size and look weird, put copy it in
				//via a temporary buffer just so we can observe something happening!
				memcpy(tmpbuff,frame_data,frame_sz);
				textures[0].SetPixels(tmpbuff);
			}
			cam->EndReadFrame(0);
		}

		//begin frame, draw the texture then end frame 
		BeginFrame(1,&textures[1],0);
		RGBToGray(1,&textures[0],-aspect_ratio/screen_aspect_ratio,-1.f,aspect_ratio/screen_aspect_ratio,1.f);
		EndFrame();
	}
	//timerEnd();
	//printf("Getting RGB of First Frame takes %.3f ms \n",elapsedTime());

	//Loop reading frames and getting the difference
	//-----------------------------------------------------------------
	printf("Running frame loop\n");
	while(1)
	{
		//timerStart();
		//lock the chosen frame buffer, and copy it directly into the corresponding open gl texture
		const void* frame_data; int frame_sz;
		if(cam->BeginReadFrame(0,frame_data,frame_sz))
		{
			//if doing argb conversion the frame data will be exactly the right size so just set directly
			textures[0].SetPixels(frame_data);
			cam->EndReadFrame(0);
		}
		//timerEnd();
		//printf("Getting camera frame takes %.3f ms \n",elapsedTime());
	
		//timerStart();
		BeginFrame(1,&textures[2],0);
		RGBToGray(1,&textures[0],-aspect_ratio/screen_aspect_ratio,-1.f,aspect_ratio/screen_aspect_ratio,1.f);
		//if (i == 99) PrintFrameBuffer();
		EndFrame();
		glFinish();
		check();
		//timerEnd();
		//printf("Getting RGB of Current Frame takes %.3f ms \n",elapsedTime());

		//timerStart();
		BeginFrame(1,&textures[3],0);
		Diff(1,&textures[1],&textures[2],-aspect_ratio/screen_aspect_ratio,-1.f,aspect_ratio/screen_aspect_ratio,1.f);
		//if (i == 99) PrintFrameBuffer();
		EndFrame();
		glFinish();
		check();
		//timerEnd();
		//printf("Getting Difference takes %.3f ms \n",elapsedTime());

		//timerStart();
		BeginFrame(1,&textures[4],0);
		Blur(1,&textures[3],-aspect_ratio/screen_aspect_ratio,-1.f,aspect_ratio/screen_aspect_ratio,1.f);
		//if (i == 99) PrintFrameBuffer();
		EndFrame();
		glFinish();
		check();
		//timerEnd();
		//printf("Getting Blurr takes %.3f ms \n",elapsedTime());

		//timerStart();
		BeginFrame(1,&textures[3],0);
		Threshold(1,&textures[4],-aspect_ratio/screen_aspect_ratio,-1.f,aspect_ratio/screen_aspect_ratio,1.f);
		//if (i == 99) PrintFrameBuffer();
		EndFrame();
		glFinish();
		check();
		//timerEnd();
		//printf("Getting Threshold takes %.3f ms \n",elapsedTime());

		//timerStart();
		BeginFrame(1,&textures[5],0);
		Moments(1,&textures[3],-aspect_ratio/screen_aspect_ratio,-1.f,aspect_ratio/screen_aspect_ratio,1.f);
		//if ( i == 99)	PrintFrameBuffer();
		glFinish();
		check();
		//timerEnd();
		//printf("Getting Moments takes %.3f ms \n",elapsedTime());
		EndFrame();

		//timerStart();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,textures[5].GetId());
		check();
		glGenerateMipmap(GL_TEXTURE_2D);
		check();
		glFinish();
		check();
		//timerEnd();
		//printf("Getting Mipmaps takes %.3f ms \n",elapsedTime());

		//timerStart();
		BeginFrame(0,&textures[5],1);
		HorizontalReduction(0,&textures[5],-aspect_ratio/screen_aspect_ratio,-1.f,aspect_ratio/screen_aspect_ratio,1.f,MAIN_TEXTURE_WIDTH*2,MAIN_TEXTURE_HEIGHT*2,16);
		glFinish();
		check();
		//timerEnd();
		//PrintFrameBuffer();
		EndFrame();		
		//printf("Getting Reduction takes %.3f ms \n",elapsedTime());
	
		int pos = GetPosition();

		sprintf(posArray, "%d ", pos); //convert from int to char[]
	        posArray[1]='\n';
    	       // fprintf(fp_out,"%c%c",posArray[0], posArray[1]);
               // fflush(fp_out);
		write(sock_descriptor, posArray, strlen(posArray));
	}

	StopCamera();
	//close output fifo, this leaves the fifo but closes the file pointer
	//fclose(fp_out);
}

int GetPosition()
{
		// Try reading the data written to the framebuffer using glreadpixels()
		//printf("\nPrint Frame Buffer\n");
		//timerStart();
                int width= 64*2; 
		int height= 64*2; 
                GLubyte *data = (GLubyte*)malloc(4*width*height* sizeof(GLubyte));
                
		//timerEnd();
		//printf("Initialization takes %.3f ms \n",elapsedTime());
		
		//timerStart();
                glReadPixels(0,0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glFinish();
		//timerEnd();
		//printf("Reading data from GPU takes %.3f ms \n",elapsedTime());

		//timerStart();
		//The actual data in the render buffer contains height rows and width columns each element is cl_ubyte
		int M01, M10, M00;
		M01=0;
		M10=0;
		M00=0;
		int max = 0;
		for (int j=0; j < height; j++) //height  
		{
			for (int i=0; i< width; i+=4) //width 
			{		
				
				//printf("(%i %i %i %i) ",data[j*width+i*4],data[j*width+i*4+1],data[j*width+i*4+2],data[j*width+i*4+3]);
				

				int tmp = data[j*width+i*4];
				if (tmp >= max)
				{
					max = tmp;
					M01= data[j*width+i*4+2]; 
					M10= data[j*width+i*4+1]; 
				}
				
			}
			//printf("\n------------------------\n");	

		}
		int posX = M10; 
		int posY = M01; 
		int pos = 0;
		int x = floor(posX / 80.0);
		int y = floor(posY / 80.0);
		pos = x + 3 * y;
		pos++;

		printf("\n--------\nPostion %i M01 %i M10 %i M00 %i posX %i posY %i  \n--------\n",pos, M01,M10,M00, posX, posY);
		if (data) free(data);
		//timerEnd();
		//printf("Moments calculations takes %.3f ms \n",elapsedTime());
		return pos;

}

void PrintFrameBuffer()
{
		// Try reading the data written to the framebuffer using glreadpixels()
		//printf("\nPrint Frame Buffer\n");
		timerStart();
                int width=1; //1920*2; //screenWidth 1024 1920
		int height=1; //1080*2; //screenHeight 768 1080
                GLubyte *data = (GLubyte*)malloc(4*width*height* sizeof(GLubyte));
                
		//for (int i=0; i< 4*width*height; i++)
		//	data[i]=0;
		timerEnd();
		printf("Initialization takes %.3f ms \n",elapsedTime());
			//printf("%i ",data[i]);	
		//printf("\n");
 	
		timerStart();
                glReadPixels(0,0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glFinish();
		timerEnd();
		printf("Reading data from GPU takes %.3f ms \n",elapsedTime());

		timerStart();
		//The actual data in the render buffer contains height rows and width columns each element is cl_ubyte
		int M01, M10, M00;
		M01=0;
		M10=0;
		M00=0;
		for (int j=0; j <height; j++) //height  
		{
			for (int i=0; i< width; i+=4) 
			{		
				if ((i ==0 )  && (j==0)) printf("(%i %i %i %i) ",data[j*width+i*4],data[j*width+i*4+1],data[j*width+i*4+2],data[j*width+i*4+3]);
				//printf("(%i) ",data[j*width+i*4]);

				int tmp = data[j*width+i*4];
				M01+= data[j*width+i*4+2]; // ( tmp == 255)? data[j*width+i*4+2]:0;
				M10+= data[j*width+i*4+1]; //( tmp == 255)? data[j*width+i*4+1]:0;
				//M00+= ( tmp == 255)? 255 : 0;
			}
			//printf("\n");	

		}
		//M10*=255;
		//M01*=255;
		int posX = M10; //( M00 != 0) ? (int) (M10 / M00) : 0;
		int posY = M01; //( M00 != 0) ? (int) (M01 / M00) : 0;
		int pos = 0;
		int x = floor(posX / 50.0);
		int y = floor(posY / 75.0);
		pos = x + 3 * y;
		pos++;

		printf("Postion %i M01 %i M10 %i M00 %i posX %i posY %i  \n",pos, M01,M10,M00, posX, posY);
		if (data) free(data);
		timerEnd();
		printf("Moments calculations takes %.3f ms \n",elapsedTime());

}
/*
void PrintFrameBuffer()
{
		// Try reading the data written to the framebuffer using glreadpixels()
		//printf("\nPrint Frame Buffer\n");

                int width = 1;
		int height = 2;
                GLubyte *data = (GLubyte*)malloc(4*width*height* sizeof(GLubyte));
                
		for (int i=0; i< 4*width*height; i++)
			data[i]=0;
			//printf("%i ",data[i]);	
		//printf("\n"); 	

                glReadPixels(0,0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glFinish();

		//The actual data in the render buffer contains height rows and width columns each element is cl_ubyte
		int M01, M10, M00;
		M01=0;
		M10=0;
		M00=0;
		for (int j=0; j <height; j++)  
		{
			for (int i=0; i< width; i+=4) 
			{		
				printf("(%i %i %i %i) ",data[j*width+i*4],data[j*width+i*4+1],data[j*width+i*4+2],data[j*width+i*4+3]);
				printf("(%i) ",data[j*width+i*4]);

				int tmp = data[j*width+i*4];
				M01+= ( tmp == 255)? data[j*width+i*4+2]:0;
				M10+= ( tmp == 255)? data[j*width+i*4+1]:0;
				M00+= ( tmp == 255)? 255 : 0;
			}
			printf("\n");	

		}
		M10*=255;
		M01*=255;
		int posX = ( M00 != 0) ? (int) (M10 / M00) : 0;
		int posY = ( M00 != 0) ? (int) (M01 / M00) : 0;
		int pos = 0;
		int x = floor(posX / 50.0);
		int y = floor(posY / 50.0);
		pos = x + 3 * y;
		pos++;

		printf("Postion %i M01 %i M10 %i M00 %i posX %i posY %i  \n",pos, M01,M10,M00, posX, posY);
		if (data) free(data);
}
*/
