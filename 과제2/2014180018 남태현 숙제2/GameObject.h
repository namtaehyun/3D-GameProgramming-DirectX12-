#pragma once
#include "Mesh.h"
#include "Camera.h"
#define EXPLOSION_DEBRISES 100
class CShader;
class CGameObject {
public: 
	int time=0;
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	CGameObject();    
	virtual ~CGameObject();

	bool m_bActive = true;

	BoundingOrientedBox m_xmOOBB;

	CGameObject * m_pObjectCollided = NULL;

	XMFLOAT3	m_xmf3MovingDirection;		// ������ ����
	float		m_fMovingSpeed;				// ������ �ӵ�

	XMFLOAT3	m_xmf3RotationAxis;			// ȸ������
	float		m_fRotationSpeed;			// ȸ���ӵ�
	
public: 
	virtual void ResetPos(float z);
	//��� ���۸� �����Ѵ�.
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList); 
	//��� ������ ������ �����Ѵ�. 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	//���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�. 
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook(); 
	XMFLOAT3 GetUp(); 
	XMFLOAT3 GetRight();
	//���� ��ü�� ��ġ�� �����Ѵ�.
	void SetPosition(float x, float y, float z); 
	void SetPosition(XMFLOAT3 xmf3Position);
	//���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f); 
	void MoveUp(float fDistance = 1.0f); 
	void MoveForward(float fDistance = 1.0f);
	//���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�. 
	virtual void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
	
	void SetRotationAxis(XMFLOAT3& xmf3RotationAxis) { m_xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis); }
	void SetRotationSpeed(float fSpeed) { m_fRotationSpeed = fSpeed; }
private: 
	int m_nReferences = 0;
public: 
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	CShader *m_pShader = NULL;
protected: 
	XMFLOAT4X4 m_xmf4x4World;
	CMesh *m_pMesh = NULL;

public: 
	void ReleaseUploadBuffers();
	virtual XMFLOAT4X4 GetWorldTransform() { return m_xmf4x4World; };
	virtual void Move(XMFLOAT3 & vDirection, float fSpeed);
	virtual void SetMesh(CMesh *pMesh);
	virtual void SetShader(CShader *pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender(); 
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
};


class CWallsObject : public CGameObject
{
public:
	CWallsObject();
	virtual ~CWallsObject();

public:
	XMFLOAT4					m_pxmf4WallPlanes[6];
};



// �����ϰ� �Ϸ���, 100���� ��ü�� �����, ������������Ѵ�.
class CExplosiveObject : public CGameObject
{
public:
	CExplosiveObject() {};
	CExplosiveObject(CMesh * mesh);
	virtual ~CExplosiveObject();

	

	float		m_fElapsedTimes = 0.0f;
	float		m_fDuration = 2.5f;
	virtual void Rotate(XMFLOAT3 *, float );
	virtual void Move(XMFLOAT3 & vDirection, float fSpeed);

	virtual void Animate(float fElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual CMesh * GetMesh() { return m_pMesh; }
};

class Bullet :public CGameObject
{
public:
	Bullet(CMesh * mesh);
	virtual void Animate(float fTimeElapsed);
	virtual ~Bullet();
};

class Particle : public CGameObject
{
public:
	Particle();
	Particle(CMesh * mesh);
	virtual void Animate(float fTimeElapsed);
	virtual ~Particle();
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void Move(XMFLOAT3 & vDirection, float fSpeed);
};

class Particles
{
public:
	Particle ** particle;
	int time=0;
	bool m_bActive = false;
	Particles();
	Particles(CMesh*mesh);
	~Particles();
	void AnimateParticles(float fTimeElapsed);							// ��ƼŬ���� animate��Ų��.
	void RenderParticles(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);								// ��ƼŬ���� Render��Ų��.
	void SetParticlesPosition(CExplosiveObject * object); //��ƼŬ���� ��ġ���͸� �������ش�.
};

///////////////////////����/////////////////////////
class Boss :public CGameObject
{
private:
	int hp;
public:
	Boss(CMesh * mesh);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Animate(float fTimeElapsed);	// ������ �׳� ť��� �����ϰ� �̵��Ѵ�.
	virtual int GetHP() { return hp; };
	virtual void MinusHP() { hp--; };
	virtual ~Boss();
};
