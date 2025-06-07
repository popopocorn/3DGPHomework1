#include"stdafx.h"
#include"Scene.h"
#include"CGameFramework.h"

CScene::CScene(CGameFramework* framework)
{
	m_pFramework = framework;

}
bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:

		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);

		break;
	case WM_RBUTTONDOWN:

		m_pLockedObject = PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam), m_pPlayer->GetCamera());

		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		//마우스 캡쳐를 해제한다.
		::ReleaseCapture();
		break;
	}
	return false;
}
bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			m_pFramework->reqeustChangeScene(new Title(m_pFramework));
			break;
		case VK_CONTROL:
			dynamic_cast<CTankPlayer*>(m_pPlayer)->fireBullet(m_pLockedObject);
			break;
		case 'W':
			EnemyCnt = 0;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return false;
}
void CScene::ProcessInput(HWND hWnd)
{
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;
	/*키보드의 상태 정보를 반환한다. 화살표 키(‘→’, ‘←’, ‘↑’, ‘↓’)를 누르면 플레이어를 오른쪽/왼쪽(로컬 x-축), 앞/
   뒤(로컬 z-축)로 이동한다. ‘Page Up’과 ‘Page Down’ 키를 누르면 플레이어를 위/아래(로컬 y-축)로 이동한다.*/
	if (::GetKeyboardState(pKeyBuffer))
	{
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;

	}
	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;
	/*마우스를 캡쳐했으면 마우스가 얼마만큼 이동하였는 가를 계산한다. 마우스 왼쪽 또는 오른쪽 버튼이 눌러질 때의
   메시지(WM_LBUTTONDOWN, WM_RBUTTONDOWN)를 처리할 때 마우스를 캡쳐하였다. 그러므로 마우스가 캡쳐된
   것은 마우스 버튼이 눌려진 상태를 의미한다. 마우스 버튼이 눌려진 상태에서 마우스를 좌우 또는 상하로 움직이면 플
   레이어를 x-축 또는 y-축으로 회전한다.*/
	if (::GetCapture() == hWnd)
	{
		//마우스 커서를 화면에서 없앤다(보이지 않게 한다).
		::SetCursor(NULL);
		//현재 마우스 커서의 위치를 가져온다.
		::GetCursorPos(&ptCursorPos);
		//마우스 버튼이 눌린 상태에서 마우스가 움직인 양을 구한다.
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		//마우스 커서의 위치를 마우스가 눌려졌던 위치로 설정한다.
		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);

	}
	//마우스 또는 키 입력이 있으면 플레이어를 이동하거나(dwDirection) 회전한다(cxDelta 또는 cyDelta).
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			/*cxDelta는 y-축의 회전을 나타내고 cyDelta는 x-축의 회전을 나타낸다. 오른쪽 마우스 버튼이 눌려진 경우
		   cxDelta는 z-축의 회전을 나타낸다.*/
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}
		/*플레이어를 dwDirection 방향으로 이동한다(실제로는 속도 벡터를 변경한다). 이동 거리는 시간에 비례하도록 한다.
		플레이어의 이동 속력은 (50/초)로 가정한다.*/

		if (dwDirection)
		{
			m_pPlayer->Move(dwDirection, 50.0f * m_pFramework->GetTimer().GetTimeElapsed(), true);
		}
	}
	//플레이어를 실제로 이동하고 카메라를 갱신한다. 중력과 마찰력의 영향을 속도 벡터에 적용한다.
	m_pPlayer->Update(m_pFramework->GetTimer().GetTimeElapsed());
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	
	CObjectsShader* newshader = new CObjectsShader;
	newshader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pShaders.push_back(newshader);



	
	CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList,
		5000.0f, 5.0f, 5000.0f);
	CGameObject* Plane = new GroundObject();
	Plane->SetMesh(pCubeMesh);
	Plane->SetPosition(0, -7, 0);
	Plane->debugName = 'p';
	m_pShaders[0]->m_ppObjects.push_back(Plane);
	

	CMesh* SmallCubeMesh = new EnemyMesh(pd3dDevice, pd3dCommandList);
	
	std::array<CGameObject*, 10> enemies;
	for (int i = 0; i < enemies.size(); ++i) {
		enemies[i] = new Enemy();;
		enemies[i]->SetMesh(SmallCubeMesh);
		enemies[i]->SetPosition(RANDOM_COLOR.x * 1000 - 500, 0, RANDOM_COLOR.z * 1000 - 500);
		
		enemies[i]->debugName = i;
		m_pShaders[0]->m_ppObjects.push_back(enemies[i]);
	}


	CTankPlayer* pPlayer = new CTankPlayer(pd3dDevice,
		pd3dCommandList, GetGraphicsRootSignature());
	m_pPlayer = pPlayer;


	CObjectsShader* newUIshader = new UIShader;
	newUIshader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pShaders.push_back(newUIshader);

	UIModel* UIMesh = new UIModel("youwin.obj", pd3dDevice, pd3dCommandList);
	YouWin = new UIObject();
	YouWin->SetMesh(UIMesh);
	YouWin->SetPosition(0.0, 0.0, 0.5);
	dynamic_cast<UIObject*>(YouWin)->rotateSpeed = 0;

	
}


void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	for (int i = 0; i < m_pShaders.size(); i++)
	{
		m_pShaders[i]->ReleaseShaderVariables();
		m_pShaders[i]->ReleaseObjects();
	}
	if (! m_pShaders.empty()) m_pShaders.clear();
	if (m_pPlayer) delete m_pPlayer;
}

bool CScene::ProcessInput(UCHAR* pKeysBuffer) {
	return false;
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	
	for (int i = 0; i < m_pShaders.size(); i++)
	{
		
		m_pShaders[i]->AnimateObjects(fTimeElapsed);
	}
	m_pPlayer->Animate(fTimeElapsed);

}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);
	for (int i = 0; i < m_pShaders.size(); i++)
	{
		m_pShaders[i]->Render(pd3dCommandList, pCamera);
	}
}

void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_pShaders.size(); i++) m_pShaders[i]->ReleaseUploadBuffers();
}

ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}


void CScene::CheckCollisions()
{
	bool collieded{ false };
	CTankPlayer* temp;
	if (temp = dynamic_cast<CTankPlayer*>(m_pPlayer)) {
		
	}else
		return;

	for (int i = 0; i < m_pShaders[0]->m_ppObjects.size(); ++i) {
		for(int j=0; j< temp->m_ppBullets.size(); ++j){
			if (GroundObject* gr = dynamic_cast<GroundObject*>(m_pShaders[0]->m_ppObjects[i]))
				continue;
			if (CExplosiveObject* ex = dynamic_cast<CExplosiveObject*>(m_pShaders[0]->m_ppObjects[i]))
				continue;
			if (m_pShaders[0]->m_ppObjects[i]->getOOBB().Intersects(temp->m_ppBullets[j]->getOOBB())) {
				m_pShaders[0]->m_ppObjects[i]->DoCollision();
				temp->m_ppBullets[j]->DoCollision();
				--EnemyCnt;
				m_pLockedObject = NULL;
				collieded = true;
				XMFLOAT3 a = m_pShaders[0]->m_ppObjects[i]->GetPosition();
				CGameObject* explosion = new CExplosiveObject();
				explosion->SetPosition(a);
				explosion->SetMesh(temp->m_ppBullets[j]->getMesh());
				dynamic_cast<CExplosiveObject*>(explosion)->PrepareExplosion();
				m_pShaders[0]->m_ppObjects.push_back(explosion);
				break;
			}
		}
		if (collieded)
			break;
	}
	/*wchar_t buffer[256];
	swprintf(buffer, 256, L"Bullet cnt: %d,\n", temp->m_ppBullets.size());
	OutputDebugString(buffer);*/
}

void CScene::CheckDelete()
{
	m_pShaders[0]->m_ppObjects.erase(
		std::remove_if(m_pShaders[0]->m_ppObjects.begin(), m_pShaders[0]->m_ppObjects.end(),
			[](CGameObject* e) {
				if (dynamic_cast<Enemy*>(e))
					return dynamic_cast<Enemy*>(e)->isAlive();
				else if (dynamic_cast<CExplosiveObject*>(e))
					return dynamic_cast<CExplosiveObject*>(e)->isAlive();
				else
					return false;

			}), m_pShaders[0]->m_ppObjects.end()
				);
	if (EnemyCnt == 0) {
		m_pShaders[1]->m_ppObjects.push_back(YouWin);
	}
}


ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
	D3D12_ROOT_PARAMETER pd3dRootParameters[2];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	//4x4행렬 하나 넘길 거니까 32비트 변수 16개
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	//4x4행렬 두 개 넘길 거니까 32비트 변수 32개
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;
	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void
			**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
	return(pd3dGraphicsRootSignature);
}


CGameObject* CScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / (float)pCamera->GetViewport().Width) - 1) / pCamera->GetProjectionMatrix()._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / (float)pCamera->GetViewport().Height) - 1) / pCamera->GetProjectionMatrix()._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmmtxView = XMLoadFloat4x4(&pCamera->GetViewMatrix());

	int nIntersected = 0;
	float fNearestHitDistance = FLT_MAX;
	CGameObject* pNearestObject = NULL;
	for (int i = 0; i < m_pShaders.size(); i++)
	{
		
		for (int j = 0; j < m_pShaders[i]->m_ppObjects.size();++j) {
			float fHitDistance = FLT_MAX;
			
			nIntersected = (m_pShaders[i]->m_ppObjects[j])->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, fHitDistance);
			/*char buffer[64];
			sprintf(buffer, "dist: %f", fNearestHitDistance);
			OutputDebugStringA(buffer);*/
			if ((nIntersected > 0) && (fHitDistance < fNearestHitDistance))
			{
				fNearestHitDistance = fHitDistance;
				pNearestObject = m_pShaders[i]->m_ppObjects[j];
			}
		}
	}
	if(pNearestObject){
		if (GroundObject* ground = dynamic_cast<GroundObject*>(pNearestObject))
		{
			pNearestObject = NULL;
			OutputDebugString(L"None\n");
			
		}else

		{
			wchar_t buffer[100];
			std::wstring debugStr = L"OBB Bounding Box Info:\n";

			// char → wchar_t 직접 변환
			swprintf_s(buffer, 100, L"pNearestObject 이름: %c\n", (wchar_t)pNearestObject->debugName);

			debugStr += buffer;

			OutputDebugStringW(debugStr.c_str());
		}
	}
	else {
		OutputDebugString(L"None\n");
	}



	
	return(pNearestObject);
}





bool Title::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		::GetCursorPos(&m_ptOldCursorPos);
		//m_pFramework->reqeustChangeScene(new Rollercoaster(m_pFramework));
		m_pLockedObject = PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam), m_pPlayer->GetCamera());
		if (m_pLockedObject)
			toTheNext();
		break;
	}
	return false;
}
bool Title::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		}
	}
	return false;
}
void Title::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	CObjectsShader* newshader = new CObjectsShader;
	newshader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pShaders.push_back(newshader);

	CTankPlayer* pPlayer = new CTankPlayer(pd3dDevice,
		pd3dCommandList, GetGraphicsRootSignature());
	pPlayer->ChangeCamera(FIRST_PERSON_CAMERA, 0);
	m_pPlayer = pPlayer;

	CObjectsShader* newUIshader = new UIShader;
	newUIshader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pShaders.push_back(newUIshader);


	UIModel* UIMesh = new UIModel("NAME.obj", pd3dDevice, pd3dCommandList);
	CGameObject* Title = new UIObject();
	Title->SetMesh(UIMesh);
	Title->SetPosition(0.0, 0.0, 0.5);
	
	m_pShaders[1]->m_ppObjects.push_back(Title);
	CCubeMeshDiffused* pBulletMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 2, 2, 2);
	explosion = new CExplosiveObject();
	explosion->SetMesh(pBulletMesh);
	dynamic_cast<CExplosiveObject*>(explosion)->PrepareExplosion();
}

void Title::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_pShaders.size(); i++)
	{

		m_pShaders[i]->AnimateObjects(fTimeElapsed);
	}
	m_pPlayer->Animate(fTimeElapsed);
	for (int i = 0; i < m_pShaders[0]->m_ppObjects.size(); ++i) {
		if (dynamic_cast<CExplosiveObject*>(m_pShaders[0]->m_ppObjects[i])) {
			if (dynamic_cast<CExplosiveObject*>(m_pShaders[0]->m_ppObjects[i])->isAlive())
				m_pFramework->reqeustChangeScene(new Rollercoaster(m_pFramework));
		}
	}
}

void Title::toTheNext()
{
	m_pShaders[1]->m_ppObjects.clear();
	m_pShaders[0]->m_ppObjects.push_back(explosion);
	
	
	
}


bool Rollercoaster::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam)
{
	

	return false;
}
bool Rollercoaster::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{

		case 'N':
			m_pFramework->reqeustChangeScene(new CScene(m_pFramework));
		case VK_ESCAPE:
			m_pFramework->popScene();
			break;
		}
	}
	return false;
}
void Rollercoaster::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	CObjectsShader* newshader = new CObjectsShader;
	newshader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pShaders.push_back(newshader);
	
	Cart* pPlayer = new Cart(pd3dDevice,
		pd3dCommandList, GetGraphicsRootSignature());
	pPlayer->SetPosition(XMFLOAT3(0, 3, -2));
	pPlayer->Rotate(0, -90, 0);
	pPlayer->ChangeCamera(THIRD_PERSON_CAMERA, 0);
	m_pPlayer = pPlayer;
	CMesh* railObject = new RollercoaterRail(pd3dDevice, pd3dCommandList);

	CGameObject* r1 = new Rail();
	r1->SetMesh(railObject);
	r1->SetPosition(0, 0, 0);
	m_pShaders[0]->m_ppObjects.push_back(r1);
	

}

void Rollercoaster::AnimateObjects(float fTimeElapsed)
{
	rideTime += fTimeElapsed;
	
	if (rideTime < 2.0f){
		speed = 10;
		
	}
	else if (rideTime < 12.5f) {
		if(m_pPlayer->GetPitch() < 15)
			m_pPlayer->Rotate(-30, 0, 0);
		speed = 7;
		
	}
	else if (rideTime < 13.0f) {
		if (m_pPlayer->GetPitch() >= 330)
			m_pPlayer->Rotate(-330, 0, 0);
		
	}
	else if (rideTime < 23.0f) {
		if (m_pPlayer->GetYaw() > 180)
			m_pPlayer->Rotate(0, -9.5*fTimeElapsed, 0);
		
	}
	else if(rideTime < 26.0f){
		if (m_pPlayer->GetYaw() != 180)
			m_pPlayer->Rotate(0, 180 - m_pPlayer->GetYaw(), 0);
		if (m_pPlayer->GetPitch() < 15)
			m_pPlayer->Rotate(32, 0, 0);
		speed += 9.8*fTimeElapsed;
		
	}
	else if (rideTime < 27.0f) {
		if(m_pPlayer->GetPitch()>30 && m_pPlayer->GetPitch() < 320)
			m_pPlayer->Rotate(-62, 0, 0);
		speed -= 9.8 * fTimeElapsed;
		 
	}
	else if (rideTime < 28.0f) {
		if (m_pPlayer->GetPitch() > 320 || m_pPlayer->GetPitch() < 40)
			m_pPlayer->Rotate(75.0 * fTimeElapsed, 0, 0);
		 
	}
	else if (rideTime < 28.5f) {
		if (m_pPlayer->GetPitch() != 40)
			m_pPlayer->Rotate(40 - m_pPlayer->GetPitch(), 0, 0);
		speed += 9.8 * fTimeElapsed;
		 
	}
	else if (rideTime < 29.0f) {
		if (m_pPlayer->GetPitch() > 0 && m_pPlayer->GetPitch() < 41)
			m_pPlayer->Rotate(-75.0*fTimeElapsed, 0, 0);
		 
	}
	else if (rideTime < 31.8f) {
		if(m_pPlayer->GetPitch() != 0)
			m_pPlayer->Rotate(0- m_pPlayer->GetPitch(), 0, 0);
		if (m_pPlayer->GetYaw() > 47)
			m_pPlayer->Rotate(0, -52.0 * fTimeElapsed, 0);
		 
	}
	else if (rideTime < 33.2f) {
		if (m_pPlayer->GetYaw() != 47)
			m_pPlayer->Rotate(0, 47 - m_pPlayer->GetYaw(), 0);
		if (m_pPlayer->GetPitch() < 5 || m_pPlayer->GetPitch() > 340)
			m_pPlayer->Rotate(-60 * fTimeElapsed, 0, 0);
		 
	}
	else if (rideTime < 34.7f) {
		if (m_pPlayer->GetPitch() < 25 || m_pPlayer->GetPitch() > 330)
			m_pPlayer->Rotate(60 * fTimeElapsed, 0, 0);
		 
	}else if(rideTime < 36.0f) {
		if (m_pPlayer->GetPitch() < 30 && m_pPlayer->GetPitch() > 0)
			m_pPlayer->Rotate( -60 * fTimeElapsed, 0, 0);
		 
	}else if (rideTime < 38.2f) {
		if (m_pPlayer->GetPitch() != 0)
			m_pPlayer->Rotate(0 - m_pPlayer->GetPitch(), 0, 0);
		 
	}
	else if (rideTime < 42.0f) {
		if (m_pPlayer->GetYaw() < 50 || m_pPlayer->GetYaw() > 270)
			m_pPlayer->Rotate(0, -48.0 * fTimeElapsed, 0);
		 
	}
	else if (rideTime < 44.0f) {
		speed = 5;
		
	}
	else if (rideTime < 45.0f) {
		speed = 0;
	}
	else {
		m_pFramework->reqeustChangeScene(new CScene(m_pFramework));
		rideTime = 0;
		
	}
	m_pPlayer->Move(Vector3::ScalarProduct(m_pPlayer->GetLookVector(), fTimeElapsed * speed), false);

	
	/*char buffer[64];
	sprintf(buffer, "dist: %f\n", m_pPlayer->GetYaw());
	OutputDebugStringA(buffer);*/
	m_pPlayer->Update(m_pFramework->GetTimer().GetTimeElapsed());
}

void Rollercoaster::ProcessInput(HWND hWnd)
{
	
}
