//=============================================================================
// ���͏��� [CInput.cpp]
// �쐬��:2013/02/22
//=============================================================================
#include "CInput.h"
//=============================================================================
//�R���X�g���N�^
//=============================================================================
CInput::CInput(){}
CInput::CInput(HINSTANCE hInst, HWND g_Hwnd)
{	
	//=============================================================================
	// ���͏����̏�����
	//=============================================================================
	// DirectInput�I�u�W�F�N�g�̍쐬
	hr_def = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
								IID_IDirectInput8, (void**)&g_pDInput, NULL);

	//=============================================================================
	// �L�[�{�[�h�̏�����
	//=============================================================================

	// �f�o�C�X�I�u�W�F�N�g���쐬
	hr_key = g_pDInput->CreateDevice(GUID_SysKeyboard, &g_pDIDevKeyboard, NULL);
	if(FAILED(hr_key) || g_pDIDevKeyboard == NULL){
		MessageBox(g_Hwnd, "�L�[�{�[�h���˂��I", "�x���I", MB_ICONWARNING);
	}

	// �f�[�^�t�H�[�}�b�g��ݒ�
	hr_key = g_pDIDevKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr_key)){
		MessageBox(g_Hwnd, "�L�[�{�[�h�̃f�[�^�t�H�[�}�b�g��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
	}

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	hr_key = g_pDIDevKeyboard->SetCooperativeLevel(g_Hwnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr_key)){
		MessageBox(g_Hwnd, "�L�[�{�[�h�̋������[�h��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
	}

	// �L�[�{�[�h�ւ̃A�N�Z�X�����l��(���͐���J�n)
	hr_key = g_pDIDevKeyboard->Acquire();

	//=============================================================================
	// �}�E�X�̏�����
	//=============================================================================
	// �f�o�C�X�I�u�W�F�N�g���쐬
	hr_mouse = g_pDInput->CreateDevice(GUID_SysMouse, &g_pDIDevMouse, NULL);
	if(FAILED(hr_mouse) || g_pDIDevMouse == NULL){
		MessageBox(g_Hwnd, "�}�E�X���˂��I", "�x���I", MB_ICONWARNING);
	}

	// �f�[�^�t�H�[�}�b�g��ݒ�
	hr_mouse = g_pDIDevMouse->SetDataFormat(&c_dfDIMouse2);
	if(FAILED(hr_mouse)){
		MessageBox(g_Hwnd, "�}�E�X�̃f�[�^�t�H�[�}�b�g��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
	}

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	hr_mouse = g_pDIDevMouse->SetCooperativeLevel(g_Hwnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr_mouse)){
		MessageBox(g_Hwnd, "�}�E�X�̋������[�h��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
	}

	// �f�o�C�X�̐ݒ�
	dipdw.diph.dwSize = sizeof(dipdw);
	dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = DIPROPAXISMODE_REL;

	hr_mouse = g_pDIDevMouse->SetProperty(DIPROP_AXISMODE, &dipdw.diph);

	// �L�[�{�[�h�ւ̃A�N�Z�X�����l��(���͐���J�n)
	hr_mouse = g_pDIDevMouse->Acquire();
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
CInput::~CInput()
{
	//�L�[�{�[�h�̏I������
	if(g_pDIDevKeyboard){
		g_pDIDevKeyboard->Unacquire();

		g_pDIDevKeyboard->Release();
		g_pDIDevKeyboard = NULL;
	}

	//�}�E�X�̏I������
	if(g_pDIDevMouse){
		g_pDIDevMouse->Unacquire();

		g_pDIDevMouse->Release();
		g_pDIDevMouse = NULL;
	}

	//�I������
	if(g_pDInput)
	{
		g_pDInput->Release();
		g_pDInput = NULL;
	}
}

//=============================================================================
// ���͏����̍X�V����
//=============================================================================
void CInput::UpdateInput(void)
{
	// �L�[�{�[�h�̍X�V
   	CInput::UpdateKeyboard();

	// �}�E�X�̍X�V
	CInput::UpdateMouse();
}

//=============================================================================
// �L�[�{�[�h�̍X�V����
//=============================================================================
HRESULT CInput::UpdateKeyboard(void)
{
	BYTE	keyStateOld[256];
	// �O��̃f�[�^��ۑ�
	memcpy(keyStateOld, g_keyState, NUM_KEY_MAX);

	// �f�o�C�X����f�[�^���擾
	hr_key = g_pDIDevKeyboard->GetDeviceState(sizeof(g_keyState), g_keyState);
	if(SUCCEEDED(hr_key)){
		for(int cnt = 0; cnt < NUM_KEY_MAX; cnt++){
			// �g���K�[�ƃ����[�X���擾
			g_keyStateTrigger[cnt] = (g_keyState[cnt] & (keyStateOld[cnt] ^ g_keyState[cnt]));
			g_keyStateRelease[cnt] = (keyStateOld[cnt] & (keyStateOld[cnt] ^ g_keyState[cnt]));
			
			// �L�[��������Ă���Ȃ烊�s�[�g�̔��菈��
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
		// �L�[�{�[�h�ւ̃A�N�Z�X�����擾
		hr_key = g_pDIDevKeyboard->Acquire();
	}

	return hr_key;
}

//=============================================================================
// �L�[�{�[�h�̃v���X��Ԃ��擾
//=============================================================================
bool CInput::GetKeyboardPress(int key)
{
	return ((g_keyState[key] & 0x80) != 0);
}

//=============================================================================
// �L�[�{�[�h�̃g���K�[��Ԃ��擾
//=============================================================================
bool CInput::GetKeyboardTrigger(int key)
{
	return ((g_keyStateTrigger[key] & 0x80) != 0);
}

//=============================================================================
// �L�[�{�[�h�̃��s�[�g��Ԃ��擾
//=============================================================================
bool CInput::GetKeyboardRepeat(int key)
{
	return ((g_keyStateRepeat[key] & 0x80) != 0);
}

//=============================================================================
// �L�[�{�[�h�̃����|�X��Ԃ��擾
//=============================================================================
bool CInput::GetKeyboardRelease(int key)
{
	return ((g_keyStateRelease[key] & 0x80) != 0);
}

//=============================================================================
// �}�E�X�̍X�V����
//=============================================================================
HRESULT CInput::UpdateMouse(void)
{
	DIMOUSESTATE2 mouseStateOld;

	// �O��̃f�[�^��ۑ�
	mouseStateOld = g_mouseState;

	// �f�o�C�X����f�[�^���擾
	hr_mouse = g_pDIDevMouse->GetDeviceState(sizeof(g_mouseState), &g_mouseState);
	if(SUCCEEDED(hr_mouse)){
		// �g���K�[�̎擾
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
// �}�E�X�f�[�^�擾(���v���X)
//=============================================================================
bool CInput::GetMouseLeftPress(void)
{
	return ((g_mouseState.rgbButtons[0] & 0x80) != 0);
}

//=============================================================================
// �}�E�X�f�[�^�擾(���g���K�[)
//=============================================================================
bool CInput::GetMouseLeftTrigger(void)
{
	return ((g_mouseStateTrigger.rgbButtons[0] & 0x80) != 0);
}

//=============================================================================
// �}�E�X�f�[�^�擾(�������[�X)
//=============================================================================
bool CInput::GetMouseLeftRelease(void)
{
	return ((g_mouseStateRelease.rgbButtons[0] & 0x80) != 0);
}

//=============================================================================
// �}�E�X�f�[�^�擾(�E�v���X)
//=============================================================================
bool CInput::GetMouseRightPress(void)
{
	return ((g_mouseState.rgbButtons[1] & 0x8) != 00);
}

//=============================================================================
// �}�E�X�f�[�^�擾(�E�g���K�[)
//=============================================================================
bool CInput::GetMouseRightTrigger(void)
{
	return ((g_mouseStateTrigger.rgbButtons[1] & 0x80) != 0);
}

//=============================================================================
// �}�E�X�f�[�^�擾(�����v���X)
//=============================================================================
bool CInput::GetMouseCenterPress(void)
{
	return ((g_mouseState.rgbButtons[2] & 0x80) != 0);
}

//=============================================================================
// �}�E�X�f�[�^�擾(�����g���K�[)
//=============================================================================
bool CInput::GetMouseCenterTrigger(void)
{
	return ((g_mouseState.rgbButtons[2] & 0x80) != 0);
}

//=============================================================================
// �}�E�X�f�[�^�擾(�w���ړ�)
//=============================================================================
long CInput::GetMouseAxisX(void)
{
	return g_mouseState.lX;
}

//=============================================================================
// �}�E�X�f�[�^�擾(�x���ړ�)
//=============================================================================
long CInput::GetMouseAxisY(void)
{
	return g_mouseState.lY;
}

//=============================================================================
// �}�E�X�f�[�^�擾(�y���ړ�)
//=============================================================================
long CInput::GetMouseAxisZ(void)
{
	return g_mouseState.lZ;
}

//=============================================================================
// �w��͈̔͂�Press���ꂽ��
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