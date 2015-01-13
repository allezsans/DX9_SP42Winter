//=================================================================================
// CFont.h
//---------------------------------------------------------------------------------
// 2013/11/26
// K.Ito
// @brif Windowsに内蔵されたフォントデータからテクスチャを生成し使用します。
// 使用したい文字が書かれたtxtデータを読み込む必要があります。
// マルチバイト文字(日本語)を扱うときはShift-Jis,ANSI形式で保存したtxtファイルを読み込む必要があります。
// UTF-8には未対応です。
//=================================================================================
#pragma once
#define NOMINMAX
#include <map>
#include <fstream>
#include <d3dx9.h>
using namespace std;

typedef map<string,LPDIRECT3DTEXTURE9> MAP;
typedef map<string,GLYPHMETRICS>	MAPINFO;
typedef pair<string,LPDIRECT3DTEXTURE9> PAIR;
typedef pair<string,GLYPHMETRICS> PAIRINFO;

class CCamera;

class CFont{
protected:
	MAP				m_mapTexture;				// 文字をキーとするテクスチャポインタ
	MAPINFO			m_mapInfo;					// 文字の情報
	LPD3DXEFFECT	m_pEffect;					// 文字描画用エフェクトファイル
	struct VTX{
		float x,y,z;
		float u,v;
	}				m_sVtx[4];					// ポリゴン
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;	// 描画用ポリゴンバッファ
	int				m_iFontSize;				// フォントサイズ
	float			screen_x, screen_y;			// スクリーンのサイズ
public:
	enum{										// 文字の表示位置
		eLEFT = -1024,							// 左寄せ
		eCENTER,								// 中央寄せ
		eRIGHT,									// 右寄せ
		eTOP,									// 上寄せ
		eBOTTOM									// 下寄せ
	};
public:
	CFont();
	virtual ~CFont();
	void	Load( const char* fileName );	// 現在のテクスチャを破棄するまで呼び出さないこと。
											// メモリに無駄がでる可能性がある。

	void	CreateTexture(	int fontSize = 32,
							const char* fontType = "HGP創英プレゼンスEB",
							const char* fontFile = NULL);	//	読み込んだ文字からテクスチャを生成する。 

	virtual void PrintChar(const char* str,float x,float y,D3DCOLOR color = 0xffffffff);			// 文字列表示

	void Release();						// テクスチャの破棄
};