//------------------------------------
// basic.fx
// Date:	2014/04/16
// Author:	K.Ito
//------------------------------------
float4x4	g_LightMat;		// view * projection
float4x4	g_worldMat;		// ワールド変換行列
float4x4	g_viewMat;		// ビュー行列
float4x4	g_projMat;		// プロジェクション行列
float4 		g_Diffuse_mat;	// でぃふゅーず
float4 		g_Specular_mat;	// すぺきゅら
float3		g_LightPos;		// ライトの位置
float3		g_LightColor;	// ライトの色
float3  	g_camerapos;	// カメラの位置	
float 		g_height;		// 高さ情報
bool		g_tex;			// テクスチャの有無
texture 	tex0_data : TEXTURE0;	// モデルのテクスチャ

// ディスプレイスメントマップ
texture 	tex1_data : TEXTURE1;	// ハイトマップ



// 通常のテクスチャ
sampler tex0 = sampler_state
{
	Texture = <tex0_data>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

// ハイトマップ
sampler tex1 = sampler_state
{
	Texture = <tex1_data>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

// 頂点シェーダー出力用構造体
struct VS_OUT{
	float4 pos : POSITION0;
	float4 diffuse : COLOR0;
	float4 specular : COLOR1;
	float3 normal : NORMAL0;
	float2 tex  : TEXCOORD0;
};

VS_OUT VS(	float4 pos : POSITION,
			float4 normal: NORMAL,
			float2 tex : TEXCOORD0,
			float2 tex_height : TEXCOORD1){

	// ディスプレイスメントマップの計算
	tex.x = -tex.x;
	tex_height.x = -tex_height.x;
	float h = tex2Dlod(tex1,float4(tex,0,0));
	//h = h - 0.5f;
	float4 pp = (float4)1.0f;
	pp.xyz = pos.xyz + h*g_height*normal.xyz;

	// Transform
	VS_OUT Out = (VS_OUT)0;
	float4x4 WVP = mul( mul(g_worldMat,g_viewMat),g_projMat);
	Out.pos = mul( pp, WVP );
	
	Out.tex = tex;
	// ディフューズの計算
	float4 lightpos = float4( g_LightPos , 1.0f );
	float4 LightDir = ( mul( pp,g_worldMat ) - lightpos );
	float3 L = -normalize( LightDir.xyz );
	float3 N = normalize( mul(normal.xyz,g_worldMat) );
	Out.diffuse = g_Diffuse_mat * max( 0.0f , dot(N, L) );
	Out.diffuse.a = 1.0f;
	// スペキュラの計算
	float3 V = normalize( g_camerapos - mul( pp,g_worldMat ));
	float3 R = reflect( -V,N );
	R=normalize( R );
	Out.specular = g_Specular_mat * pow( max(0,dot(R,L)),60);

	return Out;
}

// テクスチャある時
float4 PS(VS_OUT In) : COLOR0{
	float4 color = In.diffuse * tex2D( tex0, In.tex) * float4(g_LightColor,1.0f) + In.specular;
	return float4(1.0f,0.0f,0.0f,1.0f);
}

// テクスチャない時
float4 PS_NoTexture(VS_OUT In) : COLOR0{
	float4 color = In.diffuse * tex2D( tex0, In.tex) * float4(g_LightColor,1.0f) + In.specular;
	return float4(1.0f,0.0f,0.0f,1.0f);
}

technique basic {
	pass P0 {
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		AlphaBlendEnable = true;
		CullMode = NONE;
		VertexShader = compile	vs_3_0 VS();
		PixelShader = compile	ps_3_0 PS();
	}
}

technique basic_notexture {
	pass P0 {
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		AlphaBlendEnable = true;
		CullMode = NONE;
		VertexShader = compile	vs_3_0 VS();
		PixelShader = compile	ps_3_0 PS_NoTexture();
	}
}