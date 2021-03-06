//-----------------------------------------------------------------------------
// File: Scene.h
// 내가 표현하고 싶은 게임세상
//-----------------------------------------------------------------------------

#pragma once

#include "Timer.h"
#include "Shader.h"
#include "Camera.h"
#include "Player.h"
class CScene {
public:   
	CScene();  
	~CScene();
		   
	//씬에서 마우스와 키보드 메시지를 처리한다. 
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, CAirplanePlayer* m_pPlayer);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, CAirplanePlayer * m_pPlayer);
	CGameObject *PickObjectPointedByCursor(int xClient, int yClient, CCamera*pCamera);
	//

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList); void ReleaseObjects();
		   
	void AnimateObjects(float fTimeElapsed, CAirplanePlayer *m_pPlayer);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
		   
	ID3D12PipelineState * m_pd3dPipelineState;
	void ReleaseUploadBuffers();
	//그래픽 루트 시그너쳐를 생성한다.
	ID3D12RootSignature * CreateGraphicsRootSignature(ID3D12Device * pd3dDevice);
	ID3D12RootSignature * GetGraphicsRootSignature();
protected: 
	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다. 
	CObjectsShader *m_pShaders = NULL; 
	int m_nShaders = 0;
	ID3D12RootSignature * m_pd3dGraphicsRootSignature = NULL;
};