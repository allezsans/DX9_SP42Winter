#include "CInfo.h"
#include "C2DPolygon.h"
#include "CDirectxGraphics.h"
#include <string>

extern C2DPolygon* g_Tex;
extern IDirect3DCubeTexture9* g_apCubeMap;
extern CDirectXGraphics g_DXGrobj;

CInfo::CInfo()
{
	const string fontPath = "resource/font/";			// �t�H���g�֌W�̃t�H���_�p�X
	// �^�C�g�������̏�����
	m_pTitle = std::make_shared<CFont>();
	m_pTitle->Load( ( fontPath + "ASCII.txt" ).c_str() );
	m_pTitle->Load( ( fontPath + "�����f�[�^.txt" ).c_str() );
	m_pTitle->CreateTexture( 64, "���˂���Ղ����", ( fontPath + "cinecaption227.ttf" ).c_str() );
	SetTitle( "SP42�~�G�V�F�[�_�[�ۑ�" );

	// �f�o�C�X���̏�����
	m_pDeviceInfo = std::make_shared<CMenu>();
	m_pDeviceInfo->Load( ( fontPath + "ASCII.txt" ).c_str() );
	m_pDeviceInfo->Load( ( fontPath + "�����f�[�^.txt" ).c_str() );
	m_pDeviceInfo->CreateTexture( 32, "���˂���Ղ����", ( fontPath + "cinecaption227.ttf" ).c_str() );
	m_pDeviceInfo->SetPitch( 10.0f );
	m_pDeviceInfo->SetPosition( 10, 100 );
	SetDeviceInfo();

	// ��������̏�����
	m_pUserGuide = std::make_shared<CMenu>();
	m_pUserGuide->Load( ( fontPath + "ASCII.txt" ).c_str() );
	m_pUserGuide->Load( ( fontPath + "�����f�[�^.txt" ).c_str() );
	m_pUserGuide->CreateTexture( 32, "���˂���Ղ����", ( fontPath + "cinecaption227.ttf" ).c_str() );
	m_pUserGuide->SetPitch( 20.0f );
	m_pUserGuide->SetPosition( 10, 400 );
	SetUserGuide();

	// 2D�|���S���̍쐬
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
// @brief:�^�C�g�������̐ݒ�
//------------------------------------------------------
// @author:K.Ito
// @param:�ݒ肷�镶��
// @return:none
//======================================================
void CInfo::SetTitle(string str)
{
	m_sTitle = str;
}

//======================================================
// @brief:�f�o�C�X���̐ݒ�
//------------------------------------------------------
// @author:K.Ito
// @param:none
// @return:none
//======================================================
void CInfo::SetDeviceInfo()
{
	m_pDeviceInfo->PushMenu( "�f�o�C�X���" );
	// �}���`�����_�����O�^�[�Q�b�g�̍ő吔
	D3DCAPS9 Caps;
	g_DXGrobj.GetDXDevice()->GetDeviceCaps( &Caps );
	
	sprintf_s( buf[0], "�ő�}���`�����_�����O�^�[�Q�b�g:%d", Caps.NumSimultaneousRTs );
	m_pDeviceInfo->PushMenu( buf[0] );
	sprintf_s( buf[1], "�ő�e�N�X�`���𑜓x:%d�~%d", Caps.MaxTextureWidth, Caps.MaxTextureHeight );
	m_pDeviceInfo->PushMenu( buf[1] );
	sprintf_s( buf[2], "�o�b�N�o�b�t�@�T�C�Y:%d�~%d", g_DXGrobj.GetDXD3dpp().BackBufferWidth, g_DXGrobj.GetDXD3dpp().BackBufferHeight );
	m_pDeviceInfo->PushMenu( buf[2] );
	sprintf_s( buf[3], "���p�\VRAM:%dMB", g_DXGrobj.GetDXDevice()->GetAvailableTextureMem() / 1024 / 1024 );
	m_pDeviceInfo->PushMenu( buf[3] );
	sprintf_s( buf[4], "HAL:%s", g_DXGrobj.GetDXAdapter().Description );
	m_pDeviceInfo->PushMenu( buf[4] );
}

//======================================================
// @brief:��������̐ݒ�
//------------------------------------------------------
// @author:K.Ito
// @param:none
// @return:none
//======================================================
void CInfo::SetUserGuide()
{
	m_pUserGuide->PushMenu( "������@" );
	m_pUserGuide->PushMenu( "�����F���E��]" );
	m_pUserGuide->PushMenu( "�����F�㉺��]" );
}

//======================================================
// @brief:�`�悷��
//------------------------------------------------------
// @author:K.Ito
// @param:none
// @return:none
//======================================================
void CInfo::Draw()
{
	//g_Tex->Draw();

	m_pTitle->PrintChar( m_sTitle.c_str(), 400, 10, D3DCOLOR_XRGB( 146, 7, 131 ) );
	//m_pTitle->PrintChar( "�L���[�u���}�b�v�e�N�X�`��", 1130, 10, D3DCOLOR_XRGB( 0, 0, 0 ) );
	m_pDeviceInfo->Draw( D3DCOLOR_XRGB( 0, 104, 183 ) );
	m_pUserGuide->Draw( D3DCOLOR_XRGB( 0, 0, 0 ) );
}