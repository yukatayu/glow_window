#pragma warning(disable:4996)
#define _WIN32_WINNT 0x0500
#define DllExport extern "C" __declspec (dllexport)

#include <windows.h>
//SW_NORMAL/SW_HIDE

const LPCWSTR
	szClassName_0 = L"glow_window_layer_0",
	szClassName_1 = L"glow_window_layer_1",
	szClassName_2 = L"glow_window_layer_2",
	szClassName_3 = L"glow_window_layer_3";
BYTE col_r = 127, col_g = 255, col_b = 60;
int shadowWidth = 9;
HINSTANCE hInst;
HWND hwnd0, hwnd1, hwnd2, hwnd3;


//グラデーションのBitmapの作成
//di=0:上, 1:右, 2:下, 3:左
//淵の色の濃さの落差: (inc_num / inc_dem)
HBITMAP make_gradation(HWND hWnd, unsigned _height, unsigned _width, BYTE di, BYTE _r, BYTE _g, BYTE _b,
	int inc_num = 3, int inc_dem = 5) {
	BITMAPV5HEADER bi;
	unsigned height = _height;
	unsigned width = _width;
	void *lpBits;
	ZeroMemory(&bi, sizeof(BITMAPV5HEADER));
	bi.bV5Size = sizeof(BITMAPV5HEADER);
	bi.bV5Width = width;
	bi.bV5Height = height;
	bi.bV5Planes = 1;
	bi.bV5BitCount = 32;
	bi.bV5Compression = BI_BITFIELDS;
	bi.bV5RedMask = 0x00FF0000;
	bi.bV5GreenMask = 0x0000FF00;
	bi.bV5BlueMask = 0x000000FF;
	bi.bV5AlphaMask = 0xFF000000;
	HDC hdc = GetDC(hWnd);
	HBITMAP hbmp = CreateDIBSection(hdc, (BITMAPINFO *)&bi, DIB_RGB_COLORS,
		(void **)&lpBits, NULL, (DWORD)0);
	ReleaseDC(hWnd, hdc);
	DWORD *lpdwPixel = (DWORD *)lpBits;

	BYTE r, g, b, a;
	height = di % 2 ? _width : _height;
	width = di % 2 ? _height : _width;

	for (DWORD y = 0; y < height; ++y) {
		if (y + 1 - height) {
			a = 255 * y * inc_num / height / inc_dem;
			r = 255 - int(255 - _r) * y / height;
			g = 255 - int(255 - _g) * y / height;
			b = 255 - int(255 - _b) * y / height;
			r = r*a / 255;
			g = g*a / 255;
			b = b*a / 255;
		} else {
			a = 255;
			r = _r;
			g = _g;
			b = _b;
		}
		
		switch (di) {
		case 0:
			for (DWORD x = 0; x < width; ++x)
				lpdwPixel[(height - y - 1)*width + x] = (r << 16) | (g << 8) | b | (a << 24);
			break;
		case 1:
			for (DWORD x = 0; x < width; ++x) {
				register int r1, g1, b1, a1, r2, g2, b2, a2, t = -1;
				if (x < height) {
					t = x+1;
				} else if (x > width - height) {
					t = width - x;
				}
				if(t>0){
					a1 = 255 * y * inc_num / height / inc_dem;
					r1 = 255 - int(255 - _r) * y / height;
					g1 = 255 - int(255 - _g) * y / height;
					b1 = 255 - int(255 - _b) * y / height;
					a2 = int(a1) * t / height;
					r1 = 255 - int(255 - r1) * t/height;
					g1 = 255 - int(255 - g1) * t/height;
					b1 = 255 - int(255 - b1) * t/height;
					r2 = int(r1) * a2 / 255;
					g2 = int(g1) * a2 / 255;
					b2 = int(b1) * a2 / 255;
				} else {
					r2 = r;
					g2 = g;
					b2 = b;
					a2 = a;
				}
				lpdwPixel[x*height + y] = (r2 << 16) | (g2 << 8) | b2 | (a2 << 24);
			}
			break;
		case 2:
			for (DWORD x = 0; x < width; ++x)
				lpdwPixel[y*width + x] = (r << 16) | (g << 8) | b | (a << 24);
			break;
		case 3:
			for (DWORD x = 0; x < width; ++x) {
				register int r1, g1, b1, a1, r2, g2, b2, a2, t = -1;
				if (x < height) {
					t = x + 1;
				} else if (x > width - height) {
					t = width - x;
				}
				if (t>0) {
					a1 = 255 * y * inc_num / height / inc_dem;
					r1 = 255 - int(255 - _r) * y / height;
					g1 = 255 - int(255 - _g) * y / height;
					b1 = 255 - int(255 - _b) * y / height;
					a2 = int(a1) * t / height;
					r1 = 255 - int(255 - r1) * t / height;
					g1 = 255 - int(255 - g1) * t / height;
					b1 = 255 - int(255 - b1) * t / height;
					r2 = int(r1) * a2 / 255;
					g2 = int(g1) * a2 / 255;
					b2 = int(b1) * a2 / 255;
				} else {
					r2 = r;
					g2 = g;
					b2 = b;
					a2 = a;
				}
				lpdwPixel[x*height + (height - y + 1)] = (r2 << 16) | (g2 << 8) | b2 | (a2 << 24);
			}
			break;
		default:
			break;
		}
	}
	return hbmp;
};

//ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp, BYTE di) {
//	int id;
	HDC hdc, hdc_mem, hsdc;
//	HBRUSH hBrush;
	PAINTSTRUCT ps;
	LPCWSTR szBuf = L"Test Layer Window";
//	BITMAP bmp_info;
//	HBITMAP hBmp;
//	int wx, wy;
	const COLORREF col = RGB(0, 255, 0);
//	HGDIOBJ hOldObj;
	BLENDFUNCTION blend;
	SIZE  wndSize;
	RECT  rc;
	POINT po;
	HBITMAP hBmpGra;

	switch (msg) {
	case WM_CREATE:
		//	SetLayeredWindowAttributes(hWnd, RGB(255, 0, 0), 0, LWA_COLORKEY);
		//メモ: break無し
	case WM_PAINT:
		GetClientRect(hWnd, &rc);
		hBmpGra = make_gradation(hWnd, rc.bottom - rc.top, rc.right-rc.left, di, col_r, col_g, col_b);
		hsdc = GetDC(0);
		hdc = BeginPaint(hWnd, &ps);
		hdc_mem = CreateCompatibleDC(hdc);
		//hBmp = make_gradation(hWnd, 300, 400, 0, 255, 63);
		SelectObject(hdc_mem, hBmpGra);
		BitBlt(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdc_mem, 0, 0, SRCCOPY | CAPTUREBLT);

		wndSize.cx = rc.right - rc.left; wndSize.cy = rc.bottom - rc.top;
		po.x = po.y = 0;
		blend.BlendOp = AC_SRC_OVER;
		blend.BlendFlags = 0;
		blend.SourceConstantAlpha = 255;
		blend.AlphaFormat = AC_SRC_ALPHA;
		if (!UpdateLayeredWindow(hWnd, hsdc, nullptr, &wndSize, hdc_mem, &po, 0, &blend, ULW_ALPHA)) {
			TCHAR strErrMes[80];
			DWORD err = GetLastError();
			wsprintf(strErrMes, L"Error: %d", err);
			MessageBox(hWnd, strErrMes, L"エラー", MB_OK | MB_ICONSTOP);
		}
		//	DeleteObject(hBmp);
		DeleteDC(hdc_mem);
		SetBkMode(hdc, TRANSPARENT);
		EndPaint(hWnd, &ps);
		ReleaseDC(0, hsdc);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return (DefWindowProc(hWnd, msg, wp, lp));
	}
	return 0;
}

LRESULT CALLBACK WndProc0(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	return WndProc(hWnd, msg, wp, lp, 0);
}
LRESULT CALLBACK WndProc1(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	return WndProc(hWnd, msg, wp, lp, 1);
}
LRESULT CALLBACK WndProc2(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	return WndProc(hWnd, msg, wp, lp, 2);
}
LRESULT CALLBACK WndProc3(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	return WndProc(hWnd, msg, wp, lp, 3);
}

//ウィンドウ・クラスの登録
ATOM InitApp(HINSTANCE hInst, LPCWSTR szClassName, WNDPROC wnd) {
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = wnd;    //プロシージャ名
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;//インスタンス
	wc.hIcon = (HICON)LoadImage(NULL,
		MAKEINTRESOURCE(IDI_APPLICATION),
		IMAGE_ICON,
		0,
		0,
		LR_DEFAULTSIZE | LR_SHARED);
	wc.hCursor = (HCURSOR)LoadImage(NULL,
		MAKEINTRESOURCE(IDC_ARROW),
		IMAGE_CURSOR,
		0,
		0,
		LR_DEFAULTSIZE | LR_SHARED);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;    //メニュー名
	wc.lpszClassName = szClassName;
	wc.hIconSm = (HICON)LoadImage(NULL,
		MAKEINTRESOURCE(IDI_APPLICATION),
		IMAGE_ICON,
		0,
		0,
		LR_DEFAULTSIZE | LR_SHARED);

	return (RegisterClassEx(&wc));
}

BOOL InitInstance(HINSTANCE hInst, LPCWSTR szClassName, HWND *p_hwnd, int x, int y, int height, int width) {
	HWND hWnd;

	hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW/* | WS_EX_TOPMOST*/ | WS_EX_NOACTIVATE,
		szClassName,
		L"Layer Window",	//Title
		WS_POPUP,
		x,	//X
		y,	//Y
		width,	//width
		height,	//height
		NULL,	//hWnd
		NULL,	//Menu
		hInst,
		NULL);
	if (!hWnd)
		return FALSE;
	*p_hwnd = hWnd;
	ShowWindow(hWnd, SW_NORMAL);
	UpdateWindow(hWnd);
	return TRUE;
}

void _rePaint() {
	PostMessage(hwnd0, WM_PAINT, NULL, NULL);
	PostMessage(hwnd1, WM_PAINT, NULL, NULL);
	PostMessage(hwnd2, WM_PAINT, NULL, NULL);
	PostMessage(hwnd3, WM_PAINT, NULL, NULL);
}

//再表示にはupdateWindowSize
DllExport void invisible(int a, int b, int c, int d) {
	SetWindowPos(hwnd0, HWND_TOP, -100, -100, 50, 50, SWP_NOACTIVATE | SWP_NOZORDER);
	SetWindowPos(hwnd1, HWND_TOP, -100, -100, 50, 50, SWP_NOACTIVATE | SWP_NOZORDER);
	SetWindowPos(hwnd2, HWND_TOP, -100, -100, 50, 50, SWP_NOACTIVATE | SWP_NOZORDER);
	SetWindowPos(hwnd3, HWND_TOP, -100, -100, 50, 50, SWP_NOACTIVATE | SWP_NOZORDER);
}

//この関数は使わない
DllExport void visible(int a, int b, int c, int d) {
	//SWP_ASYNCWINDOWPOS
	SetWindowPos(hwnd0, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	SetWindowPos(hwnd1, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	SetWindowPos(hwnd2, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	SetWindowPos(hwnd3, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

DllExport void rePaint(int a, int b, int c, int d) {
	_rePaint();
}

DllExport void setShadowWidth(int w, int a, int b, int c) {
	shadowWidth = w;
	_rePaint();
}

DllExport void updateWindowPos(int x, int y, int width, int height) {
	SetWindowPos(hwnd0, HWND_TOP, x, y - shadowWidth, width, shadowWidth, SWP_NOZORDER);
	SetWindowPos(hwnd1, HWND_TOP, x - shadowWidth, y - shadowWidth, shadowWidth, height + 2 * shadowWidth, SWP_NOZORDER);
	SetWindowPos(hwnd2, HWND_TOP, x, y + height, width, shadowWidth, SWP_NOZORDER);
	SetWindowPos(hwnd3, HWND_TOP, x + width - 2, y - shadowWidth, shadowWidth, height + 2 * shadowWidth, SWP_NOZORDER);
}

DllExport void updateWindowSize(int x, int y, int width, int height) {
	updateWindowPos(x, y, width, height);
	_rePaint();
}

DllExport void setColor(int r, int g, int b, int tr) {
	col_r = r;
	col_g = g;
	col_b = b;
	_rePaint();
}

DllExport int init(HINSTANCE _hInst, int a, int b, int c) {
	hInst = _hInst;
	if (!InitApp(hInst, szClassName_0, WndProc0)||
		!InitApp(hInst, szClassName_1, WndProc1)||
		!InitApp(hInst, szClassName_2, WndProc2)||
		!InitApp(hInst, szClassName_3, WndProc3))
		return 0;
	if (!InitInstance(hInst, szClassName_0, &hwnd0, 0, 0, shadowWidth, 100)||
		!InitInstance(hInst, szClassName_1, &hwnd1, 0, 0, 100, shadowWidth)||
		!InitInstance(hInst, szClassName_2, &hwnd2, 0, 0, shadowWidth, 100)||
		!InitInstance(hInst, szClassName_3, &hwnd3, 0, 0, 100, shadowWidth))
		return 0;
//	wchar_t hwnd0_msg[128];
//	wsprintf(hwnd0_msg, L"hWnd0 = %d", hwnd0);
//	MessageBox(hwnd0, hwnd0_msg, L"hwnd0", MB_OK | MB_ICONASTERISK);
	return 1;
}
