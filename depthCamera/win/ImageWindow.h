#pragma once
#ifdef NAVIPACK_WIN

#include "windows.h"
#include "tchar.h"

class ImageWindow
{
public:
	ImageWindow();
	~ImageWindow();

	BOOL Create(TCHAR * name, BOOL auto_stretch = TRUE);
	BOOL Draw(UINT8 *img, int w, int h);
	void Close();

	void SetClientSize(int w, int h);

	static void FillBitmapInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin);
	static BOOL GetBitmapData(HDC hdc, SIZE* size, int* channels, void** data);

	int WaitKey(int delay);
	int ProcessWindowsEvents(int timeout);

private:
	BOOL InitInstance();
	void RegisterWindowClass();
	
	HWND mPaintWnd;
	HDC mHdc;
	HGDIOBJ mImage;
	TCHAR mWindowName[128];
	HINSTANCE mHInstance;

	int mWidth;
	int mHeight;

	BOOL mAutoStretchFlag;
	int mLastKey;

	static void OnPaint(ImageWindow *iw_ptr);
	static BOOL mIsRegisted;
	static const TCHAR * mPaintWindowClassName;
	static LRESULT CALLBACK PaintWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
#endif

