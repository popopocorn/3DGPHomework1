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
	virtual void ProcessInput(HWND hWnd);
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	void ReleaseObjects();
	bool ProcessInput(UCHAR* pKeysBuffer);
	virtual void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void ReleaseUploadBuffers();
	void CheckCollisions();
	void CheckDelete();
	//그래픽 루트 시그너쳐를 생성한다.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
	CPlayer* m_pPlayer = NULL;
	POINT m_ptOldCursorPos;

protected:
	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다.
	std::vector<CObjectsShader*> m_pShaders;
	int m_nShaders = 0;
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	CGameObject* m_pLockedObject = NULL;
	CGameFramework* m_pFramework;
	int EnemyCnt{ 10 };
	CGameObject* YouWin;
};

class Title : public CScene {
public:
	Title(CGameFramework* framework) :CScene(framework) { m_pFramework = framework; }
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ProcessInput(HWND hWnd) {};
	void toTheNext();
private:
	CGameObject* explosion;
};


class Rollercoaster : public CScene {
public:
	Rollercoaster(CGameFramework* framework) : CScene(framework) { m_pFramework = framework; };
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ProcessInput(HWND hWnd);
private:
	float rideTime{};
	float speed{};
};