#pragma once
#include"Timer.h"
#include"Scene.h"
#include"Camera.h"
#include"Player.h"

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();
	void ChangeSwapChainState();
	void MoveToNextFrame();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);

	void OnDestroy();

	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void WaitForGpuComplete();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void reqeustChangeScene(CScene* next) { nextScene = next; }
	void ChangeScene();
	void popScene();
	
	
	//마지막으로 마우스 버튼을 클릭할 때의 마우스 커서의 위치이다.
	POINT m_ptOldCursorPos;

private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

	
	IDXGIFactory4* m_pdxgiFactory; //DXGI팩토리 인터페이스 포인터
	IDXGISwapChain3* m_pdxgiSwapChain;// 스왑체인 인터페이스 포인터, 디스플레이 제어용
	ID3D12Device* m_pd3dDevice; // 리소스 생성용 포인터

	bool m_bMsaa4xEnable = false;

	UINT m_nMsaa4xQualityLevels = 0;

	static const UINT m_nSwapChainBuffers = 2;
	UINT m_nSwapChainBufferIndex;

	ID3D12Resource* m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;

	ID3D12Resource* m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;

	ID3D12CommandQueue* m_pd3dCommandQueue;
	ID3D12CommandAllocator* m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList* m_pd3dCommandList;

	ID3D12PipelineState* m_pd3dpipelinedState;

	ID3D12Fence* m_pd3dFence;
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent;

	CCamera* m_pCamera = NULL;

	CGameTimer m_GameTimer;
	_TCHAR m_pszFrameRate[50];

	std::vector<CScene*> m_pScene;
	CScene* nextScene{};
public:
	CGameTimer GetTimer() {	return m_GameTimer; }
};

