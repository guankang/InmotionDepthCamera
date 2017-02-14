#include "ImageWindow.h"
#ifdef NAVIPACK_WIN
#include <math.h>
#include "stdio.h"

const TCHAR * ImageWindow::mPaintWindowClassName = _T("Image Paint Window Class");
BOOL ImageWindow::mIsRegisted = FALSE;

ImageWindow::ImageWindow()
{
	mHInstance = 0;
	mPaintWnd = 0;
	mHdc = 0;
	mWindowName[0] = 0;
	mAutoStretchFlag = TRUE;
	mLastKey = -1;

	RegisterWindowClass();	
}

ImageWindow::~ImageWindow()
{
	if (mPaintWnd) {
		SendMessage(mPaintWnd, WM_CLOSE, 0, 0);
	}
}

BOOL ImageWindow::Create(TCHAR * name, BOOL auto_stretch)
{
	_tcscpy_s(mWindowName, name);

	RegisterWindowClass();
	InitInstance();

	mHdc = CreateCompatibleDC(0);
	mAutoStretchFlag = auto_stretch;

	SetClientSize(320, 240);

	return TRUE;
}

void ImageWindow::Close()
{
	if (mPaintWnd) {
		SendMessage(mPaintWnd, WM_CLOSE, 0, 0);
	}
}

void ImageWindow::SetClientSize(int w, int h)
{
	if (mPaintWnd == 0)
		return;

	RECT rcw, rcc;
	GetWindowRect(mPaintWnd, &rcw);
	GetClientRect(mPaintWnd, &rcc);

	int borderWidth = (rcw.right - rcw.left) - (rcc.right - rcc.left);
	int borderHeight = (rcw.bottom - rcw.top) - (rcc.bottom - rcc.top);

	SetWindowPos(mPaintWnd, NULL, 0, 0, w + borderWidth, h + borderHeight, SWP_NOMOVE | SWP_NOZORDER);
}

BOOL ImageWindow::InitInstance()
{
	DWORD defStyle = WS_VISIBLE | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;

	mPaintWnd = CreateWindow(mPaintWindowClassName, mWindowName, defStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, NULL, NULL, mHInstance, NULL);

	if (!mPaintWnd)
		return FALSE;

	ShowWindow(mPaintWnd, SW_SHOW);

	SetWindowLongPtr(mPaintWnd, GWLP_USERDATA, (LONG_PTR)this);

	return TRUE;
}

void ImageWindow::RegisterWindowClass()
{
	if (mIsRegisted == FALSE) {

		WNDCLASS wndc;
		wndc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		wndc.lpfnWndProc = PaintWndProc;
		wndc.cbClsExtra = 0;
		wndc.cbWndExtra = 0;
		wndc.hInstance = mHInstance;
		wndc.lpszClassName = mPaintWindowClassName;
		wndc.lpszMenuName = mPaintWindowClassName;
		wndc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wndc.hCursor = (HCURSOR)LoadCursor(0, (TCHAR *)(size_t)IDC_CROSS);
		wndc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);

		ATOM a = RegisterClass(&wndc);

		mIsRegisted = TRUE;
	}
}

void ImageWindow::OnPaint(ImageWindow * iw_ptr)
{
	if (iw_ptr->mImage != 0)
	{
		int nchannels = 3;
		SIZE size = { 0,0 };
		PAINTSTRUCT paint;
		HDC hdc;
		RGBQUAD table[256];

		// Determine the bitmap's dimensions
		iw_ptr->GetBitmapData(iw_ptr->mHdc, &size, &nchannels, 0);

		hdc = BeginPaint(iw_ptr->mPaintWnd, &paint);

		if (nchannels == 1)
		{
			int i;
			for (i = 0; i < 256; i++)
			{
				table[i].rgbBlue = (unsigned char)i;
				table[i].rgbGreen = (unsigned char)i;
				table[i].rgbRed = (unsigned char)i;
			}
			SetDIBColorTable(iw_ptr->mHdc, 0, 255, table);
		}

		if (!iw_ptr->mAutoStretchFlag)
		{
			BitBlt(hdc, 0, 0, size.cx, size.cy, iw_ptr->mHdc, 0, 0, SRCCOPY);
		}
		else
		{
			RECT rect;
			GetClientRect(iw_ptr->mPaintWnd, &rect);
			SetStretchBltMode(hdc, COLORONCOLOR);
			StretchBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
				iw_ptr->mHdc, 0, 0, size.cx, size.cy, SRCCOPY);
		}
		EndPaint(iw_ptr->mPaintWnd, &paint);
	}
}

LRESULT ImageWindow::PaintWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImageWindow *iw_ptr = (ImageWindow*)GetWindowLong(hWnd, GWLP_USERDATA);
	if (iw_ptr == NULL)
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
		break;
	case WM_PAINT:
		if (iw_ptr->mImage != 0)
			OnPaint(iw_ptr);
		else
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if ((wParam >= VK_F1 && wParam <= VK_F24) ||
			wParam == VK_HOME || wParam == VK_END ||
			wParam == VK_UP || wParam == VK_DOWN ||
			wParam == VK_LEFT || wParam == VK_RIGHT ||
			wParam == VK_INSERT || wParam == VK_DELETE ||
			wParam == VK_PRIOR || wParam == VK_NEXT)
			iw_ptr->mLastKey = (int)(wParam << 16);
		break;
	case WM_CHAR:
		iw_ptr->mLastKey = (int)(wParam);
		break;
	case WM_SIZE:
		iw_ptr->mWidth = LOWORD(lParam);
		iw_ptr->mHeight = HIWORD(lParam);
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL ImageWindow::GetBitmapData(HDC hdc, SIZE* size, int* channels, void** data)
{
	BITMAP bmp;
	GdiFlush();
	HGDIOBJ h = GetCurrentObject(hdc, OBJ_BITMAP);
	if (size)
		size->cx = size->cy = 0;
	if (data)
		*data = 0;

	if (h == NULL)
		return TRUE;

	if (GetObject(h, sizeof(bmp), &bmp) == 0)
		return TRUE;

	if (size)
	{
		size->cx = abs(bmp.bmWidth);
		size->cy = abs(bmp.bmHeight);
	}

	if (channels)
		*channels = bmp.bmBitsPixel / 8;

	if (data)
		*data = bmp.bmBits;

	return FALSE;
}

int ImageWindow::ProcessWindowsEvents(int timeout)
{
	MSG msg;
	BOOL fGotMessage;
	int time0 = GetTickCount();

	while ((fGotMessage = PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE)) != 0 && fGotMessage != -1)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if ((timeout > 0 && (abs((int)(GetTickCount() - time0)) >= timeout)))
			return -1;
		Sleep(1);
	}
	return msg.wParam;
}

int ImageWindow::WaitKey(int timeout)
{
	MSG msg;
	BOOL fGotMessage;
	int time0 = GetTickCount();

	while ((fGotMessage = PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE)) != 0 && fGotMessage != -1)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		//Key Pressed
		if (mLastKey > 0)
		{
			int key = mLastKey;
			mLastKey = -1;
			return key;
		}

		//Timeout
		if ((timeout > 0 && (abs((int)(GetTickCount() - time0)) >= timeout)))
			return -1;

		Sleep(1);
	}
	return -1;
}

void ImageWindow::FillBitmapInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin)
{
	BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);

	memset(bmih, 0, sizeof(*bmih));
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth = width;
	bmih->biHeight = origin ? abs(height) : -abs(height);
	bmih->biPlanes = 1;
	bmih->biBitCount = (unsigned short)bpp;
	bmih->biCompression = BI_RGB;

	if (bpp == 8)
	{
		RGBQUAD* palette = bmi->bmiColors;
		int i;
		for (i = 0; i < 256; i++)
		{
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}
}

BOOL ImageWindow::Draw(UINT8 *img, int w, int h)
{
	SIZE size = { 0, 0 };
	int channels = 0;
	void* dst_ptr = 0;
	bool changed_size = false;
	HGDIOBJ image = 0;

	if (mPaintWnd == 0)
		return FALSE;

	GetBitmapData(mHdc, &size, &channels, &dst_ptr);

	if (size.cx != w || size.cy != h || channels != 3)
	{
		UINT8 buffer[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
		BITMAPINFO* binfo = (BITMAPINFO*)buffer;

		DeleteObject(SelectObject(mHdc, image));
		image = 0;

		size.cx = w;
		size.cy = h;
		channels = 3;

		FillBitmapInfo(binfo, size.cx, size.cy, channels * 8, 1);

		image = SelectObject(mHdc, CreateDIBSection(mHdc, binfo, DIB_RGB_COLORS, &dst_ptr, 0, 0));
	}
	size_t s = w * h * 3;

	for (int i = h - 1; i >= 0; i--)
	{
		UINT8 *line_ptr = ((UINT8 *)dst_ptr) + i * w * 3;
		memcpy(line_ptr, img, w * 3);
		img += w * 3;
	}

	InvalidateRect(mPaintWnd, 0, 0);
	return TRUE;
}


#endif