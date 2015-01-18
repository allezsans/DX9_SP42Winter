#include "CInfo.h"
#include "C2DPolygon.h"
#include "CDirectxGraphics.h"
#include <string>

extern C2DPolygon* g_Tex;
extern IDirect3DCubeTexture9* g_apCubeMap;
extern CDirectXGraphics g_DXGrobj;

CInfo::CInfo()
{
	const string fontPath = "resource/font/";			// フォント関係のフォルダパス
	// タイトル文字の初期化
	m_pTitle = std::make_shared<CFont>();
	m_pTitle->Load( ( fontPath + "ASCII.txt" ).c_str() );
	m_pTitle->Load( ( fontPath + "文字データ.txt" ).c_str() );
	m_pTitle->CreateTexture( 64, "しねきゃぷしょん", ( fontPath + "cinecaption227.ttf" ).c_str() );
	SetTitle( "SP42冬季シェーダー課題" );

	// デバイス情報の初期化
	m_pDeviceInfo = std::make_shared<CMenu>();
	m_pDeviceInfo->Load( ( fontPath + "ASCII.txt" ).c_str() );
	m_pDeviceInfo->Load( ( fontPath + "文字データ.txt" ).c_str() );
	m_pDeviceInfo->CreateTexture( 32, "しねきゃぷしょん", ( fontPath + "cinecaption227.ttf" ).c_str() );
	m_pDeviceInfo->SetPitch( 10.0f );
	m_pDeviceInfo->SetPosition( 10, 100 );
	SetDeviceInfo();

	// 操作説明の初期化
	m_pUserGuide = std::make_shared<CMenu>();
	m_pUserGuide->Load( ( fontPath + "ASCII.txt" ).c_str() );
	m_pUserGuide->Load( ( fontPath + "文字データ.txt" ).c_str() );
	m_pUserGuide->CreateTexture( 32, "しねきゃぷしょん", ( fontPath + "cinecaption227.ttf" ).c_str() );
	m_pUserGuide->SetPitch( 20.0f );
	m_pUserGuide->SetPosition( 10, 400 );
	SetUserGuide();

	// 2Dポリゴンの作成
	g_Tex = new C2DPolygon;
	g_Tex->SetTexNo( 0 );
	g_Tex->Load( g_apCubeMap );
	g_Tex->SetSize( 512, 512 );
	g_Tex->SetPos( 1600 - 512, 0 );
}


CInfo::~CInfo()
{

}

//======================================================
// @brief:タイトル文字の設定
//------------------------------------------------------
// @author:K.Ito
// @param:設定する文字
// @return:none
//======================================================
void CInfo::SetTitle(string str)
{
	m_sTitle = str;
}

//======================================================
// @brief:デバイス情報の設定
//------------------------------------------------------
// @author:K.Ito
// @param:none
// @return:none
//======================================================
void CInfo::SetDeviceInfo()
{
	m_pDeviceInfo->PushMenu( "デバイス情報" );
	// マルチレンダリングターゲットの最大数
	D3DCAPS9 Caps;
	g_DXGrobj.GetDXDevice()->GetDeviceCaps( &Caps );
	
	sprintf_s( buf[0], "最大マルチレンダリングターゲット:%d", Caps.NumSimultaneousRTs );
	m_pDeviceInfo->PushMenu( buf[0] );
	sprintf_s( buf[1], "最大テクスチャ解像度:%d×%d", Caps.MaxTextureWidth, Caps.MaxTextureHeight );
	m_pDeviceInfo->PushMenu( buf[1] );
	sprintf_s( buf[2], "バックバッファサイズ:%d×%d", g_DXGrobj.GetDXD3dpp().BackBufferWidth, g_DXGrobj.GetDXD3dpp().BackBufferHeight );
	m_pDeviceInfo->PushMenu( buf[2] );
	sprintf_s( buf[3], "利用可能VRAM:%dMB", g_DXGrobj.GetDXDevice()->GetAvailableTextureMem() / 1024 / 1024 );
	m_pDeviceInfo->PushMenu( buf[3] );
	sprintf_s( buf[4], "HAL:%s", g_DXGrobj.GetDXAdapter().Description );
	m_pDeviceInfo->PushMenu( buf[4] );
}

//======================================================
// @brief:操作説明の設定
//------------------------------------------------------
// @author:K.Ito
// @param:none
// @return:none
//======================================================
void CInfo::SetUserGuide()
{
	m_pUserGuide->PushMenu( "操作方法" );
	m_pUserGuide->PushMenu( "←→：左右回転" );
	m_pUserGuide->PushMenu( "↑↓：上下回転" );
}

//======================================================
// @brief:描画する
//------------------------------------------------------
// @author:K.Ito
// @param:none
// @return:none
//======================================================
void CInfo::Draw()
{
	//g_Tex->Draw();

	m_pTitle->PrintChar( m_sTitle.c_str(), 400, 10, D3DCOLOR_XRGB( 146, 7, 131 ) );
	//m_pTitle->PrintChar( "キューブ環境マップテクスチャ", 1130, 10, D3DCOLOR_XRGB( 0, 0, 0 ) );
	m_pDeviceInfo->Draw( D3DCOLOR_XRGB( 0, 104, 183 ) );
	m_pUserGuide->Draw( D3DCOLOR_XRGB( 0, 0, 0 ) );
}