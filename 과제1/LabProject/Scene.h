#pragma once

#include "GameObject.h"
#include "Player.h"
#include <list>
#include <vector>

struct Ray
{
	float dirX, dirY, dirZ;			// 광선의 방향벡터
	float posX, posY, posZ;			// 광선의 위치벡터
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
	bool						picked = false;	//물체에 대한 피킹이 되었으면 true로 바꾸고, 물체가 총알에 맞으면 false로 바꾼다.
												//CGameObject				**m_ppObjects;

	CBossObject					boss;

	std::list<CExplosiveObject>		m_ppObjects;
	Ray							ray;			//광선

	CWallsObject				*m_pWallsObject = NULL;

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void CheckObjectByObjectCollisions();		//큐브와 큐브의 충돌
	virtual void CheckObjectByWallCollisions();			//벽과 큐브의 충돌
	virtual void CheckObjectByBulletsCollisions();		//큐브와 총알의 충돌
	virtual void CheckBossByWallCollisions();			//벽과 보스의 충돌
	virtual void CheckBossBulletPlayerCollisions();		//보스총알, 플레이어간 충돌
	virtual void CheckBossPlayerBulletCollisions();		//플레이어총알, 보스간 충돌

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

