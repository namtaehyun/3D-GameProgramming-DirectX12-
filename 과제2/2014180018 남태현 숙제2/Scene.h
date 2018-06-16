//-----------------------------------------------------------------------------
// File: Scene.h
// ���� ǥ���ϰ� ���� ���Ӽ���
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
		   
	//������ ���콺�� Ű���� �޽����� ó���Ѵ�. 
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, CAirplanePlayer* m_pPlayer);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, CAirplanePlayer * m_pPlayer);
	CGameObject *PickObjectPointedByCursor(int xClient, int yClient, CCamera*pCamera);
	//

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList); void ReleaseObjects();
		   
	void AnimateObjects(float fTimeElapsed, CAirplanePlayer *m_pPlayer);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
		   
	ID3D12PipelineState * m_pd3dPipelineState;
	void ReleaseUploadBuffers();
	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	ID3D12RootSignature * CreateGraphicsRootSignature(ID3D12Device * pd3dDevice);
	ID3D12RootSignature * GetGraphicsRootSignature();
protected: 
	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�. 
	CObjectsShader *m_pShaders = NULL; 
	int m_nShaders = 0;
	ID3D12RootSignature * m_pd3dGraphicsRootSignature = NULL;
};