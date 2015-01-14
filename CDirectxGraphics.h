#pragma once
#include	<d3d9.h>
#include	<d3dx9.h>
/*-------------------------------------------------------------------------------
	DirectX Grpahics 基本クラス
--------------------------------------------------------------------------------*/
class CDirectXGraphics{
private:
	LPDIRECT3D9					m_lpd3d;			// DIRECT3D8オブジェクト
	LPDIRECT3DDEVICE9			m_lpd3ddevice;		// DIRECT3DDEVICE8オブジェクト
	D3DPRESENT_PARAMETERS		m_d3dpp;			//プレゼンテーションパラメータ
	D3DDISPLAYMODE				m_disp;				// ディスプレイモード
	D3DADAPTER_IDENTIFIER9		m_adapterInfo;		// ディスプレイ情報
	int							m_adapter;			// ディスプレイアダプタ番号
	int							m_width;			// バックバッファＸサイズ
	int							m_height;			// バックバッファＹサイズ
public:
	CDirectXGraphics():m_lpd3d(NULL),m_lpd3ddevice(NULL),m_adapter(0),m_width(0),m_height(0){								// コンストラクタ
	}

	bool	Init(HWND hwnd,int fullscreenflag,int width,int height);	// 初期処理
	void	Exit();														// 終了処理
	LPDIRECT3DDEVICE9	GetDXDevice() const {							// デバイスオブジェクトＧＥＴ
		return m_lpd3ddevice;
	}
	D3DPRESENT_PARAMETERS GetDXD3dpp() const{							// プレゼンテーションパラメータＧＥＴ
		return m_d3dpp;
	}
	D3DADAPTER_IDENTIFIER9 GetDXAdapter() const
	{
		return m_adapterInfo;
	}
};
