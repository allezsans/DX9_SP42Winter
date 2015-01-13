//=========================================================
// CTextureManager.h
// Author:	K.Ito
// Date:	2014/4/26
//=========================================================

#include <string>
#include <list>
#include <vector>
#include <d3dx9.h>

enum class eTEXTURE{
	REAL = 0,
	REAL_HEIGHT,
	MAX_NUM,
};

// �t�H���_�p�X
const std::string folderPath = "resource/texture/";

// �t�@�C���p�X
const std::vector<std::string> fileName[] {
	{ "real.png" },
	{ "real_heightmap.png" },
};

class CTextureManager{
private:
	std::list<LPDIRECT3DTEXTURE9>	m_pTexture;	// �e�N�X�`��
private:
	// �e�N�X�`���̍쐬
	bool CreateTexture( const LPDIRECT3DDEVICE9 device );
	bool ReleaseTexture();
public:
	CTextureManager() {}
	CTextureManager( const LPDIRECT3DDEVICE9 device );
	~CTextureManager();
	// �f�o�C�X�ɃZ�b�g
	bool SetTexture( const LPD3DXEFFECT pEffect, const D3DXHANDLE pHandle, eTEXTURE eFile );
};