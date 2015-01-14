#pragma once
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
#include "CInfo.h"

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
LRESULT CALLBACK WndProc( HWND hWnd,			// ウインドウメッセージ関数
	UINT message,
	WPARAM wParam,
	LPARAM lParam );

unsigned int   WINAPI GameMain( void  * );							// ゲームメイン関数関数
bool	GameInit( HINSTANCE hinst, HWND hwnd, int width, int height );	// ゲームの初期処理
void	GameExit();													// ゲームの終了処理
void	CALLBACK TimerProc( UINT, UINT, DWORD, DWORD, DWORD );			// タイマ処理

#ifdef _MAIN_MODULE_
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL std::shared_ptr<CInfo> pInfo;					// フォントの作成
GLOBAL std::shared_ptr<C2DPolygon> m_pPETexture;		// 2Dポリゴン表示クラス

// モデルフォルダパス
const std::string  modelPath = "resource/model/";
// モデルファイルパス
const std::string xFileName[] = {
	"100x100plane.x",
	//"maid_pose.x",
};

// グローバル変数
GLOBAL CDirectXGraphics	g_DXGrobj;								// ＤＩＲＥＣＴＸ　ＧＲＡＰＨＩＣＳ　オブジェクト		
GLOBAL LPD3DXEFFECT		g_pEffect = nullptr;					// エフェクトオブジェクト
GLOBAL std::list<std::shared_ptr<CDirect3DXFile>>		g_pXfile;	// Xファイルマネージャ
GLOBAL std::shared_ptr<CInput>				g_pInput;				// 入力クラス
GLOBAL std::shared_ptr<CTextureManager>	g_pTextureManager;		// テクスチャマネージャ

// 3D描画用の行列
GLOBAL D3DXMATRIX	g_MatView;				// カメラ行列
GLOBAL D3DXMATRIX	g_MatProjection;		// プロジェクション変換行列
GLOBAL D3DXMATRIX	g_MatWorld;				// ワールド変換行列

// ゲームメインスレッド関連
GLOBAL HANDLE			g_hEventHandle;		// イベントハンドル
GLOBAL int				g_timerid = 0;		// タイマＩＤ
GLOBAL bool			g_EndFlag = false;	// 終了フラグ
GLOBAL HANDLE			g_hThread;			// スレッドのハンドル値
GLOBAL unsigned int	g_dwThreadId;		// スレッドＩＤ