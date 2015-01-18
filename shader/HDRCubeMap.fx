//-----------------------------------------------------------------------------
// HDRキューブマップ
// Date:2015/1/18
// Author:K.Ito
//-----------------------------------------------------------------------------

#ifndef MAX_NUM_LIGHTS
#define MAX_NUM_LIGHTS 4
#endif

float4x4 g_mWorldView;
float4x4 g_mProj;
texture  g_txCubeMap;
texture  g_txScene;
float4   g_vLightIntensity = { 15.0f, 15.0f, 15.0f, 1.0f };
float4   g_Diffuse_mat;
float4   g_vLightPosView[MAX_NUM_LIGHTS];  // ビュー空間でのライトの位置
float    g_fReflectivity;                  // 反射率

samplerCUBE g_samCubeMap = 
sampler_state
{
    Texture = <g_txCubeMap>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler2D g_samScene =
sampler_state
{
    Texture = <g_txScene>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};


//-----------------------------------------------------------------------------
// 頂点シェーダーHDR環境マッピング
//-----------------------------------------------------------------------------
void HDRVertEnvMap( float4 Pos : POSITION,
                    float3 Normal : NORMAL,
                    out float4 oPos : POSITION,
                    out float3 EnvTex : TEXCOORD0)
{
    oPos = mul( Pos, g_mWorldView );

    // カメラ空間での法線を計算
    float3 vN = mul( Normal, g_mWorldView );
    vN = normalize( vN );

    // 視点ベクトルを反転させる
    float3 vEyeR = -normalize( oPos );

    // 反射ベクトルの計算
    float3 vRef = 2 * dot( vEyeR, vN ) * vN - vEyeR;

    // texcoord0に代入
    EnvTex = vRef;

    // プロジェクションを反映
    oPos = mul( oPos, g_mProj );
}


//-----------------------------------------------------------------------------
// ピクセルシェーダーHDR環境マッピング
//-----------------------------------------------------------------------------
float4 HDRPixEnvMap( float3 Tex : TEXCOORD0) : COLOR
{
    return g_fReflectivity * texCUBE( g_samCubeMap, Tex ) * g_Diffuse_mat;
}

//-----------------------------------------------------------------------------
// 頂点シェーダーHDRシーンレンダリング
//-----------------------------------------------------------------------------
void HDRVertScene( float4 iPos : POSITION,
                   float3 iNormal : NORMAL,
                   float2 iTex : TEXCOORD0,
                   out float4 oPos : POSITION,
                   out float2 Tex : TEXCOORD0,
                   out float3 Pos : TEXCOORD1,
                   out float3 Normal : TEXCOORD2 )
{
    // ビュー空間で行列を変換
    oPos = mul( iPos, g_mWorldView );

    // ピクセルシェーダーに計算結果を渡す
    Pos = oPos;

    // スクリーン座標変換
    oPos = mul( oPos, g_mProj );

    // 法線を計算
    Normal = normalize( mul( iNormal, (float3x3)g_mWorldView ) );
    
    // テクスチャ座標を渡す
    Tex = iTex;
}


//-----------------------------------------------------------------------------
// Pixel Shader: HDRPixScene
// ピクセルシェーダーHDRシーンレンダリング
//-----------------------------------------------------------------------------
float4 HDRPixScene( float2 Tex : TEXCOORD0,
                    float3 Pos : TEXCOORD1,
                    float3 Normal : TEXCOORD2 ) : COLOR
{
    float3 N = normalize( Normal );

    // ライト変数保持用
    float4 vPixValue = (float4)0;

    // すべてのライトを反映させる
    for( int LightIndex = 0; LightIndex < MAX_NUM_LIGHTS; ++LightIndex )
    {
        // ピクセルからライトへのベクトルを計算
        float3 vRLightVec = (float3)(g_vLightPosView[LightIndex] - Pos);

        // ライトベクトルと法線からピクセルを計算
        float fDiffuse = saturate( dot( normalize( vRLightVec ), N ) );

        // ライトの強度を計算
        float fAttenuation = saturate( 1.0f / dot( vRLightVec, vRLightVec ) );

        // ピクセルに反映
        vPixValue += fDiffuse * fAttenuation;
    }
    return tex2D( g_samScene, Tex ) * vPixValue * g_vLightIntensity;
}


//-----------------------------------------------------------------------------
// 頂点シェーダーHDRライトレンダリング
//-----------------------------------------------------------------------------
void HDRVertLight( float4 iPos : POSITION,
                   out float4 oPos : POSITION,
                   out float4 Diffuse : TEXCOORD1 )
{
    // スクリーン座標を計算
    oPos = mul( iPos, g_mWorldView );
    oPos = mul( oPos, g_mProj );

    // ライトの強度を色に設定
    Diffuse = g_vLightIntensity;
}

//-----------------------------------------------------------------------------
// ピクセルシェーダーHDRライトレンダリング
//-----------------------------------------------------------------------------
float4 HDRPixLight( float4 Diffuse : TEXCOORD1 ) : COLOR
{
    // ライトの強度をそのまま出力
    return Diffuse;
}


//-----------------------------------------------------------------------------
// Technique:　RenderScene
//-----------------------------------------------------------------------------
technique RenderScene
{
    pass p0
    {
        VertexShader = compile vs_2_0 HDRVertScene();
        PixelShader = compile ps_2_0 HDRPixScene();
    }
}


//-----------------------------------------------------------------------------
// Technique: RenderLight
//-----------------------------------------------------------------------------
technique RenderLight
{
    pass p0
    {
        VertexShader = compile vs_2_0 HDRVertLight();
        PixelShader = compile ps_2_0 HDRPixLight();
    }
}


//-----------------------------------------------------------------------------
// Technique: RenderEnvMesh
//-----------------------------------------------------------------------------
technique RenderHDREnvMap
{
    pass p0
    {
        CullMode = None;
        VertexShader = compile vs_2_0 HDRVertEnvMap();
        PixelShader = compile ps_2_0 HDRPixEnvMap();
    }
}
