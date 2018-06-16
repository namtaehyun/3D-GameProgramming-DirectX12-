#pragma once

#include "GameObject.h"
#include <list>

class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();

	int							m_skill_num = 0;		// ��ų ���� - ��ũ�� óġ�� �� �ϳ��� �þ��.
	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	XMFLOAT3					m_xmf3CameraOffset;
	XMFLOAT3					m_xmf3Velocity;
	float						m_fFriction = 125.0f;

	float           			m_fPitch = 0.0f;
	float           			m_fYaw = 0.0f;
	float           			m_fRoll = 0.0f;

	int							item = 0;

	CCamera						*m_pCamera = NULL;

	void SetPosition(float x, float y, float z);
	void Move(DWORD dwDirection, float fDistance);
	void Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity);
	void Move(float x, float y, float z);
	void CPlayer::TargetTo(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);
	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
	void SetCameraOffset(XMFLOAT3& xmf3CameraOffset);
	void Update(float fTimeElapsed = 0.016f);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);
};

class CAirplanePlayer : public CPlayer
{
public:
	std::list<CBulletObject>			m_pbullets;			//�Ѿ�

	CAirplanePlayer();
	virtual ~CAirplanePlayer();
	void Targeting(CGameObject *pSelectedObject);
};

class CTerrainPlayer : public CPlayer
{
public:
	CTerrainPlayer();
	virtual ~CTerrainPlayer();
};
