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
	D3DVIEWPORT9 oldView;
	LPDIRECT3DSURFACE9	oldSurface, oldSurfaceZ;

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

		// キー入力の更新
		g_pInput->UpdateInput();

		// ターゲットバッファのクリア、Ｚバッファのクリア
		g_DXGrobj.GetDXDevice()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 );

		// 描画の開始をＤＩＲＥＣＴＸに通知
		g_DXGrobj.GetDXDevice()->BeginScene();

		// Xfileを描画
		for( const auto& xfile : g_pXfile ) {
			xfile->Draw( g_pEffect );
		}

		pInfo->Draw();

		// 描画コマンドをグラフィックスカードへ発行
		g_DXGrobj.GetDXDevice()->EndScene();

		// バックバッファからプライマリバッファへ転送
		hr = g_DXGrobj.GetDXDevice()->Present(NULL,NULL,NULL,NULL);
		if(hr!=D3D_OK){
			g_DXGrobj.GetDXDevice()->Reset(&g_DXGrobj.GetDXD3dpp());
		}
	}
	return 1;
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

	// ＤｉｒｅｃｔＸ　Ｇｒａｐｈｉｃｓオブジェクト初期化
	sts = g_DXGrobj.Init(hwnd,FULLSCREEN,width,height);
	if(!sts){		
		MessageBox(hwnd,"ERROR!!","DirectX 初期化エラー",MB_OK);
		return false;
	}

	// カメラ変換行列作成
	D3DXMatrixLookAtLH(&g_MatView,
						&D3DXVECTOR3( 0.0f, 2.0f, -5.0f ),						// 視点
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

	// 入力クラス作成
	g_pInput = std::make_shared<CInput>( hinst, hwnd );

	// テクスチャマネージャ作成
	g_pTextureManager = std::make_shared<CTextureManager>( g_DXGrobj.GetDXDevice( ) );

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

	// フォントの作成
	pInfo = std::make_shared<CInfo>();

	// 2Dポリゴンの作成
	/*m_pPETexture = std::make_shared<C2DPolygon>();
	m_pPETexture->Load( g_pTexture );
	m_pPETexture->SetTexNo( 0 );
	m_pPETexture->SetSize( SCREEN_X, SCREEN_Y );
	m_pPETexture->SetPos( 0, 0 );*/

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
