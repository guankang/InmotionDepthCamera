#include "Camera.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "configfile.h"
//#include <stdio.h>  

#ifndef NAVIPACK_WIN
#include <unistd.h>
#endif

Camera::Camera()
{
	isOpened = 0;
	mFrameBuf = NULL;
	mDepthFrameBuf = NULL;
#ifndef NAVIPACK_WIN
	m_videoIn = (struct vdIn *) calloc(1, sizeof(struct vdIn));
#endif
}

Camera::~Camera()
{
	if (mFrameBuf)
	{
		delete[] mFrameBuf;
		mFrameBuf = NULL;
	}
	if (mDepthFrameBuf)
	{
		delete[] mDepthFrameBuf;
		mDepthFrameBuf = NULL;
	}
	if (D2PTable)
	{
		delete[] D2PTable;
		D2PTable = NULL;
	}
#ifndef NAVIPACK_WIN
	free(m_videoIn);
#endif
}

int Camera::Open(const char * devName, int width, int height)
{
	int ret = -1;
#ifdef NAVIPACK_WIN
	int num = mCamera.GetVideoNum();// DepthCameraInput::listVideos();
	for (int i = 0; i < num; i++)
		printf("  %d) %s\r\n", i, mCamera.GetVideoName(i));
	if (num > 0)
	{
		ret = mCamera.Open(0, true);
		if (ret) {
			mCamera.GetFrameSize(&mFrameWidth, &mFrameHeight);
			if (mFrameWidth != width || mFrameHeight != height)
			{
				return -2;
			}
			isOpened = 1;
			if (mFrameBuf)
			{
				delete[] mFrameBuf;
				mFrameBuf = NULL;
			}
			if (mDepthFrameBuf)
			{
				delete[] mDepthFrameBuf;
				mDepthFrameBuf = NULL;
			}
			mFrameBuf = new uint8_t[mFrameWidth*mFrameHeight * 6];
			mDepthFrameBuf = new uint16_t[mFrameWidth*mFrameHeight];
		}
	}

	ReadCameraParaFromFile();
	CreateD2PTable(mFrameWidth, mFrameHeight, FocalLength_X, FocalLength_Y, PrincipalPoint_X, PrincipalPoint_Y);
#else
	//uvc camera
	ret = mCamera.init_videoIn(m_videoIn,devName, width * 2, height, V4L2_PIX_FMT_YUYV, 1);
	if (ret>=0)
	{
		isOpened = 1;
		mFrameWidth = width;//320
		mFrameHeight = height;//240


		if (mFrameBuf)
		{
			delete[] mFrameBuf;
			mFrameBuf = NULL;
		}
		if (mDepthFrameBuf)
		{
			delete[] mDepthFrameBuf;
			mDepthFrameBuf = NULL;
		}
		mFrameBuf = new uint8_t[mFrameWidth*mFrameHeight * 6];
		mDepthFrameBuf = new uint16_t[mFrameWidth*mFrameHeight];
	}
	else
	{
		isOpened = 0;
	}
#endif
	return ret;
}

int Camera::Close()
{
	int ret = -1;
#ifdef NAVIPACK_WIN
	mCamera.Close();
	isOpened = 0;

	ret = 0;
#else
	ret = mCamera.close_v4l2(m_videoIn);
#endif
	if (mFrameBuf)
	{
		delete[] mFrameBuf;
		mFrameBuf = NULL;
	}
	if (mDepthFrameBuf)
	{
		delete[] mDepthFrameBuf;
		mDepthFrameBuf = NULL;
	}
	return ret;
}

int Camera::ReadCameraParaFromFile()
{
	std::string configfilename = "CameraParams.txt";

	if (configfilename.length() > 0) {
		ConfigFile cfg(configfilename);
		//filename = (std::string) cfg.value("gfs", "filename", filename);
		//outfilename = (std::string) cfg.value("gfs", "outfilename", outfilename);
		FocalLength_X = cfg.value("CameraParams", "FocalLength_X", FocalLength_X);
		FocalLength_Y = cfg.value("CameraParams", "FocalLength_Y", FocalLength_Y);
		PrincipalPoint_X = cfg.value("CameraParams", "PrincipalPoint_X", PrincipalPoint_X);
		PrincipalPoint_Y = cfg.value("CameraParams", "PrincipalPoint_Y", PrincipalPoint_Y);
		RadialDistortion_K1 = cfg.value("CameraParams", "RadialDistortion_K1", RadialDistortion_K1);
		RadialDistortion_K2 = cfg.value("CameraParams", "RadialDistortion_K2", RadialDistortion_K2);
	}

	if (configfilename.length() <= 0) {
		std::cout << "no filename specified" << std::endl;
		return 1;
	}

	return 0;

}

int Camera::ReadDepthCameraFrame(float * anlge, unsigned short * phase_Buffer, unsigned short * amplitude_Buffer, unsigned char * ambient_Buffer, unsigned char * flags_Buffer)
{
	int ret = -1;
#ifdef NAVIPACK_WIN
	if (isOpened)
	{
		ret = mCamera.ReadDepthCameraFrame(anlge, phase_Buffer, amplitude_Buffer, ambient_Buffer, flags_Buffer);
	}
#else
	if (isOpened)
	{
		ret = mCamera.uvcGrab(m_videoIn);
		SplitRawFrame(mFrameWidth, mFrameHeight, m_videoIn->framebuffer, anlge, phase_Buffer, amplitude_Buffer, ambient_Buffer, flags_Buffer);
		PhaseToDepth(phase_Buffer, phase_Buffer);
	}
#endif
	return ret;
}

int Camera::PhaseToDepth(unsigned short * phase, unsigned short * depth)
{
	unsigned short *p = phase;
	unsigned short *d = depth;
	float *table_ptr = D2PTable;
	for (int i = 0; i < mFrameHeight; i++)
	{
		for (int j = 0; i < mFrameWidth; j++)
		{
			*d = *p*7.5 / 3.072*(*table_ptr);
			d++;
			p++;
			table_ptr++;
		}

	}
	return 0;
}

int Camera::ReadXYZFrame(float * xyz_data_Buffer)
{
	int ret = -1;
	float anlge;
	int frameLen = mFrameWidth*mFrameHeight;
	ret = ReadDepthCameraFrame(&anlge, (unsigned short*)mDepthFrameBuf, (unsigned short*)(mFrameBuf + frameLen * 2), (unsigned char*)(mFrameBuf + frameLen * 4), (unsigned char*)(mFrameBuf + frameLen * 5));
	if (ret < 0)
	{
		return 0;
	}
	DepthToPointCloud(mDepthFrameBuf, xyz_data_Buffer, mFrameWidth, mFrameHeight, FocalLength_X, FocalLength_Y, PrincipalPoint_X, PrincipalPoint_Y);
	//for (int idx=0;idx<frameLen;idx++)
	//{
	//	float x = xyz_data_Buffer[idx * 3 + 0];
	//	float y = xyz_data_Buffer[idx * 3 + 1];
	//	float z = xyz_data_Buffer[idx * 3 + 2];
	//	LOGD("X=%f, Y=%f, Z=%f", x, y, z);
	//}
	return ret;
}

int Camera::CreateD2PTable(int w, int h, float w_f, float h_f, float p_x, float p_y)
{
	float k1 = 1 / w_f;
	float k2 = 1 / h_f;
	if (D2PTable)
	{
		delete[] D2PTable;
	}
	D2PTable = new float[w * h];
	float* table_ptr = D2PTable;

	int i = 0;
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			*table_ptr++ = (float)(1.0 / sqrt(1 + k1 * k1 * (p_x - x) * (p_x - x) + k2 * k2 * (p_y - y) * (p_y - y)));
		}
	}
	return 0;

}

int Camera::DepthToPointCloud(unsigned short *phase, float *pc, int w, int h, float w_f, float h_f, float p_x, float p_y)
{
	unsigned short * p_buf = phase;
	float *c_buf = pc;
	float *table_ptr = D2PTable;
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			float z = *p_buf;//z
			*c_buf++ = -(float)(x - p_x) * z / w_f;//x
			*c_buf++ = -(float)(y - p_y) * z / h_f;//y
			*c_buf++ = z;
			p_buf++;
			table_ptr++;
		}
	}
	return 0;
}

#ifndef NAVIPACK_WIN
//linux 鐢?
void Camera::SplitRawFrame(int fw, int fh, unsigned char * source_frame, float * angle, unsigned short * phase_buffer, unsigned short * amplitude_buffer, unsigned char * ambient_buffer, unsigned char * flags_buffer)
{
	int w = fw;
	int h = fh;
	unsigned short * framePtr = (unsigned short *)source_frame;

	unsigned short * p= phase_buffer;
	unsigned short * ampl = amplitude_buffer;
	unsigned char * amb = ambient_buffer;
	unsigned char * f = flags_buffer;

	int angle_t = 0;
	if (h % 10 > 0)  //如果成了证明是3D雷达（3D雷达为了传输每一帧的角度而将每一帧多传了2行来传输角度）
	{
		int a = h;
		h = w;
		w = a;
		if (angle)
		{
			angle_t = (int)*framePtr;
			*angle = (float)angle_t / 10.0f;
		}
		framePtr += (((w % 10) * h) * 2);
		for (int i = 0; i < (w - w % 10); i++)
		{
			for (int j = 0; j < h; j++)
			{
				amplitude_buffer[j * (w - w % 10) + i] = (*framePtr & 0x0FFF);
				ambient_buffer[j * (w - w % 10) + i] = ((*framePtr & 0xF000) >> 12);
				framePtr++;
			}
			for (int j = 0; j < h; j++)
			{
				phase_buffer[j * (w - w % 10) + i] = (*framePtr & 0x0FFF);
				flags_buffer[j * (w - w % 10) + i] = ((*framePtr & 0xC000) >> 12);
				framePtr++;
			}
		}
	}
	else
	{
		*angle = 0;
		for (int i = 0; i < h; i++)
		{
			int j;
			for (j = 0; j < w; j++)
			{
				*ampl++ = (*framePtr & 0x0FFF);
				*amb++ = ((*framePtr & 0xF000) >> 12);
				framePtr++;
			}

			for (; j < w * 2; j++)
			{
				*p++ = (*framePtr & 0x0FFF);
				if (h == 60)
					*f++ = ((*framePtr & 0xC000) >> 12);
				else
					*f++ = ((*framePtr & 0xF000) >> 12);
				framePtr++;
			}
		}
	}
}
#endif
