//=============================================================================
// 入力処理 [CInput.h]
//作成日:2013/02/22
//=============================================================================
#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

const int NUM_KEY_MAX = 256;

class CInput{
private:
	LPDIRECTINPUT8			g_pDInput;							// IDirectInput8インターフェースへのポインタ
	LPDIRECTINPUTDEVICE8	g_pDIDevKeyboard;					// IDirectInputDevice8インターフェースへのポインタ(キーボード)
	LPDIRECTINPUTDEVICE8	g_pDIDevMouse;						// IDirectInputDevice8インターフェースへのポインタ(マウス)
	BYTE					g_keyState[NUM_KEY_MAX];			// キーボードの状態を受け取るワーク
	BYTE					g_keyStateTrigger[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
	BYTE					g_keyStateRepeat[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
	BYTE					g_keyStateRelease[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
	int						g_keyStateRepeatCnt[NUM_KEY_MAX];	// キーボードのリピートカウンタ
	DIMOUSESTATE2			g_mouseState;						// マウスの状態を受け取るワーク
	DIMOUSESTATE2			g_mouseStateTrigger;				// マウスの状態を受け取るワーク
	DIMOUSESTATE2			g_mouseStateRelease;				// マウスの状態を受け取るワーク
	HRESULT					hr_def,hr_key,hr_mouse;
	DIPROPDWORD				dipdw;
public:
	CInput();
	CInput(HINSTANCE,HWND);
	~CInput(void);

	void UpdateInput(void);
	HRESULT UpdateKeyboard(void);
	HRESULT UpdateMouse(void);

	bool GetKeyboardPress(int);
	bool GetKeyboardTrigger(int);
	bool GetKeyboardRepeat(int);
	bool GetKeyboardRelease(int);

	bool GetMouseLeftPress(void);
	bool GetMouseLeftTrigger(void);
	bool GetMouseLeftRelease(void);
	bool GetMouseRightPress(void);
	bool GetMouseRightTrigger(void);
	bool GetMouseCenterPress(void);
	bool GetMouseCenterTrigger(void);
	long GetMouseAxisX(void);
	long GetMouseAxisY(void);
	long GetMouseAxisZ(void);

	bool GetMouseLeftPressXY(int x,int y,int width,int height);
};