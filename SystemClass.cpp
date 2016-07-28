#include "SystemClass.h"

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
}

SystemClass::SystemClass(const SystemClass & ref)
{}

SystemClass::~SystemClass()
{}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	//�Լ��� ���̿� �ʺ� �����ϱ� �� 0���� �ʱ�ȭ.
	screenWidth = 0;
	screenHeight = 0;

	// �������� api�� ����Ͽ� �ʱ�ȭ. GrapgicsClass�� �ɹ��Լ�.
	InitializeWindows(screenWidth, screenHeight);

	// input ��ü�� �����մϴ�. �� ��ü�� �����κ��� ������ Ű���� �Է��� ó���ϱ� ���� ����մϴ�.
	m_Input = new InputClass;
	if (!m_Input)
		return false;

	// Input ��ü�� �ʱ�ȭ.
	m_Input->Initialize();

	m_Graphics = new GraphicsClass;
	//Graphics ��ü�� �����մϴ�.�� ��ü�� �� ���ø����̼��� ��� �׷��� ��Ҹ� �׸��� ���� �մϴ�.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
		return false;

	//���ݱ��� ���� ������ ���� ��ȯ.
	return true;
}

void SystemClass::Shutdown()
{
	//Grapgics ��ü�� ��ȯ�մϴ�.
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	//Input ��ü�� ��ȯ�մϴ�.
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// â�� �����ŵ�ϴ�.
	ShutdownWindows();
	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	// �޼��� ����ü�� �ʱ�ȭ�մϴ�.
	ZeroMemory(&msg, sizeof(MSG));

	//�����κ��� ���� �޼����� ������ ���� ����.
	done = false;
	while (!done)
	{
		//������ �޼����� ó��.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// �����쿡�� ���Ḧ ��û�ϴ� ���.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//�� �ܿ��� Frame �Լ��� ó��.
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}
	return;
}

LRESULT SystemClass::MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		//Ű�� ���ȴ��� Ȯ��.
	case WM_KEYDOWN:
		//���ȴٸ� input��ü�� ���.
		m_Input->KeyDown((unsigned int)wParam);
		return 0;
	case WM_KEYUP:
		//�������ٸ� input��ü���� ����.
		m_Input->KeyUp((unsigned int)wParam);
		return 0;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

bool SystemClass::Frame()
{
	bool result;

	//������ EscŰ�� ���� ���ø����̼��� �����ϱ⸦ ���ϴ��� Ȯ���մϴ�.
	if (m_Input->IsKeyDown(VK_ESCAPE))
		return false;

	//grapgics��ü�� �۾��� ó���մϴ�.
	result = m_Graphics->Frame();
	if (!result)
		return false;

	return true;
}

void SystemClass::InitializeWindows(int & screenWidth, int & screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//������ ���� �ʱ�ȭ.
	posX = posY = 0;

	//�ܺ� �����͸� �� ��ü�� �����մϴ�.
	ApplicationHandle = this;

	// �� ���ø����̼��� �ν��Ͻ��� �����ɴϴ�.
	m_hinstance = GetModuleHandle(NULL);

	// ���ø����̼��� �̸��� �����մϴ�.
	m_applicationName = L"JungHwan, DirectX11 Study";

	// ������ Ŭ������ �⺻ �������� ����ϴ�.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//������ Ŭ������ ����մϴ�.
	RegisterClassEx(&wc);

	//����� �ػ󵵸� ���ɴϴ�.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Ǯ��ũ�� ��� ������ ���� ���� ȭ�� ���� �� ���� 32bit.
	if (FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	}
	else
	{
		//������ ����� 800 * 600 �� ũ�⸦ ������ �մϴ�.
		screenWidth = 800;
		screenHeight = 600;
		//â�� ������� �߾ӿ� ������ �մϴ�.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}
	
	//������ ���� ������ â�� ����� �� �ڵ��� �����ɴϴ�.
	//m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_OVERLAPPEDWINDOW, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//�����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� �ݴϴ�.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//���콺 Ŀ���� ǥ������ �ʽ��ϴ�.
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// ���콺 Ŀ���� ǥ���մϴ�.
	ShowCursor(true);

	// Ǯ��ũ�� ��带 �������� �� ���÷��� ������ �ٲߴϴ�.
	if (FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	//â�� �����մϴ�.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// ���ø����̼� �ν��Ͻ��� �����մϴ�.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// �� Ŭ������ ���� �ܺ� ������ ������ �����մϴ�.
	ApplicationHandle = NULL;

	return;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		//�����찡 ���� �Ǿ����� Ȯ��.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	//�����찡 �������� Ȯ��.
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	default:
		return ApplicationHandle->MessageHandler(hWnd, msg, wParam, lParam);
	}
}
