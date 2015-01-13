//****************************************************************
// Font.fx
//****************************************************************

float4x4 m_WVP;

texture tex0_data : TEXTURE0;

float	screenW = 800;
float	screenH = 600;

float4	color;

// サンプラー
sampler tex0 : register(s0) = sampler_state
{
	Texture = <tex0_data>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

struct FONT_VS_OUTPUT
{
   float4 Pos   : POSITION;
   float4 Col	: COLOR0;
   float2 Tex   : TEXCOORD0;
};

FONT_VS_OUTPUT FontVS(	float4 Pos     : POSITION,
						float4 Col	   : COLOR0,
						float2 Tex     : TEXCOORD0 )
{
	FONT_VS_OUTPUT Out = (FONT_VS_OUTPUT)0;
   
	Out.Pos    = mul( Pos, m_WVP );
	//Out.Pos /= Out.Pos.w;

	//Out.Pos.x = (Out.Pos.x * screenW - 0.5f) / screenW;
	//Out.Pos.y = (Out.Pos.y * screenH + 0.5f) / screenH;
   
	Out.Tex    = Tex;
  
	//Out.Col = float4(1.0f,1.0f,0.0f,0.8f);
	Out.Col = color;
       
	return Out;
}


float4 FontPS( FONT_VS_OUTPUT In ) : COLOR0
{  
   float4 Out = (float4)0;
   
   Out = In.Col * tex2D( tex0, In.Tex );
   //Out = tex2D( tex1, In.Tex );
   //Out = In.Col;
      
   return Out;
}

technique FontShader
{
   pass P0
   {
		CullMode = None;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		AlphaBlendEnable = true;
		MultiSampleAntiAlias = false;
		ZENABLE = false;
		VertexShader = compile vs_3_0 FontVS();
		PixelShader  = compile ps_3_0 FontPS();   
   }
}

