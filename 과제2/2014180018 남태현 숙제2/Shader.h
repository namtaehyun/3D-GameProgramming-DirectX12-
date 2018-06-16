#pragma once
#include "GameObject.h"
#include "Camera.h"
#include "Player.h"
#include <list>
struct CB_GAMEOBJECT_INFO { XMFLOAT4X4 m_xmf4x4World; };
struct Ray
{
	XMFLOAT3 Dir;			// 광선의 방향벡터
	XMFLOAT3 Pos;			// 광선의 위치벡터		
};

class CShader
{
public:
	XMFLOAT4X4 m_xmf4x4World;
	Ray							ray;			//광선
	XMFLOAT3 ex_p;
	CShader();
	virtual ~CShader();
	int sElapsedTime=0;
private: 
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; } void Release() { if (--m_nReferences <= 0) delete this; }
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(); 
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(); 
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob); 
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob); 
	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList); 
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World);
	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList); 
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
protected: 
		   //파이프라인 상태 객체들의 리스트(배열)이다.
	ID3D12PipelineState **m_ppd3dPipelineStates = NULL;
	int m_nPipelineStates = 0;
};
class CPlayerShader : public CShader {
public: 
	CPlayerShader();
	virtual ~CPlayerShader();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob); 
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
};

class CObjectsShader : public CShader {
public:    
	CObjectsShader();   
	virtual ~CObjectsShader();
		  
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void AnimateObjects(float fTimeElapsed, CAirplanePlayer * m_pPlayer);
	virtual void ReleaseObjects();  
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, CAirplanePlayer * m_pPlayer);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, CAirplanePlayer* m_pPlayer);
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void CheckObjectByWallCollisions();	// 벽, 오브젝트 충돌
	virtual void CheckObjectByObjectCollisions(); // 오브젝트, 오브젝트 충돌
	virtual void CheckObjectByBulletsCollisions();	// 오브젝트, 총알 충돌
	virtual void CheckBossByWallCollisions();	// 보스, 벽 충돌
	virtual void CheckBossByBulletCollisions();	// 보스, 총알 충돌
protected: 
	CGameObject * *	m_Bullet;
	CGameObject ** boss_bullet;
	CExplosiveObject ** m_ppObjects; 
	CExplosiveObject ** m_ppObjectsRed;
	CWallsObject* m_pWallsObjects=NULL;
	Particles **m_ParticleObjectBlue;
	Particles **m_ParticleObjectRed;
	Particles **m_ParticleObjectGreen;
	Particles **m_ParticleObjectYellow;
	Particles **m_ParticleObjectCyan;
	Particles **m_ParticleObjectMagenta;
	Boss* boss;
	int							mx, my;
	CExplosiveObject			*m_pickedObject;
	bool						picked = false;	//물체에 대한 피킹이 되었으면 true로 바꾸고, 물체가 총알에 맞으면 false로 바꾼다.
												//CGameObject				**m_ppObjects;
	bool						red = false;
	XMFLOAT3 playerPos;
	XMFLOAT3 playerDir;
	bool gameOver;

	int m_nSkill_count=0;
	int m_nBossBullets = 0;
	int m_nObjects = 0;
	int m_nObjectsRed = 0;
	int m_nBullets = 0;
	int m_nParticleRed = 0;
	int m_nParticleGreen = 0;
	int m_nParticleBlue = 0;
	int m_nParticleMagenta = 0;
	int m_nParticleYellow = 0;
	int m_nParticleCyan = 0;
private:
	int particleColor=RED;
};
