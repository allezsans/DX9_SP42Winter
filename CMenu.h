//=======================================================================
// CMenu.h
//-----------------------------------------------------------------------
// CFontを使った文字のメニューリストを作成
//=======================================================================

#pragma once
#include "CFont.h"
#include <d3dx9.h>
#include <vector>
#include <memory>

typedef vector<const char*> VECTOR;
class CMenu{
private:
	D3DXVECTOR2		m_vPos;			// メニューの左上の位置
	float			m_fPitch;		// 文字と文字との距離
	VECTOR			m_cMenu;		// 文字列のマップ
	std::shared_ptr<CFont>	m_pFont;		// フォント
	int				m_fFontSize;	// フォントサイズ
	int				m_iSelect;		// 現在選ばれているメニュー番号
public:
	CMenu();					// コンストラクタ
	~CMenu();					// デストラクタ

	void Load(const char* fileName);				// 使用する文字が入ったtxtを読み込み

	void CreateTexture(	int fontSize = 32,
						const char* fontType = "HGP創英プレゼンスEB",
						const char* fontFile = NULL);	// 文字からテクスチャデータを作る

	void PushMenu(const char* str,unsigned int key = 1000);

	void PopMenu(unsigned int key = 1000);

	void SetPosition(const float x,const float y);		// 位置をセットする
	void SetPitch( const float fPitch);					// 行間をセットする

	void SetNextMenu();									// 次のメニューに進める
	void SetPrevMenu();									// 前のメニューに戻る
	D3DXVECTOR2	GetSelectMenu();					// 選ばれているメニューの座標を返す
	int	 GetSelectNum()const{return m_iSelect;}			// 選ばれているメニュー番号

	void Draw(D3DCOLOR color = 0xffffffff);// 描画
};