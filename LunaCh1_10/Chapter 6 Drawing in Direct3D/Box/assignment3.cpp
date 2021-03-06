//***************************************************************************************
// BoxDemo.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates rendering a colored box.
//
// Controls:
//		Hold the left mouse button down and move the mouse to rotate.
//      Hold the right mouse button down to zoom in and out.
//
//***************************************************************************************

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "MathHelper.h"
#include "GeometryGenerator.h"

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
	void BuildFX();
	void BuildVertexLayout();

private:
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;

	ID3D11InputLayout* mInputLayout;
	ID3D11RasterizerState* mWireframeRS;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	float mTheta;
	float mPhi;
	float mRadius;

	int vertexCount;
	int indexCount;
	bool wframe = false;
	int sliceCount = 10;
	float cylinderHeight = 1.0f;
	float cylinderRadius = 1.0f;

	POINT mLastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	BoxApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}
 

BoxApp::BoxApp(HINSTANCE hInstance)
: D3DApp(hInstance), mBoxVB(0), mBoxIB(0), mFX(0), mTech(0),
  mfxWorldViewProj(0), mInputLayout(0), mWireframeRS(0),
  mTheta(1.5f*MathHelper::Pi), mPhi(0.25f*MathHelper::Pi), mRadius(5.0f)
{
	mMainWndCaption = L"Box Demo";
	
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
}

BoxApp::~BoxApp()
{
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
	ReleaseCOM(mWireframeRS);
}

bool BoxApp::Init()
{
	if(!D3DApp::Init())
		return false;

	BuildGeometryBuffers();
	BuildFX();
	BuildVertexLayout();

	//define wireframe
	if (wframe) {
		D3D11_RASTERIZER_DESC wireframeDesc;
		ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
		wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
		wireframeDesc.CullMode = D3D11_CULL_BACK;
		wireframeDesc.FrontCounterClockwise = false;
		wireframeDesc.DepthClipEnable = true;
		HR(md3dDevice->CreateRasterizerState(&wireframeDesc, &mWireframeRS));
	}

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
	XMVECTOR pos    = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

float timer = 0;
void BoxApp::DrawScene()
{
	timer--;;
	if (timer <= 0) {
		if (GetAsyncKeyState('Q') && sliceCount > 3) {
			sliceCount--;
			timer = 200;
			BuildGeometryBuffers();
		}
		else if (GetAsyncKeyState('E') && sliceCount < 500) {
			sliceCount++;
			timer = 200;
			BuildGeometryBuffers();
		}
		else if (GetAsyncKeyState('W') && cylinderHeight < 2) {
			cylinderHeight += 0.2f;
			timer = 200;
			BuildGeometryBuffers();
		}
		else if (GetAsyncKeyState('S') && cylinderHeight > 0.4f) {
			cylinderHeight -= 0.2f;
			timer = 200;
			BuildGeometryBuffers();
		}
		else if (GetAsyncKeyState('A') && cylinderRadius > 0.4f) {
			cylinderRadius -= 0.2f;
			timer = 200;
			BuildGeometryBuffers();
		}
		else if (GetAsyncKeyState('D') && cylinderRadius < 2) {
			cylinderRadius += 0.2f;
			timer = 200;
			BuildGeometryBuffers();
		}
	}

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(mInputLayout);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//apply wireframe resterizer state
	if (wframe) {
		md3dImmediateContext->RSSetState(mWireframeRS);
	}

	UINT stride = sizeof(Vertex);
    UINT offset = 0;
    md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

	// Set constants
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view  = XMLoadFloat4x4(&mView);
	XMMATRIX proj  = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world*view*proj;

	mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

    D3DX11_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
        
		// indices for the mesh.
		md3dImmediateContext->DrawIndexed(indexCount, 0, 0);
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
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi   += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi-0.1f);
	}
	else if( (btnState & MK_RBUTTON) != 0 )
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

void GenerateCylinderCap(GeometryGenerator::MeshData &meshData, int sliceCount, float radius, float yPos) 
{
	float theta = 2.0f*XM_PI / sliceCount;

	for (size_t i = 0; i < sliceCount + 1; i++)
	{
		GeometryGenerator::Vertex vertex;
		if (i == 0) {
			vertex.Position = XMFLOAT3(0.0f, yPos, 0.0f);
		}
		else {
			vertex.Position = XMFLOAT3(radius * cosf(i*theta), yPos, radius * sinf(i*theta));
		}

		meshData.Vertices.push_back(vertex);
	}
	

	int lastVertex = (meshData.Vertices.size() - sliceCount);
	for (size_t i = 0; i < sliceCount; i++)
	{
		meshData.Indices.push_back(lastVertex);
		meshData.Indices.push_back((meshData.Vertices.size() - sliceCount) - 1);
		if (i == sliceCount - 1) {
			meshData.Indices.push_back((meshData.Vertices.size() - sliceCount));
		}
		else {
			meshData.Indices.push_back(lastVertex + 1);
		}
		lastVertex++;
	}
}

void GenerateCylinder(GeometryGenerator::MeshData &meshData, int sliceCount, float radius, float height, float yPos)
{
	GenerateCylinderCap(meshData, sliceCount, radius, yPos);
	GenerateCylinderCap(meshData, sliceCount, radius, height);

	int lastVertex = 1;
	for (size_t i = 0; i < sliceCount; i++)
	{
		meshData.Indices.push_back(lastVertex);
		meshData.Indices.push_back(lastVertex + sliceCount + 1);
		if (i == sliceCount - 1) {
			meshData.Indices.push_back(1);
		}
		else {
			meshData.Indices.push_back(lastVertex + 1);
		}
		lastVertex++;
	}

	lastVertex = sliceCount + 2;
	for (size_t i = 0; i < sliceCount; i++)
	{
		meshData.Indices.push_back(lastVertex);
		if (i == sliceCount - 1) {
			meshData.Indices.push_back(sliceCount + 2);
		}
		else {
			meshData.Indices.push_back(lastVertex + 1);
		}
		if (i == sliceCount - 1) {
			meshData.Indices.push_back(lastVertex - (sliceCount*2));
		}
		else {
			meshData.Indices.push_back(lastVertex - sliceCount);
		}
		lastVertex++;
	}
}


void BoxApp::BuildGeometryBuffers()
{
	//create mesh object
	GeometryGenerator::MeshData myMesh;
	GenerateCylinder(myMesh, sliceCount, cylinderRadius, cylinderHeight, 0.0f);
	vertexCount = myMesh.Vertices.size();
	//set vertex settings
	std::vector<Vertex> vertices(vertexCount);
	UINT k = 0;
	for (size_t i = 0; i < vertexCount; ++i, ++k)
	{
		vertices[k].Pos = myMesh.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4((i/(float)vertexCount), (i / (float)vertexCount), (i / (float)vertexCount), 1.0f);
	}
	
	//apply vertices
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex) * vertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));

	//create indices
	std::vector<UINT> indices;
	indices.insert(indices.end(), myMesh.Indices.begin(), myMesh.Indices.end());
	indexCount = myMesh.Indices.size();

	//apply indices
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));	
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
	if( compilationMsgs != 0 )
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	// Even if there are no compilationMsgs, check to make sure there were no other errors.
	if(FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	}

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 
		0, md3dDevice, &mFX));

	// Done with compiled shader.
	ReleaseCOM(compiledShader);

	mTech    = mFX->GetTechniqueByName("ColorTech");
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void BoxApp::BuildVertexLayout()
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3DX11_PASS_DESC passDesc;
    mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout));
}
