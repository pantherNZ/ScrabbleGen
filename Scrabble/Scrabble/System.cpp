// Self Include:
#include "System.h"

// Local Includes:
#include "resource.h"

// Library Includes:

// Statics

// Implementation:
CSystem::CSystem() : m_pTimer(nullptr), m_iWidth(800), m_iHeight(600)
{

}

CSystem::~CSystem()
{
	// Remove the window.
	DestroyWindow(m_hWnd);
	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hInstance);

	// Delete used classes
	DeleteOBJ(m_pGame);
	DeleteOBJ(m_pTimer);
}

bool CSystem::Initialise(HINSTANCE _hInstance)
{
	// Basic program settings
	m_applicationName = L"Scrabble";
	m_hInstance = _hInstance;

	// Initialise the windows api
	CreateRegisterWindow();

	// Create timer 
	m_pTimer = new CTimer;

	// Reset timer
	m_pTimer->Reset();

	// Create game class
	m_pGame = new CGame;

	// Initialise game class
	bool bRet = m_pGame->Initialise(m_hInstance, m_hWnd, m_iWidth, m_iHeight);

	//if(bRet) m_pGame->Render();

	// Return true / false if Initialising game worked
	return(bRet);
}

void CSystem::CreateRegisterWindow()
{
	WNDCLASSEX wc;

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);
	
	// Register the window class.
	if(!RegisterClassEx(&wc))
	{
		MessageBox(0, L"Registering Windows Class Failed", 0, 0);
		PostQuitMessage(0);
	}

	// Determine the resolution of the clients desktop screen.
	m_iWidth = GetSystemMetrics(SM_CXSCREEN);
	m_iHeight = GetSystemMetrics(SM_CYSCREEN);

	// Create the window with the screen settings and get the handle to it.
	m_hWnd = CreateWindowEx(NULL, m_applicationName, m_applicationName, 
						WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
						CW_USEDEFAULT, CW_USEDEFAULT, m_iWidth, m_iHeight,
						NULL, NULL, m_hInstance, NULL);

	// check if creating the window failed
	if(!m_hWnd)
	{
		MessageBox(0, L"Creating Windows Class Failed", 0, 0);
		PostQuitMessage(0);
	}

	int xPos = (GetSystemMetrics(SM_CXSCREEN) / 2) - (m_iWidth / 2);
	int yPos = (GetSystemMetrics(SM_CYSCREEN) / 2) - (m_iHeight / 2);

	SetWindowPos( m_hWnd, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE );

	// Load menu
	HMENU hMenu = LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_MENU1));
	SetMenu(m_hWnd, hMenu);
}

void CSystem::Run()
{
	MSG msg;

	// Initialise the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	while(msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			ExecuteFrame();
		}
	}
}

void CSystem::ExecuteFrame()
{
	// Tick the timer	
	m_pTimer->Tick();

	// Process updates
	m_pGame->Process(m_pTimer->GetDeltaTime());

	// Render scene
	m_pGame->Render();
}

CSystem* CSystem::GetInstance()
{
	if (s_pApplication == nullptr)
	{
		s_pApplication = new CSystem;
	}

	return (s_pApplication);
}

void CSystem::DestroyInstance()
{
	DeleteOBJ(s_pApplication);
}

LRESULT CALLBACK CSystem::MessageHandler(HWND _hwnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam)
{
	switch(_uiMsg)
	{
		// Menu commands
		case WM_COMMAND:
		{
			switch(LOWORD(_wParam))
			{
				case ID_FILE_QUIT:
				{
					PostQuitMessage(0);
				}
				break;

				case ID_FILE_SETTINGS:
				{
					CreateSettingsDialog();
				}
				break;	
			}
		}
		break;

		default: break;
	}
	
	return(DefWindowProc(_hwnd, _uiMsg, _wParam, _lParam));
}

LRESULT CALLBACK WndProc(HWND _hwnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam)
{
	switch (_uiMsg)
	{	
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return(CSystem::GetInstance()->MessageHandler(_hwnd, _uiMsg, _wParam, _lParam));
		}
	}
}

BOOL CALLBACK CSystem::EditDlgProc(HWND _hDlg, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam)
{
	switch(_uiMsg)
	{
		case (WM_CLOSE):
		{
			EndDialog(_hDlg, 0);
			DestroyWindow(_hDlg);
			return(0);
		}

		case(WM_COMMAND):
		{
			switch(LOWORD(_wParam))
			{
				case(IDOK):
				{
					// Update settings
					CSystem::GetInstance()->UpdateSettings();

					// Fall through
				}

				case(IDCANCEL):
				{
					EndDialog(_hDlg, 0);
					DestroyWindow(_hDlg);
				}
				break;

				default: break;
			}
			
			return(0);
		}

		default:break;
	}

	return(FALSE);
}

void CSystem::CreateSettingsDialog()
{
	// Create the dialog for settings
	m_hDlg = CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), m_hWnd, CSystem::EditDlgProc);

	int iWords, iTimer, iDelay;
	m_pGame->GetSettings(iWords, iTimer, iDelay);

	// Set current number of words
	std::wostringstream outs;   
	outs << iWords;
	SetWindowText(GetDlgItem(m_hDlg, IDC_WORDS), outs.str().c_str());

	// Set timer value
	std::wostringstream outs2;   
	outs2 << iTimer;	
	SetWindowText(GetDlgItem(m_hDlg, IDC_TIMER), outs2.str().c_str());

	// Set delay value
	std::wostringstream outs3;   
	outs3 << iDelay;
	SetWindowText(GetDlgItem(m_hDlg, IDC_SLEEPTIME), outs3.str().c_str());
}

void CSystem::UpdateSettings()
{
	wchar_t msgText[256];

	// Get number of words from dialog & clamp the value
	GetWindowText(GetDlgItem(m_hDlg, IDC_WORDS), msgText, 256);
	int iWords = (int)wcstod(msgText, nullptr);
	iWords = max(1, min(10000, iWords));

	// Get timer value & clamp
	GetWindowText(GetDlgItem(m_hDlg, IDC_TIMER), msgText, 256);
	int iTimer = (int)wcstod(msgText, nullptr);
	iTimer = max(0, min(10000, iTimer));

	// Get delay value & clamp
	GetWindowText(GetDlgItem(m_hDlg, IDC_SLEEPTIME), msgText, 256);
	int iDelay = (int)wcstod(msgText, nullptr);
	iDelay = max(0, min(1000, iDelay));

	m_pGame->UpdateSettings(iWords, iTimer, iDelay);
}