#include "InitDirect3D.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// ����� ������ ��� ������� �޸� ���� ����� �Ҵ�.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	
	InitDirect3DApp theApp(hInstance);

	if (!theApp.Init())
		return 0;
	return theApp.Run();
}

InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance) : D3DApp(hInstance)
{}

InitDirect3DApp::~InitDirect3DApp()
{}

bool InitDirect3DApp::init()
{
	if (!D3DApp::Init())
		return false;
	return true;
}

void InitDirect3DApp::OnResize()
{
	D3DApp::OnResize();
}

void InitDirect3DApp::UpdateScene(float dt)
{
}

void InitDirect3DApp::DrawScene()
{
	assert(md3dImmediateContext);
	assert(mSwapChain);

	//�ĸ� ���۸� �Ķ������� �����. Colors::Blue�� d3dUtil.h.�� ����.
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//�ĸ� ���۸� ȭ�鿡 �����Ѵ�.
	HR(mSwapChain->Present(0, 0));
}
