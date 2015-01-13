#include	"CDirectXGraphics.h"
/*-------------------------------------------------------------------------------
	DirectX Grpaphics �̏���������

		P1 : �E�C���h�E�n���h���l
		P2 : �t���X�N���[���t���O�i�O�F�E�C���h�E���[�h�@�P�F�t���X�N���[���j
		P3 : �E�C���h�E�T�C�Y�i���j
		P4 : �E�C���h�E�T�C�Y�i�����j

		�߂�l
			false : ���s�@
			true  : ����
---------------------------------------------------------------------------------*/
bool CDirectXGraphics::Init(HWND hwnd,int fullscreenflag,int width,int height){
	HRESULT	hr;

	// �c�h�q�d�b�s�R�c�X�C���^�[�t�F�[�X�擾
	m_lpd3d = Direct3DCreate9(D3D_SDK_VERSION);
	if(m_lpd3d==NULL){
        return false;
	}

	//�@�A�_�v�^�̌��݂̃f�B�X�v���C���[�h���擾����
	hr = m_lpd3d->GetAdapterDisplayMode(m_adapter,&m_disp);	
	if(hr!=D3D_OK)
	    return false;

	memset(&m_d3dpp,0,sizeof(m_d3dpp));					// �[���N���A
	m_d3dpp.BackBufferFormat = m_disp.Format;			// ���݂̃r�b�g�[�x
	m_d3dpp.BackBufferWidth  = width;					// �o�b�N�o�b�t�@�̕����Z�b�g	
	m_d3dpp.BackBufferHeight = height;					// �o�b�N�o�b�t�@�̍������Z�b�g
	m_d3dpp.BackBufferCount = 1;						// �o�b�N�o�b�t�@�̐�
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
	m_d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;	// �o�b�N�o�b�t�@�����b�N�\�ɂ���
	if(fullscreenflag){
		m_d3dpp.Windowed = FALSE;							// �t���X�N���[�����[�h
	}
	else{
		m_d3dpp.Windowed = TRUE;							// �E�C���h�E���[�h
	}

	// �y�o�b�t�@�̎����쐬�i�r�b�g�[�x�P�U�j
	m_d3dpp.EnableAutoDepthStencil = 1;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	m_d3dpp.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;	// VSYNC��҂��Ȃ�

	// �f�o�C�X�쐬
	hr = m_lpd3d->CreateDevice(m_adapter,
						D3DDEVTYPE_HAL,
						hwnd,
						D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
						&m_d3dpp,
						&m_lpd3ddevice);
	if(hr!=D3D_OK){
		hr = m_lpd3d->CreateDevice(m_adapter,
								D3DDEVTYPE_HAL,
								hwnd,
								D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
								&m_d3dpp,
								&m_lpd3ddevice);
			if(hr!=D3D_OK){
				hr = m_lpd3d->CreateDevice(m_adapter,
										D3DDEVTYPE_REF,
										hwnd,
										D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
										&m_d3dpp,
										&m_lpd3ddevice);
				if(hr!=D3D_OK){
					return(false);
				}
			}
	}

	m_height = height;
	m_width  = width;

	return(true);
}
/*-------------------------------------------------------------------------------
	DirectX Grpaphics �̏I������
---------------------------------------------------------------------------------*/
void	CDirectXGraphics::Exit()
{
	 if (m_lpd3ddevice != NULL)
	 {
	    m_lpd3ddevice->Release();
		m_lpd3ddevice=NULL;
	 }
	 if (m_lpd3d != NULL)
     {
        m_lpd3d->Release();	
		m_lpd3d=NULL;
    }
	return;
}
