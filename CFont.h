//=================================================================================
// CFont.h
//---------------------------------------------------------------------------------
// 2013/11/26
// K.Ito
// @brif Windows�ɓ������ꂽ�t�H���g�f�[�^����e�N�X�`���𐶐����g�p���܂��B
// �g�p�����������������ꂽtxt�f�[�^��ǂݍ��ޕK�v������܂��B
// �}���`�o�C�g����(���{��)�������Ƃ���Shift-Jis,ANSI�`���ŕۑ�����txt�t�@�C����ǂݍ��ޕK�v������܂��B
// UTF-8�ɂ͖��Ή��ł��B
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
	MAP				m_mapTexture;				// �������L�[�Ƃ���e�N�X�`���|�C���^
	MAPINFO			m_mapInfo;					// �����̏��
	LPD3DXEFFECT	m_pEffect;					// �����`��p�G�t�F�N�g�t�@�C��
	struct VTX{
		float x,y,z;
		float u,v;
	}				m_sVtx[4];					// �|���S��
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;	// �`��p�|���S���o�b�t�@
	int				m_iFontSize;				// �t�H���g�T�C�Y
	float			screen_x, screen_y;			// �X�N���[���̃T�C�Y
public:
	enum{										// �����̕\���ʒu
		eLEFT = -1024,							// ����
		eCENTER,								// ������
		eRIGHT,									// �E��
		eTOP,									// ���
		eBOTTOM									// ����
	};
public:
	CFont();
	virtual ~CFont();
	void	Load( const char* fileName );	// ���݂̃e�N�X�`����j������܂ŌĂяo���Ȃ����ƁB
											// �������ɖ��ʂ��ł�\��������B

	void	CreateTexture(	int fontSize = 32,
							const char* fontType = "HGP�n�p�v���[���XEB",
							const char* fontFile = NULL);	//	�ǂݍ��񂾕�������e�N�X�`���𐶐�����B 

	virtual void PrintChar(const char* str,float x,float y,D3DCOLOR color = 0xffffffff);			// ������\��

	void Release();						// �e�N�X�`���̔j��
};