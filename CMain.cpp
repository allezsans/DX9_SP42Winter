#define _MAIN_MODULE_
#include "CMain.h"

//////////////////////////
// ＷＩＮＭＡＩＮ関数	//
//////////////////////////
int APIENTRY WinMain(HINSTANCE 	hInstance, 		// アプリケーションインスタンス値
					 HINSTANCE 	hPrevInstance,	// 意味なし
					 LPSTR 		lpszArgs, 		// 起動時の引数文字列
					 int 		nWinMode)		// ウインドウ表示モード
{
	HWND			hwnd;						// ウインドウハンドル
	MSG				msg;						// メッセージ構造体
	WNDCLASSEX		wcex;						// ウインドウクラス構造体

	int				width;						// ウインドウの幅
	int				height;						// ウインドウの高さ

	// メモリリーク検出
	_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF );

	// ウインドウクラス情報のセット
	wcex.hInstance		= hInstance;			// インスタンス値のセット
	wcex.lpszClassName	= NAME;					// クラス名
	wcex.lpfnWndProc	= (WNDPROC)WndProc;		// ウインドウメッセージ関数
	wcex.style			= 0;					// ウインドウスタイル
	wcex.cbSize 		= sizeof(WNDCLASSEX);	// 構造体のサイズ
	wcex.hIcon			= LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);	// ラージアイコン
	wcex.hIconSm		= LoadIcon((HINSTANCE)NULL, IDI_WINLOGO);		// スモールアイコン
	wcex.hCursor		= LoadCursor((HINSTANCE)NULL, IDC_ARROW);		// カーソルスタイル
	wcex.lpszMenuName	= 0; 					// メニューなし
	wcex.cbClsExtra		= 0;					// エキストラなし
	wcex.cbWndExtra		= 0;					
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);		// 背景色白

	if (!RegisterClassEx(&wcex)) return FALSE;	// ウインドウクラスの登録

	timeBeginPeriod(1);			// タイマの分解能力を１ｍｓにする
	g_timerid = timeSetEvent(16,1,TimerProc,1,TIME_PERIODIC);

	// ウインドウサイズを計算
	if(FULLSCREEN){
		width = SCREEN_X;
		height = SCREEN_Y;
	}
	else{
		width = SCREEN_X+GetSystemMetrics(SM_CXDLGFRAME)*2;
		height = SCREEN_Y+GetSystemMetrics(SM_CYDLGFRAME)*2+GetSystemMetrics(SM_CYCAPTION);	
	}

	hwnd = CreateWindow(
				NAME, 							// ウインドウクラスの名前
				TITLE, 							// タイトル
				WS_CAPTION | WS_SYSMENU,		// ウインドウスタイル
				100,100,						// X座標,Y座標
				width,height,					// ウインドウサイズ
				HWND_DESKTOP, 					// 親ウインドウなし
				(HMENU)NULL, 					// メニューなし
				hInstance, 						// インスタンスハンドル
				(LPVOID)NULL);					// 追加引数なし

	if (!hwnd) return FALSE;

	// ウインドウを表示する
	ShowWindow(hwnd, nWinMode);
	UpdateWindow(hwnd);

	// ゲームの初期処理
	if(!GameInit(hInstance,hwnd,width,height)){
		GameExit();
		MessageBox(hwnd,"ERROR!","GameInit Error",MB_OK);
		return false;
	}
	
	// メッセージ･ループ
	while(1){
		// メッセージを取得
		if( !GetMessage( &msg, NULL, 0, 0 ) ) 
		{ 
			break; 
		} 
		else{ 
			TranslateMessage(&msg); 		// キーボード使用を可能にする
			DispatchMessage(&msg); 			// コントロールをＷＩＮＤＯＷＳに戻す
		} 
	}

	g_EndFlag=true;									// 終了フラグをオンにする

	if(g_timerid) timeKillEvent(g_timerid);			// マルチメディアタイマの終了
	timeEndPeriod(1);								// タイマの分解能力もとに戻す

	GameExit();

	return (int)msg.wParam;
}
//////////////////////////
// ゲームメイン関数		//
//////////////////////////
unsigned int	WINAPI GameMain(void* p1)
{

	HRESULT		hr;

	while(1){
		// イベントフラグがセットされるのを待つ　タイムアウト１０００ｍｓ
		DWORD sts=WaitForSingleObject(g_hEventHandle,1000);
		if(sts==WAIT_FAILED){
			break;
		}else if(sts == WAIT_TIMEOUT){
			if(g_EndFlag){
				break;
			}
			continue;
		}

		// キー入力処理
		InputKeyboard();

		// 環境マップレンダリング
		RenderSceneIntoCubeMap( g_DXGrobj.GetDXDevice(), g_spendTime );

		// ターゲットバッファのクリア、Ｚバッファのクリア
		g_DXGrobj.GetDXDevice()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 255, 0, 0 ), 1.0f, 0 );

		// 描画の開始をＤＩＲＥＣＴＸに通知
		g_DXGrobj.GetDXDevice()->BeginScene();

		// シーンレンダリング
		RenderScene( g_DXGrobj.GetDXDevice(), &g_MatView, &g_MatProjection, &g_pTech, true, g_spendTime );

		// デバッグ情報の表示
		pInfo->Draw();

		// 描画コマンドをグラフィックスカードへ発行
		g_DXGrobj.GetDXDevice()->EndScene();

		// バックバッファからプライマリバッファへ転送
		hr = g_DXGrobj.GetDXDevice()->Present(NULL,NULL,NULL,NULL);
		if(hr!=D3D_OK){
			g_DXGrobj.GetDXDevice()->Reset(&g_DXGrobj.GetDXD3dpp());
		}

		g_spendTime += 1/60.0f;
	}
	return 1;
}

//--------------------------------------------------------------------------------------
// キー入力処理
//--------------------------------------------------------------------------------------
void InputKeyboard()
{
	// キー入力の更新
	g_pInput->UpdateInput();

	//------------------モデル回転処理S------------------------
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
	//------------------モデル回転処理E------------------------

	//------------------ライトの強度S------------------------
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
	//------------------ライトの強度E------------------------

	//------------------反射率S------------------------
	if( g_pInput->GetKeyboardPress( DIK_F3 ) ) {
		g_fReflectivity += 0.1f;
		if( g_fReflectivity > 10.0f ) g_fReflectivity = 10.0f;
	}
	if( g_pInput->GetKeyboardPress( DIK_F4 ) ) {
		g_fReflectivity -= 0.1f;
		if( g_fReflectivity < 0.0f ) g_fReflectivity = 0.0f;
	}
	g_pEffect->SetFloat( "g_fReflectivity", g_fReflectivity );
	//------------------反射率E------------------------
}

//--------------------------------------------------------------------------------------
// 環境キューブマップのレンダリング
//--------------------------------------------------------------------------------------
void RenderSceneIntoCubeMap( IDirect3DDevice9* pd3dDevice, double fTime )
{
	// プロジェクションの計算
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

	// サーフェスのリストア
	if( pDSOld ) {
		pd3dDevice->SetDepthStencilSurface( pDSOld );
		SAFE_RELEASE( pDSOld );
	}
	pd3dDevice->SetRenderTarget( 0, pRTOld );
	SAFE_RELEASE( pRTOld );

	pd3dDevice->SetViewport( &OldViewport );
}

//--------------------------------------------------------------------------------------
// シーンのレンダリング
//--------------------------------------------------------------------------------------
void RenderScene( IDirect3DDevice9* pd3dDevice, const D3DXMATRIX* pmView, const D3DXMATRIX* pmProj,
	CTechniqueGroup* pTechGroup, bool bRenderEnvMappedMesh, double fTime )
{
	D3DXMATRIXA16 mWorldView;

	g_pEffect->SetMatrix( "g_mProj", pmProj );

	// ライトの計算
	D3DXVECTOR4 avLightPosView[NUM_LIGHTS];
	for( int i = 0; i < NUM_LIGHTS; ++i ) {
		// ライトの移動を計算
		float fDisp = ( 1.0f + cosf( fmodf( ( float ) fTime, D3DX_PI ) ) ) * 0.5f * g_aLights[i].fMoveDist;
		D3DXVECTOR4 vMove = g_aLights[i].vMoveDir * fDisp;
		D3DXMatrixTranslation( &g_aLights[i].mWorking, vMove.x, vMove.y, vMove.z );
		D3DXMatrixMultiply( &g_aLights[i].mWorking, &g_aLights[i].mWorld, &g_aLights[i].mWorking );
		vMove += g_aLights[i].vPos;
		D3DXVec4Transform( &avLightPosView[i], &vMove, pmView );
	}
	g_pEffect->SetVectorArray( "g_vLightPosView", avLightPosView, NUM_LIGHTS );

	// Xfileを描画
	auto work = g_pXfile.begin();
	if( bRenderEnvMappedMesh ) {
		D3DXMatrixMultiply( &mWorldView, &g_MatWorld, &g_MatView );
		D3DXMatrixMultiply( &mWorldView, &mWorldView, pmView );
		g_pEffect->SetMatrix( "g_mWorldView", &mWorldView );
		g_pEffect->SetTexture( "g_txCubeMap", g_apCubeMap );
		(*work)->Draw( g_pEffect,"RenderHDREnvMap" );
	}
	
	// 部屋を描画
	g_pEffect->SetMatrix( "g_mWorldView", pmView );
	work++;
	(*work)->Draw( g_pEffect,"RenderScene" );
}

//--------------------------------------------------------------------------------------
// キューブマップ作成のための6方向からのカメラ視点
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
// メッセージ処理関数（ウインドウ関数）                       //
//															  //
//	この関数は、ＯＳから呼び出されてメッセージキューから      //
//	メッセージの引渡しを受ける								  //
//															  //
////////////////////////////////////////////////////////////////
LRESULT WINAPI WndProc(HWND hwnd, 				// ウインドウハンドル値
						UINT message,			// メッセージ識別子
						WPARAM wParam,			// 付帯情報１
						LPARAM lParam)			// 付帯情報２
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
// ゲームの初期化処理	//
//////////////////////////
bool GameInit(HINSTANCE hinst,HWND hwnd,int width,int height){

	bool		sts;
	HRESULT hr;

	// ＤｉｒｅｃｔＸ　Ｇｒａｐｈｉｃｓオブジェクト初期化
	sts = g_DXGrobj.Init(hwnd,FULLSCREEN,width,height);
	if(!sts){		
		MessageBox(hwnd,"ERROR!!","DirectX 初期化エラー",MB_OK);
		return false;
	}

	// カメラ変換行列作成
	D3DXMatrixLookAtLH(&g_MatView,
						&D3DXVECTOR3( 0.0f, 0.0f, -2.0f ),  // 視点
						&D3DXVECTOR3(0.0f,0.0f,0.0f),		// 注視点
						&D3DXVECTOR3(0.0f,1.0f,0.0f));		// 上向き

	// プロジェクション変換行列作成
	D3DXMatrixPerspectiveFovLH(&g_MatProjection,				
								D3DX_PI/2,					// 視野角		
								(float)width/(float)height,	// アスペクト比
								1.0f,						// ニアプレーン
								1000.0f);					// ファープレーン

	// Ｚバッファ有効
	g_DXGrobj.GetDXDevice()->SetRenderState(D3DRS_ZENABLE,TRUE);

	// ライト無効化
	g_DXGrobj.GetDXDevice()->SetRenderState(D3DRS_LIGHTING,FALSE);

	// fxファイルの読み込み
	LPD3DXBUFFER pErr = nullptr;		// エラー受け取りバッファ
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

	// 入力クラス作成
	g_pInput = std::make_shared<CInput>( hinst, hwnd );

	// xfileの作成
	for( const auto& filename : xFileName ) {
		g_pXfile.push_back( std::make_shared<CDirect3DXFile>() );
	}
	int no = 0;
	for( const auto& xfile : g_pXfile ){
		std::string Path = modelPath + xFileName[no];
		if( !xfile->LoadXFile( Path.c_str(), g_DXGrobj.GetDXDevice() ) ) {
			std::string errMsg = xFileName[no] + "の読み込みエラー";
			MessageBox( hwnd, errMsg.c_str(),"ERROR!!", MB_OK );
			return false;
		}
		++no;
	}	

	// ライトの設定
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

	// モデルのワールド座標設定
	D3DXMatrixIdentity( &g_MatWorld );
	g_DXGrobj.GetDXDevice()->SetTransform( D3DTS_WORLD, &g_MatWorld );
	g_MatWorld._11 = g_MatWorld._22 = g_MatWorld._33 = 1.0f;
	
	// キューブマップの作成
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

	// レンダリングサーフェスの作成
	g_DXGrobj.GetDXDevice()->CreateDepthStencilSurface(
		256,
		256,
		g_DXGrobj.GetDXD3dpp().AutoDepthStencilFormat,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&g_pDepthCube,
		NULL );

	// デバッグ用情報クラスの作成
	pInfo = std::make_shared<CInfo>();

	// イベントハンドル生成
	g_hEventHandle=CreateEvent(NULL,false,false,NULL);
	if(g_hEventHandle==NULL){
		MessageBox(hwnd,"ERROR!!","CreateEvent エラー",MB_OK);
		return false;
	}

	// スレッド生成(ゲームメイン)
	g_hThread = (HANDLE)_beginthreadex(NULL,0,GameMain,0,0,&g_dwThreadId);
	if(g_hThread==0){
		MessageBox(hwnd,"ERROR!!","beginthreadex エラー",MB_OK);
		return false;
	}

	return	true;
}

//////////////////////////
// ゲームの終了処理		//
//////////////////////////
void GameExit(){

	// ゲームメインスレッドの終了を待つ
	WaitForSingleObject(g_hThread,INFINITE);

	// スレッドハンドルをクローズ
	CloseHandle(g_hThread);

	// イベントハンドルクローズ
	CloseHandle(g_hEventHandle);

	// エフェクトオブジェクトの解放
	if( g_pEffect != nullptr ) {
		g_pEffect->Release();
		g_pEffect = nullptr;
	}

	// 終了処理
	g_DXGrobj.Exit();

}

//////////////////////////
//	タイマーＰＲＯＣ	//
//////////////////////////
void CALLBACK TimerProc(UINT ,UINT,DWORD,DWORD,DWORD){
	// 終了フラグがセットされていなければイベントをセットする
	if(!g_EndFlag){
		SetEvent(g_hEventHandle);
	}
}
