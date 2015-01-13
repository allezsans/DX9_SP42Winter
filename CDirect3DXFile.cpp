#include	"CDirect3DXFile.h"

/*-----------------------------
	X Fileを読み込む
		引数
			xfilename  : Ｘファイル名
			lpd3ddevice : デバイスオブジェクト
		戻り値
			true	成功
			false   失敗
------------------------------*/
bool CDirect3DXFile::LoadXFile(const char *xfilename, LPDIRECT3DDEVICE9 lpd3ddevice )
{
	LPD3DXBUFFER	pd3dxmtrlbuffer;
	LPD3DXBUFFER	pd3dxmtrlbuffer2;
	HRESULT			hr;
	unsigned int	i;

	// Ｘファイルを読み込む
	hr = D3DXLoadMeshFromX(	xfilename,				// Ｘファイル名
							D3DXMESH_SYSTEMMEM,		// 読み込みメモリ
							lpd3ddevice,			// デバイスオブジェクト
							NULL,
							&pd3dxmtrlbuffer,		// マテリアルバッファ
							&pd3dxmtrlbuffer2,		// マテリアルバッファ2
							&m_nummaterial,			// マテリアル数
							&m_lpmesh);				// メッシュ

	if( SUCCEEDED(hr) ){
		D3DXMATERIAL	*d3dxmaterials = (D3DXMATERIAL*)pd3dxmtrlbuffer->GetBufferPointer();	// マテリアルのアドレスを取得
		m_lpmeshmaterials = new D3DMATERIAL9[m_nummaterial];		// マテリアルコンテナの作成
		m_lpmeshtextures = new LPDIRECT3DTEXTURE9[m_nummaterial];	// テクスチャコンテナの生成
		
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
		pd3dxmtrlbuffer->Release();		// マテリアルバッファのリリース
		pd3dxmtrlbuffer2->Release();	// マテリアルバッファのリリース

		//Xファイルに法線がない場合は、法線を書き込む
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
	X FileをＵｎｌｏａｄする
		引数
			なし
		戻り値
			true	成功
			false   失敗
------------------------------*/
void CDirect3DXFile::UnLoadXFile()
{
	unsigned int i;

	if(m_lpmeshmaterials!=NULL){	// マテリアルオブジェクトの解放
		delete [] m_lpmeshmaterials;
		m_lpmeshmaterials=NULL;
	}

	if(m_lpmeshtextures!=NULL){		// テクスチャオブジェクトの解放
		for( i=0 ; i<m_nummaterial ; i++ ){
			if(m_lpmeshtextures[i]!=NULL)	m_lpmeshtextures[i]->Release();
		}
		delete [] m_lpmeshtextures;
		m_lpmeshtextures=NULL;
	}

	if(m_lpmesh!=NULL){		// メッシュ解放
		m_lpmesh->Release();
		m_lpmesh=NULL;
	}
}

/*-----------------------------
	X Fileを描画する(固定パイプライン)
		引数
			lpd3ddevice :	デバイスオブジェクト
		戻り値
			なし
------------------------------*/
void CDirect3DXFile::Draw(LPDIRECT3DDEVICE9 lpd3ddevice)
{
	unsigned int i;
	for( i=0 ; i<m_nummaterial ; i++ ){
		lpd3ddevice->SetMaterial(&m_lpmeshmaterials[i]);	// マテリアルのセット
		lpd3ddevice->SetTexture(0,m_lpmeshtextures[i]);		// テクスチャのセット
		m_lpmesh->DrawSubset(i);							// サブセットの描画
	}
}

/*-----------------------------
X Fileを描画する
引数
lpd3ddevice :	デバイスオブジェクト
戻り値
なし
------------------------------*/
void CDirect3DXFile::Draw( LPD3DXEFFECT pEffect )
{
	unsigned int i;
	for( i = 0; i < m_nummaterial; i++ ) {
		float	diffuse[4] = { m_lpmeshmaterials[i].Diffuse.r, m_lpmeshmaterials[i].Diffuse.g, m_lpmeshmaterials[i].Diffuse.b, m_lpmeshmaterials[i].Diffuse.a };
		float	specular[4] = { m_lpmeshmaterials[i].Specular.r, m_lpmeshmaterials[i].Specular.g, m_lpmeshmaterials[i].Specular.b, m_lpmeshmaterials[i].Specular.a };
		pEffect->SetFloatArray( "g_Diffuse_mat", diffuse, 4 );			// マテリアルのセット
		pEffect->SetFloatArray( "g_Specular_mat", specular, 4 );		// マテリアルのセット
		// テクスチャのセット
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
X Fileの影をテクスチャに描画する
引数
lpd3ddevice :	デバイスオブジェクト
戻り値
なし
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
	X Fileを描画する(3軸を描画する)
		引数
			lpd3ddevice :	デバイスオブジェクト
		戻り値
			なし
------------------------------*/
void CDirect3DXFile::DrawWithAxis(LPDIRECT3DDEVICE9 lpd3ddevice)
{
	unsigned int i;

	DrawAxis(lpd3ddevice);	// ３軸の描画

	for(i=0;i<m_nummaterial;i++){	// サブセットの描画
		lpd3ddevice->SetMaterial(&m_lpmeshmaterials[i]);	// マテリアルのセット
		lpd3ddevice->SetTexture(0,m_lpmeshtextures[i]);		// テクスチャのセット
		m_lpmesh->DrawSubset(i);							// サブセットの描画
	}
}

/*---------------------
	３軸の描画
		引数
			lpd3ddevice :	デバイスオブジェクト
		戻り値
			なし
---------------------*/
void CDirect3DXFile::DrawAxis(LPDIRECT3DDEVICE9 lpd3ddevice)
{
	struct VERTEXDATA{
		float		x,y,z;
		D3DCOLOR	color;
	};

	// ３軸の頂点データ
	struct	VERTEXDATA	linevertices[6]={
		-400,   0,   0,  D3DCOLOR_XRGB(255,0,0),	// Ｘ軸始点 赤
		 400,   0,   0,  D3DCOLOR_XRGB(255,0,0),	// Ｘ軸終点 赤
		   0,-400,   0,  D3DCOLOR_XRGB(0,255,0),	// Ｙ軸始点 緑
		   0, 400,   0,  D3DCOLOR_XRGB(0,255,0),	// Ｙ軸終点 緑
		   0,   0,-400,  D3DCOLOR_XRGB(0,0,255),	// Ｚ軸始点 青
		   0,   0, 400,  D3DCOLOR_XRGB(0,0,255)		// Ｚ軸終点 青
	};

	lpd3ddevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);	// 頂点フォーマットをセット

	lpd3ddevice->DrawPrimitiveUP(D3DPT_LINELIST, 3, &linevertices[0], sizeof(VERTEXDATA));	// ３軸を描画
}

LPD3DXMESH CDirect3DXFile::GetMesh() const{
	return m_lpmesh;
}