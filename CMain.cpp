#include <windows.h>
#include <stdio.h>
#include <process.h>	
#include <memory>
#include <list>
#include "CDirectXGraphics.h" 
#include "CTextureManager.h"
#include "CInput.h"
#include "CDirect3DXFile.h"
#include "CFont.h"
#include "C2DPolygon.h"

// マクロの定義
#define NAME	"win32A"
#define TITLE	"SP42WinterHomework"

#define		SCREEN_X		1600
#define		SCREEN_Y		900
#define		FULLSCREEN		0				// フルスクリーンフラグ

// 頂点フォーマット定義
#define		D3DFVF_CUSTOMVERTEX	   ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX0  )

// リンクライブラリの設定
#pragma comment(lib, "d3d9.lib")			// リンク対象ライブラリにd3d9.libを追加
#pragma comment(lib, "d3dx9.lib")			// リンク対象ライブラリにd3dx9.libを追加
#pragma comment(lib, "winmm.lib")			// リンク対象ライブラリにwinmm.libを追加

// メモリリーク検出
#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

// プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd,			// ウインドウメッセージ関数
			 UINT message, 
			 WPARAM wParam, 
			 LPARAM lParam);

unsigned int   WINAPI GameMain(void  *);							// ゲームメイン関数関数
bool	GameInit(HINSTANCE hinst,HWND hwnd,int width,int height);	// ゲームの初期処理
void	GameExit();													// ゲームの終了処理
void	CALLBACK TimerProc(UINT ,UINT,DWORD,DWORD,DWORD);			// タイマ処理

std::shared_ptr<CFont> pFont;					// フォントの作成
std::shared_ptr<C2DPolygon> m_pPETexture;		// 2Dポリゴン表示クラス

// モデルフォルダパス
const std::string  modelPath = "resource/model/";
// モデルファイルパス
const std::string xFileName[] = {
	"100x100plane.x",
	//"maid_pose.x",
};

// グローバル変数
CDirectXGraphics	g_DXGrobj;								// ＤＩＲＥＣＴＸ　ＧＲＡＰＨＩＣＳ　オブジェクト		
LPD3DXEFFECT		g_pEffect = nullptr;					// エフェクトオブジェクト
std::list<std::shared_ptr<CDirect3DXFile>>		g_pXfile;	// Xファイルマネージャ
std::shared_ptr<CInput>				g_pInput;				// 入力クラス
std::shared_ptr<CTextureManager>	g_pTextureManager;		// テクスチャマネージャ

// 3D描画用の行列
D3DXMATRIX	g_MatView;				// カメラ行列
D3DXMATRIX	g_MatProjection;		// プロジェクション変換行列
D3DXMATRIX	g_MatWorld;				// ワールド変換行列

// ゲームメインスレッド関連
HANDLE			g_hEventHandle;		// イベントハンドル
int				g_timerid=0;		// タイマＩＤ
bool			g_EndFlag=false;	// 終了フラグ
HANDLE			g_hThread;			// スレッドのハンドル値
unsigned int	g_dwThreadId;		// スレッドＩＤ

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

		pFont->PrintChar( "ポストエフェクトとかそういうやつ", 10, 10 ,D3DCOLOR_XRGB( 255,0,255) );
		char buf[256];
		pFont->PrintChar( "操作方法", 10, 200 ,D3DCOLOR_ARGB( 255,255,0,0) );
		pFont->PrintChar( "↑↓:Z軸移動", 10, 220 ,D3DCOLOR_ARGB( 128,255,255,0) );
		pFont->PrintChar( "←→:Y軸回転", 10, 240 ,D3DCOLOR_ARGB( 128,255,255,0) );
		pFont->PrintChar( "WS:Y軸移動", 10, 260 ,D3DCOLOR_ARGB( 128,255,255,0) );
		pFont->PrintChar( "H:ハイトマップ表示/非表示", 10, 300 ,D3DCOLOR_ARGB( 128,255,255,0) );
		pFont->PrintChar( "F1F2:ハイトマップの強度変更", 10, 320 ,D3DCOLOR_ARGB( 128,255,255,0) );
		pFont->PrintChar( "G:ガウスフィルターのOn/Off", 10, 360, D3DCOLOR_ARGB( 128, 255, 255, 0 ) );
		pFont->PrintChar( "F3F4:ガウスフィルターの強度変更", 10, 380 ,D3DCOLOR_ARGB( 128,255,255,0) );
		pFont->PrintChar( "M:モザイクフィルターのOn/Off", 10, 420 ,D3DCOLOR_ARGB( 128,255,255,0) );
		pFont->PrintChar( "F5F6:モザイクフィルターの強度変更", 10, 440 ,D3DCOLOR_ARGB( 128,255,255,0) );
		pFont->PrintChar( "F7F8:HDRの強度変更", 10, 480, D3DCOLOR_ARGB( 128, 255, 255, 0 ) );
		pFont->PrintChar( "Z:被写界深度On/Off", 10, 520, D3DCOLOR_ARGB( 128, 255, 255, 0 ) );
		
		// 右上に表示
		pFont->PrintChar( "デバイス情報", 500, 50, D3DCOLOR_ARGB( 255, 255, 0, 0 ) );
		// マルチレンダリングターゲットの最大数
		D3DCAPS9 Caps;
		g_DXGrobj.GetDXDevice()->GetDeviceCaps( &Caps );
		sprintf_s( buf, "最大マルチレンダリングターゲット:%d", Caps.NumSimultaneousRTs );
		pFont->PrintChar( buf, 500, 70 );
		sprintf_s( buf, "最大テクスチャ解像度:%d×%d", Caps.MaxTextureWidth,Caps.MaxTextureHeight );
		pFont->PrintChar( buf, 500, 90 );

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
	pFont = std::make_shared<CFont>();
	const string fontPath = "resource/font/";	// フォント関係のフォルダパス
	pFont->Load( (fontPath + "ASCII.txt").c_str() );
	pFont->Load( (fontPath + "文字データ.txt" ).c_str() );
	pFont->CreateTexture(16,"しねきゃぷしょん",(fontPath + "cinecaption227.ttf").c_str());
	//pFont->CreateTexture(16,"吐き溜",(fontPath + "hakidame.ttf").c_str() );
	//pFont->CreateTexture( 16, "怨霊", (fontPath + "onryou.TTF").c_str() );

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
