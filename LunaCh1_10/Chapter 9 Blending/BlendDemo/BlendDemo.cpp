//***************************************************************************************
// BlendDemo.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates blending, HLSL clip(), and fogging.
//
// Controls:
//		Hold the left mouse button down and move the mouse to rotate.
//      Hold the right mouse button down to zoom in and out.
//
/*//      Press '1' - Lighting only render mode.
//      Press '2' - Texture render mode.
//      Press '3' - Fog render mode.
//
//***************************************************************************************

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"
#include "Waves.h"

enum RenderOptions
{
	Lighting = 0,
	Textures = 1,
	TexturesAndFog = 2
};

class BlendApp : public D3DApp
{
public:
	BlendApp(HINSTANCE hInstance);
	~BlendApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	float GetHillHeight(float x, float z)const;
	XMFLOAT3 GetHillNormal(float x, float z)const;
	void BuildLandGeometryBuffers();
	void BuildWaveGeometryBuffers();
	void BuildCrateGeometryBuffers();
	void BuildTriangles();

private:
	ID3D11Buffer* mLandVB;
	ID3D11Buffer* mLandIB;

	ID3D11Buffer* mWavesVB;
	ID3D11Buffer* mWavesIB;

	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;

	ID3D11Buffer* mTriangleVB;
	ID3D11Buffer* mTriangleIB;

	ID3D11ShaderResourceView* mGrassMapSRV;
	ID3D11ShaderResourceView* mWavesMapSRV;
	ID3D11ShaderResourceView* mBoxMapSRV;

	Waves mWaves;

	DirectionalLight mDirLights[3];
	Material mLandMat;
	Material mWavesMat;
	Material mBoxMat;

	XMFLOAT4X4 mGrassTexTransform;
	XMFLOAT4X4 mWaterTexTransform;
	XMFLOAT4X4 mLandWorld;
	XMFLOAT4X4 mWavesWorld;
	XMFLOAT4X4 mBoxWorld;

	ID3D11InputLayout* mInputLayout;
	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectTechnique* sTech;
	ID3DX11EffectTechnique* aTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	void BuildVertexLayout();
	void BuildFX();

	UINT mLandIndexCount;

	XMFLOAT2 mWaterTexOffset;

	RenderOptions mRenderOptions;

	XMFLOAT3 mEyePosW;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	BlendApp theApp(hInstance);

	if (!theApp.Init())
		return 0;

	return theApp.Run();
}

BlendApp::BlendApp(HINSTANCE hInstance)
	: D3DApp(hInstance), mLandVB(0), mLandIB(0), mWavesVB(0), mWavesIB(0), mBoxVB(0), mBoxIB(0), mGrassMapSRV(0), mWavesMapSRV(0), mBoxMapSRV(0),
	mWaterTexOffset(0.0f, 0.0f), mEyePosW(0.0f, 0.0f, 0.0f), mLandIndexCount(0), mRenderOptions(RenderOptions::TexturesAndFog),
	mTheta(1.3f*MathHelper::Pi), mPhi(0.4f*MathHelper::Pi), mRadius(80.0f)
{
	mMainWndCaption = L"Blend Demo";
	mEnable4xMsaa = false;

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);*/

	/*XMMATRIX boxScale = XMMatrixScaling(15.0f, 15.0f, 15.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(8.0f, 5.0f, -15.0f);
	XMStoreFloat4x4(&mBoxWorld, boxScale*boxOffset);

	XMMATRIX grassTexScale = XMMatrixScaling(5.0f, 5.0f, 0.0f);
	XMStoreFloat4x4(&mGrassTexTransform, grassTexScale);

	mDirLights[0].Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mDirLights[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	mDirLights[2].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	mLandMat.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mLandMat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mLandMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mWavesMat.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mWavesMat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	mWavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);

	mBoxMat.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mBoxMat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mBoxMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);*/
/*}

BlendApp::~BlendApp()
{
	md3dImmediateContext->ClearState();*/
	/*ReleaseCOM(mLandVB);
	ReleaseCOM(mLandIB);
	ReleaseCOM(mWavesVB);
	ReleaseCOM(mWavesIB);
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mGrassMapSRV);
	ReleaseCOM(mWavesMapSRV);
	ReleaseCOM(mBoxMapSRV);*/
	/*ReleaseCOM(mTriangleIB);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

bool BlendApp::Init()
{
	if (!D3DApp::Init())
		return false;

	BuildTriangles();
	BuildFX();
	BuildVertexLayout();

	mWaves.Init(160, 160, 1.0f, 0.03f, 5.0f, 0.3f);

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);*/

	/*HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice,
	L"Textures/grass.dds", 0, 0, &mGrassMapSRV, 0 ));

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice,
	L"Textures/water2.dds", 0, 0, &mWavesMapSRV, 0 ));

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice,
	L"Textures/WireFence.dds", 0, 0, &mBoxMapSRV, 0 ));*/

	//BuildLandGeometryBuffers();
	//BuildWaveGeometryBuffers();
	//BuildCrateGeometryBuffers();
	/*BuildTriangles();

	return true;
}

void BlendApp::OnResize()
{
	D3DApp::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void BlendApp::UpdateScene(float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	mEyePosW = XMFLOAT3(x, y, z);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);*/

	//
	// Every quarter second, generate a random wave.
	//
	/*static float t_base = 0.0f;
	if( (mTimer.TotalTime() - t_base) >= 0.1f )
	{
	t_base += 0.1f;

	DWORD i = 5 + rand() % (mWaves.RowCount()-10);
	DWORD j = 5 + rand() % (mWaves.ColumnCount()-10);

	float r = MathHelper::RandF(0.5f, 1.0f);

	mWaves.Disturb(i, j, r);
	}

	mWaves.Update(dt);

	//
	// Update the wave vertex buffer with the new solution.
	//

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(md3dImmediateContext->Map(mWavesVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	Vertex::Basic32* v = reinterpret_cast<Vertex::Basic32*>(mappedData.pData);
	for(UINT i = 0; i < mWaves.VertexCount(); ++i)
	{
	v[i].Pos    = mWaves[i];
	v[i].Normal = mWaves.Normal(i);

	// Derive tex-coords in [0,1] from position.
	v[i].Tex.x  = 0.5f + mWaves[i].x / mWaves.Width();
	v[i].Tex.y  = 0.5f - mWaves[i].z / mWaves.Depth();
	}

	md3dImmediateContext->Unmap(mWavesVB, 0);

	//
	// Animate water texture coordinates.
	//

	// Tile water texture.
	XMMATRIX wavesScale = XMMatrixScaling(5.0f, 5.0f, 0.0f);

	// Translate texture over time.
	mWaterTexOffset.y += 0.05f*dt;
	mWaterTexOffset.x += 0.1f*dt;
	XMMATRIX wavesOffset = XMMatrixTranslation(mWaterTexOffset.x, mWaterTexOffset.y, 0.0f);

	// Combine scale and translation.
	XMStoreFloat4x4(&mWaterTexTransform, wavesScale*wavesOffset);

	//
	// Switch the render mode based in key input.
	//
	if( GetAsyncKeyState('1') & 0x8000 )
	mRenderOptions = RenderOptions::Lighting;

	if( GetAsyncKeyState('2') & 0x8000 )
	mRenderOptions = RenderOptions::Textures;

	if( GetAsyncKeyState('3') & 0x8000 )
	mRenderOptions = RenderOptions::TexturesAndFog; */
/*}

void BlendApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world*view*proj;

	mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = mTech->GetPassByIndex(p);
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mTriangleVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mTriangleIB, DXGI_FORMAT_R32_UINT, 0);
		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);
		pass->Apply(0, md3dImmediateContext);

		md3dImmediateContext->Draw(3, 0);
		md3dImmediateContext->Draw(3, 3);
	}*/

	/*// Set per frame constants.
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mEyePosW);
	Effects::BasicFX->SetFogColor(Colors::Silver);
	Effects::BasicFX->SetFogStart(15.0f);
	Effects::BasicFX->SetFogRange(175.0f);

	ID3DX11EffectTechnique* boxTech;
	ID3DX11EffectTechnique* landAndWavesTech;

	switch(mRenderOptions)
	{
	case RenderOptions::Lighting:
	boxTech = Effects::BasicFX->Light3Tech;
	landAndWavesTech = Effects::BasicFX->Light3Tech;
	break;
	case RenderOptions::Textures:
	boxTech = Effects::BasicFX->Light3TexAlphaClipTech;
	landAndWavesTech = Effects::BasicFX->Light3TexTech;
	break;
	case RenderOptions::TexturesAndFog:
	boxTech = Effects::BasicFX->Light3TexAlphaClipFogTech;
	landAndWavesTech = Effects::BasicFX->Light3TexFogTech;
	break;
	}

	D3DX11_TECHNIQUE_DESC techDesc;

	//
	// Draw the box with alpha clipping.
	//

	//boxTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

	// Set per object constants.
	XMMATRIX world = XMLoadFloat4x4(&mBoxWorld);
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	XMMATRIX worldViewProj = world*view*proj;

	Effects::BasicFX->SetWorld(world);
	Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
	Effects::BasicFX->SetWorldViewProj(worldViewProj);
	Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
	Effects::BasicFX->SetMaterial(mBoxMat);
	Effects::BasicFX->SetDiffuseMap(mBoxMapSRV);

	md3dImmediateContext->RSSetState(RenderStates::NoCullRS);
	boxTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
	md3dImmediateContext->DrawIndexed(36, 0, 0);

	// Restore default render state.
	md3dImmediateContext->RSSetState(0);
	}

	//
	// Draw the hills and water with texture and fog (no alpha clipping needed).
	//

	landAndWavesTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
	//
	// Draw the hills.
	//
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mLandVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mLandIB, DXGI_FORMAT_R32_UINT, 0);

	// Set per object constants.
	XMMATRIX world = XMLoadFloat4x4(&mLandWorld);
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	XMMATRIX worldViewProj = world*view*proj;

	Effects::BasicFX->SetWorld(world);
	Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
	Effects::BasicFX->SetWorldViewProj(worldViewProj);
	Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mGrassTexTransform));
	Effects::BasicFX->SetMaterial(mLandMat);
	Effects::BasicFX->SetDiffuseMap(mGrassMapSRV);

	landAndWavesTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
	md3dImmediateContext->DrawIndexed(mLandIndexCount, 0, 0);

	//
	// Draw the waves.
	//
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mWavesVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mWavesIB, DXGI_FORMAT_R32_UINT, 0);

	// Set per object constants.
	world = XMLoadFloat4x4(&mWavesWorld);
	worldInvTranspose = MathHelper::InverseTranspose(world);
	worldViewProj = world*view*proj;

	Effects::BasicFX->SetWorld(world);
	Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
	Effects::BasicFX->SetWorldViewProj(worldViewProj);
	Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mWaterTexTransform));
	Effects::BasicFX->SetMaterial(mWavesMat);
	Effects::BasicFX->SetDiffuseMap(mWavesMapSRV);

	md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);
	landAndWavesTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
	md3dImmediateContext->DrawIndexed(3*mWaves.TriangleCount(), 0, 0);

	// Restore default blend state
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	}*/
/*
	HR(mSwapChain->Present(0, 0));
}

void BlendApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void BlendApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void BlendApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.01 unit in the scene.
		float dx = 0.1f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.1f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 20.0f, 500.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
void BlendApp::BuildTriangles()
{
	// Create vertex buffer
	std::vector<Vertex::Basic32> vertices =
	{
		{ XMFLOAT3(0, 1, 0), (const float*)&Colors::White },
		{ XMFLOAT3(-1.0f, -1.0f, 0), (const float*)&Colors::Black },
		{ XMFLOAT3(+1.0f, -1.0f, 0), (const float*)&Colors::Red },


		{ XMFLOAT3(3, 1, 0), (const float*)&Colors::White },
		{ XMFLOAT3(2.0f, -1.0f, 0), (const float*)&Colors::Black },
		{ XMFLOAT3(+4.0f, -1.0f, 0), (const float*)&Colors::Red }
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mTriangleVB));


	// Create the index buffer

	UINT indices[] = {
		// front face
		1, 0, 2,
		4, 3, 5
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * sizeof(indices);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mTriangleIB));
}



void BlendApp::BuildFX()
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"FX/color.fx", 0, 0, 0, "fx_5_0", shaderFlags,
		0, 0, &compiledShader, &compilationMsgs, 0);

	// compilationMsgs can store errors or warnings.
	if (compilationMsgs != 0)
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	// Even if there are no compilationMsgs, check to make sure there were no other errors.
	if (FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	}

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
		0, md3dDevice, &mFX));

	// Done with compiled shader.
	ReleaseCOM(compiledShader);

	mTech = mFX->GetTechniqueByName("ColorTech");
	sTech = mFX->GetTechniqueByName("ColorTech");
	aTech = mFX->GetTechniqueByName("ColorTech");

	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void BlendApp::BuildVertexLayout()
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}*/

/*float BlendApp::GetHillHeight(float x, float z)const
{
return 0.3f*( z*sinf(0.1f*x) + x*cosf(0.1f*z) );
}

XMFLOAT3 BlendApp::GetHillNormal(float x, float z)const
{
// n = (-df/dx, 1, -df/dz)
XMFLOAT3 n(
-0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z),
1.0f,
-0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));

XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
XMStoreFloat3(&n, unitNormal);

return n;
}

void BlendApp::BuildLandGeometryBuffers()
{
GeometryGenerator::MeshData grid;

GeometryGenerator geoGen;

geoGen.CreateGrid(160.0f, 160.0f, 50, 50, grid);

mLandIndexCount = grid.Indices.size();

//
// Extract the vertex elements we are interested and apply the height function to
// each vertex.
//

std::vector<Vertex::Basic32> vertices(grid.Vertices.size());
for(UINT i = 0; i < grid.Vertices.size(); ++i)
{
XMFLOAT3 p = grid.Vertices[i].Position;

p.y = GetHillHeight(p.x, p.z);

vertices[i].Pos    = p;
vertices[i].Normal = GetHillNormal(p.x, p.z);
vertices[i].Tex    = grid.Vertices[i].TexC;
}

D3D11_BUFFER_DESC vbd;
vbd.Usage = D3D11_USAGE_IMMUTABLE;
vbd.ByteWidth = sizeof(Vertex::Basic32) * grid.Vertices.size();
vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
vbd.CPUAccessFlags = 0;
vbd.MiscFlags = 0;
D3D11_SUBRESOURCE_DATA vinitData;
vinitData.pSysMem = &vertices[0];
HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mLandVB));

//
// Pack the indices of all the meshes into one index buffer.
//

D3D11_BUFFER_DESC ibd;
ibd.Usage = D3D11_USAGE_IMMUTABLE;
ibd.ByteWidth = sizeof(UINT) * mLandIndexCount;
ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
ibd.CPUAccessFlags = 0;
ibd.MiscFlags = 0;
D3D11_SUBRESOURCE_DATA iinitData;
iinitData.pSysMem = &grid.Indices[0];
HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mLandIB));
}

void BlendApp::BuildWaveGeometryBuffers()
{
// Create the vertex buffer.  Note that we allocate space only, as
// we will be updating the data every time step of the simulation.

D3D11_BUFFER_DESC vbd;
vbd.Usage = D3D11_USAGE_DYNAMIC;
vbd.ByteWidth = sizeof(Vertex::Basic32) * mWaves.VertexCount();
vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
vbd.MiscFlags = 0;
HR(md3dDevice->CreateBuffer(&vbd, 0, &mWavesVB));


// Create the index buffer.  The index buffer is fixed, so we only
// need to create and set once.

std::vector<UINT> indices(3*mWaves.TriangleCount()); // 3 indices per face

// Iterate over each quad.
UINT m = mWaves.RowCount();
UINT n = mWaves.ColumnCount();
int k = 0;
for(UINT i = 0; i < m-1; ++i)
{
for(DWORD j = 0; j < n-1; ++j)
{
indices[k]   = i*n+j;
indices[k+1] = i*n+j+1;
indices[k+2] = (i+1)*n+j;

indices[k+3] = (i+1)*n+j;
indices[k+4] = i*n+j+1;
indices[k+5] = (i+1)*n+j+1;

k += 6; // next quad
}
}

D3D11_BUFFER_DESC ibd;
ibd.Usage = D3D11_USAGE_IMMUTABLE;
ibd.ByteWidth = sizeof(UINT) * indices.size();
ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
ibd.CPUAccessFlags = 0;
ibd.MiscFlags = 0;
D3D11_SUBRESOURCE_DATA iinitData;
iinitData.pSysMem = &indices[0];
HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mWavesIB));
}

void BlendApp::BuildCrateGeometryBuffers()
{
GeometryGenerator::MeshData box;

GeometryGenerator geoGen;
geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);

//
// Extract the vertex elements we are interested in and pack the
// vertices of all the meshes into one vertex buffer.
//

std::vector<Vertex::Basic32> vertices(box.Vertices.size());

for(UINT i = 0; i < box.Vertices.size(); ++i)
{
vertices[i].Pos    = box.Vertices[i].Position;
vertices[i].Normal = box.Vertices[i].Normal;
vertices[i].Tex    = box.Vertices[i].TexC;
}

D3D11_BUFFER_DESC vbd;
vbd.Usage = D3D11_USAGE_IMMUTABLE;
vbd.ByteWidth = sizeof(Vertex::Basic32) * box.Vertices.size();
vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
vbd.CPUAccessFlags = 0;
vbd.MiscFlags = 0;
D3D11_SUBRESOURCE_DATA vinitData;
vinitData.pSysMem = &vertices[0];
HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));

//
// Pack the indices of all the meshes into one index buffer.
//

D3D11_BUFFER_DESC ibd;
ibd.Usage = D3D11_USAGE_IMMUTABLE;
ibd.ByteWidth = sizeof(UINT) * box.Indices.size();
ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
ibd.CPUAccessFlags = 0;
ibd.MiscFlags = 0;
D3D11_SUBRESOURCE_DATA iinitData;
iinitData.pSysMem = &box.Indices[0];
HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));
}*/

#include <iostream>
#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "RenderStates.h"
#include "Effects.h"

using namespace std;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class BoxApp : public D3DApp
{
public:
	BoxApp(HINSTANCE hInstance);
	~BoxApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildGeometryBuffers();
	void BuildSphereCylinderBuffers();
	void BuildFX();
	void BuildVertexLayout();

private:
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;

	ID3D11Buffer* mBoxVB2;
	ID3D11Buffer* mBoxIB2;

	ID3D11Buffer* mBoxVB3;
	ID3D11Buffer* mBoxIB3;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectTechnique* sTech;
	ID3DX11EffectTechnique* aTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;

	ID3D11InputLayout* mInputLayout;

	ID3D11RasterizerState* mWireframeRS;

	XMFLOAT4X4 mSphereWorld;
	XMFLOAT4X4 mCylWorld;
	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	int mSphereVertexOffset;
	int mCylinderVertexOffset;

	UINT mSphereIndexOffset;
	UINT mCylinderIndexOffset;

	UINT mSphereIndexCount;
	UINT mCylinderIndexCount;

	XMMATRIX sphereTranslate;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;

	float velocity = 1.0f;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	BoxApp theApp(hInstance);

	if (!theApp.Init())
		return 0;

	return theApp.Run();
}

BoxApp::BoxApp(HINSTANCE hInstance)
	: D3DApp(hInstance), mBoxVB(0), mBoxIB(0), mBoxVB2(0), mBoxIB2(0), mBoxVB3(0), mBoxIB3(0), mFX(0), mTech(0), sTech(0), aTech(0),
	mfxWorldViewProj(0), mInputLayout(0), mWireframeRS(0),
	mTheta(1.5f*MathHelper::Pi), mPhi(0.25f*MathHelper::Pi), mRadius(5.0f)
{
	mMainWndCaption = L"Box Demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mCylWorld, I);
	XMStoreFloat4x4(&mSphereWorld, I);
	XMStoreFloat4x4(&mWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);

	XMStoreFloat4x4(&mCylWorld, XMMatrixTranslation(+2.0f, 0.0f, 0.0f));

	XMStoreFloat4x4(&mSphereWorld, XMMatrixTranslation(+1.0f, 0.0f, +1.0f));

}

BoxApp::~BoxApp()
{
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mBoxVB2);
	ReleaseCOM(mBoxIB2);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
	ReleaseCOM(mWireframeRS);

	Effects::DestroyAll();
	RenderStates::DestroyAll();

}

bool BoxApp::Init()
{
	if (!D3DApp::Init())
		return false;


	Effects::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);
	BuildGeometryBuffers();
	//BuildSphereCylinderBuffers();
	BuildFX();
	BuildVertexLayout();


	/*
	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	HR(md3dDevice->CreateRasterizerState(&wireframeDesc, &mWireframeRS));
	*/

	return true;
}

void BoxApp::OnResize()
{
	D3DApp::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void BoxApp::UpdateScene(float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);

	XMMATRIX sphereScale = XMMatrixScaling(-5.0f*0.1f, 0.5f, 0.5f);
	XMMATRIX sphereOffset = XMMatrixTranslation(velocity, 1.0f, 0.0f);
	XMStoreFloat4x4(&mSphereWorld, XMMatrixTranslation(+1.0f, +1.0f, +1.0f));

}

void BoxApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//md3dImmediateContext->RSSetState(mWireframeRS);

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Set constants
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world*view*proj;

	mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	D3DX11_TECHNIQUE_DESC techDesc;

	//Draw first triangle to the back buffer
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = mTech->GetPassByIndex(p);
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);
		pass->Apply(0, md3dImmediateContext);

		// first two triangles.
		md3dImmediateContext->Draw(3, 0);
		md3dImmediateContext->Draw(3, 3);
	}


	//Multiplicative triangle blending to the back buffer
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB2, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mBoxIB2, DXGI_FORMAT_R32_UINT, 0);

	sTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = sTech->GetPassByIndex(p);

		sTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);

		pass->Apply(0, md3dImmediateContext);

		// 36 indices for the box.
		md3dImmediateContext->DrawIndexed(6, 0, 0);
		md3dImmediateContext->DrawIndexed(6, 3, 3);
	}


	//Additve triangle blending tothe back buffer
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB3, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mBoxIB3, DXGI_FORMAT_R32_UINT, 0);

	aTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = aTech->GetPassByIndex(p);

		aTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);

		pass->Apply(0, md3dImmediateContext);

		// 36 indices for the box.
		md3dImmediateContext->DrawIndexed(6, 0, 0);
		md3dImmediateContext->DrawIndexed(6, 3, 3);
	}


	//Quadvieuwer to the stencil buffer only
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = mTech->GetPassByIndex(p);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
		//md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		XMMATRIX world = XMLoadFloat4x4(&mWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());

		//Do not write to render target.
		md3dImmediateContext->OMSetBlendState(RenderStates::NoRenderTargetWritesBS, blendFactor, 0xffffffff);

		// Render visible mirror pixels to stencil buffer.
		// Do not write mirror depth to depth buffer at this point, otherwise it will occlude the reflection.
		md3dImmediateContext->OMSetDepthStencilState(RenderStates::MarkMirrorDSS, 1);

		//mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		//md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);
		pass->Apply(0, md3dImmediateContext);

		// 36 indices for the box.
		md3dImmediateContext->Draw(6, 6);

		// Restore states.
		md3dImmediateContext->OMSetDepthStencilState(0, 0);
		md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	}

	//
	// Draw the Triangles reflection.
	//
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = mTech->GetPassByIndex(p);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

		XMVECTOR mirrorPlane = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
		XMMATRIX R = XMMatrixReflect(mirrorPlane);
		XMMATRIX world = XMLoadFloat4x4(&mWorld) * R;
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		//Effects::BasicFX->SetMaterial(mSkullMat);


		// Cull clockwise triangles for reflection.
		md3dImmediateContext->RSSetState(RenderStates::CullClockwiseRS);

		// Only draw reflection into visible mirror pixels as marked by the stencil buffer. 
		md3dImmediateContext->OMSetDepthStencilState(RenderStates::DrawReflectionDSS, 1);
		pass->Apply(0, md3dImmediateContext);
		md3dImmediateContext->Draw(3, 0);
		//md3dImmediateContext->Draw(3, 3);

		// Restore default states.
		md3dImmediateContext->RSSetState(0);
		md3dImmediateContext->OMSetDepthStencilState(0, 0);
	}


	//draw the quadvieuwer back to te buffer
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = mTech->GetPassByIndex(p);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);


		/*
		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetMaterial(mMirrorMat);
		Effects::BasicFX->SetDiffuseMap(mMirrorDiffuseMapSRV);
		*/

		// Quadvieuwer
		md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);
		pass->Apply(0, md3dImmediateContext);
		md3dImmediateContext->Draw(6, 6);
	}
	HR(mSwapChain->Present(0, 0));
}

void BoxApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void BoxApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void BoxApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void BoxApp::BuildGeometryBuffers()
{

	// Create vertex buffer
	Vertex vertices[] =
	{
		//destination
		{ XMFLOAT3(+2.0f, -1.0f, +1.0f), (const float*)&Colors::Red },//+3
		{ XMFLOAT3(+3.0f, -1.0f, +1.0f), (const float*)&Colors::Red },//+1
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), (const float*)&Colors::Red },//+2

																	  //source
		{ XMFLOAT3(+2.0f, -1.0f, +0.0f), (const float*)&Colors::Blue },//+2
		{ XMFLOAT3(-0.0f, -1.0f, +0.0f), (const float*)&Colors::Blue },//+0
		{ XMFLOAT3(+1.0f, +1.0f, +0.0f), (const float*)&Colors::Blue },//+1

																	   //stencil layer
		{ XMFLOAT3(+3.0f, -2.0f, +2.0f), (const float*)&Colors::Green },
		{ XMFLOAT3(-3.0f, -2.0f, +2.0f), (const float*)&Colors::Green },
		{ XMFLOAT3(-3.0f, +1.0f, +2.0f), (const float*)&Colors::Green },
		{ XMFLOAT3(-3.0f, +1.0f, +2.0f), (const float*)&Colors::Green },
		{ XMFLOAT3(+3.0f, +1.0f, +2.0f), (const float*)&Colors::Green },
		{ XMFLOAT3(+3.0f, -2.0f, +2.0f), (const float*)&Colors::Green },


	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 12;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));

	// Create the index buffer
	UINT indices[] = {
		// front face
		0, 1, 2,
		3, 4, 5,

		//stencil face
		6, 7, 8,
		9, 6, 8,
		/*
		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
		*/
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 12;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));

	//
	//Multiplicative blending!
	//

	// Create vertex buffer
	Vertex vertices2[] =
	{
		//destination
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), (const float*)&Colors::Red },
		{ XMFLOAT3(-3.0f, -1.0f, +1.0f), (const float*)&Colors::Red },
		{ XMFLOAT3(-1.5f, +1.0f, +1.0f), (const float*)&Colors::Red },

		//source
		{ XMFLOAT3(-1.5f, -1.0f, +0.5f), (const float*)&Colors::Blue },
		{ XMFLOAT3(-3.5f, -1.0f, +0.5f), (const float*)&Colors::Blue },
		{ XMFLOAT3(-2.0f, +1.0f, +0.5f), (const float*)&Colors::Blue },
	};

	D3D11_BUFFER_DESC vbd2;
	vbd2.Usage = D3D11_USAGE_IMMUTABLE;
	vbd2.ByteWidth = sizeof(Vertex) * 6;
	vbd2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd2.CPUAccessFlags = 0;
	vbd2.MiscFlags = 0;
	vbd2.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData2;
	vinitData2.pSysMem = vertices2;
	HR(md3dDevice->CreateBuffer(&vbd2, &vinitData2, &mBoxVB2));

	// Create the index buffer
	UINT indices2[] = {
		// front face
		0, 1, 2,
		3, 4, 5,
	};

	D3D11_BUFFER_DESC ibd2;
	ibd2.Usage = D3D11_USAGE_IMMUTABLE;
	ibd2.ByteWidth = sizeof(UINT) * 6;
	ibd2.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd2.CPUAccessFlags = 0;
	ibd2.MiscFlags = 0;
	ibd2.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData2;
	iinitData2.pSysMem = indices2;
	HR(md3dDevice->CreateBuffer(&ibd2, &iinitData2, &mBoxIB2));

	//
	//Additive blending!
	//

	// Create vertex buffer
	Vertex vertices3[] =
	{
		//destination
		{ XMFLOAT3(+1.0f, -1.0f, +0.75f), (const float*)&Colors::Red },
		{ XMFLOAT3(-1.0f, -1.0f, +0.75f), (const float*)&Colors::Red },
		{ XMFLOAT3(+0.5f, +1.0f, +0.75f), (const float*)&Colors::Red },

		//source
		{ XMFLOAT3(+0.5f, -1.0f, +0.25f), (const float*)&Colors::Blue },
		{ XMFLOAT3(-1.5f, -1.0f, +0.25f), (const float*)&Colors::Blue },
		{ XMFLOAT3(-0.0f, +1.0f, +0.25f), (const float*)&Colors::Blue },
	};

	D3D11_BUFFER_DESC vbd3;
	vbd3.Usage = D3D11_USAGE_IMMUTABLE;
	vbd3.ByteWidth = sizeof(Vertex) * 6;
	vbd3.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd3.CPUAccessFlags = 0;
	vbd3.MiscFlags = 0;
	vbd3.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData3;
	vinitData3.pSysMem = vertices3;
	HR(md3dDevice->CreateBuffer(&vbd3, &vinitData3, &mBoxVB3));

	// Create the index buffer
	UINT indices3[] = {
		// front face
		0, 1, 2,
		3, 4, 5,
	};

	D3D11_BUFFER_DESC ibd3;
	ibd3.Usage = D3D11_USAGE_IMMUTABLE;
	ibd3.ByteWidth = sizeof(UINT) * 6;
	ibd3.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd3.CPUAccessFlags = 0;
	ibd3.MiscFlags = 0;
	ibd3.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData3;
	iinitData3.pSysMem = indices2;
	HR(md3dDevice->CreateBuffer(&ibd3, &iinitData3, &mBoxIB3));

}

void BoxApp::BuildSphereCylinderBuffers()
{
	//Assignment adding extra sphere and cylinder object.
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateSphere(0.5f, 20, 20, sphere);
	geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, cylinder);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mSphereVertexOffset = 0;
	mCylinderVertexOffset = sphere.Vertices.size();

	// Cache the index count of each object.
	mSphereIndexCount = sphere.Indices.size();
	mCylinderIndexCount = cylinder.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	mSphereIndexOffset = 0;
	mCylinderIndexOffset = mSphereIndexCount;

	UINT totalVertexCount =
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	UINT totalIndexCount =
		mSphereIndexCount +
		mCylinderIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex> vertices2(totalVertexCount);

	XMFLOAT4 black(129.0f, 54.0f, 227.0f, 1.0f);

	UINT k = 0;
	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices2[k].Pos = sphere.Vertices[i].Position;
		vertices2[k].Color = (const float*)&Colors::Blue;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices2[k].Pos = cylinder.Vertices[i].Position;
		vertices2[k].Color = (const float*)&Colors::Yellow;
	}

	D3D11_BUFFER_DESC vbd2;
	vbd2.Usage = D3D11_USAGE_DYNAMIC;
	vbd2.ByteWidth = sizeof(Vertex)* totalVertexCount;
	vbd2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//Assignment CPU Rendering
	vbd2.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData2;
	vinitData2.pSysMem = &vertices2[0];
	HR(md3dDevice->CreateBuffer(&vbd2, &vinitData2, &mBoxVB2));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices2;
	indices2.insert(indices2.end(), sphere.Indices.begin(), sphere.Indices.end());
	indices2.insert(indices2.end(), cylinder.Indices.begin(), cylinder.Indices.end());

	D3D11_BUFFER_DESC ibd2;
	ibd2.Usage = D3D11_USAGE_DYNAMIC;
	ibd2.ByteWidth = sizeof(UINT)* totalIndexCount;
	ibd2.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//Assignment CPU Rendering
	ibd2.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData2;
	iinitData2.pSysMem = &indices2[0];
	HR(md3dDevice->CreateBuffer(&ibd2, &iinitData2, &mBoxIB2));
}

void BoxApp::BuildFX()
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"FX/color.fx", 0, 0, 0, "fx_5_0", shaderFlags,
		0, 0, &compiledShader, &compilationMsgs, 0);

	// compilationMsgs can store errors or warnings.
	if (compilationMsgs != 0)
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	// Even if there are no compilationMsgs, check to make sure there were no other errors.
	if (FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	}

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
		0, md3dDevice, &mFX));

	// Done with compiled shader.
	ReleaseCOM(compiledShader);

	sTech = mFX->GetTechniqueByName("ColorTech");
	mTech = mFX->GetTechniqueByName("ColorTech");
	aTech = mFX->GetTechniqueByName("ColorTech");
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void BoxApp::BuildVertexLayout()
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	sTech->GetPassByIndex(0)->GetDesc(&passDesc);
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	aTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}


