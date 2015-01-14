#pragma once
#include	<d3d9.h>
#include	<d3dx9.h>
/*-------------------------------------------------------------------------------
	DirectX Grpahics ��{�N���X
--------------------------------------------------------------------------------*/
class CDirectXGraphics{
private:
	LPDIRECT3D9					m_lpd3d;			// DIRECT3D8�I�u�W�F�N�g
	LPDIRECT3DDEVICE9			m_lpd3ddevice;		// DIRECT3DDEVICE8�I�u�W�F�N�g
	D3DPRESENT_PARAMETERS		m_d3dpp;			//�v���[���e�[�V�����p�����[�^
	D3DDISPLAYMODE				m_disp;				// �f�B�X�v���C���[�h
	D3DADAPTER_IDENTIFIER9		m_adapterInfo;		// �f�B�X�v���C���
	int							m_adapter;			// �f�B�X�v���C�A�_�v�^�ԍ�
	int							m_width;			// �o�b�N�o�b�t�@�w�T�C�Y
	int							m_height;			// �o�b�N�o�b�t�@�x�T�C�Y
public:
	CDirectXGraphics():m_lpd3d(NULL),m_lpd3ddevice(NULL),m_adapter(0),m_width(0),m_height(0){								// �R���X�g���N�^
	}

	bool	Init(HWND hwnd,int fullscreenflag,int width,int height);	// ��������
	void	Exit();														// �I������
	LPDIRECT3DDEVICE9	GetDXDevice() const {							// �f�o�C�X�I�u�W�F�N�g�f�d�s
		return m_lpd3ddevice;
	}
	D3DPRESENT_PARAMETERS GetDXD3dpp() const{							// �v���[���e�[�V�����p�����[�^�f�d�s
		return m_d3dpp;
	}
	D3DADAPTER_IDENTIFIER9 GetDXAdapter() const
	{
		return m_adapterInfo;
	}
};
