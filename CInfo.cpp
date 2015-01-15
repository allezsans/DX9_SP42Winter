#include "CInfo.h"
#include "CDirectxGraphics.h"
#include <string>

extern CDirectXGraphics g_DXGrobj;

CInfo::CInfo()
{
	const string fontPath = "resource/font/";			// フォント関係のフォルダパス
	// タイトル文字の初期化
	m_pTitle = std::make_shared<CFont>();
	m_pTitle->Load( ( fontPath + "ASCII.txt" ).c_str() );
	m_pTitle->Load( ( fontPath + "文字データ.txt" ).c_str() );
	m_pTitle->CreateTexture( 32, "しねきゃぷしょん", ( fontPath + "cinecaption227.ttf" ).c_str() );
	SetTitle( "SP42冬季シェーダー課題" );

	// デバイス情報の初期化
	m_pDeviceInfo = std::make_shared<CMenu>();
	m_pDeviceInfo->Load( ( fontPath + "ASCII.txt" ).c_str() );
	m_pDeviceInfo->Load( ( fontPath + "文字データ.txt" ).c_str() );
	m_pDeviceInfo->CreateTexture( 16, "しねきゃぷしょん", ( fontPath + "cinecaption227.ttf" ).c_str() );
	m_pDeviceInfo->SetPitch( 20.0f );
	m_pDeviceInfo->SetPosition( 10, 100 );
	SetDeviceInfo();

	// 操作説明の初期化
	m_pUserGuide = std::make_shared<CMenu>();
	m_pUserGuide->Load( ( fontPath + "ASCII.txt" ).c_str() );
	m_pUserGuide->Load( ( fontPath + "文字データ.txt" ).c_str() );
	m_pUserGuide->CreateTexture( 16, "しねきゃぷしょん", ( fontPath + "cinecaption227.ttf" ).c_str() );
	m_pUserGuide->SetPitch( 20.0f );
	m_pUserGuide->SetPosition( 10, 400 );
	SetUserGuide();
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
	m_pUserGuide->PushMenu( "説明1" );
	m_pUserGuide->PushMenu( "説明2" );
	m_pUserGuide->PushMenu( "説明3" );
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
	m_pTitle->PrintChar( m_sTitle.c_str(), 600, 10, D3DCOLOR_XRGB( 255, 0, 255 ) );
	m_pDeviceInfo->Draw();
	m_pUserGuide->Draw();
}