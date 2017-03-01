#pragma once

#include <inttypes.h>
#ifdef NAVIPACK_WIN
#include "win/DepthCamera.h"
using namespace DepthCameraSDK_Cpp;
#else
#include "linux/v4l2uvc.h"
#endif

#include <iostream>

class Camera
{
public:
	Camera();
	~Camera();

	int Open(const char* devName,int width,int height);
	int Close();
	int ReadDepthCameraFrame(float * anlge, unsigned short * phase_Buffer, unsigned short *  amplitude_Buffer, unsigned char * ambient_Buffer, unsigned char * flags_Buffer);
	int ReadXYZFrame(float * xyz_data_Buffer);
	int PhaseToDepth(unsigned short * phase, unsigned short * depth);


	int DepthToPointCloud(unsigned short *phase, float *pc, int w, int h, float w_f, float h_f, float p_x, float p_y);

	int CreateD2PTable(int w, int h, float w_f, float h_f, float p_x, float p_y);

	int ReadCameraParaFromFile();

private:
	uint8_t isOpened;
	uint8_t* mFrameBuf;
	uint16_t* mDepthFrameBuf;
	int32_t mFrameWidth;
	int32_t mFrameHeight;

	float FocalLength_X;
	float FocalLength_Y;
	float PrincipalPoint_X;
	float PrincipalPoint_Y;
	float RadialDistortion_K1;
	float RadialDistortion_K2;

	float* D2PTable;

#ifdef NAVIPACK_WIN
	DepthCamera mCamera;
#else
	struct vdIn *m_videoIn;
	V4L2Uvc mCamera;
	void SplitRawFrame(int fw, int fh, unsigned char * source_frame, float * angle, unsigned short * phase_buffer, unsigned short * amplitude_buffer, unsigned char * ambient_buffer, unsigned char * flags_buffer);
#endif
	
};