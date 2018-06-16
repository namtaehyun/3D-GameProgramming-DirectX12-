#pragma once

#define FRAME_BUFFER_WIDTH		640			//가로 640픽셀
#define FRAME_BUFFER_HEIGHT		480			//세로 480픽셀

#include "Timer.h"
#include "Scene.h"
#include "Camera.h"

class CGameFramework
{
public:
	//플레이어 객체에 대한 포인터이다. 
	CAirplanePlayer *m_pPlayer = NULL;
		
	//마지막으로 마우스 버튼을 클릭할 때의 마우스 커서의 위치이다. 
	POINT m_ptOldCursorPos; 
	CCamera * m_pCamera = NULL;
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);	// 프로그램의 생성
	void OnDestroy();									// 프로그램의 소멸

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRtvAndDsvDescriptorHeaps();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void ChangeSwapChainState();						//전체화면,창모드 전환

    void BuildObjects();
    void ReleaseObjects();

    void ProcessInput();
    void AnimateObjects();
    void FrameAdvance();

	void WaitForGpuComplete();							// CPU와 GPU의 동기화를 담당해주는 것.
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);		//마우스 처리
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);		//키보드 처리
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE					m_hInstance;	//윈도우의 인스턴스핸들
	HWND						m_hWnd;			//핸들

	int							m_nWndClientWidth;	
	int							m_nWndClientHeight;	 
        
	IDXGIFactory4				*m_pdxgiFactory;	
	IDXGISwapChain3				*m_pdxgiSwapChain;	
	ID3D12Device				*m_pd3dDevice;		

	bool						m_bMsaa4xEnable = false;	// 다중샘플링을 할 수있으면 true, 없으면 false
	UINT						m_nMsaa4xQualityLevels = 0; // CheckBuffer~해서 할 수 있는지 없는지 확인

	static const UINT			m_nSwapChainBuffers = 2;
	UINT						m_nSwapChainBufferIndex;	// 후면버퍼를 어디에 그려야하는지를 나타내는 인덱스

	ID3D12Resource				*m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers]; //스왑체인 버퍼를 저장하는 인터페이스버퍼
	ID3D12DescriptorHeap		*m_pd3dRtvDescriptorHeap;						   // 뷰를 만들기위한 힙
	UINT						m_nRtvDescriptorIncrementSize;					   // 렌더타겟뷰의 사이즈

	ID3D12Resource				*m_pd3dDepthStencilBuffer;						// 리소스
	ID3D12DescriptorHeap		*m_pd3dDsvDescriptorHeap;						// Depthstencil뷰를 만들기위한 힙
	UINT						m_nDsvDescriptorIncrementSize;					// 무조건 한개이다. (필요없다) 그냥 렌더타겟뷰도 있으니까 만든거

	ID3D12CommandAllocator		*m_pd3dCommandAllocator;						
	ID3D12CommandQueue			*m_pd3dCommandQueue;
	ID3D12GraphicsCommandList	*m_pd3dCommandList;

	ID3D12Fence					*m_pd3dFence;									//CPU와 GPU 동기화
	UINT64						m_nFenceValues[m_nSwapChainBuffers];			//후면버퍼 두개가 서로 사용할 펜스가 다를 수 있어서 이렇게 함.
	HANDLE						m_hFenceEvent;									//펜스는 이벤트가 필요함.

#if defined(_DEBUG)
	ID3D12Debug					*m_pd3dDebugController = NULL;
#endif

	CScene						*m_pScene;

	CGameTimer					m_GameTimer;									//이전 프레임에서 현재 프레임까지 몇초가 지났는지 확인하거나 시간관련된 녀석들을 사용하기 위한 타이머
	_TCHAR						m_pszFrameRate[50];								// 프레임레이트를 출력하기 위한 변수
};