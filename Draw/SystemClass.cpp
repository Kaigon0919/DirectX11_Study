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

	//함수에 높이와 너비를 전달하기 전 0으로 초기화.
	screenWidth = 0;
	screenHeight = 0;

	// 윈도우즈 api를 사용하여 초기화. GrapgicsClass의 맴버함수.
	InitializeWindows(screenWidth, screenHeight);

	// input 객체를 생성합니다. 이 객체는 유저로부터 들어오는 키보드 입력을 처리하기 이해 사용합니다.
	m_Input = new InputClass;
	if (!m_Input)
		return false;

	// Input 객체를 초기화.
	m_Input->Initialize();

	m_Graphics = new GraphicsClass;
	//Graphics 객체를 생성합니다.이 객체는 이 어플리케이션의 모든 그래픽 요소를 그리는 일을 합니다.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
		return false;

	//지금까지 문제 없을시 정상 반환.
	return true;
}

void SystemClass::Shutdown()
{
	//Grapgics 객체를 반환합니다.
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	//Input 객체를 반환합니다.
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// 창을 종료시킵니다.
	ShutdownWindows();
	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	// 메세지 구조체를 초기화합니다.
	ZeroMemory(&msg, sizeof(MSG));

	//유저로부터 종료 메세지를 받을때 까지 루프.
	done = false;
	while (!done)
	{
		//윈도우 메세지를 처리.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 윈도우에서 종료를 요청하는 경우.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//그 외에는 Frame 함수를 처리.
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
		//키가 눌렸는지 확인.
	case WM_KEYDOWN:
		//눌렸다면 input객체에 기록.
		m_Input->KeyDown((unsigned int)wParam);
		return 0;
	case WM_KEYUP:
		//떼어졌다면 input객체에서 해제.
		m_Input->KeyUp((unsigned int)wParam);
		return 0;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

bool SystemClass::Frame()
{
	bool result;

	//유저가 Esc키를 눌러 어플리케이션을 종료하기를 원하는지 확인합니다.
	if (m_Input->IsKeyDown(VK_ESCAPE))
		return false;

	//grapgics객체의 작업을 처리합니다.
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

	//오류로 인한 초기화.
	posX = posY = 0;

	//외부 포인터를 이 객체로 설정합니다.
	ApplicationHandle = this;

	// 이 어플리케이션의 인스턴스를 가져옵니다.
	m_hinstance = GetModuleHandle(NULL);

	// 어플리케이션의 이름을 설정합니다.
	m_applicationName = L"JungHwan, DirectX11 Study";

	// 윈도우 클래스를 기본 설정으로 맞춥니다.
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

	//윈도우 클래스를 등록합니다.
	RegisterClassEx(&wc);

	//모니터 해상도를 얻어옵니다.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 풀스크린 모드 변수의 값에 따라 화면 설정 및 색상 32bit.
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
		//윈도우 모드라면 800 * 600 의 크기를 가지게 합니다.
		screenWidth = 800;
		screenHeight = 600;
		//창을 모니터의 중앙에 오도록 합니다.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//설정한 것을 가지고 창을 만들고 그 핸들을 가져옵니다.
	//m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_OVERLAPPEDWINDOW, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//윈도우를 화면에 표시하고 포커스르 줍니다.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//마우스 커서를 표시하지 않습니다.
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// 마우스 커서를 표시합니다.
	ShowCursor(true);

	// 풀스크린 모드를 빠져나올 때 디스플레이 설정을 바꿉니다.
	if (FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	//창을 제거합니다.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// 어플리케이션 인스턴스를 제거합니다.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// 이 클래스에 대한 외부 포인터 참조를 제거합니다.
	ApplicationHandle = NULL;

	return;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		//윈도우가 제거 되었는지 확인.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		//윈도우가 닫히는지 확인.
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	default:
		return ApplicationHandle->MessageHandler(hWnd, msg, wParam, lParam);
	}
}