#pragma once
#include <windows.h>
#include <stdio.h>
#include <process.h>	
#include <memory>
#include <list>
#include "CDirectXGraphics.h" 
#include "CInput.h"
#include "CDirect3DXFile.h"
#include "CFont.h"
#include "C2DPolygon.h"
#include "CInfo.h"

// �}�N���̒�`

#define NAME	"win32A"
#define TITLE	"SP42WinterHomework"

#define		SCREEN_X		1600
#define		SCREEN_Y		900
#define		FULLSCREEN		0				// �t���X�N���[���t���O
#define		NUM_LIGHTS		4

#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }

// ���_�t�H�[�}�b�g��`
#define		D3DFVF_CUSTOMVERTEX	   ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX0  )

// �����N���C�u�����̐ݒ�
#pragma comment(lib, "d3d9.lib")			// �����N�Ώۃ��C�u������d3d9.lib��ǉ�
#pragma comment(lib, "d3dx9.lib")			// �����N�Ώۃ��C�u������d3dx9.lib��ǉ�
#pragma comment(lib, "winmm.lib")			// �����N�Ώۃ��C�u������winmm.lib��ǉ�

// ���������[�N���o
#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

// �v���g�^�C�v�錾
struct CLight
{
	D3DXVECTOR4 vPos;      // ���[���h���W
	D3DXVECTOR4 vMoveDir;  // �ړ�����
	float fMoveDist;		// �ړ��ł���ő勗��
	D3DXMATRIXA16 mWorld;    // ���[���h�s��
	D3DXMATRIXA16 mWorking;  // ���[�N�s��
};

struct CTechniqueGroup
{
	D3DXHANDLE hRenderScene;
	D3DXHANDLE hRenderLight;
	D3DXHANDLE hRenderEnvMap;
};

LRESULT CALLBACK WndProc( HWND hWnd,			// �E�C���h�E���b�Z�[�W�֐�
	UINT message,
	WPARAM wParam,
	LPARAM lParam );

unsigned int   WINAPI GameMain( void  * );							// �Q�[�����C���֐��֐�
bool	GameInit( HINSTANCE hinst, HWND hwnd, int width, int height );	// �Q�[���̏�������
void	GameExit();													// �Q�[���̏I������
void	CALLBACK TimerProc( UINT, UINT, DWORD, DWORD, DWORD );			// �^�C�}����
void RenderSceneIntoCubeMap( IDirect3DDevice9* pd3dDevice, double fTime );
void RenderScene( IDirect3DDevice9* pd3dDevice, const D3DXMATRIX* pmView, const D3DXMATRIX* pmProj, bool bRenderEnvMappedMesh, double fTime );
D3DXMATRIX GetCubeMapViewMatrix( DWORD dwFace );
void InputKeyboard();

#ifdef _MAIN_MODULE_
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL std::shared_ptr<CInfo> pInfo;					// �t�H���g�̍쐬
GLOBAL std::shared_ptr<C2DPolygon> m_pPETexture;		// 2D�|���S���\���N���X

// ���f���t�H���_�p�X
const std::string  modelPath = "resource/model/";
// ���f���t�@�C���p�X
const std::string xFileName[] = {
	"skullocc.x",
	"maid_pose.x",
	"room.x",
};

// �O���[�o���ϐ�
GLOBAL CDirectXGraphics	g_DXGrobj;								// �c�h�q�d�b�s�w�@�f�q�`�o�g�h�b�r�@�I�u�W�F�N�g		
GLOBAL LPD3DXEFFECT		g_pEffect = nullptr;					// �G�t�F�N�g�I�u�W�F�N�g
GLOBAL std::list<std::shared_ptr<CDirect3DXFile>>		g_pXfile;	// X�t�@�C���}�l�[�W��
GLOBAL std::shared_ptr<CInput>				g_pInput;				// ���̓N���X
GLOBAL CLight g_aLights[NUM_LIGHTS];     // ���C�g�̐�
GLOBAL D3DXVECTOR4 g_vLightIntensity;
GLOBAL float g_fLightIntensity;
GLOBAL IDirect3DSurface9*	g_pDepthCube;
GLOBAL IDirect3DCubeTexture9*          g_apCubeMap;
GLOBAL float g_fReflectivity;
GLOBAL int g_nowModel;

// 3D�`��p�̍s��
GLOBAL D3DXMATRIX	g_MatView;				// �J�����s��
GLOBAL D3DXMATRIX	g_MatProjection;		// �v���W�F�N�V�����ϊ��s��
GLOBAL D3DXMATRIX	g_MatWorld;				// ���[���h�ϊ��s��

// 2D�`��p
GLOBAL C2DPolygon*	g_Tex;					// ���}�b�v�̃e�N�X�`��

// �Q�[�����C���X���b�h�֘A
GLOBAL HANDLE			g_hEventHandle;		// �C�x���g�n���h��
GLOBAL int				g_timerid = 0;		// �^�C�}�h�c
GLOBAL bool			g_EndFlag = false;	// �I���t���O
GLOBAL HANDLE			g_hThread;			// �X���b�h�̃n���h���l
GLOBAL float		g_spendTime;
GLOBAL unsigned int	g_dwThreadId;		// �X���b�h�h�c