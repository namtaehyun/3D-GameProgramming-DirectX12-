#pragma once

#include "GameObject.h"
#include "Player.h"
#include <list>
#include <vector>

struct Ray
{
	float dirX, dirY, dirZ;			// ������ ���⺤��
	float posX, posY, posZ;			// ������ ��ġ����
};
class CScene
{
public:
	CScene();
	virtual ~CScene();

	bool						gameover = false;
	CAirplanePlayer				*m_pPlayer = NULL;
	int							m_sElapsedTime = 0;
	int							m_nBullets = 0;
	int							mx, my;
	char						buf[50];
	CExplosiveObject			*m_pickedObject;
	bool						picked = false;	//��ü�� ���� ��ŷ�� �Ǿ����� true�� �ٲٰ�, ��ü�� �Ѿ˿� ������ false�� �ٲ۴�.
												//CGameObject				**m_ppObjects;

	CBossObject					boss;

	std::list<CExplosiveObject>		m_ppObjects;
	Ray							ray;			//����

	CWallsObject				*m_pWallsObject = NULL;

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void CheckObjectByObjectCollisions();		//ť��� ť���� �浹
	virtual void CheckObjectByWallCollisions();			//���� ť���� �浹
	virtual void CheckObjectByBulletsCollisions();		//ť��� �Ѿ��� �浹
	virtual void CheckBossByWallCollisions();			//���� ������ �浹
	virtual void CheckBossBulletPlayerCollisions();		//�����Ѿ�, �÷��̾ �浹
	virtual void CheckBossPlayerBulletCollisions();		//�÷��̾��Ѿ�, ������ �浹

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

