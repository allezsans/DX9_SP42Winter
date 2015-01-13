//=================================================================================
// CFont.cpp
//---------------------------------------------------------------------------------
#include "CFont.h"
#include <Shlwapi.h>
#include "CDirectxGraphics.h"
extern CDirectXGraphics	g_DXGrobj;
//---------------------------------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------------------------------
CFont::CFont():
	m_iFontSize(0)
	, screen_x(800)
	, screen_y(600)
{
	m_mapTexture.clear();
	LPD3DXBUFFER pErr=NULL;
	if( FAILED( D3DXCreateEffectFromFile(	g_DXGrobj.GetDXDevice(),
											"shader/Font.fx",
											NULL,
											NULL,
											0,
											NULL,
											&m_pEffect,
											&pErr ) ) ){
		MessageBox(nullptr,(LPCTSTR)pErr->GetBufferPointer(),"ERROR!!",MB_OK);
	}
	if( pErr != NULL )	pErr->Release();

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
			MessageBox(nullptr,"�t�H���g�|���S���o�b�t�@�̍쐬�Ɏ��s���܂����B","ERROR!!",MB_OK);
			return;
	}
	m_pVertexBuffer->Lock(0, 0, (void**)&p, 0);
	memcpy( p, m_sVtx, sizeof(VTX) *4 );
	m_pVertexBuffer->Unlock();
}

//--------------------------------------------------------------------------------
// �f�X�g���N�^
//--------------------------------------------------------------------------------
CFont::~CFont()
{
	Release();
}

//--------------------------------------------------------------------------------
// txt��ǂݍ��ݏd���f�[�^���폜����
// txt��ASCII�ƃ}���`�o�C�g���݉�
//--------------------------------------------------------------------------------
void CFont::Load( const char* fileName )
{
	using namespace std;
	// �S���ǂݍ���
	ifstream ifs( fileName );
	if( ifs.fail() ){
		string Error = " �̓ǂݍ��݂Ɏ��s���܂����B";
		string ErrorMsg = fileName + Error;
		MessageBox(nullptr,ErrorMsg.c_str(),"ERROR!!",MB_OK);
		return;
	}
	istreambuf_iterator<char> first(ifs);
	istreambuf_iterator<char> last;
	string str(first,last);
	// ifs >> str;
	string MultiStrTmp,MonoStrTmp,StrTmp;
	string::iterator it;
	it = str.begin();

	// �}���`�o�C�g������ASCII������U�蕪����
	string MultiStr;	// �}���`�o�C�g�p
	string MonoStr;		// ASCII�p
	int MultiCharCnt = 0;
	int MonoCharCnt = 0;

	while( it != str.end() ){ //�����J�E���g
		if( IsDBCSLeadByte( *it )){
			MultiStr += *it;
			++it;
			MultiStr += *it;
			MultiCharCnt++;
		}else{
			MonoStr += *it;
			MonoCharCnt++;
		}
		++it;
	}
	
	// �}���`�o�C�g�̏d�����`�F�b�N���d��������΍��B
	for(int i=0;i<2*MultiCharCnt;i+=2){
		if( i == 0 ){
			MultiStrTmp.push_back( MultiStr[i] );
			MultiStrTmp.push_back( MultiStr[i+1] );
		}else{
			string::size_type index1 = MultiStrTmp.find( MultiStr[i] );
			string::size_type index2 = MultiStrTmp.find( MultiStr[i+1] );
			if( index1 == string::npos || index2 == string::npos ){
				MultiStrTmp += MultiStr.substr(i,2);
			}else if( index1 != string::npos && index2 != string::npos
						&& ( index2 != index1 + 1) ){
				MultiStrTmp += MultiStr.substr(i,2);
			}
		}
	}

	// ASCII�̏d�����`�F�b�N���d��������΍��B
	for(int i=0;i<MonoCharCnt;i++){
		if( i == 0 ){
			MonoStrTmp.push_back( MonoStr[i] );
		}else{
			string::size_type index1 = MonoStrTmp.find( MonoStr[i] );
			if( index1 == string::npos ){
				MonoStrTmp += MonoStr[i];
			}
		}
	}
	// ����
	for( UINT i=0;i<MultiStrTmp.length();i+=2){
		string tmp = MultiStrTmp.substr( i,2 );
		PAIR input1( tmp,(LPDIRECT3DTEXTURE9)NULL );
		m_mapTexture.insert( input1 );
		GLYPHMETRICS GM;
		PAIRINFO input2( tmp, GM );
		m_mapInfo.insert( input2 );
	}
	for( UINT i=0;i<MonoStrTmp.length();i++){
		string tmp = MonoStrTmp.substr( i,1 );
		PAIR input( tmp,(LPDIRECT3DTEXTURE9)NULL );
		m_mapTexture.insert( input );
		GLYPHMETRICS GM;
		PAIRINFO input2( tmp, GM );
		m_mapInfo.insert( input2 );
	}
}

//----------------------------------------------------------------------------------------
// �e�N�X�`���f�[�^�̍쐬
//----------------------------------------------------------------------------------------
void CFont::CreateTexture(const int fontSize,const char* fontType,const char* fontFile)
{	
	HRESULT hr;
	m_iFontSize = fontSize;
	string sFontType = fontType;
	MAP::iterator it = m_mapTexture.begin();
	MAPINFO::iterator infoIt = m_mapInfo.begin();
	//++it;	// map[0]�͋󔒂Ȃ̂łP�i�߂�
	// �����̐������e�N�X�`�����쐬����
	// �e�N�X�`���쐬
	int iSize = m_mapTexture.size();
	int num = 0;
	// �t�H���g���g����悤�ɂ���
	DESIGNVECTOR design;
	hr = AddFontResourceEx(
			fontFile, //ttf�t�@�C���ւ̃p�X
			FR_PRIVATE,
			&design);

	if( FAILED( hr ) ){
		MessageBox(nullptr,"ttf�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B","ERROR!!",MB_OK);
		return;
	}

	while( it != m_mapTexture.end() ){
		// �t�H���g�n���h���̐���
		int fontWeight = FW_NORMAL;
		LOGFONT lf = {
			fontSize, 0, 0, 0, fontWeight, 0, 0, 0,
			SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
			PROOF_QUALITY, DEFAULT_PITCH | FF_MODERN,
			NULL
		};
		for(unsigned int i=0;i<sFontType.length();i++){
			lf.lfFaceName[i] = fontType[i];
		}
		HFONT hFont = CreateFontIndirect(&lf);

		// ���݂̃E�B���h�E�ɓK�p
		// �f�o�C�X�Ƀt�H���g���������Ȃ���GetGlyphOutline�֐��̓G���[�ƂȂ�
		HDC hdc = GetDC(NULL);
		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

		char *c = (char*)(*it).first.c_str();
		UINT code = 0;
		if(IsDBCSLeadByte(*c))
			code = (BYTE)c[0]<<8 | (BYTE)c[1];
		else
			code = c[0];

		// ----- ������GetGlyphOutline�֐�����r�b�g�}�b�v�擾 -----
		CONST MAT2 mat = {{0,1},{0,0},{0,0},{0,1}};
		DWORD size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &(*infoIt).second, 0, NULL, &mat);
		BYTE *pFontBMP = new BYTE[size];
		GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &(*infoIt).second, size, pFontBMP, &mat);
		// ---------------------------------------------------------

		int fontWidth = ((*infoIt).second.gmBlackBoxX + 3) / 4 * 4;
		int fontHeight = (*infoIt).second.gmBlackBoxY;

		hr = g_DXGrobj.GetDXDevice()->CreateTexture(	fontWidth,
														fontHeight,
														1,
														0,
														D3DFMT_A8R8G8B8,
														D3DPOOL_MANAGED,
														&(*it).second,
														NULL );

		// �e�N�X�`���Ƀt�H���g�r�b�g�}�b�v������������
		D3DLOCKED_RECT lockedRect;
		(*it).second->LockRect( 0, &lockedRect, NULL, D3DLOCK_DISCARD);  // ���b�N

		int grad = 16; // 17�K���̍ő�l
		DWORD *pTexBuf = (DWORD*)lockedRect.pBits;   // �e�N�X�`���������ւ̃|�C���^

		for (int y = 0; y < fontHeight; y++ ) {
			for (int x = 0; x < fontWidth; x++ ) {
				DWORD alpha = pFontBMP[y * fontWidth + x] * 255 / grad;
				pTexBuf[y * fontWidth + x] = (alpha << 24) | 0x00ffffff;
			}
		}

		(*it).second->UnlockRect( 0 );

		//(*it).second = pTex[num];
		//CopyMemory( &(*it).second,&pTex,sizeof(LPDIRECT3DTEXTURE9));
		if( pFontBMP != nullptr ) {
			delete[] pFontBMP;
			pFontBMP = nullptr;
		}
		// �f�o�C�X�R���e�L�X�g�ƃt�H���g�n���h���͂�������Ȃ��̂ŉ��
		SelectObject(hdc, oldFont);
		ReleaseDC(NULL, hdc);
		++it;
		++infoIt;
		++num;
	}
	RemoveFontResourceEx(
			fontFile, //ttf�t�@�C���ւ̃p�X
			FR_PRIVATE,
			&design);
}

//----------------------------------------------------------------------------------------
// ������\��
//----------------------------------------------------------------------------------------
void CFont::PrintChar(const char* str,float x,float y,D3DCOLOR color)
{
	
	HRESULT hr;
	string sStr = str;
	string::iterator strIt = sStr.begin();
	string pStr;
	int iCnt = 0;
	int PrevFontWidth = 0;
	int PrevFontHeight = 0;

	while( strIt != sStr.end()){
		if( IsDBCSLeadByte( *strIt )){
			pStr += *strIt;
			++strIt;
			pStr += *strIt;
		}else{
			pStr += *strIt;
		}
		MAP::iterator mapIt = m_mapTexture.find( pStr );
		MAPINFO::iterator infoIt = m_mapInfo.find( pStr );

		if( mapIt == m_mapTexture.end() ){
			MessageBox(nullptr,"txt�ɂȂ��������Ă΂�܂����B","ERROR!!",MB_OK);
			exit(1);
		}

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

		D3DXMATRIX matFont;
		D3DXMatrixIdentity(&matFont);
		D3DXMATRIX matScale;
		D3DXMatrixIdentity(&matScale);
		D3DXMATRIX matRot;
		D3DXMatrixIdentity(&matRot);
		D3DXMATRIX matTrans;
		D3DXMatrixIdentity(&matTrans);

		int fontWidth = ((*infoIt).second.gmBlackBoxX + 3) / 4 * 4;
		int fontHeight = (*infoIt).second.gmBlackBoxY;
		int fontCellWidth = (*infoIt).second.gmCellIncX;
		int fontCellHeight = (*infoIt).second.gmCellIncY;
		int fontOriginX = (*infoIt).second.gmptGlyphOrigin.x;
		int fontOriginY = (*infoIt).second.gmptGlyphOrigin.y;

		D3DXMatrixScaling( &matScale, (float)fontWidth, (float)fontHeight, 1.0f );
		D3DXMatrixTranslation( &matTrans, (float)fontOriginX, (float)fontOriginY, 0.0f );
        D3DXMATRIX localMat = matScale * matTrans;	

		float ox = -screen_x / 2 + x + PrevFontWidth;
		float oy = -m_iFontSize + screen_y / 2 + PrevFontHeight - y;

        D3DXMATRIX world;
        D3DXMATRIX worldOffset;
        D3DXMatrixTranslation( &worldOffset, ox - 0.5f, oy + 0.5f, 0.0f );
        world = localMat * worldOffset;

        D3DXMATRIX ortho;
        D3DXMatrixOrthoLH( &ortho, (float)screen_x, (float)screen_y, 0.0f, 1000.0f );

		matFont = world * ortho;

		hr = m_pEffect->SetMatrix("m_WVP",&matFont);

		hr = m_pEffect->SetTexture("tex0_data", (*mapIt).second );

		hr = m_pEffect->SetTechnique("FontShader");

		DWORD cloar = color>>16&0xff;
		float fColor[4] = {	(float)(color>>16&0xff)/255,
							(float)(color>>8&0xff)/255,
							(float)(color&0xff)/255,
							(float)(color>>24&0xff)/255};

		hr = m_pEffect->SetFloatArray("color",fColor,4);

		UINT numPass;
		m_pEffect->Begin(&numPass,0);
		m_pEffect->BeginPass(0);
		
		hr = g_DXGrobj.GetDXDevice()->DrawPrimitive(
			D3DPT_TRIANGLESTRIP,
			0,							// �J�n�n�_����̑��Έʒu
			2	// �ʂ̐�
		);

		m_pEffect->EndPass();
		m_pEffect->End();

		pDecl->Release();
		pStr.clear();
		PrevFontWidth += fontCellWidth;
		PrevFontHeight += fontCellHeight;
		++strIt;
		++iCnt;
	}
}
//----------------------------------------------------------------------------------------
// �������
//----------------------------------------------------------------------------------------
void CFont::Release()
{
	MAP::iterator it = m_mapTexture.begin();

	while( it != m_mapTexture.end() ){
		if( (*it).second != NULL ){
			(*it).second->Release();
			(*it).second = NULL;
		}
		++it;
	}
	m_mapTexture.clear();
	m_mapInfo.clear();

	if( m_pEffect != NULL){
		m_pEffect->Release();
		m_pEffect = NULL;
	}
	if( m_pVertexBuffer != NULL){
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
}