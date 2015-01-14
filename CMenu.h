//=======================================================================
// CMenu.h
//-----------------------------------------------------------------------
// CFont���g���������̃��j���[���X�g���쐬
//=======================================================================

#pragma once
#include "CFont.h"
#include <d3dx9.h>
#include <vector>
#include <memory>

typedef vector<const char*> VECTOR;
class CMenu{
private:
	D3DXVECTOR2		m_vPos;			// ���j���[�̍���̈ʒu
	float			m_fPitch;		// �����ƕ����Ƃ̋���
	VECTOR			m_cMenu;		// ������̃}�b�v
	std::shared_ptr<CFont>	m_pFont;		// �t�H���g
	int				m_fFontSize;	// �t�H���g�T�C�Y
	int				m_iSelect;		// ���ݑI�΂�Ă��郁�j���[�ԍ�
public:
	CMenu();					// �R���X�g���N�^
	~CMenu();					// �f�X�g���N�^

	void Load(const char* fileName);				// �g�p���镶����������txt��ǂݍ���

	void CreateTexture(	int fontSize = 32,
						const char* fontType = "HGP�n�p�v���[���XEB",
						const char* fontFile = NULL);	// ��������e�N�X�`���f�[�^�����

	void PushMenu(const char* str,unsigned int key = 1000);

	void PopMenu(unsigned int key = 1000);

	void SetPosition(const float x,const float y);		// �ʒu���Z�b�g����
	void SetPitch( const float fPitch);					// �s�Ԃ��Z�b�g����

	void SetNextMenu();									// ���̃��j���[�ɐi�߂�
	void SetPrevMenu();									// �O�̃��j���[�ɖ߂�
	D3DXVECTOR2	GetSelectMenu();					// �I�΂�Ă��郁�j���[�̍��W��Ԃ�
	int	 GetSelectNum()const{return m_iSelect;}			// �I�΂�Ă��郁�j���[�ԍ�

	void Draw(D3DCOLOR color = 0xffffffff);// �`��
};