// HarmoniaTestApp.cpp: アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "HarmoniaTestApp.h"

#include <stdlib.h>
#include <windows.h>

#include <fstream> 




// General
extern "C" __declspec(dllimport) void harmonia_initialize(unsigned int bufferSize);
extern "C" __declspec(dllimport) void harmonia_finalize();

extern "C" __declspec(dllimport) void harmonia_register_sound(const char* id, unsigned char* binaryData, unsigned int size);
extern "C" __declspec(dllimport) void harmonia_register_sounds(const char** id, unsigned char** binaryData, unsigned int* size, unsigned int numRegister);
extern "C" __declspec(dllimport) void harmonia_unregister_sound(const char* id);
extern "C" __declspec(dllimport) void harmonia_pause_all();
extern "C" __declspec(dllimport) void harmonia_resume_all();
extern "C" __declspec(dllimport) void harmonia_stop_all();

extern "C" __declspec(dllimport) float harmonia_get_master_volume();
extern "C" __declspec(dllimport) void harmonia_set_master_volume(float volume);

extern "C" __declspec(dllimport) void harmonia_mute_all();
extern "C" __declspec(dllimport) void harmonia_unmute_all();

extern "C" __declspec(dllimport) void harmonia_start_capture_events();
extern "C" __declspec(dllimport) const char* harmonia_get_capture_events();
extern "C" __declspec(dllimport) void harmonia_stop_capture_events();

// Sound
extern "C" __declspec(dllimport) void harmonia_sound_play(const char* registeredId, const char* soundId, const char* targetGruopId = "", bool loop = false, unsigned int loopStartTimeByMS = 0, unsigned int loopLengthTimeByMS = 0);
extern "C" __declspec(dllimport) void harmonia_sound_pause(const char* playingDataId);
extern "C" __declspec(dllimport) void harmonia_sound_resume(const char* playingDataId);
extern "C" __declspec(dllimport) void harmonia_sound_stop(const char* playingDataId);
extern "C" __declspec(dllimport) void harmonia_sound_mute(const char* playingDataId);
extern "C" __declspec(dllimport) void harmonia_sound_unmute(const char* playingDataId);
extern "C" __declspec(dllimport) float harmonia_get_sound_volume(const char* playingDataId);
extern "C" __declspec(dllimport) void harmonia_set_sound_volume(const char* playingDataId, float volume);
extern "C" __declspec(dllimport) unsigned int harmonia_get_sound_status(const char* playingDataId);

// Group
extern "C" __declspec(dllimport) void harmonia_create_group(const char* groupId, const char* parentGroupId = "", int maxPolyphony = -1);
extern "C" __declspec(dllimport) void harmonia_delete_group(const char* groupId);
extern "C" __declspec(dllimport) void harmonia_group_pause(const char* groupId);
extern "C" __declspec(dllimport) void harmonia_group_resume(const char* groupId);
extern "C" __declspec(dllimport) void harmonia_group_stop(const char* groupId);
extern "C" __declspec(dllimport) void harmonia_group_mute(const char* groupId);
extern "C" __declspec(dllimport) void harmonia_group_unmute(const char* groupId);
extern "C" __declspec(dllimport) float harmonia_get_group_volume(const char* groupId);
extern "C" __declspec(dllimport) void harmonia_set_group_volume(const char* groupId, float volume);
extern "C" __declspec(dllimport) void harmonia_set_ducking(const char* triggerGroupId, const char* targetGroupId, float ratio);
extern "C" __declspec(dllimport) unsigned int harmonia_get_group_status(const char* targetGroupId);



#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。



	// initialize
	harmonia_initialize(2048);


	std::ifstream infile("test.ogg", std::ifstream::binary);


	if (!infile.fail())
	{
		// get size of file
		infile.seekg(0, infile.end);
		unsigned int size = infile.tellg();
		infile.seekg(0);

		// allocate memory for file content
		char* buffer = new char[size];

		// read content of infile
		infile.read(buffer, size);
		infile.close();

		printf("size : %ld", size);
		// register opus
		harmonia_register_sound("hoge1", (unsigned char*)buffer, size);

		delete[] buffer;

		harmonia_sound_play("hoge1", "moge1", "");
	}

	else
	{
		printf("file not exists.\n");
	}















    // グローバル文字列を初期化しています。
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HARMONIATESTAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーションの初期化を実行します:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HARMONIATESTAPP));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HARMONIATESTAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HARMONIATESTAPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウの描画
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
