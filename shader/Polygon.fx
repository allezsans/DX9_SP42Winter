//****************************************************************
// PostEffect.fx
//****************************************************************

float4x4 m_WVP,worldMat,viewMat,projMat;
texture tex0_data : TEXTURE0;
float	grids;
float4	color;
int		loopNum;

// ガウスフィルター
float		g_MapWidth;					// スクリーンの幅
float		g_MapHeight;				// スクリーンの高さ
float		weight[8];					// 混ぜ合わせる際の重み

// サンプラー
sampler tex0 = sampler_state
{
	Texture = <tex0_data>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;
};

struct VS_OUTPUT
{
   float4 Pos   : POSITION;
   float4 Col	: COLOR0;
   float2 Tex   : TEXCOORD0;
   float2 Tex_glare   : TEXCOORD1;
   float4 Tex_depth : TEXCOORD2;
};

struct  PS_OUTPUT
{
	float4 Color1 : COLOR0;
	float4 Color2 : COLOR1;
};

VS_OUTPUT normalVS(	float4 Pos     : POSITION,
						float4 Col	   : COLOR0,
						float2 Tex     : TEXCOORD0,
						float2 Tex_g     : TEXCOORD1  )
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
   
	Out.Pos    = mul( Pos, m_WVP );
	Out.Tex    = Tex;
	Out.Tex_glare = Tex_g;
	float4x4 mat = mul( mul(worldMat,viewMat),projMat);
	Out.Tex_depth = mul(Pos,mat);
	Out.Col = color;
       
	return Out;
}

PS_OUTPUT normalPS( VS_OUTPUT In )
{  
	PS_OUTPUT Out = (PS_OUTPUT)0;
	Out.Color1 = In.Col * tex2D( tex0, In.Tex );// + tex2D( tex1, In.Tex_glare );s
	Out.Color2.r = Out.Color2.g = Out.Color2.b = Out.Color1.r * 0.299f + Out.Color1.g * 0.587f + Out.Color1.b * 0.114f;

   //テクセルを取得
	for( int i=0;i<loopNum;i++){
	   float m_TU = (float)i/800.0f;
	   float m_TV = (float)i/600.0f;
	   float2 Texel0 = In.Tex + float2( -m_TU, 0.0f );
	   float2 Texel1 = In.Tex + float2( +m_TU, 0.0f );
	   float2 Texel2 = In.Tex + float2(  0.0f, +m_TV );
	   float2 Texel3 = In.Tex + float2(  0.0f, -m_TV );

	   float2 Texel4 = In.Tex + float2( -m_TU, -m_TV );
	   float2 Texel5 = In.Tex + float2( +m_TU, -m_TV );
	   float2 Texel6 = In.Tex + float2( -m_TU, +m_TV );
	   float2 Texel7 = In.Tex + float2( +m_TU, +m_TV );
	   
	   //取得したテクセル位置のカラー情報を取得する。
	   //それぞれのカラー値にウェイトをかけている。このウェイト値はすべての合計が 1.0f になるように調整する。
	   float4 p0 = Out.Color1 * 0.2f;
	   
	   float4 p1 = tex2D( tex0, Texel0 ) * 0.1f;
	   float4 p2 = tex2D( tex0, Texel1 ) * 0.1f;
	   float4 p3 = tex2D( tex0, Texel2 ) * 0.1f;
	   float4 p4 = tex2D( tex0, Texel3 ) * 0.1f;
	   
	   float4 p5 = tex2D( tex0, Texel4 ) * 0.1f;
	   float4 p6 = tex2D( tex0, Texel5 ) * 0.1f;
	   float4 p7 = tex2D( tex0, Texel6 ) * 0.1f;
	   float4 p8 = tex2D( tex0, Texel7 ) * 0.1f;

	   Out.Color2.rgb += (p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8).r * 0.299f 
	   					+ (p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8).g * 0.587f
	   					+ (p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8).b * 0.114f;
	}
   	if( loopNum != 0){
   		Out.Color2.rgb /= (float)loopNum;
   		//Out.Color2.rgb = pow(Out.Color2.rgb,120);
   		Out.Color1.rbg += Out.Color2.rgb;
	}
	return Out;
}

//------------------------------------------------
// モザイクシェーダ
//------------------------------------------------
float4 mozaicPS( VS_OUTPUT In ) : COLOR0
{  
   float4 Out = (float4)0;
   float2 ms_uv=floor(In.Tex*grids+0.5f)/grids;
   Out = In.Col * tex2D( tex0, ms_uv );
   return Out;
}

//------------------------------------------------
// ZDepth
//------------------------------------------------
float4 depthPS( VS_OUTPUT In ) : COLOR0
{
	float4 depth = (float4)0;
	//float4 depth = In.Tex_depth.z / In.Tex_depth.w;
	depth.rgb = In.Tex_depth.z/In.Tex_depth.w;
	depth.a = 1.0f;
	return In.Col * tex2D( tex0, In.Tex );
}


//------------------------------------------------
// 頂点シェーダ(横方向にガウスフィルターを掛ける)
//------------------------------------------------
VS_OUTPUT Gauss1VS(float4 Pos : POSITION,
 				   float2 Tex : TEXCOORD0 )					
{
	VS_OUTPUT Out  = (VS_OUTPUT)0.0f;
	Out.Pos = mul( Pos,m_WVP );
	Out.Tex = Tex;
	return Out;
}

//------------------------------------------------
// ピクセルシェーダ(横方向にガウスフィルターを掛ける)
//------------------------------------------------
float4 Gauss1PS(VS_OUTPUT In ): COLOR0					
{
	float4	Color;
	
	// テクスチャ座標を補正
	In.Tex.x = In.Tex.x + 0.5f/g_MapWidth;
	In.Tex.y = In.Tex.y + 0.5f/g_MapHeight;


	Color  = weight[0] *  tex2D( tex0, In.Tex );
	Color += weight[1]
	
	 * (tex2D( tex0, In.Tex + float2( + 2.0f/g_MapWidth, 0 ) )
	 +  tex2D( tex0, In.Tex + float2( - 2.0f/g_MapWidth, 0 ) ));
	Color += weight[2]
	 * (tex2D( tex0, In.Tex + float2( + 4.0f/g_MapWidth, 0 ) )
	 +  tex2D( tex0, In.Tex + float2( - 4.0f/g_MapWidth, 0 ) ));
	Color += weight[3]
	 * (tex2D( tex0, In.Tex + float2( + 6.0f/g_MapWidth, 0 ) )
	 +  tex2D( tex0, In.Tex + float2( - 6.0f/g_MapWidth, 0 ) ));
	Color += weight[4]
	 * (tex2D( tex0, In.Tex + float2( + 8.0f/g_MapWidth, 0 ) )
	 +  tex2D( tex0, In.Tex + float2( - 8.0f/g_MapWidth, 0 ) ));
	Color += weight[5]
	 * (tex2D( tex0, In.Tex + float2( +10.0f/g_MapWidth, 0 ) )
	 +  tex2D( tex0, In.Tex + float2( -10.0f/g_MapWidth, 0 ) ));
	Color += weight[6]
	 * (tex2D( tex0, In.Tex + float2( +12.0f/g_MapWidth, 0 ) )
	 +  tex2D( tex0, In.Tex + float2( -12.0f/g_MapWidth, 0 ) ));
	Color += weight[7]
	 * (tex2D( tex0, In.Tex + float2( +14.0f/g_MapWidth, 0 ) )
	 +  tex2D( tex0, In.Tex + float2( -14.0f/g_MapWidth, 0 ) ));
    return Color;
}

//------------------------------------------------
// 頂点シェーダ(縦方向にガウスフィルターを掛ける)
//------------------------------------------------
VS_OUTPUT Gauss2VS(float4 Pos : POSITION,
 				   float2 Tex : TEXCOORD0 )					
{
	VS_OUTPUT Out = (VS_OUTPUT)0.0f;
	Out.Pos = mul( Pos,m_WVP );
	Out.Tex = Tex;
	return Out;
}

//------------------------------------------------
// ピクセルシェーダ(縦方向にガウスフィルターを掛ける)
//------------------------------------------------
float4 Gauss2PS(VS_OUTPUT In ): COLOR0					
{
    float4 Color;
	
	// テクスチャ座標のずれをなくす
	In.Tex.x = In.Tex.x+0.5/g_MapWidth;
	In.Tex.y = In.Tex.y+0.5/g_MapHeight;
	
	Color  = weight[0] *  tex2D( tex0, In.Tex );
	Color += weight[1]
	 * (tex2D( tex0, In.Tex + float2( 0, + 2.0f/g_MapHeight) )
	 +  tex2D( tex0, In.Tex + float2( 0, - 2.0f/g_MapHeight) ));
	Color += weight[2]
	 * (tex2D( tex0, In.Tex + float2( 0, + 4.0f/g_MapHeight) )
	 +  tex2D( tex0, In.Tex + float2( 0, - 4.0f/g_MapHeight) ));
	Color += weight[3]
	 * (tex2D( tex0, In.Tex + float2( 0, + 6.0f/g_MapHeight) )
	 +  tex2D( tex0, In.Tex + float2( 0, - 6.0f/g_MapHeight) ));
	Color += weight[4]
	 * (tex2D( tex0, In.Tex + float2( 0, + 8.0f/g_MapHeight) )
	 +  tex2D( tex0, In.Tex + float2( 0, - 8.0f/g_MapHeight) ));
	Color += weight[5]
	 * (tex2D( tex0, In.Tex + float2( 0, +10.0f/g_MapHeight) )
	 +  tex2D( tex0, In.Tex + float2( 0, -10.0f/g_MapHeight) ));
	Color += weight[6]
	 * (tex2D( tex0, In.Tex + float2( 0, +12.0f/g_MapHeight) )
	 +  tex2D( tex0, In.Tex + float2( 0, -12.0f/g_MapHeight) ));
	Color += weight[7]
	 * (tex2D( tex0, In.Tex + float2( 0, +14.0f/g_MapHeight) )
	 +  tex2D( tex0, In.Tex + float2( 0, -14.0f/g_MapHeight) ));
    return Color;

}

//------------------------------------------------
// ZDepth
//------------------------------------------------
VS_OUTPUT DepthOfFieldVS(	float4 Pos     : POSITION,
						float4 Col	   : COLOR0,
						float2 Tex     : TEXCOORD0,
						float2 Tex_g     : TEXCOORD1  )
{
	return normalVS(Pos,Col,Tex,Tex_g);
}

float4 DepthOfField1PS(VS_OUTPUT In ): COLOR0
{
	float zDepth = In.Tex_depth.z / In.Tex_depth.w;
	float4 Color = In.Col * tex2D( tex0, In.Tex );
	
	float f = (1.0f - zDepth) / 0.7f;
	f = clamp(f,0.0f,1.0f);
	return f * Color + (1.0f-f) * Gauss1PS(In);
}

float4 DepthOfField2PS(VS_OUTPUT In ): COLOR0
{
	float zDepth = In.Tex_depth.z / In.Tex_depth.w;
	float4 Color = In.Col * tex2D( tex0, In.Tex );
	float f = (1.0f - zDepth) / 0.7f;
	f = clamp(f,0.0f,1.0f);
	return f * Color + (1.0f-f) * Gauss2PS(In);
}

technique NormalShader
{
   pass P0
   {
		CullMode = None;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		AlphaBlendEnable = true;
		MultiSampleAntiAlias = false;
		ZENABLE = true;
		VertexShader = compile vs_3_0 normalVS();
		PixelShader  = compile ps_3_0 normalPS();   
   }
}

technique MozaicShader
{
   pass P0
   {
		CullMode = None;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		AlphaBlendEnable = true;
		MultiSampleAntiAlias = false;
		ZENABLE = true;
		VertexShader = compile vs_3_0 normalVS();
		PixelShader  = compile ps_3_0 mozaicPS();   
   }
}

technique GaussShader
{
   	pass P0
	{
        	vertexShader = compile vs_3_0 Gauss1VS();	// 横方向のガウスフィルタ
        	pixelShader  = compile ps_3_0 Gauss1PS();	// 横方向のガウスフィルタ
	}
	pass P1
	{
        	vertexShader = compile vs_3_0 Gauss2VS();	// 縦方向のガウスフィルタ
        	pixelShader  = compile ps_3_0 Gauss2PS();	// 縦方向のガウスフィルタ
	}
}

technique ZDepthShader
{
   /*pass P0
   {
		CullMode = None;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		AlphaBlendEnable = true;
		MultiSampleAntiAlias = false;
		ZENABLE = true;
		VertexShader = compile vs_3_0 normalVS();
		PixelShader  = compile ps_3_0 depthPS();   
   }*/
   pass P0
	{
        	vertexShader = compile vs_3_0 DepthOfFieldVS();	// 横方向のガウスフィルタ
        	pixelShader  = compile ps_3_0 DepthOfField1PS();	// 横方向のガウスフィルタ
	}
	pass P1
	{
        	vertexShader = compile vs_3_0 DepthOfFieldVS();	// 横方向のガウスフィルタ
        	pixelShader  = compile ps_3_0 DepthOfField2PS();	// 横方向のガウスフィルタ
	}
}