//=================================================================================
// C2DPolygon.h
//---------------------------------------------------------------------------------
// 2014/01/31
// @brif UI表示のための2Dポリゴンクラス。
//=================================================================================
#pragma once
#define NOMINMAX
#include <d3dx9.h>
#include <list>
using namespace std;

typedef list<LPDIRECT3DTEXTURE9> MYTEX;

class CCamera;

class C2DPolygon{
protected:
	MYTEX	m_pTexture;							// テクスチャ
	int		m_iSizeX,m_iSizeY;					// ポリゴンサイズ
	int		m_iPosX,m_iPosY;					// ポリゴン位置
	float			m_fAngle;					// 回転角度
	int				m_iTexNo;					// 描画するテクスチャ番号
	LPD3DXEFFECT	m_pEffect;					// 文字描画用エフェクトファイル
	float		m_screen_x, m_screen_y;
	// ポリゴン
	struct VTX{
		float x,y,z;
		float u,v;
	}				m_sVtx[4];					// ポリゴン

	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;	// 描画用ポリゴンバッファ

	// ガウスフィルタ重み
	enum {
		WEIGHT_MUN = 8,		// 重みを計算する個数
	};
	float			m_dispersion_sq;	// 分散の平方根
	float			m_tbl[WEIGHT_MUN];		// 重みの配列

	// モザイク重み
	float			m_mozaicWeight;

	//HDRの重み
	int				m_hdr;

	// ワールド行列
	D3DXMATRIX	m_matrix;
	D3DXMATRIX m_viewMat;
	D3DXMATRIX m_projMat;
public:
	C2DPolygon();
	~C2DPolygon();

	// シェーダータイプ
	enum SHADERTYPE{
		eNORMAL,
		eMOZAIC,
		eGAUSS,
		eZBuffer,
		eHDR,
	}ShaderFlg;

	void GaussWeight( float dispersion );

	void Load(const LPSTR pFileName);			// テクスチャのロード
	void Load(LPDIRECT3DTEXTURE9 pTex);			// テクスチャのロード

	void Draw(SHADERTYPE ShaderFlg = eNORMAL,D3DCOLOR color = 0xffffffff);		// ポリゴン表示

	void Release();								// テクスチャの破棄

	void SetDepth( float z){ m_sVtx[0].z = m_sVtx[1].z = m_sVtx[2].z = m_sVtx[3].z = z;}
	void SetPos( int iPosX,int iPosY){ m_iPosX = iPosX; m_iPosY = iPosY; }		// 位置のセット
	void SetAngle( float fAngle){ m_fAngle = fAngle; }		// アングルのセット
	void SetSize( int iSizeX,int iSizeY){ m_iSizeX = iSizeX; m_iSizeY = iSizeY; }	// サイズのセット
	void SetGaussWeight( float weight ) { m_dispersion_sq = weight; }	// ガウスフィルターの重みをセット
	void SetMozaicWeight( float weight ) { m_mozaicWeight = weight; }	// モザイクフィルターの重みをセット
	void SetHDRWeight( int weight ) { m_hdr = weight;  }	// HDRのウェイト
	void SetTexNo( int iNo );
	void SetMatrix( D3DXMATRIX& mat ) { m_matrix = mat; }
	void SetViewMatrix( D3DXMATRIX& mat ) { m_viewMat = mat; }
	void SetProjMatrix( D3DXMATRIX& mat ) { m_projMat = mat; }
};