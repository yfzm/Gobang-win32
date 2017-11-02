/*
	main.cpp
*/
// project:gobang
// version:1.0
// author:Li Hao


#include <windows.h>
#include <windowsx.h>

#define NUM 15

int chessboard[NUM][NUM];
int valueCB[NUM][NUM];

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
RECT GetRect(int x, int y);
POINT AIPoint();
bool IsWin(int x, int y, int w);
int GetValue(int x, int y, int w);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("FIVE");
	HWND         hwnd;
	MSG          msg;
	WNDCLASS     wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName, TEXT("五子棋 v1.0"),
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL fState[NUM][NUM];
	static int  cxBlock, cyBlock;
	static int	cxClient, cyClient;
	HDC         hdc;
	int         x, y;
	int			i, j;
	PAINTSTRUCT ps;
	RECT        rect;
	POINT		point;
	//HBRUSH		hBrush;
	TCHAR		szBuffer[10];

	switch (message)
	{
	case WM_CREATE:
		MoveWindow(hwnd, 300, 50, 618, 640, FALSE);
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		
		for (i = 0; i < NUM; i++)
		{
			for (j = 0; j < NUM; j++)
			{
				chessboard[i][j] = 0;
				valueCB[i][j] = 0;
			}
		}

		//chessboard[2][4] = -1;
		//chessboard[2][5] = 1;

		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		cxBlock = cxClient / NUM;
		cyBlock = cyClient / NUM;
		return 0;

	case WM_LBUTTONUP:
		x = GET_X_LPARAM(lParam) / cxBlock;
		y = GET_Y_LPARAM(lParam) / cyBlock;

		hdc = GetDC(hwnd);

		if (x < 0 || x >= NUM || y < 0 || y >= NUM)
		{
			ReleaseDC(hwnd, hdc);
			return 0;
		}

		chessboard[x][y] = -1;
		valueCB[x][y] = -1;
		rect = GetRect(x, y);
		InvalidateRect(hwnd, &rect, FALSE);
		UpdateWindow(hwnd);

		if (IsWin(x, y, -1))
		{
			MessageBox(hwnd, TEXT("你赢了！"), TEXT("提示"), 0);
			ReleaseDC(hwnd, hdc);
			return 0;
		}

		point = AIPoint();
		chessboard[point.x][point.y] = 1;
		valueCB[point.x][point.y] = -1;
		rect = GetRect(point.x, point.y);
		InvalidateRect(hwnd, &rect, FALSE);
		UpdateWindow(hwnd);

		if (IsWin(point.x, point.y, 1))
		{
			MessageBox(hwnd, TEXT("你输了！"), TEXT("提示"), 0);
			ReleaseDC(hwnd, hdc);
			return 0;
		}
		
		//TextOut(hdc, 0, 0, szBuffer, wsprintf(szBuffer, TEXT("%d,%d"), x, y));

		ReleaseDC(hwnd, hdc);

		return 0;

	case WM_RBUTTONUP:
		x = GET_X_LPARAM(lParam) / cxBlock;
		y = GET_Y_LPARAM(lParam) / cyBlock;

		hdc = GetDC(hwnd);
		TextOut(hdc, 0, 0, szBuffer, wsprintf(szBuffer, TEXT("(%d,%d):%d"), x, y, GetValue(x,y,1)));
		ReleaseDC(hwnd, hdc);

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		//Draw cheeseboard
		for (i = 0; i < NUM; i++)	
		{
			//vector
			MoveToEx(hdc, cxBlock / 2 + cxBlock * i, cyBlock / 2, NULL);
			LineTo(hdc, cxBlock / 2 + cxBlock * i, cyBlock * NUM - cyBlock / 2);
			//horizon
			MoveToEx(hdc, cxBlock / 2, cyBlock / 2 + cyBlock * i, NULL);
			LineTo(hdc, cxBlock * NUM - cxBlock / 2, cyBlock / 2 + cyBlock * i);
		}

		//Draw chess
		for(i=0;i<NUM;i++)
			for (j = 0; j < NUM; j++)
			{
				switch (chessboard[i][j])
				{
				case -1:
					SelectObject(hdc, GetStockObject(BLACK_BRUSH));
					rect = GetRect(i, j);
					Ellipse(hdc,rect.left,rect.top,rect.right,rect.bottom);
					break;
				case 1:
					SelectObject(hdc, GetStockObject(WHITE_BRUSH));
					rect = GetRect(i, j);
					Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
					break;
				default:
					break;
				}
			}



		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}


RECT GetRect(int x, int y)
{
	RECT rrect;
	SetRect(&rrect, x * 40, y * 40, (x + 1) * 40, (y + 1) * 40);
	return rrect;
}

bool IsWin(int x, int y, int w)
{
	int i, j;
	int n = 0;
	for (i = 0; i < NUM; ++i)
	{
		if (chessboard[i][y] == w)
		{
			++n;
			if (n == 5) return true;
		}
		else
			n = 0;
	}

	n = 0;
	for (i = 0; i < NUM; ++i)
	{
		if (chessboard[x][i] == w)
		{
			++n;
			if (n == 5) return true;
		}
		else
			n = 0;
	}

	n = 0;
	for (i = x > y ? (x - y) : 0, j = x > y ? 0 : (y - x); i < NUM && j < NUM; ++i, ++j)
	{
		if (chessboard[i][j] == w)
		{
			++n;
			if (n == 5) return true;
		}
		else
			n = 0;
	}

	n = 0;
	for (i = x, j = y; i >= 0 && i < NUM && j >= 0 && j < NUM; ++i, --j)
	{
		if (chessboard[i][j] == w)
		{
			++n;
			if (n == 5) return true;
		}
		else
			break;
	}
	--n;
	for (i = x, j = y; i >= 0 && i < NUM && j >= 0 && j < NUM; --i, ++j)
	{
		if (chessboard[i][j] == w)
		{
			++n;
			if (n == 5) return true;
		}
		else
			break;
	}

	return false;
}

POINT AIPoint()
{
	POINT point;
	int temp = 0;
	point.x = 0;
	point.y = 0;

	for (int i = 0; i < NUM; i++)
	{
		for (int j = 0; j < NUM; j++)
		{
			if (valueCB[i][j] >= 0)
			{
				valueCB[i][j] = GetValue(i, j, 1);
				if (valueCB[i][j] > temp)
				{
					temp = valueCB[i][j];
					point.x = i;
					point.y = j;
				}
			}
		}
	}

	return point;
}

int GetValue(int x, int y, int w)
{
	int value = 0;
	int i, j;
	int nax = 0, nay = 0;	//表示活的棋子数
	int ndx = 0, ndy = 0;	//表示死的棋子数


	//横线上，进攻
	for (i = x - 1; i >= 0; i--)
	{
		if (chessboard[i][y] == 0)
		{
			ndx = 0;
			break;
		}
		else if (chessboard[i][y] == -w)
		{
			nax = 0;
			break;
		}
		else
		{
			value++;
			nax++;
			ndx++;
		}
	}	
	for (i = x + 1; i < NUM; i++)
	{
		if (chessboard[i][y] == 0)
		{
			ndy = 0;
			break;
		}
		else if (chessboard[i][y] == -w)
		{
			nay = 0;
			break;
		}
		else
		{
			value++;
			nay++;
			ndy++;
		}
	}
	if (nax + nay == 2) value += 8;
	if (nax + nay == 3) value += 25;
	if (nax + nay >= 4) value += 150;
	if (ndx + ndy >= 4) value += 150;
	if (nax + ndy >= 4) value += 150;
	if (ndx + nay >= 4) value += 150;


	//横线上，防御
	nax = nay = ndx = ndy = 0;
	for (i = x - 1; i >= 0; i--)
	{
		if (chessboard[i][y] == 0)
		{
			ndx = 0;
			//if (nax == 2) value += 5;
			//if (nax == 3) value += 20;
			//if (nax == 4) value += 75;
			break;
		}
		else if (chessboard[i][y] == w)
		{
			nax = 0;
			break;
		}
		else
		{
			value++;
			nax++;
			ndx++;
		}
	}
	for (i = x + 1; i < NUM; i++)
	{
		if (chessboard[i][y] == 0)
		{
			ndy = 0;
			break;
		}
		else if (chessboard[i][y] == w)
		{
			nay = 0;
			break;
		}
		else
		{
			value++;
			nay++;
			ndy++;
		}
	}
	if (nax + nay == 2) value += 5;
	if (nax + nay == 3) value += 20;
	if (nax + nay >= 4) value += 75;
	if (ndx + ndy >= 4) value += 75;
	if (nax + ndy >= 4) value += 75;
	if (ndx + nay >= 4) value += 75;

	//竖线上，进攻
	nax = nay = ndx = ndy = 0;
	for (i = y - 1; i >= 0; i--)
	{
		if (chessboard[x][i] == 0)
		{
			ndx = 0;
			break;
		}
		else if (chessboard[x][i] == -w)
		{
			nax = 0;
			break;
		}
		else
		{
			value++;
			nax++;
			ndx++;
		}
	}
	for (i = y + 1; i < NUM; i++)
	{
		if (chessboard[x][i] == 0)
		{
			ndy = 0;
			break;
		}
		else if (chessboard[x][i] == -w)
		{
			nay = 0;
			break;
		}
		else
		{
			value++;
			nay++;
			ndy++;
		}
	}
	if (nax + nay == 2) value += 8;
	if (nax + nay == 3) value += 25;
	if (nax + nay >= 4) value += 150;
	if (ndx + ndy >= 4) value += 150;
	if (nax + ndy >= 4) value += 150;
	if (ndx + nay >= 4) value += 150;

	//竖线上，防御
	nax = nay = ndx = ndy = 0;
	for (i = y - 1; i >= 0; i--)
	{
		if (chessboard[x][i] == 0)
		{
			ndx = 0;
			break;
		}
		else if (chessboard[x][i] == w)
		{
			nax = 0;
			break;
		}
		else
		{
			value++;
			nax++;
			ndx++;
		}
	}
	for (i = y + 1; i < NUM; i++)
	{
		if (chessboard[x][i] == 0)
		{
			ndy = 0;
			break;
		}
		else if (chessboard[x][i] == w)
		{
			nay = 0;
			break;
		}
		else
		{
			value++;
			nay++;
			ndy++;
		}
	}
	if (nax + nay == 2) value += 5;
	if (nax + nay == 3) value += 20;
	if (nax + nay >= 4) value += 75;
	if (ndx + ndy >= 4) value += 75;
	if (nax + ndy >= 4) value += 75;
	if (ndx + nay >= 4) value += 75;

	//左下到右上，进攻
	nax = nay = ndx = ndy = 0;
	for (i = x - 1, j = y + 1; i >= 0 && j < NUM; i--, j++)
	{
		if (chessboard[i][j] == 0)
		{
			ndx = 0;
			break;
		}
		else if (chessboard[i][j] == -w)
		{
			nax = 0;
			break;
		}
		else
		{
			value++;
			nax++;
			ndx++;
		}
	}
	for (i = x + 1, j = y - 1; i < NUM && j >= 0; i++, j--)
	{
		if (chessboard[i][j] == 0)
		{
			ndy = 0;
			break;
		}
		else if (chessboard[i][j] == -w)
		{
			nay = 0;
			break;
		}
		else
		{
			value++;
			nay++;
			ndy++;
		}
	}
	if (nax + nay == 2) value += 8;
	if (nax + nay == 3) value += 25;
	if (nax + nay >= 4) value += 150;
	if (ndx + ndy >= 4) value += 150;
	if (nax + ndy >= 4) value += 150;
	if (ndx + nay >= 4) value += 150;

	//左下到右上，防御
	nax = nay = ndx = ndy = 0;
	for (i = x - 1, j = y + 1; i >= 0 && j < NUM; i--, j++)
	{
		if (chessboard[i][j] == 0)
		{
			ndx = 0;
			break;
		}
		else if (chessboard[i][j] == w)
		{
			nax = 0;
			break;
		}
		else
		{
			value++;
			nax++;
			ndx++;
		}
	}
	for (i = x + 1, j = y - 1; i < NUM && j >= 0; i++, j--)
	{
		if (chessboard[i][j] == 0)
		{
			ndy = 0;
			break;
		}
		else if (chessboard[i][j] == w)
		{
			nay = 0;
			break;
		}
		else
		{
			value++;
			nay++;
			ndy++;
		}
	}
	if (nax + nay == 2) value += 5;
	if (nax + nay == 3) value += 20;
	if (nax + nay >= 4) value += 75;
	if (ndx + ndy >= 4) value += 75;
	if (nax + ndy >= 4) value += 75;
	if (ndx + nay >= 4) value += 75;

	//左上到右下，进攻
	nax = nay = ndx = ndy = 0;
	for (i = x - 1, j = y - 1; i >= 0 && j >= 0; i--, j--)
	{
		if (chessboard[i][j] == 0)
		{
			ndx = 0;
			break;
		}
		else if (chessboard[i][j] == -w)
		{
			nax = 0;
			break;
		}
		else
		{
			value++;
			nax++;
			ndx++;
		}
	}
	for (i = x + 1, j = y + 1; i < NUM && j < NUM; i++, j++)
	{
		if (chessboard[i][j] == 0)
		{
			ndy = 0;
			break;
		}
		else if (chessboard[i][j] == -w)
		{
			nay = 0;
			break;
		}
		else
		{
			value++;
			nay++;
			ndy++;
		}
	}
	if (nax + nay == 2) value += 8;
	if (nax + nay == 3) value += 25;
	if (nax + nay >= 4) value += 150;
	if (ndx + ndy >= 4) value += 150;
	if (nax + ndy >= 4) value += 150;
	if (ndx + nay >= 4) value += 150;

	//左上到右下，防御
	nax = nay = ndx = ndy = 0;
	for (i = x - 1, j = y - 1; i >= 0 && j >= 0; i--, j--)
	{
		if (chessboard[i][j] == 0)
		{
			ndx = 0;
			break;
		}
		else if (chessboard[i][j] == w)
		{
			nax = 0;
			break;
		}
		else
		{
			value++;
			nax++;
			ndx++;
		}
	}
	for (i = x + 1, j = y + 1; i < NUM && j < NUM; i++, j++)
	{
		if (chessboard[i][j] == 0)
		{
			ndy = 0;
			break;
		}
		else if (chessboard[i][j] == w)
		{
			nay = 0;
			break;
		}
		else
		{
			value++;
			nay++;
			ndy++;
		}
	}
	if (nax + nay == 2) value += 5;
	if (nax + nay == 3) value += 20;
	if (nax + nay >= 4) value += 75;
	if (ndx + ndy >= 4) value += 75;
	if (nax + ndy >= 4) value += 75;
	if (ndx + nay >= 4) value += 75;


	return value;
}
