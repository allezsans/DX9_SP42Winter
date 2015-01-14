#pragma once
#include "CFont.h"
#include "CMenu.h"
#include <memory>

class CInfo
{
	string m_sTitle;				// タイトル

	shared_ptr<CFont> m_pTitle;		// タイトル文字
	shared_ptr<CMenu> m_pDeviceInfo;// デバイス情報
	shared_ptr<CMenu> m_pUserGuide;	// 操作説明

private:
	void SetTitle( string str );	// タイトルの設定
	void SetDeviceInfo();			// デバイス情報の設定
	void SetUserGuide();			// 操作説明の設定

public:
	CInfo();
	~CInfo();

	void Draw();					// 描画
};

