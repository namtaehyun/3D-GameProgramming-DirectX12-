#pragma once

#define FRAME_BUFFER_WIDTH		640			//���� 640�ȼ�
#define FRAME_BUFFER_HEIGHT		480			//���� 480�ȼ�

#include "Timer.h"
#include "Scene.h"
#include "Camera.h"

class CGameFramework
{
public:
	//�÷��̾� ��ü�� ���� �������̴�. 
	CAirplanePlayer *m_pPlayer = NULL;
		
	//���������� ���콺 ��ư�� Ŭ���� ���� ���콺 Ŀ���� ��ġ�̴�. 
	POINT m_ptOldCursorPos; 
	CCamera * m_pCamera = NULL;
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);	// ���α׷��� ����
	void OnDestroy();									// ���α׷��� �Ҹ�

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRtvAndDsvDescriptorHeaps();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void ChangeSwapChainState();						//��üȭ��,â��� ��ȯ

    void BuildObjects();
    void ReleaseObjects();

    void ProcessInput();
    void AnimateObjects();
    void FrameAdvance();

	void WaitForGpuComplete();							// CPU�� GPU�� ����ȭ�� ������ִ� ��.
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);		//���콺 ó��
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);		//Ű���� ó��
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE					m_hInstance;	//�������� �ν��Ͻ��ڵ�
	HWND						m_hWnd;			//�ڵ�

	int							m_nWndClientWidth;	
	int							m_nWndClientHeight;	 
        
	IDXGIFactory4				*m_pdxgiFactory;	
	IDXGISwapChain3				*m_pdxgiSwapChain;	
	ID3D12Device				*m_pd3dDevice;		

	bool						m_bMsaa4xEnable = false;	// ���߻��ø��� �� �������� true, ������ false
	UINT						m_nMsaa4xQualityLevels = 0; // CheckBuffer~�ؼ� �� �� �ִ��� ������ Ȯ��

	static const UINT			m_nSwapChainBuffers = 2;
	UINT						m_nSwapChainBufferIndex;	// �ĸ���۸� ��� �׷����ϴ����� ��Ÿ���� �ε���

	ID3D12Resource				*m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers]; //����ü�� ���۸� �����ϴ� �������̽�����
	ID3D12DescriptorHeap		*m_pd3dRtvDescriptorHeap;						   // �並 ��������� ��
	UINT						m_nRtvDescriptorIncrementSize;					   // ����Ÿ�ٺ��� ������

	ID3D12Resource				*m_pd3dDepthStencilBuffer;						// ���ҽ�
	ID3D12DescriptorHeap		*m_pd3dDsvDescriptorHeap;						// Depthstencil�並 ��������� ��
	UINT						m_nDsvDescriptorIncrementSize;					// ������ �Ѱ��̴�. (�ʿ����) �׳� ����Ÿ�ٺ䵵 �����ϱ� �����

	ID3D12CommandAllocator		*m_pd3dCommandAllocator;						
	ID3D12CommandQueue			*m_pd3dCommandQueue;
	ID3D12GraphicsCommandList	*m_pd3dCommandList;

	ID3D12Fence					*m_pd3dFence;									//CPU�� GPU ����ȭ
	UINT64						m_nFenceValues[m_nSwapChainBuffers];			//�ĸ���� �ΰ��� ���� ����� �潺�� �ٸ� �� �־ �̷��� ��.
	HANDLE						m_hFenceEvent;									//�潺�� �̺�Ʈ�� �ʿ���.

#if defined(_DEBUG)
	ID3D12Debug					*m_pd3dDebugController = NULL;
#endif

	CScene						*m_pScene;

	CGameTimer					m_GameTimer;									//���� �����ӿ��� ���� �����ӱ��� ���ʰ� �������� Ȯ���ϰų� �ð����õ� �༮���� ����ϱ� ���� Ÿ�̸�
	_TCHAR						m_pszFrameRate[50];								// �����ӷ���Ʈ�� ����ϱ� ���� ����
};