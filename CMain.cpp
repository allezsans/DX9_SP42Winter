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

		// �L�[���͏���
		InputKeyboard();

		// ���}�b�v�����_�����O
		RenderSceneIntoCubeMap( g_DXGrobj.GetDXDevice(), g_spendTime );

		// �^�[�Q�b�g�o�b�t�@�̃N���A�A�y�o�b�t�@�̃N���A
		g_DXGrobj.GetDXDevice()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 255, 0, 0 ), 1.0f, 0 );

		// �`��̊J�n���c�h�q�d�b�s�w�ɒʒm
		g_DXGrobj.GetDXDevice()->BeginScene();

		// �V�[�������_�����O
		RenderScene( g_DXGrobj.GetDXDevice(), &g_MatView, &g_MatProjection, &g_pTech, true, g_spendTime );

		// �f�o�b�O���̕\��
		pInfo->Draw();

		// �`��R�}���h���O���t�B�b�N�X�J�[�h�֔��s
		g_DXGrobj.GetDXDevice()->EndScene();

		// �o�b�N�o�b�t�@����v���C�}���o�b�t�@�֓]��
		hr = g_DXGrobj.GetDXDevice()->Present(NULL,NULL,NULL,NULL);
		if(hr!=D3D_OK){
			g_DXGrobj.GetDXDevice()->Reset(&g_DXGrobj.GetDXD3dpp());
		}

		g_spendTime += 1/60.0f;
	}
	return 1;
}

//--------------------------------------------------------------------------------------
// �L�[���͏���
//--------------------------------------------------------------------------------------
void InputKeyboard()
{
	// �L�[���͂̍X�V
	g_pInput->UpdateInput();

	//------------------���f����]����S------------------------
	D3DXMATRIXA16 workRotateMat;
	D3DXMatrixIdentity( &workRotateMat );
	if( g_pInput->GetKeyboardPress( DIK_UP ) ) {
		D3DXMatrixRotationX( &workRotateMat, 0.1f );
	}
	if( g_pInput->GetKeyboardPress( DIK_DOWN ) ) {
		D3DXMatrixRotationX( &workRotateMat, -0.1f );
	}
	D3DXMatrixMultiply( &g_MatWorld, &g_MatWorld, &workRotateMat );

	if( g_pInput->GetKeyboardPress( DIK_RIGHT ) ) {
		D3DXMatrixRotationY( &workRotateMat, 0.1f );
	}
	if( g_pInput->GetKeyboardPress( DIK_LEFT ) ) {
		D3DXMatrixRotationY( &workRotateMat, -0.1f );
	}
	D3DXMatrixMultiply( &g_MatWorld, &g_MatWorld, &workRotateMat );	
	//------------------���f����]����E------------------------

	//------------------���C�g�̋��xS------------------------
	if( g_pInput->GetKeyboardPress( DIK_F1 ) ) {
		g_fLightIntensity += 0.5f;
		if( g_fLightIntensity >= 100.0f ) g_fLightIntensity = 100.0f;
	}
	if( g_pInput->GetKeyboardPress( DIK_F2 ) ) {
		g_fLightIntensity -= 0.5f;
		if( g_fLightIntensity < 0.0f ) g_fLightIntensity = 0.0f;
	}
	g_vLightIntensity = D3DXVECTOR4( g_fLightIntensity, g_fLightIntensity, g_fLightIntensity, g_fLightIntensity );
	g_pEffect->SetVector( "g_vLightIntensity", &g_vLightIntensity );
	//------------------���C�g�̋��xE------------------------

	//------------------���˗�S------------------------
	if( g_pInput->GetKeyboardPress( DIK_F3 ) ) {
		g_fReflectivity += 0.1f;
		if( g_fReflectivity > 10.0f ) g_fReflectivity = 10.0f;
	}
	if( g_pInput->GetKeyboardPress( DIK_F4 ) ) {
		g_fReflectivity -= 0.1f;
		if( g_fReflectivity < 0.0f ) g_fReflectivity = 0.0f;
	}
	g_pEffect->SetFloat( "g_fReflectivity", g_fReflectivity );
	//------------------���˗�E------------------------
}

//--------------------------------------------------------------------------------------
// ���L���[�u�}�b�v�̃����_�����O
//--------------------------------------------------------------------------------------
void RenderSceneIntoCubeMap( IDirect3DDevice9* pd3dDevice, double fTime )
{
	// �v���W�F�N�V�����̌v�Z
	D3DXMATRIXA16 mProj;
	D3DXMatrixPerspectiveFovLH( &mProj, D3DX_PI * 0.5f, 1.0f, 0.01f, 100.0f );

	D3DXMATRIXA16 mViewDir( g_MatView );
	mViewDir._41 = mViewDir._42 = mViewDir._43 = 0.0f;

	LPDIRECT3DSURFACE9 pRTOld = NULL;
	pd3dDevice->GetRenderTarget( 0, &pRTOld );
	LPDIRECT3DSURFACE9 pDSOld = NULL;
	if( SUCCEEDED( pd3dDevice->GetDepthStencilSurface( &pDSOld ) ) ) {
		pd3dDevice->SetDepthStencilSurface( g_pDepthCube );
	}

	D3DVIEWPORT9 OldViewport, Viewport;
	pd3dDevice->GetViewport( &OldViewport );
	Viewport.Height = 256;
	Viewport.Width = 256;
	Viewport.MaxZ = 1.0f;
	Viewport.MinZ = 0.0f;
	Viewport.X = 0;
	Viewport.Y = 0;
	pd3dDevice->SetViewport( &Viewport );

	for( int nCube = 0; nCube < 1; ++nCube )
		for( int nFace = 0; nFace < 6; ++nFace ) {
			LPDIRECT3DSURFACE9 pSurf;

			g_apCubeMap->GetCubeMapSurface( ( D3DCUBEMAP_FACES ) nFace, 0, &pSurf );
			pd3dDevice->SetRenderTarget( 0, pSurf );
			SAFE_RELEASE( pSurf );

			D3DXMATRIXA16 mView = GetCubeMapViewMatrix( nFace );
			D3DXMatrixMultiply( &mView, &mViewDir, &mView );

			pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 255, 0, 0 ), 1.0f, 0L );

			if( SUCCEEDED( pd3dDevice->BeginScene() ) ) {
				RenderScene( pd3dDevice, &mView, &mProj, &g_pTech, false, fTime );
				pd3dDevice->EndScene();
			}
		}

	// �T�[�t�F�X�̃��X�g�A
	if( pDSOld ) {
		pd3dDevice->SetDepthStencilSurface( pDSOld );
		SAFE_RELEASE( pDSOld );
	}
	pd3dDevice->SetRenderTarget( 0, pRTOld );
	SAFE_RELEASE( pRTOld );

	pd3dDevice->SetViewport( &OldViewport );
}

//--------------------------------------------------------------------------------------
// �V�[���̃����_�����O
//--------------------------------------------------------------------------------------
void RenderScene( IDirect3DDevice9* pd3dDevice, const D3DXMATRIX* pmView, const D3DXMATRIX* pmProj,
	CTechniqueGroup* pTechGroup, bool bRenderEnvMappedMesh, double fTime )
{
	D3DXMATRIXA16 mWorldView;

	g_pEffect->SetMatrix( "g_mProj", pmProj );

	// ���C�g�̌v�Z
	D3DXVECTOR4 avLightPosView[NUM_LIGHTS];
	for( int i = 0; i < NUM_LIGHTS; ++i ) {
		// ���C�g�̈ړ����v�Z
		float fDisp = ( 1.0f + cosf( fmodf( ( float ) fTime, D3DX_PI ) ) ) * 0.5f * g_aLights[i].fMoveDist;
		D3DXVECTOR4 vMove = g_aLights[i].vMoveDir * fDisp;
		D3DXMatrixTranslation( &g_aLights[i].mWorking, vMove.x, vMove.y, vMove.z );
		D3DXMatrixMultiply( &g_aLights[i].mWorking, &g_aLights[i].mWorld, &g_aLights[i].mWorking );
		vMove += g_aLights[i].vPos;
		D3DXVec4Transform( &avLightPosView[i], &vMove, pmView );
	}
	g_pEffect->SetVectorArray( "g_vLightPosView", avLightPosView, NUM_LIGHTS );

	// Xfile��`��
	auto work = g_pXfile.begin();
	if( bRenderEnvMappedMesh ) {
		D3DXMatrixMultiply( &mWorldView, &g_MatWorld, &g_MatView );
		D3DXMatrixMultiply( &mWorldView, &mWorldView, pmView );
		g_pEffect->SetMatrix( "g_mWorldView", &mWorldView );
		g_pEffect->SetTexture( "g_txCubeMap", g_apCubeMap );
		(*work)->Draw( g_pEffect,"RenderHDREnvMap" );
	}
	
	// ������`��
	g_pEffect->SetMatrix( "g_mWorldView", pmView );
	work++;
	(*work)->Draw( g_pEffect,"RenderScene" );
}

//--------------------------------------------------------------------------------------
// �L���[�u�}�b�v�쐬�̂��߂�6��������̃J�������_
//--------------------------------------------------------------------------------------
D3DXMATRIX GetCubeMapViewMatrix( DWORD dwFace )
{
	D3DXVECTOR3 vEyePt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vLookDir;
	D3DXVECTOR3 vUpDir;

	switch( dwFace ) {
		case D3DCUBEMAP_FACE_POSITIVE_X:
			vLookDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
			vUpDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_X:
			vLookDir = D3DXVECTOR3( -1.0f, 0.0f, 0.0f );
			vUpDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Y:
			vLookDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			vUpDir = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Y:
			vLookDir = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
			vUpDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Z:
			vLookDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
			vUpDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Z:
			vLookDir = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
			vUpDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			break;
	}

	D3DXMATRIXA16 mView;
	D3DXMatrixLookAtLH( &mView, &vEyePt, &vLookDir, &vUpDir );
	return mView;
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
	HRESULT hr;

	// �c�����������w�@�f���������������I�u�W�F�N�g������
	sts = g_DXGrobj.Init(hwnd,FULLSCREEN,width,height);
	if(!sts){		
		MessageBox(hwnd,"ERROR!!","DirectX �������G���[",MB_OK);
		return false;
	}

	// �J�����ϊ��s��쐬
	D3DXMatrixLookAtLH(&g_MatView,
						&D3DXVECTOR3( 0.0f, 0.0f, -2.0f ),  // ���_
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
		"shader/HDRCubeMap.fx",
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

	// ���C�g�̐ݒ�
	g_aLights[0].vPos = D3DXVECTOR4( -3.5f, 2.3f, -4.0f, 1.0f );
	g_aLights[0].vMoveDir = D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 0.0f );
	g_aLights[0].fMoveDist = 8.0f;

	g_aLights[1].vPos = D3DXVECTOR4( 3.5f, 2.3f, 4.0f, 1.0f );
	g_aLights[1].vMoveDir = D3DXVECTOR4( 0.0f, 0.0f, -1.0f, 0.0f );
	g_aLights[1].fMoveDist = 8.0f;

	g_aLights[2].vPos = D3DXVECTOR4( -3.5f, 2.3f, 4.0f, 1.0f );
	g_aLights[2].vMoveDir = D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 0.0f );
	g_aLights[2].fMoveDist = 7.0f;

	g_aLights[3].vPos = D3DXVECTOR4( 3.5f, 2.3f, -4.0f, 1.0f );
	g_aLights[3].vMoveDir = D3DXVECTOR4( -1.0f, 0.0f, 0.0f, 0.0f );
	g_aLights[3].fMoveDist = 7.0f;

	g_fLightIntensity = 24.0f;
	g_vLightIntensity = D3DXVECTOR4( g_fLightIntensity, g_fLightIntensity, g_fLightIntensity, g_fLightIntensity );
	g_fReflectivity = 1.0f;

	g_pEffect->SetFloat( "g_fReflectivity", g_fReflectivity );

	g_pEffect->SetVector( "g_vLightIntensity", &g_vLightIntensity );

	D3DXMATRIXA16 mWorld, m;
	for(int i = 0; i < NUM_LIGHTS; ++i ) {
		D3DXMatrixTranslation( &m, g_aLights[i].vPos.x,
			g_aLights[i].vPos.y,
			g_aLights[i].vPos.z );
		D3DXMatrixMultiply( &g_aLights[i].mWorld, &mWorld, &m );
	}

	// ���f���̃��[���h���W�ݒ�
	D3DXMatrixIdentity( &g_MatWorld );
	g_DXGrobj.GetDXDevice()->SetTransform( D3DTS_WORLD, &g_MatWorld );
	g_MatWorld._11 = g_MatWorld._22 = g_MatWorld._33 = 1.0f;
	
	// �L���[�u�}�b�v�̍쐬
	hr = g_DXGrobj.GetDXDevice()->CreateCubeTexture( 
		256,
		1,
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A16B16G16R16F,
		D3DPOOL_DEFAULT,
		&g_apCubeMap,
		NULL );

	if( FAILED( hr ) ) {
		g_DXGrobj.GetDXDevice()->CreateCubeTexture(
			256,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_G16R16F,
			D3DPOOL_DEFAULT,
			&g_apCubeMap,
			NULL );
	}

	// �����_�����O�T�[�t�F�X�̍쐬
	g_DXGrobj.GetDXDevice()->CreateDepthStencilSurface(
		256,
		256,
		g_DXGrobj.GetDXD3dpp().AutoDepthStencilFormat,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&g_pDepthCube,
		NULL );

	// �f�o�b�O�p���N���X�̍쐬
	pInfo = std::make_shared<CInfo>();

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
