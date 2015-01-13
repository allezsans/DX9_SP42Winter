//=================================================================================
// CFont.cpp
//---------------------------------------------------------------------------------
#include "CFont.h"
#include <Shlwapi.h>
#include "CDirectxGraphics.h"
extern CDirectXGraphics	g_DXGrobj;
//---------------------------------------------------------------------------------
// コンストラクタ
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
		
	//UVのバッファを作成
	if(FAILED(g_DXGrobj.GetDXDevice()->CreateVertexBuffer(
		sizeof( VTX ) * 4,
		0,
		0,
		D3DPOOL_MANAGED,
		&m_pVertexBuffer,
		NULL ))){
			MessageBox(nullptr,"フォントポリゴンバッファの作成に失敗しました。","ERROR!!",MB_OK);
			return;
	}
	m_pVertexBuffer->Lock(0, 0, (void**)&p, 0);
	memcpy( p, m_sVtx, sizeof(VTX) *4 );
	m_pVertexBuffer->Unlock();
}

//--------------------------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------------------------
CFont::~CFont()
{
	Release();
}

//--------------------------------------------------------------------------------
// txtを読み込み重複データを削除する
// txtはASCIIとマルチバイト混在可
//--------------------------------------------------------------------------------
void CFont::Load( const char* fileName )
{
	using namespace std;
	// 全部読み込む
	ifstream ifs( fileName );
	if( ifs.fail() ){
		string Error = " の読み込みに失敗しました。";
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

	// マルチバイト文字とASCII文字を振り分ける
	string MultiStr;	// マルチバイト用
	string MonoStr;		// ASCII用
	int MultiCharCnt = 0;
	int MonoCharCnt = 0;

	while( it != str.end() ){ //文字カウント
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
	
	// マルチバイトの重複をチェックし重複があれば削る。
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

	// ASCIIの重複をチェックし重複があれば削る。
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
	// 統合
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
// テクスチャデータの作成
//----------------------------------------------------------------------------------------
void CFont::CreateTexture(const int fontSize,const char* fontType,const char* fontFile)
{	
	HRESULT hr;
	m_iFontSize = fontSize;
	string sFontType = fontType;
	MAP::iterator it = m_mapTexture.begin();
	MAPINFO::iterator infoIt = m_mapInfo.begin();
	//++it;	// map[0]は空白なので１つ進める
	// 文字の数だけテクスチャを作成する
	// テクスチャ作成
	int iSize = m_mapTexture.size();
	int num = 0;
	// フォントを使えるようにする
	DESIGNVECTOR design;
	hr = AddFontResourceEx(
			fontFile, //ttfファイルへのパス
			FR_PRIVATE,
			&design);

	if( FAILED( hr ) ){
		MessageBox(nullptr,"ttfファイルの読み込みに失敗しました。","ERROR!!",MB_OK);
		return;
	}

	while( it != m_mapTexture.end() ){
		// フォントハンドルの生成
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

		// 現在のウィンドウに適用
		// デバイスにフォントを持たせないとGetGlyphOutline関数はエラーとなる
		HDC hdc = GetDC(NULL);
		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

		char *c = (char*)(*it).first.c_str();
		UINT code = 0;
		if(IsDBCSLeadByte(*c))
			code = (BYTE)c[0]<<8 | (BYTE)c[1];
		else
			code = c[0];

		// ----- ここでGetGlyphOutline関数からビットマップ取得 -----
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

		// テクスチャにフォントビットマップ情報を書き込み
		D3DLOCKED_RECT lockedRect;
		(*it).second->LockRect( 0, &lockedRect, NULL, D3DLOCK_DISCARD);  // ロック

		int grad = 16; // 17階調の最大値
		DWORD *pTexBuf = (DWORD*)lockedRect.pBits;   // テクスチャメモリへのポインタ

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
		// デバイスコンテキストとフォントハンドルはもういらないので解放
		SelectObject(hdc, oldFont);
		ReleaseDC(NULL, hdc);
		++it;
		++infoIt;
		++num;
	}
	RemoveFontResourceEx(
			fontFile, //ttfファイルへのパス
			FR_PRIVATE,
			&design);
}

//----------------------------------------------------------------------------------------
// 文字列表示
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
			MessageBox(nullptr,"txtにない文字が呼ばれました。","ERROR!!",MB_OK);
			exit(1);
		}

		LPDIRECT3DVERTEXDECLARATION9	pDecl;
		D3DVERTEXELEMENT9 elems[] = {	
			{// ストリーム 0 頂点
				0,
				0,
				D3DDECLTYPE_FLOAT3,
				D3DDECLMETHOD_DEFAULT,
				D3DDECLUSAGE_POSITION,
				0
			},
			{// ストリーム0 UV
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
			0,							// 開始地点からの相対位置
			2	// 面の数
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
// 解放処理
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