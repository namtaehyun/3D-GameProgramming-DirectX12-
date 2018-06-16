#pragma once

#include "Mesh.h"
#include "Camera.h"
#include <list>
#define PINK 0
#define GREEN 1
#define BLUE 2
#define RED 3
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGameObject
{
public:
	CGameObject(CMesh *pMesh);
	CGameObject();
	virtual ~CGameObject();

public:
	bool						m_bActive = true;
	int							m_color;			//큐브 색상

	CMesh						*m_pMesh = NULL;
	XMFLOAT4X4					m_xmf4x4World;

	BoundingOrientedBox			m_xmOOBB;
	float						m_fElapsedTimes = 0.0f;
	float						elapsed_Time = 0.0f;			// 2초후에 따라오도록 만들거임.

																//CGameObject					*m_pObjectCollided = NULL;

	DWORD						m_dwColor;

	XMFLOAT3					m_xmf3MovingDirection;
	float						m_fMovingSpeed;
	float						m_fMovingRange;

	XMFLOAT3					m_xmf3RotationAxis;
	float						m_fRotationSpeed;

public:
	void SetMesh(CMesh *pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }
	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3& xmf3Position);

	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
	void SetMovingRange(float fRange) { m_fMovingRange = fRange; }

	void SetRotationAxis(XMFLOAT3& xmf3RotationAxis) { m_xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis); }
	void SetRotationSpeed(float fSpeed) { m_fRotationSpeed = fSpeed; }

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void Move(XMFLOAT3& vDirection, float fSpeed);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3& xmf3Axis, float fAngle);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);
};

class CExplosiveObject : public CGameObject
{
public:
	CExplosiveObject(float z, int color);
	CExplosiveObject();
	virtual ~CExplosiveObject();

	bool						m_bBlowingUp = false;

	XMFLOAT4X4					m_pxmf4x4Transforms[EXPLOSION_DEBRISES];

	CExplosiveObject			*m_pObjectCollided = NULL;
	float						m_fElapsedTimes = 0.0f;
	float						m_fDuration = 2.5f;
	float						m_fExplosionSpeed = 10.0f;
	float						m_fExplosionRotation = 720.0f;

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);

public:
	static CMesh				*m_pExplosionMesh;
	static XMFLOAT3				m_pxmf3SphereVectors[EXPLOSION_DEBRISES];

	static void PrepareExplosion();
};

class CWallsObject : public CGameObject
{
public:
	CWallsObject();
	virtual ~CWallsObject();

public:
	XMFLOAT4					m_pxmf4WallPlanes[6];
};

class CBulletObject : public CGameObject
{
public:
	CBulletObject(XMFLOAT3 pos, XMFLOAT3 dir);
	virtual ~CBulletObject();
};


class CBossBulletObject :public CGameObject
{
public:
	CBossBulletObject(XMFLOAT3 pos, XMFLOAT3 dir);
	virtual ~CBossBulletObject();
};

class CBossObject :public CExplosiveObject		// 보스도 hp가 다까이면 그냥 죽는거야
{
public:
	CBossObject();
	virtual ~CBossObject();
	std::list<CBossBulletObject> boss_bullet;		//보스의 총탄
	void SetFirst();				//보스 초기화
	int hpCount=10;					//보스는 10대맞으면 죽는다.

private:
	CCubeMesh * pObjectCubeMesh = new CCubeMesh(30.0f, 30.0f,30.0f);
};
