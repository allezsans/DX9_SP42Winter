//=================================================================================
// C2DPolygon.h
//---------------------------------------------------------------------------------
// 2014/01/31
// @brif UI�\���̂��߂�2D�|���S���N���X�B
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
	MYTEX	m_pTexture;							// �e�N�X�`��
	int		m_iSizeX,m_iSizeY;					// �|���S���T�C�Y
	int		m_iPosX,m_iPosY;					// �|���S���ʒu
	float			m_fAngle;					// ��]�p�x
	int				m_iTexNo;					// �`�悷��e�N�X�`���ԍ�
	LPD3DXEFFECT	m_pEffect;					// �����`��p�G�t�F�N�g�t�@�C��
	float		m_screen_x, m_screen_y;
	// �|���S��
	struct VTX{
		float x,y,z;
		float u,v;
	}				m_sVtx[4];					// �|���S��

	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;	// �`��p�|���S���o�b�t�@

	// �K�E�X�t�B���^�d��
	enum {
		WEIGHT_MUN = 8,		// �d�݂��v�Z�����
	};
	float			m_dispersion_sq;	// ���U�̕�����
	float			m_tbl[WEIGHT_MUN];		// �d�݂̔z��

	// ���U�C�N�d��
	float			m_mozaicWeight;

	//HDR�̏d��
	int				m_hdr;

	// ���[���h�s��
	D3DXMATRIX	m_matrix;
	D3DXMATRIX m_viewMat;
	D3DXMATRIX m_projMat;
public:
	C2DPolygon();
	~C2DPolygon();

	// �V�F�[�_�[�^�C�v
	enum SHADERTYPE{
		eNORMAL,
		eMOZAIC,
		eGAUSS,
		eZBuffer,
		eHDR,
	}ShaderFlg;

	void GaussWeight( float dispersion );

	void Load(const LPSTR pFileName);			// �e�N�X�`���̃��[�h
	void Load(LPDIRECT3DTEXTURE9 pTex);			// �e�N�X�`���̃��[�h

	void Draw(SHADERTYPE ShaderFlg = eNORMAL,D3DCOLOR color = 0xffffffff);		// �|���S���\��

	void Release();								// �e�N�X�`���̔j��

	void SetDepth( float z){ m_sVtx[0].z = m_sVtx[1].z = m_sVtx[2].z = m_sVtx[3].z = z;}
	void SetPos( int iPosX,int iPosY){ m_iPosX = iPosX; m_iPosY = iPosY; }		// �ʒu�̃Z�b�g
	void SetAngle( float fAngle){ m_fAngle = fAngle; }		// �A���O���̃Z�b�g
	void SetSize( int iSizeX,int iSizeY){ m_iSizeX = iSizeX; m_iSizeY = iSizeY; }	// �T�C�Y�̃Z�b�g
	void SetGaussWeight( float weight ) { m_dispersion_sq = weight; }	// �K�E�X�t�B���^�[�̏d�݂��Z�b�g
	void SetMozaicWeight( float weight ) { m_mozaicWeight = weight; }	// ���U�C�N�t�B���^�[�̏d�݂��Z�b�g
	void SetHDRWeight( int weight ) { m_hdr = weight;  }	// HDR�̃E�F�C�g
	void SetTexNo( int iNo );
	void SetMatrix( D3DXMATRIX& mat ) { m_matrix = mat; }
	void SetViewMatrix( D3DXMATRIX& mat ) { m_viewMat = mat; }
	void SetProjMatrix( D3DXMATRIX& mat ) { m_projMat = mat; }
};