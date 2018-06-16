#pragma once

//������ ǥ���ϱ� ���� Ŭ������ �����Ѵ�. 
class CVertex 
{ 
protected: //������ ��ġ �����̴�(��� ������ �ּ��� ��ġ ���͸� ������ �Ѵ�).  
	XMFLOAT3 m_xmf3Position;
public: 
	XMFLOAT3 GetPos(){return m_xmf3Position;}
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); } 
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() {}
};

class CDiffusedVertex : public CVertex {
protected: //������ �����̴�.   
	XMFLOAT4 m_xmf4Diffuse;
public:
	CDiffusedVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); 
	m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f); } 
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf4Diffuse = xmf4Diffuse; }
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) { m_xmf3Position = xmf3Position; m_xmf4Diffuse = xmf4Diffuse; } 
	~CDiffusedVertex() { }
};

class CMesh {
protected: 
	int m_nIndices;
	ID3D12Resource * m_pd3dIndexBuffer = NULL; 
	ID3D12Resource *m_pd3dIndexUploadBuffer = NULL; /*�ε��� ����(�ε����� �迭)�� �ε��� ���۸� ���� ���ε� ���ۿ� ���� �������̽� �������̴�. �ε��� ���۴� ���� ����(�迭)�� ���� �ε����� ������.*/ 
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;
	int m_nBaseVertex = 0; //�ε��� ������ �ε����� ������ �ε����̴�. 
public:   
	BoundingOrientedBox m_xmOOBB;	//�浹�ڽ��̴�.
	void SetOOBB(XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation) { m_xmOOBB = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation); }
	CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);   
	virtual ~CMesh();
	
private:
	int m_nReferences = 0;
public: 
	void AddRef() { m_nReferences++; } 
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseUploadBuffers();

protected: 

	ID3D12Resource * m_pd3dVertexBuffer = NULL; 
	ID3D12Resource *m_pd3dVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nSlot = 0; 
	UINT m_nVertices = 0; 
	UINT m_nStride = 0; 
	UINT m_nOffset = 0;
public: 
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
};

class CWallMeshDiffused : public CMesh
{
public:
	CWallMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CWallMeshDiffused();
};

class CTriangleMesh : public CMesh { 
public:   
	CTriangleMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);   
	virtual ~CTriangleMesh() { } 
};
class CCubeMeshDiffused : public CMesh {
private:
	CDiffusedVertex pVertices[36];
	int idx = 0;
public: 
	int ReturnColor(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection,XMFLOAT4X4 worldTransform);
	//������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü �޽��� �����Ѵ�. 
	CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		 float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);

	virtual ~CCubeMeshDiffused(); 
};

class CCubeColorMeshDiffused : public CMesh {
public:
	//������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü �޽��� �����Ѵ�. 
	CCubeColorMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		XMFLOAT4 xmf3Color,float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeColorMeshDiffused();
};


class CAirplaneMeshDiffused : public CMesh
{
public:
	CAirplaneMeshDiffused(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f,
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CAirplaneMeshDiffused();
};