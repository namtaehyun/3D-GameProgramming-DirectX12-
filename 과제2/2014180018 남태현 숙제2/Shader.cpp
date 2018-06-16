#include "stdafx.h"
#include "Shader.h"

/*
BuildObject에서 100개의 배열을 만들어놓고,
시간에 따라서 한개씩 살아나게 한다.
죽으면 Active의 값을 false로 바꾼다. 
100까지 다 생성했으면, 처음부터 돌면서 Active의 값이 false인 녀석을 다시 살린다.
이 때 살아나는 녀석의 위치는 랜덤으로 한다.
*/





CShader::CShader()
{
}

CShader::~CShader() 
{ 
	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++) 
			if (m_ppd3dPipelineStates[i]) m_ppd3dPipelineStates[i]->Release(); 
		delete[] m_ppd3dPipelineStates; 
	} 
}
//래스터라이저 상태를 설정하기 위한 구조체를 반환한다. 
D3D12_RASTERIZER_DESC CShader::CreateRasterizerState() 
{ 
	D3D12_RASTERIZER_DESC d3dRasterizerDesc; 
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC)); 
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID; 
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0; 
	d3dRasterizerDesc.DepthBiasClamp = 0.0f; 
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f; 
	d3dRasterizerDesc.DepthClipEnable = TRUE; 
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE; 
	d3dRasterizerDesc.ForcedSampleCount = 0; 
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}
//깊이-스텐실 검사를 위한 상태를 설정하기 위한 구조체를 반환한다. 
D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{ 
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc; 
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE; 
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL; 
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS; 
	d3dDepthStencilDesc.StencilEnable = FALSE; 
	d3dDepthStencilDesc.StencilReadMask = 0x00; 
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP; 
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP; 
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	return(d3dDepthStencilDesc);
}

//블렌딩 상태를 설정하기 위한 구조체를 반환한다.
D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc; 
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE; 
	d3dBlendDesc.IndependentBlendEnable = FALSE; 
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE; 
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE; 
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO; 
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD; 
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD; 
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP; 
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}
//입력 조립기에게 정점 버퍼의 구조를 알려주기 위한 구조체를 반환한다. 
D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout() 
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc; 
	d3dInputLayoutDesc.pInputElementDescs = NULL; 
	d3dInputLayoutDesc.NumElements = 0;
	return(d3dInputLayoutDesc);
}

//정점 셰이더 바이트 코드를 생성(컴파일)한다. 
D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob) {
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;
	return(d3dShaderByteCode);
}
D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob) {
	D3D12_SHADER_BYTECODE d3dShaderByteCode; 
	d3dShaderByteCode.BytecodeLength = 0; 
	d3dShaderByteCode.pShaderBytecode = NULL;
	return(d3dShaderByteCode);
}

//셰이더 소스 코드를 컴파일하여 바이트 코드 구조체를 반환한다. 
D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob) 
{
	UINT nCompileFlags = 0; 
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, NULL);

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize(); 
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

//그래픽스 파이프라인 상태 객체를 생성한다. 
void CShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dRootSignature) 
{ 
	//그래픽스 파이프라인 상태 객체 배열을 생성한다.
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc; 
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC)); 
	d3dPipelineStateDesc.pRootSignature = pd3dRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob); 
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState(); 
	d3dPipelineStateDesc.BlendState = CreateBlendState(); 
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX; 
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; 
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; 
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; 
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);
	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release(); 
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] 
		d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList) { 
	//파이프라인에 그래픽스 상태 객체를 설정한다.
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
}

void CShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) { }
void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList) { }

void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World) 
{
	XMFLOAT4X4 xmf4x4World; 
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World))); 
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0); 
}

void CShader::ReleaseShaderVariables() { }


void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera) 
{
	OnPrepareRender(pd3dCommandList); 
}

CPlayerShader::CPlayerShader() { }
CPlayerShader::~CPlayerShader() { }

D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout() {
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }; 
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs; 
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob) 
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3dShaderBlob)); 
}
D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob) 
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}

void CPlayerShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature) 
{
	m_nPipelineStates = 1; 
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

CObjectsShader::CObjectsShader() { }
CObjectsShader::~CObjectsShader() { }

D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout() {
	UINT nInputElementDescs = 2; 
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }; 
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc; 
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs; 
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}
D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob) 
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}
void CObjectsShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature) {
	m_nPipelineStates = 1; 
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) { 
	//가로x세로x높이가 12x12x12인 정육면체 메쉬를 생성한다. 
	CCubeMeshDiffused *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList,12.0f, 12.0f, 12.0f);
	CCubeColorMeshDiffused *pCubeMeshRed = new CCubeColorMeshDiffused(pd3dDevice, pd3dCommandList, RED_COLOR, 12.0f, 12.0f, 12.0f);

	CCubeColorMeshDiffused *pBulletMesh = new CCubeColorMeshDiffused(pd3dDevice, pd3dCommandList,RED_COLOR, 2.0f, 2.0f, 2.0f);

	CCubeColorMeshDiffused *pExplosiveMeshRed = new CCubeColorMeshDiffused(pd3dDevice, pd3dCommandList, RED_COLOR, 2.0f, 2.0f, 2.0f);
	CCubeColorMeshDiffused *pExplosiveMeshGreen = new CCubeColorMeshDiffused(pd3dDevice, pd3dCommandList, GREEN_COLOR, 2.0f, 2.0f, 2.0f);
	CCubeColorMeshDiffused *pExplosiveMeshBlue = new CCubeColorMeshDiffused(pd3dDevice, pd3dCommandList, BLUE_COLOR, 2.0f, 2.0f, 2.0f);
	CCubeColorMeshDiffused *pExplosiveMeshYellow = new CCubeColorMeshDiffused(pd3dDevice, pd3dCommandList, YELLOW_COLOR, 2.0f, 2.0f, 2.0f);
	CCubeColorMeshDiffused *pExplosiveMeshCyan = new CCubeColorMeshDiffused(pd3dDevice, pd3dCommandList, CYAN_COLOR, 2.0f, 2.0f, 2.0f);
	CCubeColorMeshDiffused *pExplosiveMeshMagenta = new CCubeColorMeshDiffused(pd3dDevice, pd3dCommandList, MAGENTA_COLOR, 2.0f, 2.0f, 2.0f);

	CCubeColorMeshDiffused *pBossMesh = new CCubeColorMeshDiffused(pd3dDevice, pd3dCommandList, YELLOW_COLOR, 12.0f, 12.0f, 12.0f);


	m_nParticleMagenta = 0;
	m_nBullets = 0;			// 처음 총알의 개수는 0이다.
	m_nObjects = 0;			// 처음 오브젝트의 개수는 0이다.
	
	boss = new Boss(pBossMesh);
	m_ppObjects = new CExplosiveObject*[100];
	m_ppObjectsRed = new CExplosiveObject*[20];
	m_Bullet = new CGameObject*[10];
	m_ParticleObjectRed = new Particles*[3];
	m_ParticleObjectGreen = new Particles*[3];
	m_ParticleObjectBlue = new Particles*[3];
	m_ParticleObjectCyan = new Particles*[3];
	m_ParticleObjectYellow = new Particles*[3];
	m_ParticleObjectMagenta = new Particles*[3];

	boss_bullet = new CGameObject*[10];

	for (int i = 0; i < 100; ++i)
	{
		m_ppObjects[i] = new CExplosiveObject(pCubeMesh);
	}
	for (int i = 0; i < 20; ++i)
	{
		m_ppObjectsRed[i] = new CExplosiveObject(pCubeMeshRed);
	}

	for (int i = 0; i < 3; ++i)
	{
		m_ParticleObjectRed[i] = new Particles(pExplosiveMeshRed);
		m_ParticleObjectGreen[i] = new Particles(pExplosiveMeshGreen);
		m_ParticleObjectBlue[i] = new Particles(pExplosiveMeshBlue);
		m_ParticleObjectCyan[i] = new Particles(pExplosiveMeshCyan);
		m_ParticleObjectYellow[i] = new Particles(pExplosiveMeshYellow);
		m_ParticleObjectMagenta[i] = new Particles(pExplosiveMeshMagenta);
	}

	for (int i = 0; i < 10; ++i)
	{
		m_Bullet[i] = new Bullet(pBulletMesh);
	}
	for (int i = 0; i < 10; ++i)
	{
		boss_bullet[i] = new Bullet(pBulletMesh);
	}


	float fHalfWidth = 12.5f, fHalfHeight = 12.5f, fHalfDepth = 125.0f;
	CWallMeshDiffused *pWallMesh = new CWallMeshDiffused(pd3dDevice, pd3dCommandList,fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f);
	pWallMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	CWallsObject *pWallObject = new CWallsObject();

	pWallObject->SetPosition(0.0f, 0.0f, 0.0f);
	pWallObject->SetMesh(pWallMesh);
	m_pWallsObjects = pWallObject;
	m_pWallsObjects->m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fHalfWidth*3);
	m_pWallsObjects->m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fHalfWidth * 3);
	m_pWallsObjects->m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHalfHeight * 3);
	m_pWallsObjects->m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHalfHeight * 3);
	m_pWallsObjects->m_pxmf4WallPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, fHalfDepth * 3);
	m_pWallsObjects->m_pxmf4WallPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, fHalfDepth * 3);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CObjectsShader::ReleaseObjects() 
{
	if (m_ppObjectsRed)
	{
		for (int i = 0; i < 20; i++)
		{
			if (m_ppObjectsRed[i])delete m_ppObjectsRed[i];
		}
		delete[] m_ppObjectsRed;
	}
	if (m_ppObjects)
	{
		for (int j = 0; j < 100; j++)
		{
			if (m_ppObjects[j]) delete m_ppObjects[j];
		}
		delete[] m_ppObjects;
	}
	if (m_Bullet)
	{
		for (int j = 0; j < 3; j++)
		{
			if (m_Bullet[j]) delete m_Bullet[j];
		}
		delete[] m_Bullet;
	}
	if (m_ParticleObjectRed)
	{
		for (int j = 0; j < 3; j++)
		{
			if (m_ParticleObjectRed[j]) delete m_ParticleObjectRed[j];
		}
		delete[] m_ParticleObjectRed;
	}
	if (m_ParticleObjectGreen)
	{
		for (int j = 0; j < 3; j++)
		{
			if (m_ParticleObjectGreen[j]) delete m_ParticleObjectGreen[j];
		}
		delete[] m_ParticleObjectGreen;
	}
	if (m_ParticleObjectBlue)
	{
		for (int j = 0; j < 3; j++)
		{
			if (m_ParticleObjectBlue[j]) delete m_ParticleObjectBlue[j];
		}
		delete[] m_ParticleObjectBlue;
	}
	if (m_ParticleObjectMagenta)
	{
		for (int j = 0; j < 3; j++)
		{
			if (m_ParticleObjectMagenta[j]) delete m_ParticleObjectMagenta[j];
		}
		delete[] m_ParticleObjectMagenta;
	}
	if (m_ParticleObjectYellow)
	{
		for (int j = 0; j < 3; j++)
		{
			if (m_ParticleObjectYellow[j]) delete m_ParticleObjectYellow[j];
		}
		delete[] m_ParticleObjectYellow;
	}
	if (m_ParticleObjectCyan)
	{
		for (int j = 0; j < 3; j++)
		{
			if (m_ParticleObjectCyan[j]) delete m_ParticleObjectCyan[j];
		}
		delete[] m_ParticleObjectCyan;
	}

	if (boss_bullet)
	{
		for (int j = 0; j < 10; j++)
		{
			if (boss_bullet[j]) delete boss_bullet[j];
		}
		delete[] boss_bullet;
	}
	delete m_pWallsObjects;
	delete boss;
}

void CObjectsShader::AnimateObjects(float fTimeElapsed,CAirplanePlayer * m_pPlayer)		// 여기서 움직임.
{
	sElapsedTime ++;

	if (!(m_pPlayer->GetPosition().x > -50&& m_pPlayer->GetPosition().x < 50)||
		!(m_pPlayer->GetPosition().y > -50 && m_pPlayer->GetPosition().y < 50)||
		!(m_pPlayer->GetPosition().z > -500 && m_pPlayer->GetPosition().z < 500))
	{
		printf("충돌");
		m_pPlayer->SetPosition(ex_p);
		m_pPlayer->ChangeRedModel();	
	}
	else
	{
		ex_p = m_pPlayer->GetPosition();
		m_pPlayer->ChangeGreenModel();
	}
	playerPos = m_pPlayer->GetPosition();
	playerDir = m_pPlayer->GetLookVector();

	// 적 큐브 생성 100마리
	if (sElapsedTime % 200 == 0)
	{
		// sElapsedTime이 100으로 나눴을때 나머지가 0이되면,
		for (int i = 0; i < 100; ++i)
		{
			// 100개의 오브젝트 중에서, 활성화가 안되있는 녀석을 찾아서 그 녀석을 활성화 시킨다.
			// 그리고 오브젝트의 갯수를 한개 늘린다.
			if (m_ppObjects[i]->m_bActive == false)
			{
				m_nObjects++;
				m_ppObjects[i]->ResetPos(m_pPlayer->GetPosition().z);
				m_ppObjects[i]->m_bActive = true;
				break;
				// 활성화를 시켰으면, 루프를 빠져나간다.
			}
		}
	}

	if (sElapsedTime % 500 == 0)
	{
		for (int i = 0; i < 20; ++i)
		{
			// 100개의 오브젝트 중에서, 활성화가 안되있는 녀석을 찾아서 그 녀석을 활성화 시킨다.
			// 그리고 오브젝트의 갯수를 한개 늘린다.
			if (m_ppObjectsRed[i]->m_bActive == false)
			{
				m_nObjectsRed++;
				m_ppObjectsRed[i]->ResetPos(m_pPlayer->GetPosition().z);
				m_ppObjectsRed[i]->m_bActive = true;
				break;
				// 활성화를 시켰으면, 루프를 빠져나간다.
			}
		}
	}



	for (int i = 0; i < 10; ++i)
	{
		if (m_Bullet[i]->m_bActive == true)
		{
			// 활성화된 총알인 경우, 시간초를 계속해서 흘러가게 한다.
			m_Bullet[i]->time++;
		}
		// 50만큼의 시간이 지나면 총알을 비활성화시킨 후에, 시간을 초기화시켜준다. 
		if (m_Bullet[i]->time > 50)
		{
			m_Bullet[i]->m_bActive = false;
			m_Bullet[i]->time = 0;
			m_nBullets--;
		}
	}

	for (int i = 0; i < 100; ++i)
	{
		if (m_ppObjects[i]->m_bActive == true)
		{
			m_ppObjects[i]->Animate(fTimeElapsed);
			m_ppObjects[i]->time++;
			if (m_ppObjects[i]->GetPosition().z < m_pPlayer->GetPosition().z)
			{
				m_pPlayer->ChangeMagentaModel();
			}
		}
	}

	for (int i = 0; i < 20; ++i)
	{
		if (m_ppObjectsRed[i]->m_bActive == true)
		{
			m_ppObjectsRed[i]->Animate(fTimeElapsed);
			m_ppObjectsRed[i]->time++;
			if (m_ppObjectsRed[i]->GetPosition().z < m_pPlayer->GetPosition().z)
			{
				m_pPlayer->ChangeMagentaModel();
			}
		}
	}

	for (int i = 0; i < 10; ++i)
	{
		if (m_Bullet[i]->m_bActive == true)
		{
			m_Bullet[i]->Animate(fTimeElapsed);
		}
	}

	for (int i = 0; i < 10; ++i)
	{
		if (boss_bullet[i]->m_bActive == true)
		{
			boss_bullet[i]->Animate(fTimeElapsed);
			boss_bullet[i]->time++;
		}
		if (boss_bullet[i]->time > 30)
		{
			boss_bullet[i]->m_bActive = false;
			boss_bullet[i]->time = 0;
			m_nBossBullets--;
		}
	}


	for (int i = 0; i < 3; ++i)
	{
		if (m_ParticleObjectRed[i]->m_bActive == true)
		{
			m_ParticleObjectRed[i]->AnimateParticles(fTimeElapsed);
			m_ParticleObjectRed[i]->time++;
		}

		if (m_ParticleObjectRed[i]->time > 30)
		{
			m_ParticleObjectRed[i]->m_bActive = false;
			m_ParticleObjectRed[i]->time=0;
			m_nParticleRed--;
		}
		///////////////////////////////////////
		if (m_ParticleObjectGreen[i]->m_bActive == true)
		{
			m_ParticleObjectGreen[i]->AnimateParticles(fTimeElapsed);
			m_ParticleObjectGreen[i]->time++;
		}

		if (m_ParticleObjectGreen[i]->time > 30)
		{
			m_ParticleObjectGreen[i]->m_bActive = false;
			m_ParticleObjectGreen[i]->time = 0;
			m_nParticleGreen--;
		}
		/////////////////////////////////////////
		if (m_ParticleObjectBlue[i]->m_bActive == true)
		{
			m_ParticleObjectBlue[i]->AnimateParticles(fTimeElapsed);
			m_ParticleObjectBlue[i]->time++;
		}

		if (m_ParticleObjectBlue[i]->time > 30)
		{
			m_ParticleObjectBlue[i]->m_bActive = false;
			m_ParticleObjectBlue[i]->time = 0;
			m_nParticleMagenta--;
		}
		/////////////////////////////////////////
		if (m_ParticleObjectMagenta[i]->m_bActive == true)
		{
			m_ParticleObjectMagenta[i]->AnimateParticles(fTimeElapsed);
			m_ParticleObjectMagenta[i]->time++;
		}

		if (m_ParticleObjectMagenta[i]->time > 30)
		{
			m_ParticleObjectMagenta[i]->m_bActive = false;
			m_ParticleObjectMagenta[i]->time = 0;
			m_nParticleMagenta--;
		}
		//////////////////////////////////////////////////
		if (m_ParticleObjectYellow[i]->m_bActive == true)
		{
			m_ParticleObjectYellow[i]->AnimateParticles(fTimeElapsed);
			m_ParticleObjectYellow[i]->time++;
		}

		if (m_ParticleObjectYellow[i]->time > 30)
		{
			m_ParticleObjectYellow[i]->m_bActive = false;
			m_ParticleObjectYellow[i]->time = 0;
			m_nParticleYellow--;
		}
		/////////////////////////////////////////////////////////////
		if (m_ParticleObjectCyan[i]->m_bActive == true)
		{
			m_ParticleObjectCyan[i]->AnimateParticles(fTimeElapsed);
			m_ParticleObjectCyan[i]->time++;
		}

		if (m_ParticleObjectCyan[i]->time > 30)
		{
			m_ParticleObjectCyan[i]->m_bActive = false;
			m_ParticleObjectCyan[i]->time = 0;
			m_nParticleCyan--;
		}

	}
	for (int i = 0;i<m_nObjects;++i)						
	{
		if (m_ppObjects[i]->m_xmOOBB.Contains(XMLoadFloat3(&m_pPlayer->GetPosition())) == CONTAINS)
		{
			gameOver = true;
		}
	}
	for (int i = 0; i<m_nObjectsRed; ++i)
	{
		if (m_ppObjectsRed[i]->m_xmOOBB.Contains(XMLoadFloat3(&m_pPlayer->GetPosition())) == CONTAINS)
		{
			gameOver = true;
		}
	}

	for (int i = 0; i<m_nBossBullets; ++i)						
	{
		if (boss_bullet[i]->m_xmOOBB.Contains(XMLoadFloat3(&m_pPlayer->GetPosition())) == CONTAINS)
		{
			gameOver = true;
		}
	}


	if (sElapsedTime % 20 == 0&&sqrt(
		(m_pPlayer->GetPosition().x-boss->GetPosition().x)*(m_pPlayer->GetPosition().x - boss->GetPosition().x)+
		(m_pPlayer->GetPosition().y - boss->GetPosition().y)*(m_pPlayer->GetPosition().y - boss->GetPosition().y)+
		(m_pPlayer->GetPosition().z - boss->GetPosition().z)*(m_pPlayer->GetPosition().z - boss->GetPosition().z)
	)<200) {
		for (int i = 0; i < 10; i++)
		{
			// 총알 10개중에 비활성화 된 녀석을 활성화시킨다.
			if (boss_bullet[i]->m_bActive == false)
			{
				boss_bullet[i]->m_bActive = true;
				boss_bullet[i]->SetMovingDirection(Vector3::Normalize(Vector3::Subtract(playerPos,boss->GetPosition())));
				boss_bullet[i]->SetPosition(boss->GetPosition());
				m_nBossBullets++;
				break;
			}
		}
	}
	
	if (gameOver == true)		// 게임내의 객체들을 모두 초기화 시켜준다.
	{
		m_pPlayer->SetFirst();
		m_nSkill_count = 0;
		boss->SetPosition(XMFLOAT3(0, 0, 450));
		for (int i = 0; i < m_nObjects; ++i)
		{
			m_ppObjects[i]->m_bActive = false;
			m_ppObjects[i]->time = 0;
		}
		for (int i = 0; i < m_nObjectsRed; ++i)
		{
			m_ppObjectsRed[i]->m_bActive=false;
			m_ppObjectsRed[i]->time = 0;
		}
		for (int i = 0; i < m_nBullets; ++i)
		{
			m_Bullet[i]->m_bActive = false;
		}
		gameOver = false;
	}
	
	for (int i=0;i<m_nObjects;++i)		// 따라와라 큐브큐브
	{
		if (m_ppObjects[i]->time > 50)
		{
			XMFLOAT3 dir;
			dir.x = m_pPlayer->GetPosition().x - m_ppObjects[i]->GetPosition().x;
			dir.y = m_pPlayer->GetPosition().y - m_ppObjects[i]->GetPosition().y;
			dir.z = m_pPlayer->GetPosition().z - m_ppObjects[i]->GetPosition().z;
			m_ppObjects[i]->SetMovingDirection(dir);
		}
	}

	for (int i = 0; i<m_nObjectsRed; ++i)		// 따라와라 큐브큐브
	{
		if (m_ppObjectsRed[i]->time > 50)
		{
			XMFLOAT3 dir;
			dir.x = m_pPlayer->GetPosition().x - m_ppObjectsRed[i]->GetPosition().x;
			dir.y = m_pPlayer->GetPosition().y - m_ppObjectsRed[i]->GetPosition().y;
			dir.z = m_pPlayer->GetPosition().z - m_ppObjectsRed[i]->GetPosition().z;
			m_ppObjectsRed[i]->SetMovingDirection(dir);
		}
	}


	boss->Animate(fTimeElapsed);

	CheckObjectByBulletsCollisions();
	CheckObjectByWallCollisions();
	CheckObjectByObjectCollisions();
	CheckBossByWallCollisions();
	CheckBossByBulletCollisions();

	if (boss->GetHP() < 0)
	{
		exit(0);
	}
}
void CObjectsShader::CheckBossByWallCollisions()	// 보스, 벽 충돌
{
	ContainmentType containType = m_pWallsObjects->m_xmOOBB.Contains(boss->m_xmOOBB);
	switch (containType)
	{
	case DISJOINT:
	{
		int nPlaneIndex = -1;
		for (int j = 0; j < 6; j++)
		{
			PlaneIntersectionType intersectType = boss->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObjects->m_pxmf4WallPlanes[j]));
			if (intersectType == BACK)
			{
				nPlaneIndex = j;
				break;
			}
		}
		if (nPlaneIndex != -1)
		{
			XMVECTOR xmvNormal = XMVectorSet(m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
			XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&boss->m_xmf3MovingDirection), xmvNormal);
			XMStoreFloat3(&boss->m_xmf3MovingDirection, xmvReflect);
		}
		break;
	}
	case INTERSECTS:
	{
		int nPlaneIndex = -1;
		for (int j = 0; j < 6; j++)
		{
			PlaneIntersectionType intersectType = boss->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObjects->m_pxmf4WallPlanes[j]));
			if (intersectType == INTERSECTING)
			{
				nPlaneIndex = j;
				break;
			}
		}
		if (nPlaneIndex != -1)
		{
			XMVECTOR xmvNormal = XMVectorSet(m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
			XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&boss->m_xmf3MovingDirection), xmvNormal);
			XMStoreFloat3(&boss->m_xmf3MovingDirection, xmvReflect);
		}
		break;
	}
	case CONTAINS:
		break;
	}
}
void CObjectsShader::CheckBossByBulletCollisions()	// 보스, 총알 충돌
{
	for (int j = 0; j < m_nBullets; ++j)
	{
		if (boss->m_xmOOBB.Intersects(m_Bullet[j]->m_xmOOBB) && m_Bullet[j]->m_bActive == true)
		{
			m_Bullet[j]->m_bActive = false;
			m_Bullet[j]->time = 0;
			m_nBullets--;
			boss->MinusHP();
			printf("맞았다");
		}
	}
}

void CObjectsShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < 100; j++)
			m_ppObjects[j]->ReleaseUploadBuffers();
	}
	if (m_Bullet)
	{
		for (int j = 0; j < 10; j++)
			m_Bullet[j]->ReleaseUploadBuffers();
	}

	if (m_ParticleObjectMagenta)
	{
		for (int j = 0; j < 3; j++) {
			for (int i = 0; i < 100; ++i) {
				m_ParticleObjectMagenta[j]->particle[i]->ReleaseUploadBuffers();
			}
		}
	}

	if (m_ParticleObjectYellow)
	{
		for (int j = 0; j < 3; j++) {
			for (int i = 0; i < 100; ++i) {
				m_ParticleObjectYellow[j]->particle[i]->ReleaseUploadBuffers();
			}
		}
	}

	if (m_ParticleObjectCyan)
	{
		for (int j = 0; j < 3; j++) {
			for (int i = 0; i < 100; ++i) {
				m_ParticleObjectCyan[j]->particle[i]->ReleaseUploadBuffers();
			}
		}
	}

	if (m_ParticleObjectRed)
	{
		for (int j = 0; j < 3; j++) {
			for (int i = 0; i < 100; ++i) {
				m_ParticleObjectRed[j]->particle[i]->ReleaseUploadBuffers();
			}
		}
	}

	if (m_ParticleObjectBlue)
	{
		for (int j = 0; j < 3; j++) {
			for (int i = 0; i < 100; ++i) {
				m_ParticleObjectBlue[j]->particle[i]->ReleaseUploadBuffers();
			}
		}
	}

	if (m_ParticleObjectGreen)
	{
		for (int j = 0; j < 3; j++) {
			for (int i = 0; i < 100; ++i) {
				m_ParticleObjectGreen[j]->particle[i]->ReleaseUploadBuffers();
			}
		}
	}

	if (boss_bullet)
	{
		for (int i = 0; i < 10; i++)
		{
			boss_bullet[i]->ReleaseUploadBuffers();
		}
	}
	boss->ReleaseUploadBuffers();
}

void CObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera) {
	CShader::Render(pd3dCommandList, pCamera);

	for (int i = 0; i < 100; ++i)
	{
		if (m_ppObjects[i]->m_bActive == true)
		{
			m_ppObjects[i]->Render(pd3dCommandList, pCamera);
		}
	}

	for (int i = 0; i < 20; ++i)
	{
		if (m_ppObjectsRed[i]->m_bActive == true)
		{
			m_ppObjectsRed[i]->Render(pd3dCommandList, pCamera);
		}
	}

	for (int i = 0; i < 10; ++i)
	{
		if (m_Bullet[i]->m_bActive == true)
		{
			m_Bullet[i]->Render(pd3dCommandList, pCamera);
		}
	}
	for (int i = 0; i < 3; ++i)
	{
		if (m_ParticleObjectMagenta[i]->m_bActive == true)
		{
			m_ParticleObjectMagenta[i]->RenderParticles(pd3dCommandList, pCamera);
		}
	}
	for (int i = 0; i < 3; ++i)
	{
		if (m_ParticleObjectCyan[i]->m_bActive == true)
		{
			m_ParticleObjectCyan[i]->RenderParticles(pd3dCommandList, pCamera);
		}
	}

	for (int i = 0; i < 3; ++i)
	{
		if (m_ParticleObjectYellow[i]->m_bActive == true)
		{
			m_ParticleObjectYellow[i]->RenderParticles(pd3dCommandList, pCamera);
		}
	}
	for (int i = 0; i < 3; ++i)
	{
		if (m_ParticleObjectGreen[i]->m_bActive == true)
		{
			m_ParticleObjectGreen[i]->RenderParticles(pd3dCommandList, pCamera);
		}
	}
	for (int i = 0; i < 3; ++i)
	{
		if (m_ParticleObjectBlue[i]->m_bActive == true)
		{
			m_ParticleObjectBlue[i]->RenderParticles(pd3dCommandList, pCamera);
		}
	}
	for (int i = 0; i < 3; ++i)
	{
		if (m_ParticleObjectRed[i]->m_bActive == true)
		{
			m_ParticleObjectRed[i]->RenderParticles(pd3dCommandList, pCamera);
		}
	}

	for (int i = 0; i < 10; ++i)
	{
		if (boss_bullet[i]->m_bActive == true)
		{
			boss_bullet[i]->Render(pd3dCommandList, pCamera);
		}
	}
	m_pWallsObjects->Render(pd3dCommandList, pCamera);
	boss->Render(pd3dCommandList, pCamera);
}

void CObjectsShader::CheckObjectByWallCollisions()
{
	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjects[i]->m_bActive == true) 
		{
			ContainmentType containType = m_pWallsObjects->m_xmOOBB.Contains(m_ppObjects[i]->m_xmOOBB);
			switch (containType)
			{
			case DISJOINT:
			{
				int nPlaneIndex = -1;
				for (int j = 0; j < 6; j++)
				{
					PlaneIntersectionType intersectType = m_ppObjects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObjects->m_pxmf4WallPlanes[j]));
					if (intersectType == BACK)
					{
						nPlaneIndex = j;
						break;
					}
				}
				if (nPlaneIndex != -1)
				{
					XMVECTOR xmvNormal = XMVectorSet(m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
					XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppObjects[i]->m_xmf3MovingDirection), xmvNormal);
					XMStoreFloat3(&m_ppObjects[i]->m_xmf3MovingDirection, xmvReflect);
				}
				break;
			}
			case INTERSECTS:
			{
				int nPlaneIndex = -1;
				for (int j = 0; j < 6; j++)
				{
					PlaneIntersectionType intersectType = m_ppObjects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObjects->m_pxmf4WallPlanes[j]));
					if (intersectType == INTERSECTING)
					{
						nPlaneIndex = j;
						break;
					}
				}
				if (nPlaneIndex != -1)
				{
					XMVECTOR xmvNormal = XMVectorSet(m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
					XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppObjects[i]->m_xmf3MovingDirection), xmvNormal);
					XMStoreFloat3(&m_ppObjects[i]->m_xmf3MovingDirection, xmvReflect);
				}
				break;
			}
			case CONTAINS:
				break;
			}
		}
	}

	for (int i = 0; i < m_nObjectsRed; i++)
	{
		if (m_ppObjectsRed[i]->m_bActive == true)
		{
			ContainmentType containType = m_pWallsObjects->m_xmOOBB.Contains(m_ppObjectsRed[i]->m_xmOOBB);
			switch (containType)
			{
			case DISJOINT:
			{
				int nPlaneIndex = -1;
				for (int j = 0; j < 6; j++)
				{
					PlaneIntersectionType intersectType = m_ppObjectsRed[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObjects->m_pxmf4WallPlanes[j]));
					if (intersectType == BACK)
					{
						nPlaneIndex = j;
						break;
					}
				}
				if (nPlaneIndex != -1)
				{
					XMVECTOR xmvNormal = XMVectorSet(m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
					XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppObjectsRed[i]->m_xmf3MovingDirection), xmvNormal);
					XMStoreFloat3(&m_ppObjectsRed[i]->m_xmf3MovingDirection, xmvReflect);
				}
				break;
			}
			case INTERSECTS:
			{
				int nPlaneIndex = -1;
				for (int j = 0; j < 6; j++)
				{
					PlaneIntersectionType intersectType = m_ppObjectsRed[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObjects->m_pxmf4WallPlanes[j]));
					if (intersectType == INTERSECTING)
					{
						nPlaneIndex = j;
						break;
					}
				}
				if (nPlaneIndex != -1)
				{
					XMVECTOR xmvNormal = XMVectorSet(m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObjects->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
					XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppObjectsRed[i]->m_xmf3MovingDirection), xmvNormal);
					XMStoreFloat3(&m_ppObjectsRed[i]->m_xmf3MovingDirection, xmvReflect);
				}
				break;
			}
			case CONTAINS:
				break;
			}
		}
	}
}


void CObjectsShader::CheckObjectByObjectCollisions()
{
	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->m_pObjectCollided = NULL;
	for (int i = 0; i < m_nObjects; i++)
	{
		for (int j = (i + 1); j < m_nObjects; j++)
		{
			if (m_ppObjects[i]->m_xmOOBB.Intersects(m_ppObjects[j]->m_xmOOBB) && m_ppObjects[i]->m_bActive == true && m_ppObjects[j]->m_bActive == true)
			{
				m_ppObjects[i]->m_pObjectCollided = m_ppObjects[j];
				m_ppObjects[j]->m_pObjectCollided = m_ppObjects[i];
			}
		}
	}
	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjects[i]->m_pObjectCollided&&m_ppObjects[i]->m_bActive==true)
		{
			XMFLOAT3 xmf3MovingDirection = m_ppObjects[i]->m_xmf3MovingDirection;
			float fMovingSpeed = m_ppObjects[i]->m_fMovingSpeed;
			m_ppObjects[i]->m_xmf3MovingDirection = m_ppObjects[i]->m_pObjectCollided->m_xmf3MovingDirection;
			m_ppObjects[i]->m_fMovingSpeed = m_ppObjects[i]->m_pObjectCollided->m_fMovingSpeed;
			m_ppObjects[i]->m_pObjectCollided->m_xmf3MovingDirection = xmf3MovingDirection;
			m_ppObjects[i]->m_pObjectCollided->m_fMovingSpeed = fMovingSpeed;
			m_ppObjects[i]->m_pObjectCollided->m_pObjectCollided = NULL;
			m_ppObjects[i]->m_pObjectCollided = NULL;
		}
	}

	for (int i = 0; i < m_nObjectsRed; i++) m_ppObjectsRed[i]->m_pObjectCollided = NULL;
	for (int i = 0; i < m_nObjectsRed; i++)
	{
		for (int j = (i + 1); j < m_nObjects; j++)
		{
			if (m_ppObjectsRed[i]->m_xmOOBB.Intersects(m_ppObjectsRed[j]->m_xmOOBB) && m_ppObjectsRed[i]->m_bActive == true && m_ppObjectsRed[j]->m_bActive == true)
			{
				m_ppObjectsRed[i]->m_pObjectCollided = m_ppObjectsRed[j];
				m_ppObjectsRed[j]->m_pObjectCollided = m_ppObjectsRed[i];
			}
		}
	}
	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjectsRed[i]->m_pObjectCollided&&m_ppObjects[i]->m_bActive == true)
		{
			XMFLOAT3 xmf3MovingDirection = m_ppObjectsRed[i]->m_xmf3MovingDirection;
			float fMovingSpeed = m_ppObjectsRed[i]->m_fMovingSpeed;
			m_ppObjectsRed[i]->m_xmf3MovingDirection = m_ppObjectsRed[i]->m_pObjectCollided->m_xmf3MovingDirection;
			m_ppObjectsRed[i]->m_fMovingSpeed = m_ppObjectsRed[i]->m_pObjectCollided->m_fMovingSpeed;
			m_ppObjectsRed[i]->m_pObjectCollided->m_xmf3MovingDirection = xmf3MovingDirection;
			m_ppObjectsRed[i]->m_pObjectCollided->m_fMovingSpeed = fMovingSpeed;
			m_ppObjectsRed[i]->m_pObjectCollided->m_pObjectCollided = NULL;
			m_ppObjectsRed[i]->m_pObjectCollided = NULL;
		}
	}
}


void CObjectsShader::CheckObjectByBulletsCollisions() 
{
	// 계산이 복잡하기 때문에 시간이 걸린다.
	for (int i=0;i<m_nObjects;++i)
	{
		for (int j=0;j<m_nBullets;++j)
		{
			if (m_ppObjects[i]->m_xmOOBB.Intersects(m_Bullet[j]->m_xmOOBB)&&m_Bullet[j]->m_bActive==true&& m_ppObjects[i]->m_bActive == true)
			{
				m_Bullet[j]->m_bActive = false;
				m_ppObjects[i]->m_bActive = false;
				m_ppObjects[i]->time = 0;
				m_Bullet[j]->time = 0;
				m_nBullets--;

				// 파티클 생성
				for (int k = 0; k < 3; ++k)
				{
					if (m_ParticleObjectMagenta[k]->m_bActive == false&&particleColor==MAGENTA)
					{
						m_ParticleObjectMagenta[k]->SetParticlesPosition(m_ppObjects[i]);
						m_ParticleObjectMagenta[k]->m_bActive = true;
						m_nParticleMagenta++;
						break;
					}
					if (m_ParticleObjectYellow[k]->m_bActive == false && particleColor == YELLOW)
					{
						m_ParticleObjectYellow[k]->SetParticlesPosition(m_ppObjects[i]);
						m_ParticleObjectYellow[k]->m_bActive = true;
						m_nParticleYellow++;
						break;
					}
					if (m_ParticleObjectCyan[k]->m_bActive == false && particleColor == CYAN)
					{
						m_ParticleObjectCyan[k]->SetParticlesPosition(m_ppObjects[i]);
						m_ParticleObjectCyan[k]->m_bActive = true;
						m_nParticleMagenta++;
						break;
					}
					if (m_ParticleObjectRed[k]->m_bActive == false && particleColor == RED)
					{
						m_ParticleObjectRed[k]->SetParticlesPosition(m_ppObjects[i]);
						m_ParticleObjectRed[k]->m_bActive = true;
						m_nParticleMagenta++;
						break;
					}
					if (m_ParticleObjectGreen[k]->m_bActive == false && particleColor == GREEN)
					{
						m_ParticleObjectGreen[k]->SetParticlesPosition(m_ppObjects[i]);
						m_ParticleObjectGreen[k]->m_bActive = true;
						m_nParticleGreen++;
						break;
					}
					if (m_ParticleObjectBlue[k]->m_bActive == false && particleColor == BLUE)
					{
						m_ParticleObjectBlue[k]->SetParticlesPosition(m_ppObjects[i]);
						m_ParticleObjectBlue[k]->m_bActive = true;
						m_nParticleMagenta++;
						break;
					}

				}
			}
		}
	}

	for (int i = 0; i<m_nObjectsRed; ++i)
	{
		for (int j = 0; j<m_nBullets; ++j)
		{
			if (m_ppObjectsRed[i]->m_xmOOBB.Intersects(m_Bullet[j]->m_xmOOBB) && m_Bullet[j]->m_bActive == true && m_ppObjectsRed[i]->m_bActive == true
				)
			{
				m_Bullet[j]->m_bActive = false;
				m_ppObjectsRed[i]->m_bActive = false;
				m_ppObjectsRed[i]->time = 0;
				m_Bullet[j]->time = 0;
				m_nSkill_count++;
				m_nBullets--;

				// 파티클 생성
				for (int k = 0; k < 3; ++k)
				{
					m_ParticleObjectRed[k]->SetParticlesPosition(m_ppObjectsRed[i]);
					m_ParticleObjectRed[k]->m_bActive = true;
					m_nParticleMagenta++;
					break;
				}
			}
		}
	}
}

bool CObjectsShader::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam,CAirplanePlayer * m_pPlayer)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_SPACE:
			if(m_nSkill_count>0)
			{
			m_nSkill_count--;
			for (int i = 0; i < m_nObjects; ++i) {
				if (m_pPlayer->GetPosition().z>m_ppObjects[i]->GetPosition().z-100 && m_pPlayer->GetPosition().z<m_ppObjects[i]->GetPosition().z + 100
					&&m_ppObjects[i]->m_bActive==true)
				{
					m_ppObjects[i]->m_bActive = false;
					// 파티클 생성
					for (int k = 0; k < 3; ++k)
					{
						if (m_ParticleObjectMagenta[k]->m_bActive == false && particleColor == MAGENTA)
						{
							m_ParticleObjectMagenta[k]->SetParticlesPosition(m_ppObjects[i]);
							m_ParticleObjectMagenta[k]->m_bActive = true;
							m_nParticleMagenta++;
							break;
						}
						if (m_ParticleObjectYellow[k]->m_bActive == false && particleColor == YELLOW)
						{
							m_ParticleObjectYellow[k]->SetParticlesPosition(m_ppObjects[i]);
							m_ParticleObjectYellow[k]->m_bActive = true;
							m_nParticleYellow++;
							break;
						}
						if (m_ParticleObjectCyan[k]->m_bActive == false && particleColor == CYAN)
						{
							m_ParticleObjectCyan[k]->SetParticlesPosition(m_ppObjects[i]);
							m_ParticleObjectCyan[k]->m_bActive = true;
							m_nParticleMagenta++;
							break;
						}
						if (m_ParticleObjectRed[k]->m_bActive == false && particleColor == RED)
						{
							m_ParticleObjectRed[k]->SetParticlesPosition(m_ppObjects[i]);
							m_ParticleObjectRed[k]->m_bActive = true;
							m_nParticleMagenta++;
							break;
						}
						if (m_ParticleObjectGreen[k]->m_bActive == false && particleColor == GREEN)
						{
							m_ParticleObjectGreen[k]->SetParticlesPosition(m_ppObjects[i]);
							m_ParticleObjectGreen[k]->m_bActive = true;
							m_nParticleGreen++;
							break;
						}
						if (m_ParticleObjectBlue[k]->m_bActive == false && particleColor == BLUE)
						{
							m_ParticleObjectBlue[k]->SetParticlesPosition(m_ppObjects[i]);
							m_ParticleObjectBlue[k]->m_bActive = true;
							m_nParticleMagenta++;
							break;
						}
					}
				}
			}
			}
			break;
		case VK_CONTROL:
			// ctrl키를 누르면 총알이 발사된다.
			// 총알은 방향과 위치를 알고 있어야 한다.
			for (int i = 0; i < 10; i++)
			{
				if (picked == false) {
					// 총알 10개중에 비활성화 된 녀석을 활성화시킨다.
					if (m_Bullet[i]->m_bActive == false)
					{
						m_Bullet[i]->m_bActive = true;
						m_Bullet[i]->SetMovingDirection(playerDir);
						m_Bullet[i]->SetPosition(playerPos);
						m_nBullets++;
						break;
					}
				}
				else
				{
					if (m_Bullet[i]->m_bActive == false)
					{
						m_Bullet[i]->m_bActive = true;
						m_Bullet[i]->SetMovingDirection(playerDir);
						m_Bullet[i]->SetPosition(playerPos);
						m_nBullets++;
						break;
					}
					m_pPlayer->Targeting(m_pickedObject);
				}
			}
			break;
		}
		break;
	default:
		break;
	}
	return true;
}


void CObjectsShader::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam,CAirplanePlayer* m_pPlayer)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	{
		int n;
		float dist = 100000000;
		float intersectDist = 10000000;			// 최소 플레이어와 오브젝트간 거리
		float vx, vy, vz;
		mx = LOWORD(lParam);	//xScreen
		my = HIWORD(lParam);	//yScreen
		vx = (((((mx - 0/*뷰포트의 left*/)*2.0f / 640/*뷰포트의 width*/) - 1.0f)) - m_pPlayer->GetCamera()->GetProjectionMatrix()._31) / m_pPlayer->GetCamera()->GetProjectionMatrix()._11;
		vy = ((-(((my - 0/*뷰포트의 top*/)*2.0f / 480) - 1.0f/*뷰포트의 height*/)) - m_pPlayer->GetCamera()->GetProjectionMatrix()._32) / m_pPlayer->GetCamera()->GetProjectionMatrix()._22;
		vz = 1.0f;
		XMFLOAT4X4 m = Matrix4x4::Inverse(m_pPlayer->GetCamera()->GetViewMatrix());
		ray.Dir.x = vx * m._11 + vy * m._21 + vz * m._31;
		ray.Dir.y = vx * m._12 + vy * m._22 + vz * m._32;
		ray.Dir.z = vx * m._13 + vy * m._23 + vz * m._33;
		ray.Pos.x = m._41;
		ray.Pos.y = m._42;
		ray.Pos.z = m._43;
		picked = false;
		for (int i = 0; i < m_nObjects; ++i)
		{
			if (m_ppObjects[i]->m_bActive==true&&m_ppObjects[i]->m_xmOOBB.Intersects(XMLoadFloat3(&XMFLOAT3(ray.Pos.x, ray.Pos.y, ray.Pos.z)), XMLoadFloat3(&Vector3::Normalize(XMFLOAT3(ray.Dir.x, ray.Dir.y, ray.Dir.z))), dist))
			{
				if (sqrt((m_ppObjects[i]->GetPosition().x - m_pPlayer->GetPosition().x)*(m_ppObjects[i]->GetPosition().x - m_pPlayer->GetPosition().x)
					+ (m_ppObjects[i]->GetPosition().y - m_pPlayer->GetPosition().y)*(m_ppObjects[i]->GetPosition().y - m_pPlayer->GetPosition().y)
					+ (m_ppObjects[i]->GetPosition().z - m_pPlayer->GetPosition().z)*(m_ppObjects[i]->GetPosition().z - m_pPlayer->GetPosition().z)) < intersectDist)
				{
					intersectDist = sqrt((m_ppObjects[i]->GetPosition().x - m_pPlayer->GetPosition().x)*(m_ppObjects[i]->GetPosition().x - m_pPlayer->GetPosition().x)
						+ (m_ppObjects[i]->GetPosition().y - m_pPlayer->GetPosition().y)*(m_ppObjects[i]->GetPosition().y - m_pPlayer->GetPosition().y)
						+ (m_ppObjects[i]->GetPosition().z - m_pPlayer->GetPosition().z)*(m_ppObjects[i]->GetPosition().z - m_pPlayer->GetPosition().z));
					m_pickedObject = m_ppObjects[i];
					picked = true;
					red = false;
					n = i;
				}
			}
		}
		for (int i = 0; i < m_nObjects; ++i)
		{
			if (m_ppObjectsRed[i]->m_bActive == true && m_ppObjectsRed[i]->m_xmOOBB.Intersects(XMLoadFloat3(&XMFLOAT3(ray.Pos.x, ray.Pos.y, ray.Pos.z)), XMLoadFloat3(&Vector3::Normalize(XMFLOAT3(ray.Dir.x, ray.Dir.y, ray.Dir.z))), dist))
			{
				if (sqrt((m_ppObjectsRed[i]->GetPosition().x - m_pPlayer->GetPosition().x)*(m_ppObjectsRed[i]->GetPosition().x - m_pPlayer->GetPosition().x)
					+ (m_ppObjectsRed[i]->GetPosition().y - m_pPlayer->GetPosition().y)*(m_ppObjectsRed[i]->GetPosition().y - m_pPlayer->GetPosition().y)
					+ (m_ppObjectsRed[i]->GetPosition().z - m_pPlayer->GetPosition().z)*(m_ppObjectsRed[i]->GetPosition().z - m_pPlayer->GetPosition().z)) < intersectDist)
				{
					m_pickedObject = m_ppObjectsRed[i];
					picked = true;
					red = true;
				}
			}
		}

		if (picked == true&&red==false)
		{
			CCubeMeshDiffused * mesh=(CCubeMeshDiffused*)m_ppObjects[n]->GetMesh();
			particleColor = mesh->ReturnColor(ray.Pos, Vector3::Normalize(ray.Dir),m_ppObjects[n]->GetWorldTransform());
		}

	}
	// 광선과 오브젝트간의 충돌 -> 충돌된 오브젝트를 구했을 때, 그 오브젝트의 메쉬는 삼각형이 12개있음.
	// 삼각형 12개중에 어떤 삼각형과 광선이 충돌했는지 체크, 맞은 삼각형의 색깔을 리턴받아서 그 색깔대로 파티클을 튕겨줌.

		break;
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	}
}
/*

이제 빨강색 죽였을때 아이템 만들고 쓰는거 만들어주면 됨.



*/