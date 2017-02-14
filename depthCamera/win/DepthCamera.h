#pragma once
#ifdef NAVIPACK_WIN
#include "Windows.h"

namespace DepthCameraSDK_Cpp {


/// Depth Camera Frame Call Back Def. This callBack function will be called when there is a new frame from depth camera
/// @param      angle Current view angle. This parameter is use in rotation depth camera.
/// @param      phase_buffer The frame of depth distance. Each pixel of phase frame is 16bits which indicate the distance from  object pixel to camera.
/// @param      amplitude_buffer The frame of confidence. Each pixel of amplitude frame is 16bits which indicate the confidence(signal amplitude) of the corresponding depth pixel.
/// @param      ambient_buffer The frame of ambient light. Each pixel of ambient frame is 8bits which indicate the ambient light amplitude of the environment.
/// @param      flags_buffer The frame of flags. Each pixel of flags frame is 8bits which indicate the if the corresponding depth pixel is saturated.
/// @remarks    The length of each frame buffer is the size of a depth camera frame : widht * height. Width & height value can been read by GetFrameSize function 
/// @note       phase_buffer and amplitude_buffer is 16bits, ambient_buffer and flags_buffer is 8Bits
typedef void(__stdcall *OnDepthCameraFrameCallBack)(float angle, unsigned short * phase_buffer, unsigned short * amplitude_buffer, unsigned char * ambient_buffer, unsigned char * flags_buffer);


/// User friendly class for using the SDK DLL 
class DepthCamera
{
public:
	/// Constructor function.
	/// @note Constructor function will load the SDK dll and get the function in the dll. 
	///       All instance of this class will load the dll just one time.
	DepthCamera();

	~DepthCamera();
	
	/// Get the depth camera name by given ID
	/// @param      id The depth camera id, from 0 to n - 1, where n is the value return by GetDeviceNum()
	/// @return     return the pointer of the name buffer.
	static const char * GetVideoName(int id);

	/// Get the number of depth camera connected in the system.
	/// @return     The number of depth camera.
	static int GetVideoNum();

	/// Close depth camera
	/// @return     Success : 1, Failed : 0
	int Close();

	/// Open the depth camera by given ID
	/// @param      id The depth camera id  
	/// @param      openCmd Choose whether to open the CMD. 1: open 0: not open 
	/// @return     Success : 1, Failed : 0
	int Open(int id, int openCmd);

	/// Set the depth camera frame call back function
	/// @param      cb The call back function
	/// @return     Success : 1, Failed : 0
	int SetDepthCameraFrameCallBack(OnDepthCameraFrameCallBack cb);

	/// Check new frame
	/// @return     Has new frame : 1, else : 0
	int IsFrameNew();

	/// Read Depth Data Frame
	/// @param      angle Current view angle. This parameter is use in rotation depth camera.
	/// @param      phase_buffer The frame of depth distance. Each pixel of phase frame is 16bits which indicate the distance from  object pixel to camera.
	/// @param      amplitude_buffer The frame of confidence. Each pixel of amplitude frame is 16bits which indicate the confidence(signal amplitude) of the corresponding depth pixel.
	/// @param      ambient_buffer The frame of ambient light. Each pixel of ambient frame is 8bits which indicate the ambient light amplitude of the environment.
	/// @param      flags_buffer The frame of flags. Each pixel of flags frame is 8bits which indicate the if the corresponding depth pixel is saturated.
	/// @remarks    The length of each frame buffer is the size of a depth camera frame : widht * height. Width & height value can been read by GetFrameSize function 
	/// @note       phase_buffer and amplitude_buffer is 16bits, ambient_buffer and flags_buffer is 8Bits
	/// @return     Success : 1, Failed : 0
	int ReadDepthCameraFrame(float * angle, unsigned short * phase_buffer, unsigned short * amplitude_buffer, unsigned char * ambient_buffer, unsigned char * flags_buffer);

	/// Check if the device had opened 
	/// @return     Opened: 1, No opened: 0
	int IsVideoSetup();

	/// Get the information of the device, include width, height
	/// @param      w the width buffer pointer
	/// @param      h the height buffer pointer
	void GetFrameSize(int *w, int *h);

	/// Get the current frame rate per second
	/// @return    the fps
	int GetCurrentFps();

	/// Get the SDK version
	/// @return the version value
	/// @note the version code is organized by three part: main version(Bits 24 ~31), sub version(Bits 16 ~ 23), build number(Bits 0 ~ 15)
	static unsigned int GetSDKVersion();

	/// Convert 16Bits Buffer to 24Bit RGB Buffer. This is a public static method.
	/// @param      source 16Bits value buffer.
	/// @param      source_maxValue Define the maximum value of source data, used to scale the rgb color.
	/// @param      source_minValue Define the minimum value of source data, used to scale the rgb color.
	/// @param      source_size Source data size.(Unit is 16bit)
	/// @param      rgb_buf RGB data buffer.
	/// @param      rgb_size RGB data buffer size.
	/// @param      color_flag Identify if use gray map or color map
	/// @note       rgb_buf is point to the RGB24 data buffer, and the rgb_size must greater or equal to the source_size * 3.
	static void Bit16ToRGB24(unsigned short* source, short source_maxValue, short source_minValue, int source_size, unsigned char* rgb_buf, int rgb_size, int color_flag);

	/// Convert 8Bits Buffer to 24Bit RGB Buffer. This is a public static method.
	/// @param      source 8Bits value buffer.
	/// @param      source_maxValue Define the maximum value of source data, used to scale the rgb color.
	/// @param      source_minValue Define the minimum value of source data, used to scale the rgb color.
	/// @param      source_size Source data size.(Unit is 8bit)
	/// @param      rgb_buf RGB data buffer.
	/// @param      rgb_size RGB data buffer size.
	/// @param      color_flag Identify if use gray map or color map
	/// @note       rgb_buf is point to the RGB24 data buffer, and the rgb_size must greater or equal to the source_size * 3.
	static void Bit8ToRGB24(unsigned char* source, unsigned char source_maxValue, unsigned char source_minValue, int source_size, unsigned char* rgb_buf, int rgb_size, int color_flag);

	/// Convert Phase to Point Cloud. This is a public static method.
	/// @param      phase The phase buffer where read from ReadDepthCameraFrame or CallBack functions .
	/// @param      pc Point Cloud buffer where to store the output point cloud values.
	/// @param      depth_scale Scale of the Point Cloud.
	/// @note       pc buffer size should >= width * height * 3.
	void PhaseToPointCloud(unsigned short *phase, float* pc, float depth_scale);

	/// Use amplitude and flags to filter the phase signals, the main work includes convolution filtering, smoothing, and delete edge noise.
	/// @param      phase The phase buffer where read from ReadDepthCameraFrame or CallBack functions .
	/// @param      amplitude The amplitude buffer where read from ReadDepthCameraFrame or CallBack functions .
	/// @param      flags The flags buffer where read from ReadDepthCameraFrame or CallBack functions .
	/// @param      DstFrame The output phase buffer
	/// @param      Amp_Thr Amplitude threshold.  Phase whoes amplitude less than the threshold  will be deleted.
	void PhaseDenoise(unsigned short *phase, unsigned short *amplitude, unsigned char *flags, unsigned short* DstFrame, int Amp_Thr);

	/// Check the cmd status
	/// @return   Has busy : 1, else : 0
	int IsCmdBusy();

	/// Check the cmd status
	/// @return   Has opened : 1, else : 0
	int IsCmdStart();

	/// Set the depth camera integration time
	/// @param    value is the integration time, Value range 0 - 66%
	/// @return    Success : 0, Failed : other
	int SetIntegrationTime(unsigned char value);

	/// Set the light of th external
	/// @param    value is the External light, Value range 0 - 255
	/// @return    Success : 0, Failed : other
	int SetExternalLight(unsigned char value);

	/// Set the light of th internal
	/// @param    value is the internal light, Value range 0 - 230 mA
	/// @return    Success : 0, Failed : other
	int SetInternalLight(unsigned char value);

	/// Set the frame rate
	/// @param    value is the frame rate, Value range 0 - 500 FPS
	/// @return   Success : 0, Failed : other
	int SetFrameRate(unsigned short value);

	/// Set the motor speed
	/// @param    value is the motor speed, Value range 0 - 10 rps
	/// @return   Success : 0, Failed : other
	int SetMotorSpeed(unsigned char value);

	/// Set the switch mirror of the X axis
	/// @return     Success : 0, Failed : other
	int SwitchXMirror();

	/// Set the pixel binning of th X axis
	/// @param    rows is the  X axis, Value range 0 - 3
	/// @param    columns is the Y axis,Value range 0 - 3
	/// @return   Success : 0, Failed : other
	int SetPixelBinning(unsigned char rows, unsigned char columns);

	/// Restore factory settings of the system config
	/// @return     Success : 0, Failed : other
	int RestoreFactorySettings();

	/// Save system config
	/// @return     Success : 0, Failed : other
	int SaveConfig();

	/// Get system running status and the other information
	/// @param    read_buffer is save information Buffer.
	/// @param    buffer_size is sizeof(read_buffer).
	/// @param    read_len is read len.
	/// @return     Success : 0, Failed : other
	int GetSystemStatus(unsigned char * read_buffer, int buffer_size, int *read_len);

	/// update the depth camera firmware 	
	/// @param    path is the file path
	/// @return     Success : 0, Failed : other
	int UpdateFirmwareStart(char* path);

	/// stop update firmware 
	/// @return     Success : 0, Failed : other
	void UpdateFirmwareStop();

	/// Get update the depth camera firmware progress
	/// @return    update progress.
	int GetUpdateProgress();

	/// Check is being updated
	/// @return   updating : 1, else : 0
	int IsUpdating();

private:
	int mIndex;
	int Init();
};


}

#endif