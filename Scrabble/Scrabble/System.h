#pragma once

#ifndef _CSystem_H_
#define _CSystem_H_

// Definitions

// Library Includes:

// Local Includes:
#include "Utility.h"
#include "Game.h"
#include "Timer.h"

// Class Declaration:
class CSystem
{
	// Class functions
	public:	
		~CSystem();

		bool Initialise(HINSTANCE _hInstance);
		void Run();

		LRESULT CALLBACK MessageHandler(HWND _hwnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam);
		static BOOL CALLBACK EditDlgProc(HWND _hDlg, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam);

		// Singleton Methods
		static CSystem* GetInstance();
		static void DestroyInstance();

	protected:

	private:

		// Private constructor (singleton)
		CSystem();

		// Privately called functions
		void ExecuteFrame();
		void CreateRegisterWindow();
		void CreateSettingsDialog();
		void UpdateSettings();

	// Class members
	public:

	protected:

	private:
		// Singleton Instance
		static CSystem* s_pApplication;

		// System app data
		HWND m_hWnd;
		int m_iWidth;
		int m_iHeight;

		// Other data
		LPCWSTR m_applicationName;
		HINSTANCE m_hInstance;		

		// Classes used
		CGame* m_pGame;
		CTimer* m_pTimer;

		// Dialog data
		HWND m_hDlg;
};

// Windows callback definition
static LRESULT CALLBACK WndProc(HWND _hwnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam);

#endif // _CSystem_H_