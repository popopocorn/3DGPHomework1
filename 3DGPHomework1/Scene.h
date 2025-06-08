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
	//������ ���콺�� Ű���� �޽����� ó���Ѵ�.
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
	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
	CPlayer* m_pPlayer = NULL;
	POINT m_ptOldCursorPos;

protected:
	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�.
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