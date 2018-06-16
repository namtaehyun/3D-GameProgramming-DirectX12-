#include "stdafx.h"
#include "Mesh.h"


CMesh::CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
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
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pd3dVertexUploadBuffer)
		m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;

};
void CMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology); pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	if (m_pd3dIndexBuffer) {
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
		//인덱스 버퍼가 있으면 인덱스 버퍼를 파이프라인(IA: 입력 조립기)에 연결하고 인덱스를 사용하여 렌더링한다. 
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}


int CCubeMeshDiffused::ReturnColor(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection, XMFLOAT4X4 worldTransform)
{
	//광선은 모델 좌표계로 표현된다.
	XMVECTOR xmRayOrigin = XMLoadFloat3(&xmf3RayOrigin);
	XMVECTOR xmRayDirection = XMLoadFloat3(&xmf3RayDirection);
	float fNearHitDistance = 10000000;

	float fHitDistance;

	/*메쉬의 모든 프리미티브(삼각형)들에 대하여 픽킹 광선과의 충돌을 검사한다. 충돌하는 모든 삼각형을 찾아 광선의
	시작점(실제로는 카메라 좌표계의 원점)에 가장 가까운 삼각형을 찾는다.*/
	for (int i = 0; i < 36; i+=3)
	{
		// pVertices가 모델좌표임....;;
		XMVECTOR v0 = XMVector3TransformCoord(XMLoadFloat3(&XMFLOAT3(pVertices[i].GetPos().x, pVertices[i].GetPos().y, pVertices[i].GetPos().z))
			, XMLoadFloat4x4(&worldTransform));
		XMVECTOR v1 = XMVector3TransformCoord(XMLoadFloat3(&XMFLOAT3(pVertices[i+1].GetPos().x, pVertices[i+1].GetPos().y, pVertices[i+1].GetPos().z))
			, XMLoadFloat4x4(&worldTransform));
		XMVECTOR v2 = XMVector3TransformCoord(XMLoadFloat3(&XMFLOAT3(pVertices[i+2].GetPos().x, pVertices[i+2].GetPos().y, pVertices[i+2].GetPos().z))
			, XMLoadFloat4x4(&worldTransform));

		//pVertices의 위치벡터와 월드변환을 곱해준다.

		// v0 v1 v2는 삼각형 정점들의 위치벡터

		BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0,v1, v2, fHitDistance);	// 광선x삼각형 충돌검사 
		if (bIntersected)	// 충돌했다면
		{
			if (fHitDistance < fNearHitDistance)	// 거리를 잰다음에
			{
				idx = i;
				fNearHitDistance = fHitDistance;	// 가까운넘으로 초기화
			}
		}
	}
	if (-1 < idx&&idx < 6)
	{
		return RED;
	}
	else if (5 < idx&&idx < 12)
	{
		return BLUE;
	}
	else if (11 < idx&&idx < 18)
	{
		return GREEN;
	}
	else if (17 < idx&&idx < 24)
	{
		return MAGENTA;
	}
	else if (23 < idx&&idx < 30)
	{
		return YELLOW;
	}
	else if (29 < idx&&idx < 36)
	{
		return CYAN;
	}
	return 0;
}


CTriangleMesh::CTriangleMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{ //삼각형 메쉬를 정의한다. 
	m_nVertices = 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	/*정점(삼각형의 꼭지점)의 색상은 시계방향 순서대로 빨간색, 녹색,
	파란색으로 지정한다. RGBA(Red, Green, Blue, Alpha) 4개의 파라메터를
	사용하여 색상을 표현한다. 각 파라메터는 0.0~1.0 사이의 실수값을 가진다.*/
	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));
	//삼각형 메쉬를 리소스(정점 버퍼)로 생성한다.
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	//정점 버퍼 뷰를 생성한다. 
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}
CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
	float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList) { //직육면체는 6개의 면 가로(x-축) 길이 

																					  //직육면체는 꼭지점(정점)이 8개이다.
	m_nVertices = 36;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//fWidth: 직육면체 가로(x-축) 길이, fHeight: 직육면체 세로(y-축) 길이, fDepth: 직육면체 깊이(z-축) 길이
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	
	int i = 0;
	//정점 버퍼 데이터는 삼각형 리스트이므로 36개의 정점 데이터를 준비한다.
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz),ONE_RED_COLOR);		//0
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz),ONE_RED_COLOR);		//1
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz),ONE_RED_COLOR);		//2
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형					
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz),ONE_RED_COLOR);		//3
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz),ONE_RED_COLOR);		//4
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz),ONE_RED_COLOR);		//5

	// 0,1,2	3,4,5 -> 빨간색 삼각형 2개

	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), ONE_BLUE_COLOR);		//6
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), ONE_BLUE_COLOR);		//7
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), ONE_BLUE_COLOR);		//8
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형					
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), ONE_BLUE_COLOR);		//9
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), ONE_BLUE_COLOR);		//10
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), ONE_BLUE_COLOR);		//11
															 
	// 6,7,8	9,10,11 -> 파란색 삼각형 2개				 
															 
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형					 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), ONE_GREEN_COLOR);		//12
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), ONE_GREEN_COLOR);		//13
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), ONE_GREEN_COLOR);		//14
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형					
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), ONE_GREEN_COLOR);		//15
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), ONE_GREEN_COLOR);		//16
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), ONE_GREEN_COLOR);		//17
															  
	// 12,13,14		15,16,17 -> 초록색 삼각형 2개			  
															  
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형					
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), ONE_MAGENTA_COLOR);	//18
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), ONE_MAGENTA_COLOR);	//19
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), ONE_MAGENTA_COLOR);	//20
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형				
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), ONE_MAGENTA_COLOR);	//21
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), ONE_MAGENTA_COLOR);	//22
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), ONE_MAGENTA_COLOR);	//23
															 
	// 18,19,20		21,22,23 -> 자홍색 삼각형 2개			 
															 
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형					 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), ONE_YELLOW_COLOR);	//24
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), ONE_YELLOW_COLOR);	//25
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), ONE_YELLOW_COLOR);	//26
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형					 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), ONE_YELLOW_COLOR);	//27
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), ONE_YELLOW_COLOR);	//28
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), ONE_YELLOW_COLOR);	//29
															  
	// 24,25,26		27,28,29 -> 빨간색 삼각형 2개			  
															  
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형					  
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), ONE_CYAN_COLOR);		//30
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), ONE_CYAN_COLOR);		//31
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), ONE_CYAN_COLOR);		//32
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형					 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), ONE_CYAN_COLOR);		//33
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), ONE_CYAN_COLOR);		//34
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), ONE_CYAN_COLOR);		//35

	// 30,31,32		33,34,35 -> 빨간색 삼각형 2개

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	//정점 버퍼 뷰를 생성한다.
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}



CCubeMeshDiffused::~CCubeMeshDiffused() { }

CAirplaneMeshDiffused::CAirplaneMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
	float fWidth, float fHeight, float fDepth, XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList) {

	m_nVertices = 24 * 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	//위의 그림과 같은 비행기 메쉬를 표현하기 위한 정점 데이터이다. 
	CDiffusedVertex pVertices[24 * 3];
	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3);
	int i = 0;
	//비행기 메쉬의 위쪽 면 z
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	//비행기 메쉬의 아래쪽 면 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	//비행기 메쉬의 오른쪽 면 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	//비행기 메쉬의 뒤쪽/오른쪽 면 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	//비행기 메쉬의 왼쪽 면 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	//비행기 메쉬의 뒤쪽/왼쪽 면 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}
CAirplaneMeshDiffused::~CAirplaneMeshDiffused() { }


CWallMeshDiffused::CWallMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList) { //직육면체는 6개의 면 가로(x-축) 길이 

																																															 //직육면체는 꼭지점(정점)이 8개이다.
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 2.0f, fy = fHeight * 2.0f, fz = fDepth * 2.0f;
	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다. 
	CDiffusedVertex pVertices[8];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), Vector4::Add(YELLOW_COLOR,RANDOM_COLOR));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), Vector4::Add(RED_COLOR, RANDOM_COLOR));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), Vector4::Add(BLUE_COLOR, RANDOM_COLOR));
	pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), Vector4::Add(GREEN_COLOR, RANDOM_COLOR));
	pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), Vector4::Add(BLUE_COLOR, RANDOM_COLOR));
	pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), Vector4::Add(RED_COLOR, RANDOM_COLOR));
	pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), Vector4::Add(YELLOW_COLOR, RANDOM_COLOR));
	pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), Vector4::Add(MAGENTA_COLOR, RANDOM_COLOR));
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride; m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	/*인덱스 버퍼는 직육면체의 6개의 면(사각형)에 대한 기하 정보를 갖는다. 삼각형 리스트로 직육면체를 표현할 것이 므로 각 면은 2개의 삼각형을 가지고 각 삼각형은 3개의 정점이 필요하다. 즉, 인덱스 버퍼는 전체 36(=6*2*3)개의 인 덱스를 가져야 한다.*/
	m_nIndices = 24;
	UINT pnIndices[24];
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형  
	pnIndices[0] = 0;
	pnIndices[1] = 1;
	pnIndices[2] = 3;
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형   
	pnIndices[3] = 3;
	pnIndices[4] = 1;
	pnIndices[5] = 2;
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형    
	pnIndices[6] = 7;
	pnIndices[7] = 4;
	pnIndices[8] = 3;
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형  
	pnIndices[9] = 3;
	pnIndices[10] = 4;
	pnIndices[11] = 0;
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형    
	pnIndices[12] = 5;
	pnIndices[13] = 6;
	pnIndices[14] = 1;
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형    
	pnIndices[15] = 1;
	pnIndices[16] = 6;
	pnIndices[17] = 2;
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	pnIndices[18] = 5;
	pnIndices[19] = 4;
	pnIndices[20] = 6;
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형 
	pnIndices[21] = 6;
	pnIndices[22] = 4;
	pnIndices[23] = 7;

	//인덱스 버퍼를 생성한다. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	//인덱스 버퍼 뷰를 생성한다. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}


CWallMeshDiffused::~CWallMeshDiffused()
{

}



CCubeColorMeshDiffused::CCubeColorMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
	XMFLOAT4 xmf3Color, float fWidth, float fHeight , float fDepth ) : CMesh(pd3dDevice, pd3dCommandList)
{
	//직육면체는 꼭지점(정점)이 8개이다. 
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다. 
	CDiffusedVertex pVertices[8];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), Vector4::Add(xmf3Color, RANDOM_COLOR));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), Vector4::Add(xmf3Color, RANDOM_COLOR));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), Vector4::Add(xmf3Color, RANDOM_COLOR));
	pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), Vector4::Add(xmf3Color, RANDOM_COLOR));
	pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), Vector4::Add(xmf3Color, RANDOM_COLOR));
	pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), Vector4::Add(xmf3Color, RANDOM_COLOR));
	pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), Vector4::Add(xmf3Color, RANDOM_COLOR));
	pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), Vector4::Add(xmf3Color, RANDOM_COLOR));
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride; m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	/*인덱스 버퍼는 직육면체의 6개의 면(사각형)에 대한 기하 정보를 갖는다. 삼각형 리스트로 직육면체를 표현할 것이 므로 각 면은 2개의 삼각형을 가지고 각 삼각형은 3개의 정점이 필요하다. 즉, 인덱스 버퍼는 전체 36(=6*2*3)개의 인 덱스를 가져야 한다.*/
	m_nIndices = 36;
	UINT pnIndices[36];
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형  
	pnIndices[0] = 3;
	pnIndices[1] = 1;
	pnIndices[2] = 0;
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형   
	pnIndices[3] = 2;
	pnIndices[4] = 1;
	pnIndices[5] = 3;
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형   
	pnIndices[6] = 0;
	pnIndices[7] = 5;
	pnIndices[8] = 4;
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형  
	pnIndices[9] = 1;
	pnIndices[10] = 5;
	pnIndices[11] = 0;
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형    
	pnIndices[12] = 3;
	pnIndices[13] = 4;
	pnIndices[14] = 7;
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형  
	pnIndices[15] = 0;
	pnIndices[16] = 4;
	pnIndices[17] = 3;
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형    
	pnIndices[18] = 1;
	pnIndices[19] = 6;
	pnIndices[20] = 5;
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형    
	pnIndices[21] = 2;
	pnIndices[22] = 6;
	pnIndices[23] = 1;
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형  
	pnIndices[24] = 2;
	pnIndices[25] = 7;
	pnIndices[26] = 6;
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형   
	pnIndices[27] = 3;
	pnIndices[28] = 7;
	pnIndices[29] = 2;
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	pnIndices[30] = 6;
	pnIndices[31] = 4;
	pnIndices[32] = 5;
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형 
	pnIndices[33] = 7;
	pnIndices[34] = 4;
	pnIndices[35] = 6;

	//인덱스 버퍼를 생성한다. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	//인덱스 버퍼 뷰를 생성한다. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}
CCubeColorMeshDiffused::~CCubeColorMeshDiffused()
{

}