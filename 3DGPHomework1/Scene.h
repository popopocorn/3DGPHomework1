#pragma once

#include"Shader.h"
#include"Camera.h"
#include"Player.h"


class CGameFramework;
class CScene
{
public:
	CScene(CGameFramework* framework);
	//~CScene();
	//씬에서 마우스와 키보드 메시지를 처리한다.
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	void ProcessInput(HWND hWnd);
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	void ReleaseObjects();
	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void ReleaseUploadBuffers();
	//그래픽 루트 시그너쳐를 생성한다.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
	CPlayer* m_pPlayer = NULL;
	POINT m_ptOldCursorPos;
	
protected:
	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다.
	CObjectsShader* m_pShaders = NULL;
	int m_nShaders = 0;
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	CGameFramework* m_pFramework;
};

class Title : public CScene {
public:
	Title(CGameFramework* framework) {};
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	
};