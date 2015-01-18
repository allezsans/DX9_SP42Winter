#pragma once
#include "CFont.h"
#include "CMenu.h"
#include <memory>

class CInfo
{
	char buf[5][256];					// ���o�b�t�@
	char varBuf[2][256];			// �ϐ��o�b�t�@

	string m_sTitle;				// �^�C�g��


	shared_ptr<CFont> m_pTitle;		// �^�C�g������
	shared_ptr<CFont> m_pVarInfo;	// �^�C�g������
	shared_ptr<CMenu> m_pDeviceInfo;// �f�o�C�X���
	shared_ptr<CMenu> m_pUserGuide;	// �������

private:
	void SetTitle( string str );	// �^�C�g���̐ݒ�
	void SetDeviceInfo();			// �f�o�C�X���̐ݒ�
	void SetUserGuide();			// ��������̐ݒ�

public:
	CInfo();
	~CInfo();

	void Draw();					// �`��
};

