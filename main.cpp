#include "depthCamera/Camera.h"
#include <stdio.h>

#ifndef NAVIPACK_WIN
#include <unistd.h>
#include <string.h>
#define Sleep(n) usleep(n*1000)
#endif
//NAVIPACK_WIN 指的是使用windows平台  否则为linux平台

void PrintBuf(const void *pPos, unsigned int MaxLength, const char *name)
{

	if (name && strlen(name) > 0)
		printf("%s", name);

	unsigned char cTmp;
	unsigned char *pTmp = (unsigned char*)pPos;
	unsigned int i;
	for (i = 0; i < MaxLength; i++)
	{
		if (i % 16 == 0)
			printf("\n");

		cTmp = *pTmp;
		printf("%02x ", cTmp);
		pTmp++;
	}
	printf("\n");
}


int main()
{
	int width = 320;
	int height = 240;
	Camera camera;
#ifdef NAVIPACK_WIN
	int ret = camera.Open(NULL, width, height);
#else
	//linux
	int ret = camera.Open("/dev/video0", width, height);
#endif
	if (ret < 0)
	{
		printf("open failed!\n");
		return -1;
	}
	float angle = 0;
	unsigned short* phase_Buffer = new unsigned short[width * height];
	unsigned short* amplitude_Buffer = new unsigned short[width * height];
	unsigned char* ambient_Buffer = new unsigned char[width * height];
	unsigned char* flags_Buffer = new unsigned char[width * height];
	while (1)
	{
		int ret = camera.ReadDepthCameraFrame(&angle, phase_Buffer, amplitude_Buffer, ambient_Buffer, flags_Buffer);
		Sleep(30);
		PrintBuf(phase_Buffer,64,"phase_Buffer:");
	}

	camera.Close();
	delete[] phase_Buffer;
	delete[] amplitude_Buffer;
	delete[] ambient_Buffer;
	delete[] flags_Buffer;

	return 0;
}