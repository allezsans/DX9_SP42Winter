//=============================================================================
// 入力処理 [CInput.cpp]
// 作成日:2013/02/22
//=============================================================================
#include "CInput.h"
//=============================================================================
//コンストラクタ
//=============================================================================
CInput::CInput(){}
CInput::CInput(HINSTANCE hInst, HWND g_Hwnd)
{	
	//=============================================================================
	// 入力処理の初期化
	//=============================================================================
	// DirectInputオブジェクトの作成
	hr_def = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
								IID_IDirectInput8, (void**)&g_pDInput, NULL);

	//=============================================================================
	// キーボードの初期化
	//=============================================================================

	// デバイスオブジェクトを作成
	hr_key = g_pDInput->CreateDevice(GUID_SysKeyboard, &g_pDIDevKeyboard, NULL);
	if(FAILED(hr_key) || g_pDIDevKeyboard == NULL){
		MessageBox(g_Hwnd, "キーボードがねぇ！", "警告！", MB_ICONWARNING);
	}

	// データフォーマットを設定
	hr_key = g_pDIDevKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr_key)){
		MessageBox(g_Hwnd, "キーボードのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr_key = g_pDIDevKeyboard->SetCooperativeLevel(g_Hwnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr_key)){
		MessageBox(g_Hwnd, "キーボードの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
	}

	// キーボードへのアクセス権を獲得(入力制御開始)
	hr_key = g_pDIDevKeyboard->Acquire();

	//=============================================================================
	// マウスの初期化
	//=============================================================================
	// デバイスオブジェクトを作成
	hr_mouse = g_pDInput->CreateDevice(GUID_SysMouse, &g_pDIDevMouse, NULL);
	if(FAILED(hr_mouse) || g_pDIDevMouse == NULL){
		MessageBox(g_Hwnd, "マウスがねぇ！", "警告！", MB_ICONWARNING);
	}

	// データフォーマットを設定
	hr_mouse = g_pDIDevMouse->SetDataFormat(&c_dfDIMouse2);
	if(FAILED(hr_mouse)){
		MessageBox(g_Hwnd, "マウスのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr_mouse = g_pDIDevMouse->SetCooperativeLevel(g_Hwnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr_mouse)){
		MessageBox(g_Hwnd, "マウスの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
	}

	// デバイスの設定
	dipdw.diph.dwSize = sizeof(dipdw);
	dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = DIPROPAXISMODE_REL;

	hr_mouse = g_pDIDevMouse->SetProperty(DIPROP_AXISMODE, &dipdw.diph);

	// キーボードへのアクセス権を獲得(入力制御開始)
	hr_mouse = g_pDIDevMouse->Acquire();
}

//=============================================================================
// デストラクタ
//=============================================================================
CInput::~CInput()
{
	//キーボードの終了処理
	if(g_pDIDevKeyboard){
		g_pDIDevKeyboard->Unacquire();

		g_pDIDevKeyboard->Release();
		g_pDIDevKeyboard = NULL;
	}

	//マウスの終了処理
	if(g_pDIDevMouse){
		g_pDIDevMouse->Unacquire();

		g_pDIDevMouse->Release();
		g_pDIDevMouse = NULL;
	}

	//終了処理
	if(g_pDInput)
	{
		g_pDInput->Release();
		g_pDInput = NULL;
	}
}

//=============================================================================
// 入力処理の更新処理
//=============================================================================
void CInput::UpdateInput(void)
{
	// キーボードの更新
   	CInput::UpdateKeyboard();

	// マウスの更新
	CInput::UpdateMouse();
}

//=============================================================================
// キーボードの更新処理
//=============================================================================
HRESULT CInput::UpdateKeyboard(void)
{
	BYTE	keyStateOld[256];
	// 前回のデータを保存
	memcpy(keyStateOld, g_keyState, NUM_KEY_MAX);

	// デバイスからデータを取得
	hr_key = g_pDIDevKeyboard->GetDeviceState(sizeof(g_keyState), g_keyState);
	if(SUCCEEDED(hr_key)){
		for(int cnt = 0; cnt < NUM_KEY_MAX; cnt++){
			// トリガーとリリースを取得
			g_keyStateTrigger[cnt] = (g_keyState[cnt] & (keyStateOld[cnt] ^ g_keyState[cnt]));
			g_keyStateRelease[cnt] = (keyStateOld[cnt] & (keyStateOld[cnt] ^ g_keyState[cnt]));
			
			// キーが押されているならリピートの判定処理
			if(g_keyState[cnt]){
				if(g_keyStateRepeatCnt[cnt] < 20){
					g_keyStateRepeatCnt[cnt]++;
				}
			}else{
				g_keyStateRepeatCnt[cnt] = 0;
				g_keyStateRepeat[cnt] = 0;
			}
		}
	}else{
		// キーボードへのアクセス権を取得
		hr_key = g_pDIDevKeyboard->Acquire();
	}

	return hr_key;
}

//=============================================================================
// キーボードのプレス状態を取得
//=============================================================================
bool CInput::GetKeyboardPress(int key)
{
	return ((g_keyState[key] & 0x80) != 0);
}

//=============================================================================
// キーボードのトリガー状態を取得
//=============================================================================
bool CInput::GetKeyboardTrigger(int key)
{
	return ((g_keyStateTrigger[key] & 0x80) != 0);
}

//=============================================================================
// キーボードのリピート状態を取得
//=============================================================================
bool CInput::GetKeyboardRepeat(int key)
{
	return ((g_keyStateRepeat[key] & 0x80) != 0);
}

//=============================================================================
// キーボードのリリ－ス状態を取得
//=============================================================================
bool CInput::GetKeyboardRelease(int key)
{
	return ((g_keyStateRelease[key] & 0x80) != 0);
}

//=============================================================================
// マウスの更新処理
//=============================================================================
HRESULT CInput::UpdateMouse(void)
{
	DIMOUSESTATE2 mouseStateOld;

	// 前回のデータを保存
	mouseStateOld = g_mouseState;

	// デバイスからデータを取得
	hr_mouse = g_pDIDevMouse->GetDeviceState(sizeof(g_mouseState), &g_mouseState);
	if(SUCCEEDED(hr_mouse)){
		// トリガーの取得
		g_mouseStateTrigger.lX = g_mouseState.lX & (mouseStateOld.lX ^ g_mouseState.lX);
		g_mouseStateTrigger.lY = g_mouseState.lY & (mouseStateOld.lY ^ g_mouseState.lY);
		g_mouseStateTrigger.lZ = g_mouseState.lZ & (mouseStateOld.lZ ^ g_mouseState.lZ);
		for(int cnt = 0; cnt < 8; cnt++){
			g_mouseStateTrigger.rgbButtons[cnt] = ((mouseStateOld.rgbButtons[cnt] ^ g_mouseState.rgbButtons[cnt]) & g_mouseState.rgbButtons[cnt]);
			g_mouseStateRelease.rgbButtons[cnt] = (mouseStateOld.rgbButtons[cnt] & ( mouseStateOld.rgbButtons[cnt] ^ g_mouseState.rgbButtons[cnt] ));
		}
	}else{
		g_pDIDevMouse->Acquire();
	}

	return S_OK;
}

//=============================================================================
// マウスデータ取得(左プレス)
//=============================================================================
bool CInput::GetMouseLeftPress(void)
{
	return ((g_mouseState.rgbButtons[0] & 0x80) != 0);
}

//=============================================================================
// マウスデータ取得(左トリガー)
//=============================================================================
bool CInput::GetMouseLeftTrigger(void)
{
	return ((g_mouseStateTrigger.rgbButtons[0] & 0x80) != 0);
}

//=============================================================================
// マウスデータ取得(左リリース)
//=============================================================================
bool CInput::GetMouseLeftRelease(void)
{
	return ((g_mouseStateRelease.rgbButtons[0] & 0x80) != 0);
}

//=============================================================================
// マウスデータ取得(右プレス)
//=============================================================================
bool CInput::GetMouseRightPress(void)
{
	return ((g_mouseState.rgbButtons[1] & 0x8) != 00);
}

//=============================================================================
// マウスデータ取得(右トリガー)
//=============================================================================
bool CInput::GetMouseRightTrigger(void)
{
	return ((g_mouseStateTrigger.rgbButtons[1] & 0x80) != 0);
}

//=============================================================================
// マウスデータ取得(中央プレス)
//=============================================================================
bool CInput::GetMouseCenterPress(void)
{
	return ((g_mouseState.rgbButtons[2] & 0x80) != 0);
}

//=============================================================================
// マウスデータ取得(中央トリガー)
//=============================================================================
bool CInput::GetMouseCenterTrigger(void)
{
	return ((g_mouseState.rgbButtons[2] & 0x80) != 0);
}

//=============================================================================
// マウスデータ取得(Ｘ軸移動)
//=============================================================================
long CInput::GetMouseAxisX(void)
{
	return g_mouseState.lX;
}

//=============================================================================
// マウスデータ取得(Ｙ軸移動)
//=============================================================================
long CInput::GetMouseAxisY(void)
{
	return g_mouseState.lY;
}

//=============================================================================
// マウスデータ取得(Ｚ軸移動)
//=============================================================================
long CInput::GetMouseAxisZ(void)
{
	return g_mouseState.lZ;
}

//=============================================================================
// 指定の範囲でPressされたか
//=============================================================================
bool CInput::GetMouseLeftPressXY(int x,int y,int width,int height)
{
	float MouseX = (float)GetMouseAxisX();
	float MouseY = (float)GetMouseAxisY();
	if( MouseX >= x && MouseX <= (x+width)
		&& MouseY >= y && MouseY <= (y+height) )
		if(GetMouseLeftPress()) return true;
	return false;
}