#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"
inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}


XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne))
			return(XMVector3Normalize(v));
	}
}

CGameObject::CGameObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	if (m_pMesh)
	{
		m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}
void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle) {
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

CWallsObject::CWallsObject()
{

}

CWallsObject::~CWallsObject()
{

}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release(); 
	if (m_pShader) { 
		m_pShader->ReleaseShaderVariables(); 
		m_pShader->Release(); 
	}
}
void CGameObject::SetShader(CShader *pShader)
{ 
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader; 
	if (m_pShader) m_pShader->AddRef(); 
}
void CGameObject::SetMesh(CMesh *pMesh) 
{ 
	if (m_pMesh) m_pMesh->Release(); 
	m_pMesh = pMesh; 
	if (m_pMesh) m_pMesh->AddRef(); 
}

void CGameObject::ReleaseUploadBuffers() 
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
	if (m_pMesh) 
		m_pMesh->ReleaseUploadBuffers(); 
}
void CGameObject::Animate(float fTimeElapsed) {  }
void CGameObject::OnPrepareRender() { }

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera) {
	OnPrepareRender();
	UpdateShaderVariables(pd3dCommandList);
	if (m_pShader) { 
		//게임 객체의 월드 변환 행렬을 셰이더의 상수 버퍼로 전달(복사)한다. 
		m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
		m_pShader->Render(pd3dCommandList, pCamera);
	}
	if (m_pMesh) 
		m_pMesh->Render(pd3dCommandList);
}

void CGameObject::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) { }

void CGameObject::ReleaseShaderVariables() { }

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList) 
{
	XMFLOAT4X4 xmf4x4World; 
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World))); 
	//객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사한다. 
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0); 
}

void CGameObject::SetPosition(float x, float y, float z) 
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y; 
	m_xmf4x4World._43 = z; 
}
void CGameObject::SetPosition(XMFLOAT3 xmf3Position) 
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z); 
}
XMFLOAT3 CGameObject::GetPosition() 
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43)); 
}
//게임 객체의 로컬 z-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetLook() 
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33))); 
}
//게임 객체의 로컬 y-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetUp() 
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23))); 
}
//게임 객체의 로컬 x-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetRight() 
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13))); 
}
//게임 객체를 로컬 x-축 방향으로 이동한다. 
void CGameObject::MoveStrafe(float fDistance) 
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance); 
	CGameObject::SetPosition(xmf3Position); 
}
//게임 객체를 로컬 y-축 방향으로 이동한다. 
void CGameObject::MoveUp(float fDistance) {
	XMFLOAT3 xmf3Position = GetPosition(); 
	XMFLOAT3 xmf3Up = GetUp(); 
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
//게임 객체를 로컬 z-축 방향으로 이동한다. 
void CGameObject::MoveForward(float fDistance) 
{
	XMFLOAT3 xmf3Position = GetPosition(); 
	XMFLOAT3 xmf3Look = GetLook(); 
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position); 
}
//게임 객체를 주어진 각도로 회전한다. 
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll) 
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World); 
}

void CGameObject::ResetPos(float z)
{
	SetPosition(RandF(-40.0f, 40.0f), RandF(-40.0f, 40.0f), z+100.0f);
}
void CGameObject::Move(XMFLOAT3 & vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x*fSpeed, m_xmf4x4World._42 + vDirection.y*fSpeed, m_xmf4x4World._43 + vDirection.z*fSpeed);
	//벡터의 합이 아닌, 그냥 방향벡터와 속도값을 곱한값을 위치벡터에 더해준다.
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
CExplosiveObject::CExplosiveObject(CMesh * mesh)
{
	mesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(6.0f, 6.0f, 6.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	SetMesh(mesh);
	SetPosition(RandF(-40.0f,40.0f), RandF(-40.0f, 40.0f), RandF(-400.0f, 400.0f));
	SetRotationAxis(Vector3::Normalize(XMFLOAT3(RandF(-1.0f,1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f))));
	SetRotationSpeed(RandF(30.0f,90.0f));
	SetMovingDirection(Vector3::Normalize(XMFLOAT3(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f))));
	SetMovingSpeed(RandF(5.0f,15.0f));
	m_bActive = false;
}
CExplosiveObject::~CExplosiveObject(){ }


void CExplosiveObject:: Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CExplosiveObject::Move(XMFLOAT3 & vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x*fSpeed, m_xmf4x4World._42 + vDirection.y*fSpeed, m_xmf4x4World._43 + vDirection.z*fSpeed);
	//벡터의 합이 아닌, 그냥 방향벡터와 속도값을 곱한값을 위치벡터에 더해준다.
}


void CExplosiveObject::Animate(float fElapsedTime)
{
		if (m_fRotationSpeed != 0.0f)
			Rotate(&m_xmf3RotationAxis, m_fRotationSpeed*fElapsedTime);
		if (m_fMovingSpeed != 0.0f)
			Move(m_xmf3MovingDirection, m_fMovingSpeed*fElapsedTime);
		if (m_pMesh)
		{
			m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
			XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
		}
}



void CExplosiveObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CGameObject::Render(pd3dCommandList, pCamera);
}
/////////////////////////////////////////////////////////////////////////////총알///////////////////////////////////////////////
Bullet::Bullet(CMesh * mesh)
{
	m_bActive = false;
	mesh->SetOOBB(XMFLOAT3(0.0f,0.0f,0.0f),XMFLOAT3(1.0f,1.0f,1.0f),XMFLOAT4(0.0f,0.0f,0.0f,1.0f));
	SetMesh(mesh);
	SetMovingSpeed(5.0f);
	SetRotationAxis(XMFLOAT3(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f)));
	SetRotationSpeed(RandF(10.0f, 50.0f));
}
Bullet::~Bullet()
{

}
void Bullet::Animate(float fTimeElapsed)
{
	SetPosition(m_xmf4x4World._41 + m_xmf3MovingDirection.x*m_fMovingSpeed,
		m_xmf4x4World._42 + m_xmf3MovingDirection.y*m_fMovingSpeed, m_xmf4x4World._43 + m_xmf3MovingDirection.z*m_fMovingSpeed);
	if (m_pMesh)
	{
		m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}
//////////////////////////////////////////////////////////////파티클////////////////////////////////
Particle::Particle()
{
	XMFLOAT3 spherevector;
	m_bActive = false;
	m_fMovingSpeed = 10.0f;
	m_fRotationSpeed = 10.0f;
	m_xmf3RotationAxis = (XMFLOAT3(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f)));
	XMStoreFloat3(&spherevector, RandomUnitVectorOnSphere());
	m_xmf3MovingDirection = spherevector;

}
Particle::Particle(CMesh * mesh)
{
	m_bActive = false;
	SetMesh(mesh);
	XMFLOAT3 spherevector;
	m_bActive = false;
	m_fMovingSpeed = 100.0f;
	m_fRotationSpeed = 10.0f;
	m_xmf3RotationAxis = (XMFLOAT3(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f)));
	XMStoreFloat3(&spherevector, RandomUnitVectorOnSphere());
	m_xmf3MovingDirection = spherevector;
}

Particle::~Particle()
{
}

void Particle::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}
void Particle::Move(XMFLOAT3 & vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x*fSpeed, m_xmf4x4World._42 + vDirection.y*fSpeed, m_xmf4x4World._43 + vDirection.z*fSpeed);
	//벡터의 합이 아닌, 그냥 방향벡터와 속도값을 곱한값을 위치벡터에 더해준다.
}

void Particle::Animate(float fTimeElapsed)
{
	if (m_fRotationSpeed != 0.0f)
		Rotate(&m_xmf3RotationAxis, 250.0f*fTimeElapsed);
	if (m_fMovingSpeed != 0.0f)
		Move(m_xmf3MovingDirection, 50.0f*fTimeElapsed);
}

/////////////////////////////////////////////////파티클 묶음///////////////////////////
Particles::Particles(){}
Particles::Particles(CMesh * mesh)
{
	particle = new Particle*[100];
	for (int i = 0; i < 100; ++i)
	{
		particle[i] = new Particle(mesh);
	}
	m_bActive = false;
}
Particles::~Particles()
{

}
void Particles::AnimateParticles(float fTimeElapsed)							// 파티클들을 animate시킨다.
{
	for (int i = 0; i < 100; ++i)
	{
		particle[i]->Animate(fTimeElapsed);
	}
}
void Particles::RenderParticles(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)								// 파티클들을 Render시킨다.
{
	for (int i = 0; i < 100; ++i)
	{
		particle[i]->Render(pd3dCommandList,pCamera);
	}

}
void Particles::SetParticlesPosition(CExplosiveObject * object) //파티클들의 위치벡터를 세팅해준다.
{
	for (int i = 0; i < 100; ++i)
	{
		particle[i]->SetPosition(object->GetPosition());
	}
}

//////////////////////////보스///////////////////
Boss::Boss(CMesh * mesh)
{
	mesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(6.0f, 6.0f, 6.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	SetMesh(mesh);
	SetPosition(XMFLOAT3(0, 0, 450));
	hp = 3;
	SetRotationAxis(Vector3::Normalize(XMFLOAT3(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f))));
	SetRotationSpeed(RandF(10.0f, 30.0f));
	SetMovingDirection(Vector3::Normalize(XMFLOAT3(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f))));
	SetMovingSpeed(RandF(10.0f, 30.0f));
}
void Boss::Animate(float fTimeElapsed)	// 보스는 그냥 큐브와 동일하게 이동한다.
{
	if (hp >= 0) {
		if (m_fRotationSpeed != 0.0f)
			Rotate(&m_xmf3RotationAxis, m_fRotationSpeed*fTimeElapsed);
		if (m_fMovingSpeed != 0.0f)
			Move(m_xmf3MovingDirection, m_fMovingSpeed*fTimeElapsed);
		if (m_pMesh)
		{
			m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
			XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
		}
	}
}
void Boss::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (hp >= 0) {
		CGameObject::Render(pd3dCommandList, pCamera);
	}
}
Boss::~Boss()
{

}
