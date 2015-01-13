//=========================================================
// CTextureManager.h
// Author:	K.Ito
// Date:	2014/4/26
//=========================================================

#include "CTextureManager.h"

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
CTextureManager::CTextureManager( const LPDIRECT3DDEVICE9 device )
{
	CreateTexture( device );
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
CTextureManager::~CTextureManager()
{
	ReleaseTexture();
}

//---------------------------------------------------------
// �e�N�X�`�����쐬 
//---------------------------------------------------------
bool CTextureManager::CreateTexture( const LPDIRECT3DDEVICE9 device )
{
	for( const auto& name : fileName ) {
		LPDIRECT3DTEXTURE9 tex;
		std::string fullFileName = folderPath + name[0];
		if( FAILED( D3DXCreateTextureFromFile( device , fullFileName.c_str(), &tex ) ) ) {
			std::string Msg = fullFileName + "�̓ǂݍ��ݎ��s";
			MessageBox( nullptr, Msg.c_str() , "ERROR!!", MB_OK );
			return false;
		}
		m_pTexture.push_back( tex );
	}
	return true;
}

//---------------------------------------------------------
// �e�N�X�`�������
//---------------------------------------------------------
bool CTextureManager::ReleaseTexture()
{
	for( auto& tex : m_pTexture ) {
		tex->Release( );
		tex = nullptr;
	}
	m_pTexture.clear();
	return true;
}

//---------------------------------------------------------
// �f�o�C�X�Ƀe�N�X�`�����Z�b�g
//---------------------------------------------------------
bool  CTextureManager::SetTexture( const LPD3DXEFFECT pEffect, const D3DXHANDLE pHandle, eTEXTURE eFile )
{
	int textureNo = static_cast<int>(eFile);
	auto& lpTexture = m_pTexture.begin();
	for( int i = 0; i < textureNo; ++i, ++lpTexture );
	pEffect->SetTexture( pHandle, *lpTexture);
	return true;
}