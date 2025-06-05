#include<fstream>
#include<iostream>
#include<algorithm>
#include "stdafx.h"
#include "Mesh.h"

CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{

}

CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();

}

void CMesh::ReleaseUploadBuffers()
{
	if(m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
}


void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	Render(pd3dCommandList, 1);
}


void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, nInstances, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0);
	}
}
void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances,
	D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView)
{
	//정점 버퍼 뷰와 인스턴싱 버퍼 뷰를 입력-조립 단계에 설정한다.
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[] = { m_d3dVertexBufferView,
   d3dInstancingBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, _countof(pVertexBufferViews),
		pVertexBufferViews);
	Render(pd3dCommandList, nInstances);
}

void CMesh::setOOBB() {

	std::vector<CDiffusedVertex> temp = vertices;

	if (temp.empty()) {
		OutputDebugString(L"메쉬ㅇㄷ?\n");
		return;
	}

	auto minmaxX = std::minmax_element(temp.begin(), temp.end(),
		[](const CDiffusedVertex& a, const CDiffusedVertex& b) {
			return a.m_xmf3Position.x < b.m_xmf3Position.x;
		});

	auto minmaxY = std::minmax_element(temp.begin(), temp.end(),
		[](const CDiffusedVertex& a, const CDiffusedVertex& b) {
			return a.m_xmf3Position.y < b.m_xmf3Position.y;
		});

	auto minmaxZ = std::minmax_element(temp.begin(), temp.end(),
		[](const CDiffusedVertex& a, const CDiffusedVertex& b) {
			return a.m_xmf3Position.z < b.m_xmf3Position.z;
		});

	XMFLOAT3 center = XMFLOAT3(
		(minmaxX.first->m_xmf3Position.x + minmaxX.second->m_xmf3Position.x) / 2,
		(minmaxX.first->m_xmf3Position.y + minmaxX.second->m_xmf3Position.y) / 2,
		(minmaxX.first->m_xmf3Position.z + minmaxX.second->m_xmf3Position.z) / 2
	);
	XMFLOAT3 extents = XMFLOAT3(
		(minmaxX.second->m_xmf3Position.x - minmaxX.first->m_xmf3Position.x) / 2.0f,
		(minmaxY.second->m_xmf3Position.y - minmaxY.first->m_xmf3Position.y) / 2.0f,
		(minmaxZ.second->m_xmf3Position.z - minmaxZ.first->m_xmf3Position.z) / 2.0f
	);
	XMFLOAT4 orientation = XMFLOAT4(0, 0, 0, 1);

	m_xmOOBB.Center = center;
	m_xmOOBB.Extents = extents;
	m_xmOOBB.Orientation = orientation;

	//std::wstring debugStr = L"OBB Bounding Box Info:\n";
	//debugStr += L"Center - X: " + std::to_wstring(m_xmOOBB.Center.x) + L"\n";
	//debugStr += L"Center - Y: " + std::to_wstring(m_xmOOBB.Center.y) + L"\n";
	//debugStr += L"Center - Z: " + std::to_wstring(m_xmOOBB.Center.z) + L"\n";

	//debugStr += L"Extents - X: " + std::to_wstring(m_xmOOBB.Extents.x) + L"\n";
	//debugStr += L"Extents - Y: " + std::to_wstring(m_xmOOBB.Extents.y) + L"\n";
	//debugStr += L"Extents - Z: " + std::to_wstring(m_xmOOBB.Extents.z) + L"\n";

	//debugStr += L"Orient - X: " + std::to_wstring(m_xmOOBB.Orientation.x) + L"\n";
	//debugStr += L"Orient - Y: " + std::to_wstring(m_xmOOBB.Orientation.y) + L"\n";
	//debugStr += L"Orient - Z: " + std::to_wstring(m_xmOOBB.Orientation.z) + L"\n";
	//	
	//// 디버그 뷰어에 출력
	//OutputDebugStringW(debugStr.c_str());

}

BOOL CMesh::RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance)
{
	float fHitDistance;
	BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);
	if (bIntersected && (fHitDistance < *pfNearHitDistance)) *pfNearHitDistance = fHitDistance;



	return(bIntersected);
}


int CMesh::CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float& pfNearHitDistance)
{
	int nIntersections = 0;
	bool bIntersected = m_xmOOBB.Intersects(xmvPickRayOrigin, xmvPickRayDirection, pfNearHitDistance);
	if (bIntersected)
	{
		for (int i = 0; i < vertices.size()/3;++i) {
			XMVECTOR v0 = XMLoadFloat3(&(vertices[i * 3].m_xmf3Position));
			XMVECTOR v1 = XMLoadFloat3(&(vertices[i * 3 + 1].m_xmf3Position));
			XMVECTOR v2 = XMLoadFloat3(&(vertices[i * 3 + 2].m_xmf3Position));
			BOOL bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, &pfNearHitDistance);
			if (bIntersected) nIntersections++;
		}

	}
	return(nIntersections);
}
 
CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice,
		pd3dCommandList)
{
	//직육면체는 꼭지점(정점)이 8개이다.
	ReadModel("cube1.obj", vertices, XMFLOAT3(fWidth, fHeight, fDepth), XMFLOAT4());
	m_nVertices = vertices.size();
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, vertices.data(),
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	setOOBB();
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}

EnemyMesh::EnemyMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice,
		pd3dCommandList)
{
	//직육면체는 꼭지점(정점)이 8개이다.
	ReadModel("Enemy.obj", vertices, XMFLOAT3(fWidth, fHeight, fDepth), XMFLOAT4(1.0, 0.0, 0.0, 1.0));
	m_nVertices = vertices.size();
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, vertices.data(),
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	setOOBB();
}

EnemyMesh::~EnemyMesh()
{
}

CTankMeshDiffused::CTankMeshDiffused(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList)
{
	
	ReadModel("PUMA_Tank.obj", vertices, XMFLOAT3(10, 10, 10), XMFLOAT4(0.0, 1.0, 0.0, 1.0));
	m_nVertices = vertices.size();
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, vertices.data(),
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	setOOBB();
}
CTankMeshDiffused::~CTankMeshDiffused()
{
}

void ReadModel(const char* modelName, std::vector<CDiffusedVertex>& vertexArray, XMFLOAT3 scale, XMFLOAT4 color) {

	std::ifstream in{ modelName };
	std::vector<CDiffusedVertex> tempVertex;
	std::vector<int> tempIndex;
	float offset = 0.0f;
	if (modelName == "PUMA_Tank.obj")
		offset = 6.0;

	while (in) {
		std::string line;
		in >> line;
		if (line == "v") {
			float x, y, z;
			in >> x >> y >> z;
			tempVertex.push_back(CDiffusedVertex(XMFLOAT3(x * scale.x - offset, y * scale.y,z * scale.z),
				Vector4::Add(color, RANDOM_COLOR)));

		}
		else if (line == "vn") {
			float x, y, z;
			in >> x >> y >> z;
		}
		else if (line == "f") {
			char a;
			int i1, i2, i3;
			int t;
			in >> i1 >> a >> t >> a >> t >> i2 >> a >> t >> a >> t >> i3 >> a >> t >> a >> t;
			tempIndex.push_back(--i1);
			tempIndex.push_back(--i2);
			tempIndex.push_back(--i3);
		}
	}
	for (int i = 0; i < tempIndex.size(); ++i) {
		vertexArray.push_back(tempVertex[tempIndex[i]]);
	}
}

RollercoaterRail::RollercoaterRail(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4 xmf4Color)
	: CMesh(pd3dDevice, pd3dCommandList) 
{
	ReadModel("rail.obj", vertices, XMFLOAT3(1, 1, 1), XMFLOAT4(1.0, 1.0, 0.0, 1.0));
	m_nVertices = vertices.size();
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, vertices.data(),
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	setOOBB();
}

RollercoaterRail::~RollercoaterRail()
{

}

CartModel::CartModel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4 xmf4Color)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	ReadModel("cube1.obj", vertices, XMFLOAT3(7, 5, 10), XMFLOAT4(0.0, 1.0, 0.0, 1.0));
	m_nVertices = vertices.size();
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, vertices.data(),
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	setOOBB();
}

CartModel::~CartModel()
{
}
