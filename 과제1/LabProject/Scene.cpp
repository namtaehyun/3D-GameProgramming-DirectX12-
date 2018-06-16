#include "stdafx.h"
#include "Scene.h"

#define OBJECT_NUM 100
CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN: {
		float dist = 100000000;
		float intersectDist=10000000;			// 최소 플레이어와 오브젝트간 거리
		float vx, vy, vz;
		mx = LOWORD(lParam);	//xScreen
		my = HIWORD(lParam);	//yScreen
		vx = (((((mx - 0/*뷰포트의 left*/)*2.0f / 640/*뷰포트의 width*/) - 1.0f)) - m_pPlayer->m_pCamera->m_xmf4x4Projection._31) / m_pPlayer->m_pCamera->m_xmf4x4Projection._11;
		vy = ((-(((my - 0/*뷰포트의 top*/)*2.0f / 480) - 1.0f/*뷰포트의 height*/)) - m_pPlayer->m_pCamera->m_xmf4x4Projection._32) / m_pPlayer->m_pCamera->m_xmf4x4Projection._22;
		vz = 1.0f;
		XMFLOAT4X4 m = Matrix4x4::Inverse(m_pPlayer->m_pCamera->m_xmf4x4View);
		ray.dirX = vx * m._11 + vy * m._21 + vz * m._31;
		ray.dirY = vx * m._12 + vy * m._22 + vz * m._32;
		ray.dirZ = vx * m._13 + vy * m._23 + vz * m._33;
		ray.posX = m._41;
		ray.posY = m._42;
		ray.posZ = m._43;
		picked = false;
		for (auto &p : m_ppObjects)
		{
			if (p.m_xmOOBB.Intersects(XMLoadFloat3(&XMFLOAT3(ray.posX, ray.posY, ray.posZ)), XMLoadFloat3(&Vector3::Normalize(XMFLOAT3(ray.dirX, ray.dirY, ray.dirZ))), dist))
			{
				if (sqrt((p.GetPosition().x-m_pPlayer->GetPosition().x)*(p.GetPosition().x - m_pPlayer->GetPosition().x)
				+(p.GetPosition().y-m_pPlayer->GetPosition().y)*(p.GetPosition().y - m_pPlayer->GetPosition().y)
				+ (p.GetPosition().z - m_pPlayer->GetPosition().z)*(p.GetPosition().z - m_pPlayer->GetPosition().z))<intersectDist)
				{
					intersectDist = sqrt((p.GetPosition().x - m_pPlayer->GetPosition().x)*(p.GetPosition().x - m_pPlayer->GetPosition().x)
						+ (p.GetPosition().y - m_pPlayer->GetPosition().y)*(p.GetPosition().y - m_pPlayer->GetPosition().y)
						+ (p.GetPosition().z - m_pPlayer->GetPosition().z)*(p.GetPosition().z - m_pPlayer->GetPosition().z));
					m_pickedObject = &p;
					picked = true;
				}// 최고 작은 거리에서의 오브젝트 찾기.
			}
		}

		/*
		피킹 처리하는 방법
		화면 좌표 -> 화면 역변환 -> 투영 역변환 -> 카메라 좌표
		*/
		//640x480
		break;
	}
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		break;
	}
}


void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_KEYDOWN:
			switch (wParam)
			{
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					break;
				case VK_CONTROL:
				{
					if (picked == false) {
						XMFLOAT3					p_pos = m_pPlayer->GetPosition();		// 플레이어의 위치
						XMFLOAT3					move_Dir = m_pPlayer->GetUp();			// 왜 업벡터가 룩벡터지;
						m_pPlayer->m_pbullets.emplace_back(p_pos, move_Dir);
					}
					else        // 만약 물체가 선택되었다면, 총구를 물체방향으로 돌려주고, 물체가 충돌되면 터뜨리고, picked상태를 false로 바꿔준다.
					{
						XMFLOAT3					p_pos = m_pPlayer->GetPosition();		// 플레이어의 위치
						XMFLOAT3					move_Dir = m_pPlayer->GetUp();			// 왜 업벡터가 룩벡터지;
						m_pPlayer->m_pbullets.emplace_back(p_pos, move_Dir);
						m_pPlayer->Targeting(m_pickedObject);
					}
				}
					break;
				case VK_SPACE:
					if (m_pPlayer->m_skill_num > 0) {
						for (auto &p : m_ppObjects)
						{
							if (sqrt((m_pPlayer->GetPosition().x - p.GetPosition().x)*
								(m_pPlayer->GetPosition().x - p.GetPosition().x) +
								(m_pPlayer->GetPosition().y - p.GetPosition().y)*
								(m_pPlayer->GetPosition().y - p.GetPosition().y) +
								(m_pPlayer->GetPosition().z - p.GetPosition().z)*
								(m_pPlayer->GetPosition().z - p.GetPosition().z)) < 30.0f)
							{
								p.m_bBlowingUp = true;
							}
						}
						m_pPlayer->m_skill_num--;
					}
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

void CScene::BuildObjects()
{

	CExplosiveObject::PrepareExplosion();

	float fHalfWidth = 45.0f, fHalfHeight = 45.0f, fHalfDepth = 500.0f;
	CWallMesh *pWallCubeMesh = new CWallMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 30);
	pWallCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	m_pWallsObject = new CWallsObject();
	m_pWallsObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pWallsObject->SetMesh(pWallCubeMesh);
	m_pWallsObject->SetColor(RGB(0, 0, 0));
	m_pWallsObject->m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, fHalfDepth);
	m_pWallsObject->m_pxmf4WallPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, fHalfDepth);

}

void CScene::ReleaseObjects()
{
	if (CExplosiveObject::m_pExplosionMesh) CExplosiveObject::m_pExplosionMesh->Release();
	//m_pPlayer->.clear();
	m_ppObjects.clear();
	if (m_pWallsObject) delete m_pWallsObject;
}

void CScene::CheckObjectByObjectCollisions()
{
	for (auto &p : m_ppObjects) p.m_pObjectCollided = NULL;
	for (auto &p : m_ppObjects)
	{
		for (auto &m : m_ppObjects) // 충돌체크를 반만하면 됨.
		{
			if (p.m_xmOOBB.Intersects(m.m_xmOOBB))
			{
				p.m_pObjectCollided = &m; // 누구와 충돌을 했는가를 집어넣음. 속도벡터를 교환하면 정반대의 방향으로 나갈 수 있음.
				m.m_pObjectCollided = &p;
			} 
		}
	}
	for (auto &p : m_ppObjects)
	{
		if (p.m_pObjectCollided)
		{
			XMFLOAT3 xmf3MovingDirection = p.m_xmf3MovingDirection;
			float fMovingSpeed = p.m_fMovingSpeed;
			p.m_xmf3MovingDirection = p.m_pObjectCollided->m_xmf3MovingDirection;
			p.m_fMovingSpeed = p.m_pObjectCollided->m_fMovingSpeed;
			p.m_pObjectCollided->m_xmf3MovingDirection = xmf3MovingDirection;
			p.m_pObjectCollided->m_fMovingSpeed = fMovingSpeed;
			p.m_pObjectCollided->m_pObjectCollided = NULL;
			p.m_pObjectCollided = NULL;
		} // 부딪힌 객체들끼리 벡터를 바꿔준다.
	}
}

void CScene::CheckObjectByBulletsCollisions() {

	for (auto &m : m_ppObjects)
	{
		for (auto p = m_pPlayer->m_pbullets.begin(); p != m_pPlayer->m_pbullets.end();)
		{
			if (m.m_xmOOBB.Intersects(p->m_xmOOBB))
			{
				if (m.m_color == RED)
				{
					m_pPlayer->m_skill_num++;
				}
				m.m_bActive = false;
				m.m_bBlowingUp = true;
				m_pPlayer->m_pbullets.erase(p++);
			}
			else
			{
				++p;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////

void CScene::CheckObjectByWallCollisions()
{
	for (auto &p : m_ppObjects)
	{
		ContainmentType containType = m_pWallsObject->m_xmOOBB.Contains(p.m_xmOOBB);
		switch (containType)
		{
		case DISJOINT:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 6; j++)
			{
				PlaneIntersectionType intersectType = p.m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
				if (intersectType == BACK)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&p.m_xmf3MovingDirection), xmvNormal);
				XMStoreFloat3(&p.m_xmf3MovingDirection, xmvReflect);
			}
			break;
		}
		case INTERSECTS:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 6; j++)
			{
				PlaneIntersectionType intersectType = p.m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
				if (intersectType == INTERSECTING)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&p.m_xmf3MovingDirection), xmvNormal);
				XMStoreFloat3(&p.m_xmf3MovingDirection, xmvReflect);
			}
			break;
		}
		case CONTAINS:
			break;
		}
	}
}


void CScene::CheckBossBulletPlayerCollisions()
{
	for (auto &p : boss.boss_bullet)
	{
		if (p.m_xmOOBB.Contains(XMLoadFloat3(&m_pPlayer->m_xmf3Position)) == CONTAINS)
		{
			gameover = true;
		}
	}
}


void CScene::CheckBossPlayerBulletCollisions()
{
	for (auto p=m_pPlayer->m_pbullets.begin();p!=m_pPlayer->m_pbullets.end();)
	{
		if (p->m_xmOOBB.Intersects(boss.m_xmOOBB))
		{
			boss.hpCount--;
			m_pPlayer->m_pbullets.erase(p++);
		}
		else
		{
			p++;
		}
	}
	if (boss.hpCount < 0)
	{
		exit(0);
	}
}

void CScene::CheckBossByWallCollisions()
{
	ContainmentType containType = m_pWallsObject->m_xmOOBB.Contains(boss.m_xmOOBB);
	switch (containType)
	{
	case DISJOINT:
	{
		int nPlaneIndex = -1;
		for (int j = 0; j < 6; j++)
		{
			PlaneIntersectionType intersectType = boss.m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
			if (intersectType == BACK)
			{
				nPlaneIndex = j;
				break;
			}
		}
		if (nPlaneIndex != -1)
		{
			XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
			XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&boss.m_xmf3MovingDirection), xmvNormal);
			XMStoreFloat3(&boss.m_xmf3MovingDirection, xmvReflect);
		}
		break;
	}
	case INTERSECTS:
	{
		int nPlaneIndex = -1;
		for (int j = 0; j < 6; j++)
		{
			PlaneIntersectionType intersectType = boss.m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
			if (intersectType == INTERSECTING)
			{
				nPlaneIndex = j;
				break;
			}
		}
		if (nPlaneIndex != -1)
		{
			XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
			XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&boss.m_xmf3MovingDirection), xmvNormal);
			XMStoreFloat3(&boss.m_xmf3MovingDirection, xmvReflect);
		}
		break;
	}
	case CONTAINS:
		break;
	}
}

void CScene::Animate(float fElapsedTime)
{
	m_sElapsedTime++;
	CCubeMesh * pObjectCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f);
	pObjectCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	if (m_sElapsedTime % 100 == 0 && m_ppObjects.size()<OBJECT_NUM - 1)			//시간에 따라서 생성된다.
	{
		m_ppObjects.emplace_back(m_pPlayer->GetPosition().z, rand()%3);

	}
	if (m_sElapsedTime % 1000 == 0 && m_ppObjects.size()<OBJECT_NUM - 1)			//시간에 따라서 생성된다.
	{
		m_ppObjects.emplace_back(m_pPlayer->GetPosition().z, RED);
	}

	XMFLOAT3 ex_p = m_pPlayer->GetPosition();
	m_pWallsObject->Animate(fElapsedTime);
	if (m_pWallsObject->m_xmOOBB.Contains(XMLoadFloat3(&m_pPlayer->m_xmf3Position)) == DISJOINT) //m_pWallsObject->SetPosition(m_pPlayer->m_xmf3Position);
	{
		m_pPlayer->SetColor(RGB(0, 0, 255));
		m_pPlayer->SetPosition(ex_p.x, ex_p.y, ex_p.z);
		// 벽이랑 플레이어가 부딪히면 벽을 옮긴다. = > 플레이어의 위치를 전의 위치로 바꿔준다.
	}
	else
	{
		m_pPlayer->SetColor(RGB(255, 0, 0));
	}

	for (auto p = m_ppObjects.begin(); p != m_ppObjects.end(); p++)						//플레이어와 오브젝트가 부딪히면 맨처음위치로 돌아간다.
	{
		if (p->m_xmOOBB.Contains(XMLoadFloat3(&m_pPlayer->m_xmf3Position)) == CONTAINS)
		{
			gameover = true;
		}
	}
	if (gameover == true)
	{
		m_ppObjects.clear();
		m_pPlayer->m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_pPlayer->m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
		m_pPlayer->m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_pPlayer->m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
		m_pPlayer->m_xmf3Position.x = 0;
		m_pPlayer->m_xmf3Position.y = 0;
		m_pPlayer->m_xmf3Position.z = -500;
		boss.SetFirst();
		boss.boss_bullet.clear();
		gameover = false;
	}

	for (auto &p = m_ppObjects.begin(); p != m_ppObjects.end(); p++)
	{
		p->elapsed_Time += fElapsedTime;
		p->Animate(fElapsedTime);
	}
	for (auto &p = m_ppObjects.begin(); p != m_ppObjects.end(); p++)
	{
		if (p->elapsed_Time > 10)
		{
			XMFLOAT3 dir;
			dir.x = m_pPlayer->GetPosition().x - p->GetPosition().x;
			dir.y = m_pPlayer->GetPosition().y - p->GetPosition().y;
			dir.z = m_pPlayer->GetPosition().z - p->GetPosition().z;
			p->SetMovingDirection(dir);
		}

		if (m_pPlayer->GetPosition().z > p->GetPosition().z)
		{
			m_pPlayer->SetColor(RGB(255, 0, 255));
		}
	}

	for (auto p = m_pPlayer->m_pbullets.begin(); p != m_pPlayer->m_pbullets.end();) {
		p->Animate(fElapsedTime);
		// 플레이어보다 100 정도 멀어지면 없어짐
		if (p->GetPosition().z >= m_pPlayer->GetPosition().z + 100.0f) {
			p->SetMesh(NULL);
			m_pPlayer->m_pbullets.erase(p++);
		}
		else
			p++;
	}

	for (auto p = m_ppObjects.begin(); p != m_ppObjects.end();) {
		if (p->m_bActive == false && p->m_bBlowingUp == false)
		{
			p->SetMesh(NULL);
			m_ppObjects.erase(p++);
		}
		else
			p++;
	}


	if (picked == true) {
		if (m_pickedObject->m_bBlowingUp == true)
		{
			picked = false;
		}
	}

		if (sqrt((m_pPlayer->GetPosition().x - boss.GetPosition().x)*
			(m_pPlayer->GetPosition().x - boss.GetPosition().x) +
			(m_pPlayer->GetPosition().y - boss.GetPosition().y)*
			(m_pPlayer->GetPosition().y - boss.GetPosition().y) +
			(m_pPlayer->GetPosition().z - boss.GetPosition().z)*
			(m_pPlayer->GetPosition().z - boss.GetPosition().z)) < 300.0f)
		{
			XMFLOAT3 dir;
			dir.x = m_pPlayer->GetPosition().x - boss.GetPosition().x;
			dir.y = m_pPlayer->GetPosition().y - boss.GetPosition().y;
			dir.z = m_pPlayer->GetPosition().z - boss.GetPosition().z;
			if (m_sElapsedTime % 30 == 0) {
				boss.boss_bullet.emplace_back(boss.GetPosition(), dir);
			}
		}
		boss.Animate(fElapsedTime);
		for (auto&p : boss.boss_bullet)
		{
			p.Animate(fElapsedTime);
		}
		for (auto p = boss.boss_bullet.begin(); p != boss.boss_bullet.end();) {
			p->Animate(fElapsedTime);
			// 플레이어보다 100 정도 멀어지면 없어짐
			if (p->GetPosition().z >= boss.GetPosition().z + 100.0f) {
				p->SetMesh(NULL);
				boss.boss_bullet.erase(p++);
			}
			else
				p++;

		}
	
	CheckBossByWallCollisions();

	CheckBossBulletPlayerCollisions();

	CheckBossPlayerBulletCollisions();

	CheckObjectByBulletsCollisions();

	CheckObjectByWallCollisions();

	CheckObjectByObjectCollisions();
}


void CScene::Render(HDC hDCFrameBuffer, CCamera *pCamera)
{
	m_pWallsObject->Render(hDCFrameBuffer, pCamera);

	Rectangle(hDCFrameBuffer, 520, 300, 640, 480);

	HBRUSH hBrush = ::CreateSolidBrush(RGB(235,255,21));
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDCFrameBuffer, hBrush);
	Rectangle(hDCFrameBuffer,m_pPlayer->GetPosition().x+575,465-(m_pPlayer->GetPosition().z+500)*0.16, m_pPlayer->GetPosition().x+580,470 - (m_pPlayer->GetPosition().z + 500)*0.16);

	for (auto &p : m_ppObjects)
	{
		if (p.m_color == RED) 
		{
			HBRUSH hBrush = ::CreateSolidBrush(RGB(255, 0, 0));
			HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDCFrameBuffer, hBrush);
			Ellipse(hDCFrameBuffer, p.GetPosition().x + 575, 465 - (p.GetPosition().z + 500)*0.16, p.GetPosition().x + 580, 470 - (p.GetPosition().z + 500)*0.16);
			SelectObject(hDCFrameBuffer, hOldBrush);
			DeleteObject(hBrush);
		}
		if (p.m_color == BLUE)
		{
			HBRUSH hBrush = ::CreateSolidBrush(RGB(0, 0,255));
			HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDCFrameBuffer, hBrush);
			Ellipse(hDCFrameBuffer, p.GetPosition().x + 575, 465 - (p.GetPosition().z + 500)*0.16, p.GetPosition().x + 580, 470 - (p.GetPosition().z + 500)*0.16);
			SelectObject(hDCFrameBuffer, hOldBrush);
			DeleteObject(hBrush);
		}
		if (p.m_color == PINK)
		{
			HBRUSH hBrush = ::CreateSolidBrush(RGB(255, 0, 255));
			HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDCFrameBuffer, hBrush);
			Ellipse(hDCFrameBuffer, p.GetPosition().x + 575, 465 - (p.GetPosition().z + 500)*0.16, p.GetPosition().x + 580, 470 - (p.GetPosition().z + 500)*0.16);
			SelectObject(hDCFrameBuffer, hOldBrush);
			DeleteObject(hBrush);
		}
		if (p.m_color == GREEN)
		{
			HBRUSH hBrush = ::CreateSolidBrush(RGB(0, 255, 0));
			HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDCFrameBuffer, hBrush);
			Ellipse(hDCFrameBuffer, p.GetPosition().x + 575, 465 - (p.GetPosition().z + 500)*0.16, p.GetPosition().x + 580, 470 - (p.GetPosition().z + 500)*0.16);
			SelectObject(hDCFrameBuffer, hOldBrush);
			DeleteObject(hBrush);
		}

	}

	SelectObject(hDCFrameBuffer, hOldBrush);
	DeleteObject(hBrush);


	for (auto p = m_ppObjects.begin(); p != m_ppObjects.end(); p++)
	{
		p->Render(hDCFrameBuffer, pCamera);
	}
	for (auto p = m_pPlayer->m_pbullets.begin(); p != m_pPlayer->m_pbullets.end(); ++p)
	{
		p->Render(hDCFrameBuffer, pCamera);
	}

		boss.Render(hDCFrameBuffer, pCamera);
		for (auto&p : boss.boss_bullet)
		{
			p.Render(hDCFrameBuffer, pCamera);
		}

		{
			HBRUSH hBrush = ::CreateSolidBrush(RGB(250, 143, 143));
			HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDCFrameBuffer, hBrush);
			Rectangle(hDCFrameBuffer, boss.GetPosition().x + 570, 460 - (boss.GetPosition().z + 500)*0.16, boss.GetPosition().x + 590, 480 - (boss.GetPosition().z + 500)*0.16);
			SelectObject(hDCFrameBuffer, hOldBrush);
			DeleteObject(hBrush);
		}

		if (sqrt((m_pPlayer->GetPosition().x - boss.GetPosition().x)*
			(m_pPlayer->GetPosition().x - boss.GetPosition().x) +
			(m_pPlayer->GetPosition().y - boss.GetPosition().y)*
			(m_pPlayer->GetPosition().y - boss.GetPosition().y) +
			(m_pPlayer->GetPosition().z - boss.GetPosition().z)*
			(m_pPlayer->GetPosition().z - boss.GetPosition().z)) < 300.0f)
		{
			HBRUSH hBrush = ::CreateSolidBrush(RGB(0, 0, 255));
			HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDCFrameBuffer, hBrush);
			for (int i = 0; i < boss.hpCount; ++i)
			{
				Rectangle(hDCFrameBuffer, 100 + 30 * i, 50, 120 + 30 * i, 70);
			}
			SelectObject(hDCFrameBuffer, hOldBrush);
			DeleteObject(hBrush);
		}

}
