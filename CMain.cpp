#define _MAIN_MODULE_
#include "CMain.h"

//////////////////////////
// �v�h�m�l�`�h�m�֐�	//
//////////////////////////
int APIENTRY WinMain(HINSTANCE 	hInstance, 		// �A�v���P�[�V�����C���X�^���X�l
					 HINSTANCE 	hPrevInstance,	// �Ӗ��Ȃ�
					 LPSTR 		lpszArgs, 		// �N�����̈���������
					 int 		nWinMode)		// �E�C���h�E�\�����[�h
{
	HWND			hwnd;						// �E�C���h�E�n���h��
	MSG				msg;						// ���b�Z�[�W�\����
	WNDCLASSEX		wcex;						// �E�C���h�E�N���X�\����

	int				width;						// �E�C���h�E�̕�
	int				height;						// �E�C���h�E�̍���

	// ���������[�N���o
	_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF );

	// �E�C���h�E�N���X���̃Z�b�g
	wcex.hInstance		= hInstance;			// �C���X�^���X�l�̃Z�b�g
	wcex.lpszClassName	= NAME;					// �N���X��
	wcex.lpfnWndProc	= (WNDPROC)WndProc;		// �E�C���h�E���b�Z�[�W�֐�
	wcex.style			= 0;					// �E�C���h�E�X�^�C��
	wcex.cbSize 		= sizeof(WNDCLASSEX);	// �\���̂̃T�C�Y
	wcex.hIcon			= LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);	// ���[�W�A�C�R��
	wcex.hIconSm		= LoadIcon((HINSTANCE)NULL, IDI_WINLOGO);		// �X���[���A�C�R��
	wcex.hCursor		= LoadCursor((HINSTANCE)NULL, IDC_ARROW);		// �J�[�\���X�^�C��
	wcex.lpszMenuName	= 0; 					// ���j���[�Ȃ�
	wcex.cbClsExtra		= 0;					// �G�L�X�g���Ȃ�
	wcex.cbWndExtra		= 0;					
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);		// �w�i�F��

	if (!RegisterClassEx(&wcex)) return FALSE;	// �E�C���h�E�N���X�̓o�^

	timeBeginPeriod(1);			// �^�C�}�̕���\�͂��P�����ɂ���
	g_timerid = timeSetEvent(16,1,TimerProc,1,TIME_PERIODIC);

	// �E�C���h�E�T�C�Y���v�Z
	if(FULLSCREEN){
		width = SCREEN_X;
		height = SCREEN_Y;
	}
	else{
		width = SCREEN_X+GetSystemMetrics(SM_CXDLGFRAME)*2;
		height = SCREEN_Y+GetSystemMetrics(SM_CYDLGFRAME)*2+GetSystemMetrics(SM_CYCAPTION);	
	}

	hwnd = CreateWindow(
				NAME, 							// �E�C���h�E�N���X�̖��O
				TITLE, 							// �^�C�g��
				WS_CAPTION | WS_SYSMENU,		// �E�C���h�E�X�^�C��
				100,100,						// X���W,Y���W
				width,height,					// �E�C���h�E�T�C�Y
				HWND_DESKTOP, 					// �e�E�C���h�E�Ȃ�
				(HMENU)NULL, 					// ���j���[�Ȃ�
				hInstance, 						// �C���X�^���X�n���h��
				(LPVOID)NULL);					// �ǉ������Ȃ�

	if (!hwnd) return FALSE;

	// �E�C���h�E��\������
	ShowWindow(hwnd, nWinMode);
	UpdateWindow(hwnd);

	// �Q�[���̏�������
	if(!GameInit(hInstance,hwnd,width,height)){
		GameExit();
		MessageBox(hwnd,"ERROR!","GameInit Error",MB_OK);
		return false;
	}
	
	// ���b�Z�[�W����[�v
	while(1){
		// ���b�Z�[�W���擾
		if( !GetMessage( &msg, NULL, 0, 0 ) ) 
		{ 
			break; 
		} 
		else{ 
			TranslateMessage(&msg); 		// �L�[�{�[�h�g�p���\�ɂ���
			DispatchMessage(&msg); 			// �R���g���[�����v�h�m�c�n�v�r�ɖ߂�
		} 
	}

	g_EndFlag=true;									// �I���t���O���I���ɂ���

	if(g_timerid) timeKillEvent(g_timerid);			// �}���`���f�B�A�^�C�}�̏I��
	timeEndPeriod(1);								// �^�C�}�̕���\�͂��Ƃɖ߂�

	GameExit();

	return (int)msg.wParam;
}
//////////////////////////
// �Q�[�����C���֐�		//
//////////////////////////
unsigned int	WINAPI GameMain(void* p1)
{

	HRESULT		hr;
	D3DVIEWPORT9 oldView;
	LPDIRECT3DSURFACE9	oldSurface, oldSurfaceZ;

	while(1){
		// �C�x���g�t���O���Z�b�g�����̂�҂@�^�C���A�E�g�P�O�O�O����
		DWORD sts=WaitForSingleObject(g_hEventHandle,1000);
		if(sts==WAIT_FAILED){
			break;
		}else if(sts == WAIT_TIMEOUT){
			if(g_EndFlag){
				break;
			}
			continue;
		}

		// �L�[���͂̍X�V
		g_pInput->UpdateInput();

		// �^�[�Q�b�g�o�b�t�@�̃N���A�A�y�o�b�t�@�̃N���A
		g_DXGrobj.GetDXDevice()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 );

		// �`��̊J�n���c�h�q�d�b�s�w�ɒʒm
		g_DXGrobj.GetDXDevice()->BeginScene();

		// Xfile��`��
		for( const auto& xfile : g_pXfile ) {
			xfile->Draw( g_pEffect );
		}

		pInfo->Draw();

		// �`��R�}���h���O���t�B�b�N�X�J�[�h�֔��s
		g_DXGrobj.GetDXDevice()->EndScene();

		// �o�b�N�o�b�t�@����v���C�}���o�b�t�@�֓]��
		hr = g_DXGrobj.GetDXDevice()->Present(NULL,NULL,NULL,NULL);
		if(hr!=D3D_OK){
			g_DXGrobj.GetDXDevice()->Reset(&g_DXGrobj.GetDXD3dpp());
		}
	}
	return 1;
}

////////////////////////////////////////////////////////////////
// ���b�Z�[�W�����֐��i�E�C���h�E�֐��j                       //
//															  //
//	���̊֐��́A�n�r����Ăяo����ă��b�Z�[�W�L���[����      //
//	���b�Z�[�W�̈��n�����󂯂�								  //
//															  //
////////////////////////////////////////////////////////////////
LRESULT WINAPI WndProc(HWND hwnd, 				// �E�C���h�E�n���h���l
						UINT message,			// ���b�Z�[�W���ʎq
						WPARAM wParam,			// �t�я��P
						LPARAM lParam)			// �t�я��Q
{
	switch (message){
		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_ESCAPE:
					PostQuitMessage(0);
					break;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

//////////////////////////
// �Q�[���̏���������	//
//////////////////////////
bool GameInit(HINSTANCE hinst,HWND hwnd,int width,int height){

	bool		sts;

	// �c�����������w�@�f���������������I�u�W�F�N�g������
	sts = g_DXGrobj.Init(hwnd,FULLSCREEN,width,height);
	if(!sts){		
		MessageBox(hwnd,"ERROR!!","DirectX �������G���[",MB_OK);
		return false;
	}

	// �J�����ϊ��s��쐬
	D3DXMatrixLookAtLH(&g_MatView,
						&D3DXVECTOR3( 0.0f, 2.0f, -5.0f ),						// ���_
						&D3DXVECTOR3(0.0f,0.0f,0.0f),		// �����_
						&D3DXVECTOR3(0.0f,1.0f,0.0f));		// �����

	// �v���W�F�N�V�����ϊ��s��쐬
	D3DXMatrixPerspectiveFovLH(&g_MatProjection,				
								D3DX_PI/2,					// ����p		
								(float)width/(float)height,	// �A�X�y�N�g��
								1.0f,						// �j�A�v���[��
								1000.0f);					// �t�@�[�v���[��

	// �y�o�b�t�@�L��
	g_DXGrobj.GetDXDevice()->SetRenderState(D3DRS_ZENABLE,TRUE);

	// ���C�g������
	g_DXGrobj.GetDXDevice()->SetRenderState(D3DRS_LIGHTING,FALSE);

	// fx�t�@�C���̓ǂݍ���
	LPD3DXBUFFER pErr = nullptr;		// �G���[�󂯎��o�b�t�@
	if( FAILED( D3DXCreateEffectFromFile( 
		g_DXGrobj.GetDXDevice(),
		"shader/basic.fx",
		NULL,
		NULL,
		NULL,
		NULL,
		&g_pEffect,
		&pErr ) ) ) {
		MessageBox( hwnd, ( LPCTSTR ) pErr->GetBufferPointer( ), "ERROR!!", MB_OK );
		return false;
	}

	// ���̓N���X�쐬
	g_pInput = std::make_shared<CInput>( hinst, hwnd );

	// �e�N�X�`���}�l�[�W���쐬
	g_pTextureManager = std::make_shared<CTextureManager>( g_DXGrobj.GetDXDevice( ) );

	// xfile�̍쐬
	for( const auto& filename : xFileName ) {
		g_pXfile.push_back( std::make_shared<CDirect3DXFile>() );
	}
	int no = 0;
	for( const auto& xfile : g_pXfile ){
		std::string Path = modelPath + xFileName[no];
		if( !xfile->LoadXFile( Path.c_str(), g_DXGrobj.GetDXDevice() ) ) {
			std::string errMsg = xFileName[no] + "�̓ǂݍ��݃G���[";
			MessageBox( hwnd, errMsg.c_str(),"ERROR!!", MB_OK );
			return false;
		}
		++no;
	}

	// �t�H���g�̍쐬
	pInfo = std::make_shared<CInfo>();

	// 2D�|���S���̍쐬
	/*m_pPETexture = std::make_shared<C2DPolygon>();
	m_pPETexture->Load( g_pTexture );
	m_pPETexture->SetTexNo( 0 );
	m_pPETexture->SetSize( SCREEN_X, SCREEN_Y );
	m_pPETexture->SetPos( 0, 0 );*/

	// �C�x���g�n���h������
	g_hEventHandle=CreateEvent(NULL,false,false,NULL);
	if(g_hEventHandle==NULL){
		MessageBox(hwnd,"ERROR!!","CreateEvent �G���[",MB_OK);
		return false;
	}

	// �X���b�h����(�Q�[�����C��)
	g_hThread = (HANDLE)_beginthreadex(NULL,0,GameMain,0,0,&g_dwThreadId);
	if(g_hThread==0){
		MessageBox(hwnd,"ERROR!!","beginthreadex �G���[",MB_OK);
		return false;
	}

	return	true;
}

//////////////////////////
// �Q�[���̏I������		//
//////////////////////////
void GameExit(){

	// �Q�[�����C���X���b�h�̏I����҂�
	WaitForSingleObject(g_hThread,INFINITE);

	// �X���b�h�n���h�����N���[�Y
	CloseHandle(g_hThread);

	// �C�x���g�n���h���N���[�Y
	CloseHandle(g_hEventHandle);

	// �G�t�F�N�g�I�u�W�F�N�g�̉��
	if( g_pEffect != nullptr ) {
		g_pEffect->Release();
		g_pEffect = nullptr;
	}

	// �I������
	g_DXGrobj.Exit();

}

//////////////////////////
//	�^�C�}�[�o�q�n�b	//
//////////////////////////
void CALLBACK TimerProc(UINT ,UINT,DWORD,DWORD,DWORD){
	// �I���t���O���Z�b�g����Ă��Ȃ���΃C�x���g���Z�b�g����
	if(!g_EndFlag){
		SetEvent(g_hEventHandle);
	}
}
