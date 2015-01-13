//=============================================================================
// ���͏��� [CInput.h]
//�쐬��:2013/02/22
//=============================================================================
#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

const int NUM_KEY_MAX = 256;

class CInput{
private:
	LPDIRECTINPUT8			g_pDInput;							// IDirectInput8�C���^�[�t�F�[�X�ւ̃|�C���^
	LPDIRECTINPUTDEVICE8	g_pDIDevKeyboard;					// IDirectInputDevice8�C���^�[�t�F�[�X�ւ̃|�C���^(�L�[�{�[�h)
	LPDIRECTINPUTDEVICE8	g_pDIDevMouse;						// IDirectInputDevice8�C���^�[�t�F�[�X�ւ̃|�C���^(�}�E�X)
	BYTE					g_keyState[NUM_KEY_MAX];			// �L�[�{�[�h�̏�Ԃ��󂯎�郏�[�N
	BYTE					g_keyStateTrigger[NUM_KEY_MAX];		// �L�[�{�[�h�̏�Ԃ��󂯎�郏�[�N
	BYTE					g_keyStateRepeat[NUM_KEY_MAX];		// �L�[�{�[�h�̏�Ԃ��󂯎�郏�[�N
	BYTE					g_keyStateRelease[NUM_KEY_MAX];		// �L�[�{�[�h�̏�Ԃ��󂯎�郏�[�N
	int						g_keyStateRepeatCnt[NUM_KEY_MAX];	// �L�[�{�[�h�̃��s�[�g�J�E���^
	DIMOUSESTATE2			g_mouseState;						// �}�E�X�̏�Ԃ��󂯎�郏�[�N
	DIMOUSESTATE2			g_mouseStateTrigger;				// �}�E�X�̏�Ԃ��󂯎�郏�[�N
	DIMOUSESTATE2			g_mouseStateRelease;				// �}�E�X�̏�Ԃ��󂯎�郏�[�N
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