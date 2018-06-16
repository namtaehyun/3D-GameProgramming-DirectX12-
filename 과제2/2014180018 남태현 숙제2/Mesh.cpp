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
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��.
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
		//�ε��� ���۰� ������ �ε��� ���۸� ����������(IA: �Է� ������)�� �����ϰ� �ε����� ����Ͽ� �������Ѵ�. 
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}


int CCubeMeshDiffused::ReturnColor(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection, XMFLOAT4X4 worldTransform)
{
	//������ �� ��ǥ��� ǥ���ȴ�.
	XMVECTOR xmRayOrigin = XMLoadFloat3(&xmf3RayOrigin);
	XMVECTOR xmRayDirection = XMLoadFloat3(&xmf3RayDirection);
	float fNearHitDistance = 10000000;

	float fHitDistance;

	/*�޽��� ��� ������Ƽ��(�ﰢ��)�鿡 ���Ͽ� ��ŷ �������� �浹�� �˻��Ѵ�. �浹�ϴ� ��� �ﰢ���� ã�� ������
	������(�����δ� ī�޶� ��ǥ���� ����)�� ���� ����� �ﰢ���� ã�´�.*/
	for (int i = 0; i < 36; i+=3)
	{
		// pVertices�� ����ǥ��....;;
		XMVECTOR v0 = XMVector3TransformCoord(XMLoadFloat3(&XMFLOAT3(pVertices[i].GetPos().x, pVertices[i].GetPos().y, pVertices[i].GetPos().z))
			, XMLoadFloat4x4(&worldTransform));
		XMVECTOR v1 = XMVector3TransformCoord(XMLoadFloat3(&XMFLOAT3(pVertices[i+1].GetPos().x, pVertices[i+1].GetPos().y, pVertices[i+1].GetPos().z))
			, XMLoadFloat4x4(&worldTransform));
		XMVECTOR v2 = XMVector3TransformCoord(XMLoadFloat3(&XMFLOAT3(pVertices[i+2].GetPos().x, pVertices[i+2].GetPos().y, pVertices[i+2].GetPos().z))
			, XMLoadFloat4x4(&worldTransform));

		//pVertices�� ��ġ���Ϳ� ���庯ȯ�� �����ش�.

		// v0 v1 v2�� �ﰢ�� �������� ��ġ����

		BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0,v1, v2, fHitDistance);	// ����x�ﰢ�� �浹�˻� 
		if (bIntersected)	// �浹�ߴٸ�
		{
			if (fHitDistance < fNearHitDistance)	// �Ÿ��� �������
			{
				idx = i;
				fNearHitDistance = fHitDistance;	// ���������� �ʱ�ȭ
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
{ //�ﰢ�� �޽��� �����Ѵ�. 
	m_nVertices = 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	/*����(�ﰢ���� ������)�� ������ �ð���� ������� ������, ���,
	�Ķ������� �����Ѵ�. RGBA(Red, Green, Blue, Alpha) 4���� �Ķ���͸�
	����Ͽ� ������ ǥ���Ѵ�. �� �Ķ���ʹ� 0.0~1.0 ������ �Ǽ����� ������.*/
	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));
	//�ﰢ�� �޽��� ���ҽ�(���� ����)�� �����Ѵ�.
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	//���� ���� �並 �����Ѵ�. 
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}
CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
	float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList) { //������ü�� 6���� �� ����(x-��) ���� 

																					  //������ü�� ������(����)�� 8���̴�.
	m_nVertices = 36;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//fWidth: ������ü ����(x-��) ����, fHeight: ������ü ����(y-��) ����, fDepth: ������ü ����(z-��) ����
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	
	int i = 0;
	//���� ���� �����ʹ� �ﰢ�� ����Ʈ�̹Ƿ� 36���� ���� �����͸� �غ��Ѵ�.
	//�� �ո�(Front) �簢���� ���� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz),ONE_RED_COLOR);		//0
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz),ONE_RED_COLOR);		//1
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz),ONE_RED_COLOR);		//2
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��					
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz),ONE_RED_COLOR);		//3
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz),ONE_RED_COLOR);		//4
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz),ONE_RED_COLOR);		//5

	// 0,1,2	3,4,5 -> ������ �ﰢ�� 2��

	//�� ����(Top) �簢���� ���� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), ONE_BLUE_COLOR);		//6
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), ONE_BLUE_COLOR);		//7
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), ONE_BLUE_COLOR);		//8
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��					
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), ONE_BLUE_COLOR);		//9
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), ONE_BLUE_COLOR);		//10
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), ONE_BLUE_COLOR);		//11
															 
	// 6,7,8	9,10,11 -> �Ķ��� �ﰢ�� 2��				 
															 
	//�� �޸�(Back) �簢���� ���� �ﰢ��					 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), ONE_GREEN_COLOR);		//12
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), ONE_GREEN_COLOR);		//13
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), ONE_GREEN_COLOR);		//14
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��					
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), ONE_GREEN_COLOR);		//15
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), ONE_GREEN_COLOR);		//16
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), ONE_GREEN_COLOR);		//17
															  
	// 12,13,14		15,16,17 -> �ʷϻ� �ﰢ�� 2��			  
															  
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��					
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), ONE_MAGENTA_COLOR);	//18
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), ONE_MAGENTA_COLOR);	//19
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), ONE_MAGENTA_COLOR);	//20
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��				
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), ONE_MAGENTA_COLOR);	//21
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), ONE_MAGENTA_COLOR);	//22
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), ONE_MAGENTA_COLOR);	//23
															 
	// 18,19,20		21,22,23 -> ��ȫ�� �ﰢ�� 2��			 
															 
	//�� ����(Left) �簢���� ���� �ﰢ��					 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), ONE_YELLOW_COLOR);	//24
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), ONE_YELLOW_COLOR);	//25
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), ONE_YELLOW_COLOR);	//26
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��					 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), ONE_YELLOW_COLOR);	//27
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), ONE_YELLOW_COLOR);	//28
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), ONE_YELLOW_COLOR);	//29
															  
	// 24,25,26		27,28,29 -> ������ �ﰢ�� 2��			  
															  
	//�� ����(Right) �簢���� ���� �ﰢ��					  
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), ONE_CYAN_COLOR);		//30
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), ONE_CYAN_COLOR);		//31
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), ONE_CYAN_COLOR);		//32
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��					 
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), ONE_CYAN_COLOR);		//33
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), ONE_CYAN_COLOR);		//34
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), ONE_CYAN_COLOR);		//35

	// 30,31,32		33,34,35 -> ������ �ﰢ�� 2��

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	//���� ���� �並 �����Ѵ�.
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
	//���� �׸��� ���� ����� �޽��� ǥ���ϱ� ���� ���� �������̴�. 
	CDiffusedVertex pVertices[24 * 3];
	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3);
	int i = 0;
	//����� �޽��� ���� �� z
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
	//����� �޽��� �Ʒ��� �� 
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
	//����� �޽��� ������ �� 
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
	//����� �޽��� ����/������ �� 
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
	//����� �޽��� ���� �� 
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
	//����� �޽��� ����/���� �� 
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


CWallMeshDiffused::CWallMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList) { //������ü�� 6���� �� ����(x-��) ���� 

																																															 //������ü�� ������(����)�� 8���̴�.
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 2.0f, fy = fHeight * 2.0f, fz = fDepth * 2.0f;
	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������. 
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
	/*�ε��� ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�. �ﰢ�� ����Ʈ�� ������ü�� ǥ���� ���� �Ƿ� �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� ������ �ʿ��ϴ�. ��, �ε��� ���۴� ��ü 36(=6*2*3)���� �� ������ ������ �Ѵ�.*/
	m_nIndices = 24;
	UINT pnIndices[24];
	//�� �ո�(Front) �簢���� ���� �ﰢ��  
	pnIndices[0] = 0;
	pnIndices[1] = 1;
	pnIndices[2] = 3;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��   
	pnIndices[3] = 3;
	pnIndices[4] = 1;
	pnIndices[5] = 2;
	//�� �޸�(Back) �簢���� ���� �ﰢ��    
	pnIndices[6] = 7;
	pnIndices[7] = 4;
	pnIndices[8] = 3;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[9] = 3;
	pnIndices[10] = 4;
	pnIndices[11] = 0;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��    
	pnIndices[12] = 5;
	pnIndices[13] = 6;
	pnIndices[14] = 1;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��    
	pnIndices[15] = 1;
	pnIndices[16] = 6;
	pnIndices[17] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��
	pnIndices[18] = 5;
	pnIndices[19] = 4;
	pnIndices[20] = 6;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ�� 
	pnIndices[21] = 6;
	pnIndices[22] = 4;
	pnIndices[23] = 7;

	//�ε��� ���۸� �����Ѵ�. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	//�ε��� ���� �並 �����Ѵ�. 
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
	//������ü�� ������(����)�� 8���̴�. 
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������. 
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
	/*�ε��� ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�. �ﰢ�� ����Ʈ�� ������ü�� ǥ���� ���� �Ƿ� �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� ������ �ʿ��ϴ�. ��, �ε��� ���۴� ��ü 36(=6*2*3)���� �� ������ ������ �Ѵ�.*/
	m_nIndices = 36;
	UINT pnIndices[36];
	//�� �ո�(Front) �簢���� ���� �ﰢ��  
	pnIndices[0] = 3;
	pnIndices[1] = 1;
	pnIndices[2] = 0;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��   
	pnIndices[3] = 2;
	pnIndices[4] = 1;
	pnIndices[5] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ��   
	pnIndices[6] = 0;
	pnIndices[7] = 5;
	pnIndices[8] = 4;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[9] = 1;
	pnIndices[10] = 5;
	pnIndices[11] = 0;
	//�� �޸�(Back) �簢���� ���� �ﰢ��    
	pnIndices[12] = 3;
	pnIndices[13] = 4;
	pnIndices[14] = 7;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��  
	pnIndices[15] = 0;
	pnIndices[16] = 4;
	pnIndices[17] = 3;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��    
	pnIndices[18] = 1;
	pnIndices[19] = 6;
	pnIndices[20] = 5;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��    
	pnIndices[21] = 2;
	pnIndices[22] = 6;
	pnIndices[23] = 1;
	//�� ����(Left) �簢���� ���� �ﰢ��  
	pnIndices[24] = 2;
	pnIndices[25] = 7;
	pnIndices[26] = 6;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��   
	pnIndices[27] = 3;
	pnIndices[28] = 7;
	pnIndices[29] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��
	pnIndices[30] = 6;
	pnIndices[31] = 4;
	pnIndices[32] = 5;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ�� 
	pnIndices[33] = 7;
	pnIndices[34] = 4;
	pnIndices[35] = 6;

	//�ε��� ���۸� �����Ѵ�. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	//�ε��� ���� �並 �����Ѵ�. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}
CCubeColorMeshDiffused::~CCubeColorMeshDiffused()
{

}