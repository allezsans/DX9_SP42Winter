//=================================================================================
// C2DPolygon.cpp
//---------------------------------------------------------------------------------
#include "C2DPolygon.h"
#include "CDirectxGraphics.h"
extern CDirectXGraphics	g_DXGrobj;
//---------------------------------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------------------------------
C2DPolygon::C2DPolygon():
	m_pEffect(NULL),
	m_pVertexBuffer(NULL),
	m_iTexNo(0),
	m_screen_x(800),
	m_screen_y(600),
	m_dispersion_sq(0.1f),
	m_hdr(0)
{
	LPD3DXBUFFER pErr=NULL;
	if( FAILED( D3DXCreateEffectFromFile(	g_DXGrobj.GetDXDevice(),
											"shader/Polygon.fx",
											NULL,
											NULL,
											0,
											NULL,
											&m_pEffect,
											&pErr ) ) ){
		MessageBox(nullptr,(LPCTSTR)pErr->GetBufferPointer(),"ERROR!!",MB_OK);
	}
	if( pErr != NULL )	pErr->Release();

	m_iSizeX  = 0;
	m_iSizeY  = 0;
	m_iPosX = 0;
	m_iPosY = 0;
	m_fAngle = 0.0f;

	m_sVtx[0].x = 0.0f;
	m_sVtx[0].y = -1.0f;
	m_sVtx[0].z = 1.0f;
	m_sVtx[0].u = 0.0f;
	m_sVtx[0].v = 1.0f;
	m_sVtx[1].x = 0.0f;
	m_sVtx[1].y = 0.0f;
	m_sVtx[1].z = 1.0f;
	m_sVtx[1].u = 0.0f;
	m_sVtx[1].v = 0.0f;
	m_sVtx[2].x = 1.0f;
	m_sVtx[2].y = -1.0f;
	m_sVtx[2].z = 1.0f;
	m_sVtx[2].u = 1.0f;
	m_sVtx[2].v = 1.0f;
	m_sVtx[3].x = 1.0f;
	m_sVtx[3].y = 0.0f;
	m_sVtx[3].z = 1.0f;
	m_sVtx[3].u = 1.0f;
	m_sVtx[3].v = 0.0f;

	VTX *p = 0;
		
	//UV�̃o�b�t�@���쐬
	if(FAILED(g_DXGrobj.GetDXDevice()->CreateVertexBuffer(
		sizeof( VTX ) * 4,
		0,
		0,
		D3DPOOL_MANAGED,
		&m_pVertexBuffer,
		NULL ))){
			MessageBox(nullptr,"2D�|���S���o�b�t�@�̍쐬�Ɏ��s���܂����B","ERROR!!",MB_OK);
			return;
	}
	m_pVertexBuffer->Lock(0, 0, (void**)&p, 0);
	memcpy( p, m_sVtx, sizeof(VTX) *4 );
	m_pVertexBuffer->Unlock();
}

//--------------------------------------------------------------------------------
// �f�X�g���N�^
//--------------------------------------------------------------------------------
C2DPolygon::~C2DPolygon()
{
	Release();
}

/*--------------------------------------------------------------
�K�E�V�A���t�B���^�[�̏d�݂��v�Z
---------------------------------------------------------------*/
void C2DPolygon::GaussWeight( float dispersion )
{
	DWORD i;

	float total = 0;
	for( i = 0; i<WEIGHT_MUN; i++ ) {
		m_tbl[i] = expf( -0.5f*( FLOAT ) ( i*i ) / dispersion );
		if( 0 == i ) {
			total += m_tbl[i];
		}
		else {
			// ���S�ȊO�́A�Q�񓯂��W�����g���̂łQ�{
			total += 2.0f*m_tbl[i];
		}
	}
	// �K�i��
	for( i = 0; i<WEIGHT_MUN; i++ ) m_tbl[i] /= total;

	if( m_pEffect ) m_pEffect->SetFloatArray( "weight", m_tbl, WEIGHT_MUN );
}

//--------------------------------------------------------------------------------
// �e�N�X�`���̃��[�h
//--------------------------------------------------------------------------------
void C2DPolygon::Load(const LPSTR pFileName)
{
	LPDIRECT3DTEXTURE9	pTex;

	HRESULT hr;

	hr = D3DXCreateTextureFromFile( g_DXGrobj.GetDXDevice() ,
								pFileName,
								&pTex);

	if( FAILED(hr) ){
		MessageBox(nullptr,"2D�|���S���e�N�X�`���̓ǂݍ��݂Ɏ��s���܂����B","ERROR!!",MB_OK);
			return;
	}

	m_pTexture.push_back(pTex);
}

//--------------------------------------------------------------------------------
// �e�N�X�`���̃��[�h
//--------------------------------------------------------------------------------
void C2DPolygon::Load(LPDIRECT3DTEXTURE9 pTex)
{
	m_pTexture.push_back(pTex);
}

//----------------------------------------------------------------------------------------
// �\��
//----------------------------------------------------------------------------------------
void C2DPolygon::Draw(SHADERTYPE ShaderFlg,D3DCOLOR color)
{
	HRESULT hr;

	LPDIRECT3DVERTEXDECLARATION9	pDecl;
	D3DVERTEXELEMENT9 elems[] = {
		{// �X�g���[�� 0 ���_
			0,
			0,
			D3DDECLTYPE_FLOAT3,
			D3DDECLMETHOD_DEFAULT,
			D3DDECLUSAGE_POSITION,
			0
		},
		{// �X�g���[��0 UV
			0,
			12,
			D3DDECLTYPE_FLOAT2,
			D3DDECLMETHOD_DEFAULT,
			D3DDECLUSAGE_TEXCOORD,
			0
		},
		D3DDECL_END()
	};

	g_DXGrobj.GetDXDevice()->CreateVertexDeclaration( elems, &pDecl );
	g_DXGrobj.GetDXDevice()->SetVertexDeclaration( pDecl );

	g_DXGrobj.GetDXDevice()->SetStreamSource( 0, m_pVertexBuffer, 0, sizeof( VTX ) );

	D3DXMATRIX matPolygon;
	D3DXMatrixIdentity(&matPolygon);
	D3DXMATRIX matScale;
	D3DXMatrixIdentity(&matScale);
	D3DXMATRIX matRot;
	D3DXMatrixIdentity(&matRot);
	D3DXMATRIX matTrans;
	D3DXMatrixIdentity(&matTrans);

	D3DXMATRIX	matRotTemp1,matRotTemp2;
	D3DXMatrixIdentity(&matRotTemp1);
	D3DXMatrixIdentity(&matRotTemp2);

	D3DXMatrixScaling( &matScale, (float)m_iSizeX, (float)m_iSizeY, 1.0f );
	D3DXMatrixTranslation( &matRotTemp1, (float)-m_iSizeX/2, (float)m_iSizeY/2, 0.0f );
	D3DXMatrixRotationZ( &matRot, -m_fAngle );
	D3DXMatrixTranslation( &matRotTemp2, (float)m_iSizeX/2, (float)-m_iSizeY/2, 0.0f );
	D3DXMatrixTranslation( &matTrans, (float)m_iPosX, (float)-m_iPosY, 0.0f);
    D3DXMATRIX localMat = matScale * matRotTemp1 * matRot * matRotTemp2 * matTrans;

	int ox = -m_screen_x/2, oy = m_screen_y/2;
    D3DXMATRIX world;
    D3DXMATRIX worldOffset;
    D3DXMatrixTranslation( &worldOffset, (float)ox - 0.5f, (float)oy + 0.5f, 0.0f );
    world = localMat * worldOffset;

    D3DXMATRIX ortho;
    D3DXMatrixOrthoLH( &ortho, (float)m_screen_x, (float)m_screen_y, 0.0f, 1000.0f );

	matPolygon = world * ortho;

	float fColor[4] = {	(float)(color>>16&0xff)/255,
						(float)(color>>8&0xff)/255,
						(float)(color&0xff)/255,
						(float)(color>>24&0xff)/255};

	hr = m_pEffect->SetFloatArray("color",fColor,4);

	hr = m_pEffect->SetFloat( "grids" ,m_mozaicWeight );

	hr = m_pEffect->SetInt( "loopNum", m_hdr );

	hr = m_pEffect->SetMatrix("m_WVP",&matPolygon);
	
	hr = m_pEffect->SetMatrix( "worldMat", &m_matrix );
	hr = m_pEffect->SetMatrix( "viewMat", &m_viewMat );
	hr = m_pEffect->SetMatrix( "projMat", &m_projMat );

	MYTEX::iterator it = m_pTexture.begin();
	for( int i=0;i<m_iTexNo;i++,++it);
	m_pEffect->SetTexture("tex0_data",(*it) );
	GaussWeight( m_dispersion_sq*m_dispersion_sq );
	m_pEffect->SetFloat( "g_MapWidth", m_screen_x );
	m_pEffect->SetFloat( "g_MapHeight", m_screen_y );

	switch( ShaderFlg ){
		case eNORMAL:
		m_pEffect->SetTechnique("NormalShader");
		break;
		case eMOZAIC:
		m_pEffect->SetTechnique("MozaicShader");
		break;
		case eGAUSS:
		m_pEffect->SetTechnique("GaussShader");
		break;
		case eZBuffer:
		m_pEffect->SetTechnique( "ZDepthShader" );
		break;
	}

	UINT numPass;
	m_pEffect->Begin(&numPass,0);
	for(int j=0; j < numPass; j++ ) {
		m_pEffect->BeginPass( j );
		g_DXGrobj.GetDXDevice()->DrawPrimitive(
			D3DPT_TRIANGLESTRIP,
			0,	// �J�n�n�_����̑��Έʒu
			2 );	// �ʂ̐�

		m_pEffect->EndPass();
	}
	m_pEffect->End();

	pDecl->Release();
}

//----------------------------------------------------------------------------------------
// �e�N�X�`���ԍ��̃Z�b�g
//----------------------------------------------------------------------------------------
void C2DPolygon::SetTexNo(int iNo)
{
	int size = m_pTexture.size();
	// �G���[�`�F�b�N
	if( iNo <= size-1 && iNo >= 0 ){
		m_iTexNo = iNo;
		return;
	}else{
		return;
	}
}

//----------------------------------------------------------------------------------------
// �������
//----------------------------------------------------------------------------------------
void C2DPolygon::Release()
{
	MYTEX::iterator it = m_pTexture.begin();

	while( it != m_pTexture.end() ){
		if( *it != NULL ){
			(*it)->Release();
			(*it) = NULL;
		}
		++it;
	}

	m_pTexture.clear();

	if( m_pEffect != NULL){
		m_pEffect->Release();
		m_pEffect = NULL;
	}
	if( m_pVertexBuffer != NULL){
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
}