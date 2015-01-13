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

// フォルダパス
const std::string folderPath = "resource/texture/";

// ファイルパス
const std::vector<std::string> fileName[] {
	{ "real.png" },
	{ "real_heightmap.png" },
};

class CTextureManager{
private:
	std::list<LPDIRECT3DTEXTURE9>	m_pTexture;	// テクスチャ
private:
	// テクスチャの作成
	bool CreateTexture( const LPDIRECT3DDEVICE9 device );
	bool ReleaseTexture();
public:
	CTextureManager() {}
	CTextureManager( const LPDIRECT3DDEVICE9 device );
	~CTextureManager();
	// デバイスにセット
	bool SetTexture( const LPD3DXEFFECT pEffect, const D3DXHANDLE pHandle, eTEXTURE eFile );
};