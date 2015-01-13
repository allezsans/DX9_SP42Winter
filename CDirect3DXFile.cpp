#include	"CDirect3DXFile.h"

/*-----------------------------
	X File��ǂݍ���
		����
			xfilename  : �w�t�@�C����
			lpd3ddevice : �f�o�C�X�I�u�W�F�N�g
		�߂�l
			true	����
			false   ���s
------------------------------*/
bool CDirect3DXFile::LoadXFile(const char *xfilename, LPDIRECT3DDEVICE9 lpd3ddevice )
{
	LPD3DXBUFFER	pd3dxmtrlbuffer;
	LPD3DXBUFFER	pd3dxmtrlbuffer2;
	HRESULT			hr;
	unsigned int	i;

	// �w�t�@�C����ǂݍ���
	hr = D3DXLoadMeshFromX(	xfilename,				// �w�t�@�C����
							D3DXMESH_SYSTEMMEM,		// �ǂݍ��݃�����
							lpd3ddevice,			// �f�o�C�X�I�u�W�F�N�g
							NULL,
							&pd3dxmtrlbuffer,		// �}�e���A���o�b�t�@
							&pd3dxmtrlbuffer2,		// �}�e���A���o�b�t�@2
							&m_nummaterial,			// �}�e���A����
							&m_lpmesh);				// ���b�V��

	if( SUCCEEDED(hr) ){
		D3DXMATERIAL	*d3dxmaterials = (D3DXMATERIAL*)pd3dxmtrlbuffer->GetBufferPointer();	// �}�e���A���̃A�h���X���擾
		m_lpmeshmaterials = new D3DMATERIAL9[m_nummaterial];		// �}�e���A���R���e�i�̍쐬
		m_lpmeshtextures = new LPDIRECT3DTEXTURE9[m_nummaterial];	// �e�N�X�`���R���e�i�̐���
		
		for( i=0 ; i<m_nummaterial ; i++ ){
			m_lpmeshmaterials[i] = d3dxmaterials[i].MatD3D;
			m_lpmeshmaterials[i].Emissive = m_lpmeshmaterials[i].Specular = m_lpmeshmaterials[i].Ambient = m_lpmeshmaterials[i].Diffuse;

			hr = D3DXCreateTextureFromFile(	lpd3ddevice,
											d3dxmaterials[i].pTextureFilename,
											&m_lpmeshtextures[i]);
			if( FAILED(hr) ){
				m_lpmeshtextures[i]=nullptr;
			}
		}
		pd3dxmtrlbuffer->Release();		// �}�e���A���o�b�t�@�̃����[�X
		pd3dxmtrlbuffer2->Release();	// �}�e���A���o�b�t�@�̃����[�X

		//X�t�@�C���ɖ@�����Ȃ��ꍇ�́A�@������������
		if( !( m_lpmesh->GetFVF() & D3DFVF_NORMAL )
			&& !(m_lpmesh->GetFVF() == 0) ) {

			LPD3DXMESH pTempMesh = nullptr;

			m_lpmesh->CloneMeshFVF( m_lpmesh->GetOptions(),m_lpmesh->GetFVF() | D3DFVF_NORMAL, lpd3ddevice, &pTempMesh );

			D3DXComputeNormals( pTempMesh, nullptr );
			m_lpmesh->Release();
			m_lpmesh = pTempMesh;
		}
	}else{
		return false;
	}
	return true;
}

/*-----------------------------
	X File���t��������������
		����
			�Ȃ�
		�߂�l
			true	����
			false   ���s
------------------------------*/
void CDirect3DXFile::UnLoadXFile()
{
	unsigned int i;

	if(m_lpmeshmaterials!=NULL){	// �}�e���A���I�u�W�F�N�g�̉��
		delete [] m_lpmeshmaterials;
		m_lpmeshmaterials=NULL;
	}

	if(m_lpmeshtextures!=NULL){		// �e�N�X�`���I�u�W�F�N�g�̉��
		for( i=0 ; i<m_nummaterial ; i++ ){
			if(m_lpmeshtextures[i]!=NULL)	m_lpmeshtextures[i]->Release();
		}
		delete [] m_lpmeshtextures;
		m_lpmeshtextures=NULL;
	}

	if(m_lpmesh!=NULL){		// ���b�V�����
		m_lpmesh->Release();
		m_lpmesh=NULL;
	}
}

/*-----------------------------
	X File��`�悷��(�Œ�p�C�v���C��)
		����
			lpd3ddevice :	�f�o�C�X�I�u�W�F�N�g
		�߂�l
			�Ȃ�
------------------------------*/
void CDirect3DXFile::Draw(LPDIRECT3DDEVICE9 lpd3ddevice)
{
	unsigned int i;
	for( i=0 ; i<m_nummaterial ; i++ ){
		lpd3ddevice->SetMaterial(&m_lpmeshmaterials[i]);	// �}�e���A���̃Z�b�g
		lpd3ddevice->SetTexture(0,m_lpmeshtextures[i]);		// �e�N�X�`���̃Z�b�g
		m_lpmesh->DrawSubset(i);							// �T�u�Z�b�g�̕`��
	}
}

/*-----------------------------
X File��`�悷��
����
lpd3ddevice :	�f�o�C�X�I�u�W�F�N�g
�߂�l
�Ȃ�
------------------------------*/
void CDirect3DXFile::Draw( LPD3DXEFFECT pEffect )
{
	unsigned int i;
	for( i = 0; i < m_nummaterial; i++ ) {
		float	diffuse[4] = { m_lpmeshmaterials[i].Diffuse.r, m_lpmeshmaterials[i].Diffuse.g, m_lpmeshmaterials[i].Diffuse.b, m_lpmeshmaterials[i].Diffuse.a };
		float	specular[4] = { m_lpmeshmaterials[i].Specular.r, m_lpmeshmaterials[i].Specular.g, m_lpmeshmaterials[i].Specular.b, m_lpmeshmaterials[i].Specular.a };
		pEffect->SetFloatArray( "g_Diffuse_mat", diffuse, 4 );			// �}�e���A���̃Z�b�g
		pEffect->SetFloatArray( "g_Specular_mat", specular, 4 );		// �}�e���A���̃Z�b�g
		// �e�N�X�`���̃Z�b�g
		if( m_lpmeshtextures[i] != nullptr ){
			pEffect->SetTechnique( "basic" );
			pEffect->SetTexture( "tex0_data", m_lpmeshtextures[i] );
		}
		else {
			pEffect->SetTechnique( "basic_notexture" );
		}
		//pEffect->CommitChanges();
		UINT passNum = 0;
		pEffect->Begin( 0, 0 );
		pEffect->BeginPass( 0 );
		m_lpmesh->DrawSubset( i );
		pEffect->EndPass();
		pEffect->End();
	}
}

/*-----------------------------
X File�̉e���e�N�X�`���ɕ`�悷��
����
lpd3ddevice :	�f�o�C�X�I�u�W�F�N�g
�߂�l
�Ȃ�
------------------------------*/
void CDirect3DXFile::DrawShadow( LPD3DXEFFECT pEffect )
{
	unsigned int i;
	for( i = 0; i<m_nummaterial; i++ ) {
		pEffect->SetTechnique( "shadow" );
		// pEffect->CommitChanges();
		pEffect->Begin( 0, 0 );
		pEffect->BeginPass( 0 );
		m_lpmesh->DrawSubset( i );
		pEffect->EndPass();
		pEffect->End();
	}
}

/*-----------------------------
	X File��`�悷��(3����`�悷��)
		����
			lpd3ddevice :	�f�o�C�X�I�u�W�F�N�g
		�߂�l
			�Ȃ�
------------------------------*/
void CDirect3DXFile::DrawWithAxis(LPDIRECT3DDEVICE9 lpd3ddevice)
{
	unsigned int i;

	DrawAxis(lpd3ddevice);	// �R���̕`��

	for(i=0;i<m_nummaterial;i++){	// �T�u�Z�b�g�̕`��
		lpd3ddevice->SetMaterial(&m_lpmeshmaterials[i]);	// �}�e���A���̃Z�b�g
		lpd3ddevice->SetTexture(0,m_lpmeshtextures[i]);		// �e�N�X�`���̃Z�b�g
		m_lpmesh->DrawSubset(i);							// �T�u�Z�b�g�̕`��
	}
}

/*---------------------
	�R���̕`��
		����
			lpd3ddevice :	�f�o�C�X�I�u�W�F�N�g
		�߂�l
			�Ȃ�
---------------------*/
void CDirect3DXFile::DrawAxis(LPDIRECT3DDEVICE9 lpd3ddevice)
{
	struct VERTEXDATA{
		float		x,y,z;
		D3DCOLOR	color;
	};

	// �R���̒��_�f�[�^
	struct	VERTEXDATA	linevertices[6]={
		-400,   0,   0,  D3DCOLOR_XRGB(255,0,0),	// �w���n�_ ��
		 400,   0,   0,  D3DCOLOR_XRGB(255,0,0),	// �w���I�_ ��
		   0,-400,   0,  D3DCOLOR_XRGB(0,255,0),	// �x���n�_ ��
		   0, 400,   0,  D3DCOLOR_XRGB(0,255,0),	// �x���I�_ ��
		   0,   0,-400,  D3DCOLOR_XRGB(0,0,255),	// �y���n�_ ��
		   0,   0, 400,  D3DCOLOR_XRGB(0,0,255)		// �y���I�_ ��
	};

	lpd3ddevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);	// ���_�t�H�[�}�b�g���Z�b�g

	lpd3ddevice->DrawPrimitiveUP(D3DPT_LINELIST, 3, &linevertices[0], sizeof(VERTEXDATA));	// �R����`��
}

LPD3DXMESH CDirect3DXFile::GetMesh() const{
	return m_lpmesh;
}