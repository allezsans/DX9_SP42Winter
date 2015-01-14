//========================================================================
// CMenu.cpp
//========================================================================
#include "CMenu.h"

//------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------
CMenu::CMenu():
	m_pFont(NULL),
	m_fPitch(0.0f),
	m_fFontSize(0),
	m_iSelect(0)
{
	m_vPos.x = m_vPos.y = 0.0f;
	m_cMenu.clear();
	m_cMenu.clear();

	m_pFont = std::make_shared<CFont>();
}
//------------------------------------------------------------------------
// デストラクタ
//------------------------------------------------------------------------
CMenu::~CMenu()
{
	if( m_pFont ) {
		m_pFont->Release();
		m_pFont = nullptr;
	}
}


void CMenu::Load( const char* fileName )
{
	m_pFont->Load( fileName );
}
//------------------------------------------------------------------------
// 文字からテクスチャデータを作る
//------------------------------------------------------------------------
void CMenu::CreateTexture(int fontSize,const char* fontType,const char* fontFile)
{
	m_pFont->CreateTexture(fontSize,fontType,fontFile);
	m_fFontSize = fontSize;
}
//------------------------------------------------------------------------
// メニューの登録
//------------------------------------------------------------------------
void CMenu::PushMenu(const char* str,unsigned int key)
{
	VECTOR::iterator it = m_cMenu.begin();
	if( key < m_cMenu.size() ){
		for(unsigned int i=0;i<key;i++){
			++it;
		}
		m_cMenu.insert( it,str );
	}else{
		m_cMenu.push_back( str );
	}
}
//------------------------------------------------------------------------
// メニューの削除
//------------------------------------------------------------------------
void CMenu::PopMenu(unsigned int key)
{
	VECTOR::iterator it = m_cMenu.begin();
	if( m_cMenu.size() > 1 )
	if( key < m_cMenu.size() ){
		for(unsigned int i=0;i<key;i++){
			++it;
		}
		it = m_cMenu.erase( it );
		if( m_iSelect >= (int)key){
			SetPrevMenu();
		}
	}else{
		m_cMenu.pop_back();
		if( m_iSelect == (int)m_cMenu.size()){
			SetPrevMenu();
		}
	}
}
//------------------------------------------------------------------------
// 位置を登録する
//------------------------------------------------------------------------
void CMenu::SetPosition( const float x, const float y)
{
	m_vPos.x = x;
	m_vPos.y = y;
}
//------------------------------------------------------------------------
// 行間を設定する
//------------------------------------------------------------------------
void CMenu::SetPitch( const float fPitch)
{
	m_fPitch = fPitch;
}


void CMenu::SetNextMenu()
{
	if( m_iSelect == (int)m_cMenu.size() - 1 ){
		m_iSelect = 0;
	}else{
		m_iSelect++;
	}
}

void CMenu::SetPrevMenu()
{
	if( m_iSelect == 0 ){
		m_iSelect = (int)m_cMenu.size() - 1;
	}else{
		m_iSelect--;
	}
}
//------------------------------------------------------------------------
// 選ばれているメニューの位置を返す
//------------------------------------------------------------------------
D3DXVECTOR2 CMenu::GetSelectMenu()
{
	D3DXVECTOR2 vPos = m_vPos;
	VECTOR::iterator it = m_cMenu.begin();
	for(int i=0;i<m_iSelect;i++){
		vPos.y += m_fPitch + m_fFontSize;
		++it;
	}
	return vPos;
}
//------------------------------------------------------------------------
// メニューの描画
//------------------------------------------------------------------------
void CMenu::Draw(D3DCOLOR color)
{
	float nextPos = m_vPos.y;
	VECTOR::iterator it = m_cMenu.begin();
	while( it != m_cMenu.end() ){
		m_pFont->PrintChar( (*it),m_vPos.x,nextPos,color);
		nextPos += m_fPitch + m_fFontSize;
		++it;
	}
}