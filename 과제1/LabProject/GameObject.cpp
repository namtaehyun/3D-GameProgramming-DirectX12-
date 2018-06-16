#include "StdAfx.h"
#include "GameObject.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX)*(fMax - fMin));
}

XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f); //1벡터
	XMVECTOR xmvZero = XMVectorZero();	//0벡터


										// 폭발물을 구체내에 랜덤하게 만들고 퍼뜨리기위한 함수
	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		//v^2이 단위벡터보다 작으면 리턴
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne))return (XMVector3Normalize(v));
	}
}


CGameObject::CGameObject()
{
	m_pMesh = NULL;
	m_xmf4x4World = Matrix4x4::Identity(); //초기 월드행렬은 단위행렬

	m_dwColor = RGB(0, 0, 0);
	m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_fMovingSpeed = 0.0f;
	m_fMovingRange = 0.0f;

	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 0.0f;
}

CGameObject::CGameObject(CMesh *pMesh) :CGameObject() // 생성자+메쉬초기화
{
	m_pMesh = pMesh;
}


CGameObject::~CGameObject()
{
	if (m_pMesh)m_pMesh->Release();
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3& xmf3Position)
{
	m_xmf4x4World._41 = xmf3Position.x;
	m_xmf4x4World._42 = xmf3Position.y;
	m_xmf4x4World._43 = xmf3Position.z;
}
XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	XMFLOAT3 xmf3LookAt(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	xmf3LookAt = Vector3::Normalize(xmf3LookAt);
	return(xmf3LookAt);
}

XMFLOAT3 CGameObject::GetUp()
{
	XMFLOAT3 xmf3Up(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	xmf3Up = Vector3::Normalize(xmf3Up);
	return(xmf3Up);
}

XMFLOAT3 CGameObject::GetRight()
{
	XMFLOAT3 xmf3Right(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	xmf3Right = Vector3::Normalize(xmf3Right);
	return(xmf3Right);
}

void CGameObject::MoveStrafe(float fDistance) // 오른쪽 이동
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Right, fDistance));
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Up, fDistance));
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3LookAt = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3LookAt, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll) // 오일러 각 회전
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(fYaw, fPitch, fRoll);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3 & xmf3RotationAxis, float fAngle)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationAxis(xmf3RotationAxis, fAngle);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Move(XMFLOAT3 & vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x*fSpeed, m_xmf4x4World._42 + vDirection.y*fSpeed, m_xmf4x4World._43 + vDirection.z*fSpeed);
	//벡터의 합이 아닌, 그냥 방향벡터와 속도값을 곱한값을 위치벡터에 더해준다.
}

void CGameObject::Animate(float fElapsedTime) // Animate함수에서 회전, 이동을 함.
{
	if (m_fRotationSpeed != 0.0f)Rotate(m_xmf3RotationAxis, m_fRotationSpeed*fElapsedTime);
	if (m_fMovingSpeed != 0.0f)Move(m_xmf3MovingDirection, m_fMovingSpeed*fElapsedTime);

	if (m_pMesh)
	{
		m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}

void CGameObject::Render(HDC hDCFrameBuffer, CCamera * pCamera)
{
	if (m_pMesh)
	{
		HPEN hPen = CreatePen(PS_SOLID, 0, m_dwColor);
		HPEN hOldPen = (HPEN)SelectObject(hDCFrameBuffer, hPen);
		m_pMesh->Render(hDCFrameBuffer, m_xmf4x4World, pCamera);
		SelectObject(hDCFrameBuffer, hOldPen);
		DeleteObject(hPen);
	}
}
////////////////////////////////////////
CWallsObject::CWallsObject()
{

}

CWallsObject::~CWallsObject()
{

}

//////////////////////////////////////////

XMFLOAT3 CExplosiveObject::m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
CMesh * CExplosiveObject::m_pExplosionMesh = NULL;

CExplosiveObject::CExplosiveObject(float z, int color)
{
	m_color = color;
	float size;
	size = RandF(3.0f, 6.0f);
	CCubeMesh * pObjectCubeMesh = new CCubeMesh(size, size,size);
	pObjectCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(size/2,size/2, size/2), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	SetMesh(pObjectCubeMesh);
	if (m_color == RED)
	{
		SetColor(RGB(255, 0, 0));
	}
	else if (m_color == BLUE)
	{
		SetColor(RGB(0, 0, 255));
	}
	else if (m_color == GREEN)
	{
		SetColor(RGB(0, 255, 0));
	}
	else
	{
		SetColor(RGB(255, 0, 255));
	}
	if (z < 300) {
		SetPosition(RandF(-15.0f, 15.0f), RandF(-15.0f, 15.0f), z + 100);
	}
	else
	{
		SetPosition(RandF(-15.0f, 15.0f), RandF(-15.0f, 15.0f), 300);
	}
	SetRotationAxis(XMFLOAT3(RandF(-1.0f,1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f)));
	SetRotationSpeed(RandF(10.0f, 50.0f));
	SetMovingDirection(XMFLOAT3(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f)));
	SetMovingSpeed(RandF(10.0f, 30.0f));

}
CExplosiveObject::~CExplosiveObject()
{

}

void CExplosiveObject::PrepareExplosion()
{
	for (int i = 0; i < EXPLOSION_DEBRISES; i++) XMStoreFloat3(&m_pxmf3SphereVectors[i], RandomUnitVectorOnSphere());
	// 폭발물 잔해의 m_pxmf3SphereVectors에 EXPLOSION_DEBRISES수만큼 저장해준다.
	m_pExplosionMesh = new CCubeMesh(0.5f, 0.5f, 0.5f);
}

void CExplosiveObject::Animate(float fElapsedTime)
{
	if (m_bBlowingUp)
	{
		m_fElapsedTimes += fElapsedTime;
		if (m_fElapsedTimes <= m_fDuration)	//경과된시간이 지속시간보다 짧을경우 폭발물의 움직임
		{
			XMFLOAT3 xmf3Position = GetPosition();
			for (int i = 0; i < EXPLOSION_DEBRISES; i++)
			{
				m_pxmf4x4Transforms[i] = Matrix4x4::Identity();
				m_pxmf4x4Transforms[i]._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._42 = xmf3Position.y + m_pxmf3SphereVectors[i].y * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * m_fExplosionSpeed * m_fElapsedTimes;
				auto p = Matrix4x4::RotationAxis(m_pxmf3SphereVectors[i], m_fExplosionRotation * m_fElapsedTimes);
				m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(p, m_pxmf4x4Transforms[i]);
			}
			// 회전,이동을 함.
		}
		else
		{
			m_bBlowingUp = false;
			m_fElapsedTimes = 0.0f;
		}
	}
	else
	{
		CGameObject::Animate(fElapsedTime);
	}
}
CExplosiveObject::CExplosiveObject()
{

}
void CExplosiveObject::Render(HDC hDCFrameBuffer, CCamera *pCamera)
{
	if (m_bBlowingUp)
	{
		for (int i = 0; i < EXPLOSION_DEBRISES; i++)
		{
			if (m_pExplosionMesh)
			{
				HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
				HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
				m_pExplosionMesh->Render(hDCFrameBuffer, m_pxmf4x4Transforms[i], pCamera);
				SelectObject(hDCFrameBuffer, hOldPen);
				DeleteObject(hPen);
			}
		}
	}
	else
	{
		CGameObject::Render(hDCFrameBuffer, pCamera);
	}
}

CBulletObject::CBulletObject(XMFLOAT3 pos, XMFLOAT3 dir)
{
	CCubeMesh * pObjectCubeMesh = new CCubeMesh(1.0f, 1.0f, 1.0f);
	pObjectCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f ), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	SetPosition(pos.x, pos.y, pos.z);
	SetMesh(pObjectCubeMesh);
	SetMovingDirection(dir);
	SetMovingSpeed(100.0f);
	SetRotationAxis(XMFLOAT3(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f)));
	SetRotationSpeed(RandF(10.0f, 50.0f));
	SetColor(RGB(0, 0, 0));

}
CBulletObject::~CBulletObject()
{

}


CBossObject::CBossObject()
{
	pObjectCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(30.0f, 30.0f, 30.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	SetPosition(0, 0,500);
	SetMesh(pObjectCubeMesh);
	SetMovingDirection(XMFLOAT3(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f)));
	SetMovingSpeed(20.0f);
	SetRotationAxis(XMFLOAT3(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f)));
	SetRotationSpeed(RandF(10.0f, 50.0f));
	SetColor(RGB(250, 143, 143));
}

CBossObject::~CBossObject()
{

}

void CBossObject::SetFirst()
{
	hpCount = 10;
	SetPosition(0, 0, 500);
	m_bBlowingUp = false;
}

CBossBulletObject::CBossBulletObject(XMFLOAT3 pos, XMFLOAT3 dir)
{
	CCubeMesh * pObjectCubeMesh = new CCubeMesh(1.0f, 1.0f, 1.0f);
	pObjectCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f * 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	SetPosition(pos.x, pos.y, pos.z);
	SetMesh(pObjectCubeMesh);
	SetMovingDirection(dir);
	SetMovingSpeed(70.0f);
	SetRotationAxis(XMFLOAT3(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f)));
	SetRotationSpeed(20.0f);
	SetColor(RGB(32, 151, 15));
}
CBossBulletObject::~CBossBulletObject()
{

}


