#include "DepthCamera.h"
#ifdef NAVIPACK_WIN
#include "tchar.h"

#define DLLNAME _T("DepthCameraSDK.dll")

using namespace DepthCameraSDK_Cpp;

typedef unsigned int(WINAPI *GetSDKVersionFuncDef)();
typedef const char *(WINAPI *GetDeviceNameFuncDef)(int);
typedef int(WINAPI *GetDeviceNumFuncDef)();
typedef int(WINAPI *IntIntFuncDef)(int);
typedef int(WINAPI *IntIntOpenFuncDef)(int,int);
typedef int(WINAPI *SetFrameCallBackFuncDef)(int, OnDepthCameraFrameCallBack);
typedef int(WINAPI *ReadFrameFuncDef)(int, float * , unsigned short * , unsigned short * , unsigned char * , unsigned char * );
typedef void(WINAPI *GetFrameSizeFuncDef)(int, int*, int*);
typedef int(WINAPI *GetCurrentFpsFuncDef)(int);
typedef void(WINAPI *Bit16ToRGB24FuncDef)(unsigned short *source, unsigned short source_maxValue, unsigned short source_minValue, int source_size, unsigned char *rgb_buf, int rgb_size, char color_flag);
typedef void(WINAPI *Bit8ToRGB24FuncDef)(unsigned char *source, unsigned char source_maxValue, unsigned char source_minValue, int source_size, unsigned char *rgb_buf, int rgb_size, char color_flag);
typedef int(WINAPI *PhaseToPointCloudDef)(int id,unsigned short *phase, float* pc, float depth_scale);
typedef int(WINAPI *PhaseDenoiseDef)(unsigned short *phase, unsigned short *amplitude, unsigned char *flags, unsigned short* DstFrame, int Amp_Thr);
typedef int(WINAPI *SetCameraFuncDef0)();
typedef int(WINAPI *SetCameraFuncDef1)(unsigned char);
typedef int(WINAPI *SetCameraFuncDef2)(unsigned char, unsigned char);
typedef int(WINAPI *SetCameraFuncDef3)(unsigned short);
typedef int(WINAPI *GetSystemStatusFuncDef)(unsigned char *, int, int*);
typedef int(WINAPI *UpdateFirmwareStartFuncDef)(int,char *);
typedef void(WINAPI *UpdateFirmwareStopFuncDef)(int);



static HINSTANCE mDllInst = NULL;
static GetSDKVersionFuncDef GetSDKVersionFunc = NULL;
static GetDeviceNumFuncDef GetVideoNumFunc = NULL;
static GetDeviceNameFuncDef GetVideoNameFunc = NULL;
static IntIntFuncDef CloseFunc = NULL;
static IntIntOpenFuncDef OpenByIdFunc = NULL;
static SetFrameCallBackFuncDef SetDepthCameraFrameCallBackFunc = NULL;
static IntIntFuncDef IsFrameNewFunc = NULL;
static ReadFrameFuncDef ReadDepthCameraFrameFunc = NULL;
static IntIntFuncDef IsVideoSetupFunc = NULL;
static GetFrameSizeFuncDef GetFrameSizeFunc = NULL;
static GetCurrentFpsFuncDef GetCurrentFpsFunc = NULL;
static Bit16ToRGB24FuncDef Bit16ToRGB24Func = NULL;
static Bit8ToRGB24FuncDef Bit8ToRGB24Func = NULL;
static PhaseToPointCloudDef PhaseToPointCloudFunc = NULL;
static PhaseDenoiseDef PhaseDenoiseFunc = NULL;

static SetCameraFuncDef0 SaveModifyConfigFunc = NULL;
static SetCameraFuncDef0 ResetCameraSettingsFunc = NULL;
static SetCameraFuncDef0 SwitchImageMirrorFunc = NULL;
static SetCameraFuncDef2 SetPixelBinningFunc = NULL;
static SetCameraFuncDef3 SetCurrentFpsFunc = NULL;
static SetCameraFuncDef1 SetInLightLuminanceFunc = NULL;
static SetCameraFuncDef1 SetExLightLuminanceFunc = NULL;
static SetCameraFuncDef1 SetIntegralTimeFunc = NULL;
static SetCameraFuncDef1 SetMotorSpeedFunc = NULL;
static GetSystemStatusFuncDef GetSystemStatusFunc = NULL;
static GetDeviceNumFuncDef IsCmdStartFunc = NULL;
static GetDeviceNumFuncDef IsCmdBusyFunc = NULL;
static GetDeviceNumFuncDef IsUpdatingFunc = NULL;
static GetDeviceNumFuncDef GetUpdateProgressFunc = NULL;
static UpdateFirmwareStopFuncDef UpdateFirmwareStopFunc = NULL;
static UpdateFirmwareStartFuncDef UpdateFirmwareStartFunc = NULL;

DepthCamera::DepthCamera()
{
	Init();
	mIndex = -1;
}

DepthCameraSDK_Cpp::DepthCamera::~DepthCamera()
{
	Close();
}

int DepthCamera::Init()
{
	if (mDllInst == NULL) {
		mDllInst = LoadLibrary(DLLNAME);
		if (mDllInst){
			if (!(GetSDKVersionFunc = (GetSDKVersionFuncDef)GetProcAddress(mDllInst, "GetSDKVersion"))) return 0;
			if (!(GetVideoNumFunc = (GetDeviceNumFuncDef)GetProcAddress(mDllInst, "GetVideoNum"))) return 0;
			if (!(GetVideoNameFunc = (GetDeviceNameFuncDef)GetProcAddress(mDllInst, "GetVideoName"))) return 0;
			if (!(CloseFunc = (IntIntFuncDef)GetProcAddress(mDllInst, "Close"))) return 0;
			if (!(OpenByIdFunc = (IntIntOpenFuncDef)GetProcAddress(mDllInst, "OpenById"))) return 0;
			if (!(SetDepthCameraFrameCallBackFunc = (SetFrameCallBackFuncDef)GetProcAddress(mDllInst, "SetDepthCameraFrameCallBack"))) return 0;
			if (!(IsFrameNewFunc = (IntIntFuncDef)GetProcAddress(mDllInst, "IsFrameNew"))) return 0;
			if (!(ReadDepthCameraFrameFunc = (ReadFrameFuncDef)GetProcAddress(mDllInst, "ReadDepthCameraFrame"))) return 0;
			if (!(IsVideoSetupFunc = (IntIntFuncDef)GetProcAddress(mDllInst, "IsVideoSetup"))) return 0;
			if (!(GetFrameSizeFunc = (GetFrameSizeFuncDef)GetProcAddress(mDllInst, "GetFrameSize"))) return 0;
			if (!(GetCurrentFpsFunc = (GetCurrentFpsFuncDef)GetProcAddress(mDllInst, "GetCurrentFps"))) return 0;
			if (!(Bit16ToRGB24Func = (Bit16ToRGB24FuncDef)GetProcAddress(mDllInst, "Bit16ToRGB24"))) return 0;
			if (!(Bit8ToRGB24Func = (Bit8ToRGB24FuncDef)GetProcAddress(mDllInst, "Bit8ToRGB24"))) return 0;
			if (!(PhaseToPointCloudFunc = (PhaseToPointCloudDef)GetProcAddress(mDllInst, "DepthToPointCloud"))) return 0;
			if (!(PhaseDenoiseFunc = (PhaseDenoiseDef)GetProcAddress(mDllInst, "PhaseDenoise"))) return 0;

			if (!(SaveModifyConfigFunc = (SetCameraFuncDef0)GetProcAddress(mDllInst, "SaveConfig"))) return 0;
			if (!(ResetCameraSettingsFunc = (SetCameraFuncDef0)GetProcAddress(mDllInst, "RestoreFactorySettings"))) return 0;
			if (!(SwitchImageMirrorFunc = (SetCameraFuncDef0)GetProcAddress(mDllInst, "SwitchMirror"))) return 0;
			if (!(SetPixelBinningFunc = (SetCameraFuncDef2)GetProcAddress(mDllInst, "SetBinning"))) return 0;
			if (!(SetCurrentFpsFunc = (SetCameraFuncDef3)GetProcAddress(mDllInst, "SetFrameRate"))) return 0;
			if (!(SetExLightLuminanceFunc = (SetCameraFuncDef1)GetProcAddress(mDllInst, "SetExLight"))) return 0; 
		    if (!(SetInLightLuminanceFunc = (SetCameraFuncDef1)GetProcAddress(mDllInst, "SetInLight"))) return 0;
			if (!(SetIntegralTimeFunc = (SetCameraFuncDef1)GetProcAddress(mDllInst, "SetIntegrationTime"))) return 0;
			if (!(GetSystemStatusFunc = (GetSystemStatusFuncDef)GetProcAddress(mDllInst, "GetSystemStatus"))) return 0;
			if (!(IsCmdStartFunc = (GetDeviceNumFuncDef)GetProcAddress(mDllInst, "IsCmdStart"))) return 0;
			if (!(IsCmdBusyFunc = (GetDeviceNumFuncDef)GetProcAddress(mDllInst, "IsCmdBusy"))) return 0;
			if (!(IsUpdatingFunc = (GetDeviceNumFuncDef)GetProcAddress(mDllInst, "IsUpdating"))) return 0;
			if (!(GetUpdateProgressFunc = (GetDeviceNumFuncDef)GetProcAddress(mDllInst, "GetUpdateProgress"))) return 0;
			if (!(UpdateFirmwareStopFunc = (UpdateFirmwareStopFuncDef)GetProcAddress(mDllInst, "UpdateFirmwareStop"))) return 0;
			if (!(UpdateFirmwareStartFunc = (UpdateFirmwareStartFuncDef)GetProcAddress(mDllInst, "UpdateFirmwareStart"))) return 0;
			if (!(SetMotorSpeedFunc = (SetCameraFuncDef1)GetProcAddress(mDllInst, "SetMotorSpeed"))) return 0;

		

		}
		else
			return 0;
	}

	return 1;
}

const char * DepthCamera::GetVideoName(int id)
{
	if(GetVideoNameFunc)
		return GetVideoNameFunc(id);
	return NULL;
}

int DepthCamera::GetVideoNum()
{
	if(GetVideoNumFunc)
		return GetVideoNumFunc();
	return 0;
}

int DepthCamera::Close()
{
	if (CloseFunc && (mIndex >= 0))
	{
		int ret = CloseFunc(mIndex);
		mIndex = -1;
		return ret;
	}
	return 0;
}

int DepthCamera::Open(int id,int openCmd)
{
	int ret = 0;
	if (OpenByIdFunc && (ret = OpenByIdFunc(id, openCmd)))
		mIndex = id;
	return ret;
}

int DepthCamera::SetDepthCameraFrameCallBack(OnDepthCameraFrameCallBack cb)
{
	if (SetDepthCameraFrameCallBackFunc && (mIndex >= 0))
		return SetDepthCameraFrameCallBackFunc(mIndex, cb);
	return 0;
}

int DepthCamera::IsFrameNew()
{
	if (IsFrameNewFunc && (mIndex >= 0))
		return IsFrameNewFunc(mIndex);
	return 0;
}

int DepthCamera::ReadDepthCameraFrame(float * angle, unsigned short * phase_buffer, unsigned short * amplitude_buffer, unsigned char * ambient_buffer, unsigned char * flags_buffer)
{
	if (ReadDepthCameraFrameFunc && (mIndex >= 0))
		return ReadDepthCameraFrameFunc(mIndex,  angle,  phase_buffer,  amplitude_buffer,  ambient_buffer,  flags_buffer);
	return 0;
}

int DepthCamera::IsVideoSetup()
{
	if (IsVideoSetupFunc && (mIndex >= 0))
		return IsVideoSetupFunc(mIndex);
	return 0;
}

void DepthCamera::GetFrameSize(int * w, int * h)
{
	if (GetFrameSizeFunc && (mIndex >= 0))
		GetFrameSizeFunc(mIndex, w, h);
}

int DepthCamera::GetCurrentFps()
{
	if (GetCurrentFpsFunc && (mIndex >= 0))
		return GetCurrentFpsFunc(mIndex);
	return 0;
}

void DepthCamera::Bit16ToRGB24(unsigned short* source, short source_maxValue, short source_minValue, int source_size, unsigned char* rgb_buf, int rgb_size, int color_flag)
{
	if (Bit16ToRGB24Func)
		Bit16ToRGB24Func(source, source_maxValue, source_minValue, source_size, rgb_buf, rgb_size, color_flag);
}

void DepthCamera::Bit8ToRGB24(unsigned char* source, unsigned char source_maxValue, unsigned char source_minValue, int source_size, unsigned char* rgb_buf, int rgb_size, int color_flag)
{
	if (Bit8ToRGB24Func)
		Bit8ToRGB24Func(source, source_maxValue, source_minValue, source_size, rgb_buf, rgb_size, color_flag);
}

void DepthCamera::PhaseToPointCloud(unsigned short *phase, float* pc, float depth_scale)
{
	if (PhaseToPointCloudFunc && (mIndex >= 0))
		PhaseToPointCloudFunc(mIndex, phase, pc, depth_scale);

}

void DepthCameraSDK_Cpp::DepthCamera::PhaseDenoise(unsigned short * phase, unsigned short * amplitude, unsigned char * flags, unsigned short * DstFrame, int Amp_Thr)
{
	if (PhaseDenoiseFunc && (mIndex >= 0))
		PhaseDenoiseFunc(phase, amplitude, flags, DstFrame, Amp_Thr);
}

unsigned int DepthCamera::GetSDKVersion()
{
	if (GetSDKVersionFunc)
		return GetSDKVersionFunc();
	return 0;
}


int DepthCamera::IsCmdStart()
{
	if (IsCmdStartFunc && (mIndex >= 0))
		return IsCmdStartFunc();
	return 0;
}


int DepthCamera::IsCmdBusy()
{
	if (IsCmdBusyFunc && (mIndex >= 0))
		return IsCmdBusy()>0;
	return 0;
}


int DepthCamera::GetSystemStatus(unsigned char* read_buffer, int buffer_size, int *read_len)
{
	if (GetSystemStatusFunc && (mIndex >= 0))
		return GetSystemStatusFunc(read_buffer, buffer_size, read_len);
	return -1;
}


int DepthCamera::SetIntegrationTime(unsigned char value)
{
	if (SetIntegralTimeFunc && (mIndex >= 0))
		return SetIntegralTimeFunc(value);
	return -1;
}

int DepthCamera::SetExternalLight(unsigned char value)
{
	if (SetExLightLuminanceFunc && (mIndex >= 0))
		return SetExLightLuminanceFunc(value);
	return -1;
}


int DepthCamera::SetInternalLight(unsigned char value)
{
	if (SetInLightLuminanceFunc && (mIndex >= 0))
		return SetInLightLuminanceFunc(value);
	return -1;
}

int DepthCamera::SetFrameRate(unsigned short value)
{
	if (SetCurrentFpsFunc && (mIndex >= 0))
		return SetCurrentFpsFunc(value);
	return -1;
}

int DepthCamera::SwitchXMirror()
{
	if (SwitchImageMirrorFunc && (mIndex >= 0))
		return SwitchImageMirrorFunc();
	return -1;
}


int DepthCamera::SetPixelBinning(unsigned char rows, unsigned char columns)
{
	if (SetPixelBinningFunc && (mIndex >= 0))
		return SetPixelBinningFunc(rows, columns);
	return -1;
}

int DepthCamera::RestoreFactorySettings()
{
	if (ResetCameraSettingsFunc && (mIndex >= 0))
		return ResetCameraSettingsFunc();
	return -1;
}

int DepthCamera::SaveConfig()
{
	if (SaveModifyConfigFunc && (mIndex >= 0))
		return SaveModifyConfigFunc();
	return -1;
}

int DepthCamera::SetMotorSpeed(unsigned char value)
{
	if (SetMotorSpeedFunc && (mIndex >= 0))
		return SetMotorSpeedFunc(value);
	return -1;
}


int DepthCamera::UpdateFirmwareStart(char* path)
{
	if (UpdateFirmwareStartFunc && (mIndex >= 0))
		return UpdateFirmwareStartFunc(mIndex, path);
	return -1;
}

void DepthCamera::UpdateFirmwareStop()
{
	if (UpdateFirmwareStopFunc && (mIndex >= 0))
	  UpdateFirmwareStopFunc(mIndex);
}

int DepthCamera::GetUpdateProgress()
{
	if (GetUpdateProgressFunc && (mIndex >= 0))
		return GetUpdateProgressFunc();
	return -1;
}

int DepthCamera::IsUpdating()
{
	if (IsUpdatingFunc && (mIndex >= 0))
		return IsUpdatingFunc();
	return -1;
}
#endif