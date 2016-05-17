#define _WIN32_WINNT 0x500
#include "Graphics.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include<conio.h>
#include <math.h>
#include "resource.h"
#include "AudioIn.h"
#pragma comment(lib, "winmm.lib")


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int bars;
int* freq;
int nCmdShow;
double y_scale;
HFONT g_hfFont;
char*** rms;
int rmsI;
double power;
double short_term_power;
double long_term_power;
int avg_freq;
int short_term_avg_freq;
int long_term_avg_freq;


// default constructor
Graphics::Graphics(HINSTANCE hInstance, int mainCmdShow, int* freqp)
{

	nCmdShow=mainCmdShow;
	bars = 41;
	y_scale = 20;
	rmsI=2;
	g_hfFont = NULL;
	g_bOpaque = TRUE;
	g_rgbText = RGB(0, 0, 0);
	g_rgbBackground = RGB(255, 255, 255);
	freq=freqp;

	strcpy(g_szClassName,"myWindowClass");
	for (int i=0; i<16; i++)
		 g_rgbCustom[i]=0;
	rms = (char***)malloc(7*sizeof(char**));
	for(int i=0; i<7; i++){
		rms[i]=(char**)malloc(2*sizeof(char*));
		rms[i][0]=(char*)malloc(4*sizeof(char*));
		rms[i][1]=(char*)malloc(4*sizeof(char*));
	}
	strcpy(rms[0][0],"500");
	strcpy(rms[0][1],"250");

	strcpy(rms[1][0],"250");
	strcpy(rms[1][1],"125");

	strcpy(rms[2][0],"100");
	strcpy(rms[2][1]," 50");

	strcpy(rms[3][0]," 50");
	strcpy(rms[3][1]," 25");

	strcpy(rms[4][0]," 20");
	strcpy(rms[4][1]," 10");

	strcpy(rms[5][0]," 10");
	strcpy(rms[5][1],"  5");

	strcpy(rms[6][0],"  5");
	strcpy(rms[6][1],"2.5");



	
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	//wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	//wc.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MYMENU);
	wc.lpszClassName = g_szClassName;
	//wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));
	wc.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 16, 16, 0);
	

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
	}

	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"Digital Spectrum Analizer - Uri Stemmer, Oran Grabovski",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 700, 400,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
	}
}


void Graphics::start(){
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

int Graphics:: getBarsNum(){
	return bars;
}

double Graphics:: getYScale(){
	return y_scale;
}

void Graphics::refresh(){
	RedrawWindow(hwnd, 0, 0, RDW_INTERNALPAINT );
}
void Graphics::setCurrentPower(double po){
	power=po;
}
void Graphics::setShortPower(double po){
	short_term_power=po;
}
void Graphics::setLongPower(double po){
	long_term_power=po;
}

void Graphics::setCurrentAvgFreq(int af){
	avg_freq=af;
}
void Graphics::setShortAvgFreq(int af){
	short_term_avg_freq=af;
}
void Graphics::setLongAvgFreq(int af){
	long_term_avg_freq=af;
}

void DrawFreq(HDC hdc, LPRECT rcClientP) 
{ 
	int i,rectop;
	char szSize[100]; //freq axis

	//setting collors:
	SetDCBrushColor(hdc, RGB(255,255,255));
	SetDCPenColor(hdc, RGB(255,255,39));
	SetTextColor(hdc, RGB(255,255,255));
	SelectObject(hdc, GetStockObject(DC_BRUSH));  
	SelectObject(hdc, GetStockObject(DC_PEN));
	SetBkMode(hdc, TRANSPARENT);


	//draw x axis:
	MoveToEx(hdc, 40, rcClientP->bottom - 21, (LPPOINT) NULL); 
	LineTo(hdc, rcClientP->right - 50, rcClientP->bottom - 21);
	LineTo(hdc, rcClientP->right - 55, rcClientP->bottom - 26);
	MoveToEx(hdc, rcClientP->right - 50, rcClientP->bottom - 21, (LPPOINT) NULL); 
	LineTo(hdc, rcClientP->right - 55, rcClientP->bottom - 16);
	TextOut(hdc,rcClientP->right - 45,rcClientP->bottom - 27, "f [kHz]", 7);

	//draw y axis:
	MoveToEx(hdc, 40, rcClientP->bottom - 21, (LPPOINT) NULL);
	LineTo(hdc, 40, 30);
	LineTo(hdc, 45, 35);
	MoveToEx(hdc, 40, 30, (LPPOINT) NULL);
	LineTo(hdc, 35, 35);
	TextOut(hdc,15,10, "|X(w)| [V]", 10);
	TextOut(hdc,10,40, rms[rmsI][0], 3);
	TextOut(hdc,10,(rcClientP->bottom)/2, rms[rmsI][1], 3);
	TextOut(hdc,10,rcClientP->bottom - 35, "  0", 3);


	for (i=0; i<bars; i++){
		rectop = ((100-freq[i])*(rcClientP->bottom-30))/100;
		if (rectop < 50 ) rectop = 50;
		Rectangle(hdc, ((rcClientP->right - 120)*i)/bars + 50, rectop, ((rcClientP->right - 120)*(i+1))/bars-2 + 50, rcClientP->bottom-30); 
	}
	for (i=0; i<=9; i++){
			wsprintf(szSize, "%d", i);
			TextOut(hdc,((rcClientP->right - 100)*i)/23 + 50,rcClientP->bottom - 20, szSize, 1);
	}
	for (i=10; i<=22; i++){
			wsprintf(szSize, "%d", i);
			TextOut(hdc,((rcClientP->right - 100)*i)/23 + 50,rcClientP->bottom - 20, szSize, 2);
	}
} 



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char szSize[500];
	switch(msg)
	{
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ID_FILE_EXIT:
					PostMessage(hwnd, WM_CLOSE, 0, 0);
				break;
				case ID_X_41:
					bars = 41;
				break;
				case ID_X_101:
					bars = 101;
				break;
				case ID_X_201:
					bars = 201;
				break;

				case ID_Y_1000:
					//y_scale = 1000000000;
					y_scale = 500;
					rmsI=0;
				break;
				case ID_Y_100:
					//y_scale = 100000000;
					y_scale = 250;
					rmsI=1;
				break;
				case ID_Y_10:
					//y_scale = 10000000;
					y_scale = 100;
					rmsI=2;
				break;
				case ID_Y_5:
					//y_scale = 5000000;
					y_scale = 50;
					rmsI=3;
				break;
				case ID_Y_2_5:
					//y_scale = 2500000;
					y_scale = 20;
					rmsI=4;
				break;
				case ID_Y_1:
					//y_scale = 1000000;
					y_scale = 10;
					rmsI=5;
				break;
				case ID_Y_0_5:
					//y_scale = 500000;
					y_scale = 5;
					rmsI=6;
				break;

				case ID_PARAMS_1:
					wsprintf(szSize, "          \n          Instantaneous Power: %d µW          \n\n          Weighted Average Power: %d µW          \n\n          Average Power: %d µW          \n\n          ", ((int)(power*1000000)), ((int)(short_term_power*1000000)), ((int)(long_term_power*1000000)) );
					MessageBox(hwnd, szSize, "Parameters", MB_OK);
				break;
				case ID_PARAMS_2:
					wsprintf(szSize, "          \n\n          Instantaneous center frequency: %d Hz          \n\n          Weighted Average of the center frequency: %d Hz          \n\n          Average center frequency: %d Hz          \n\n          ", avg_freq, short_term_avg_freq, long_term_avg_freq );
					MessageBox(hwnd, szSize, "Parameters", MB_OK);
				break;
			}
		break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
		break;
		case WM_PAINT:
		{
			RECT rcClient;

			HDC hdc = GetDC(hwnd);
			GetClientRect(hwnd, &rcClient);

			//HBRUSH hb = CreateSolidBrush(RGB(255,255,255)); 
			HBRUSH hb=CreatePatternBrush((HBITMAP) LoadImage(0,("background.bmp"),IMAGE_BITMAP,rcClient.right,rcClient.bottom,LR_CREATEDIBSECTION|LR_LOADFROMFILE));
			FillRect(hdc, &rcClient, hb);
			DeleteObject(hb);

			DrawFreq(hdc, &rcClient);

			//EndPaint(hwnd, &ps);
			ReleaseDC(hwnd, hdc);

			RedrawWindow(hwnd, 0, 0, RDW_VALIDATE );
		}
		break;
		case WM_DESTROY:
			DeleteObject(g_hfFont);

			PostQuitMessage(0);
		break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}