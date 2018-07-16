
#pragma once

#ifndef _CGAME_H_
#define _CGAME_H_

// Library Includes:

// Local Includes:
#include "Utility.h"
#include "DirectX.h"
#include "Sprite.h"
#include "Camera.h"
#include "SpriteShader.h"
#include "ScrabbleWord.h"
#include "ThreadManager.h"

// Class Declaration:
class CGame
{
	// Class Functions
	public:
		CGame();
		~CGame();

		bool Initialise(HINSTANCE _hInstance, HWND _hWnd, int _iWidth, int _iHeight);
		void Process(float _fDelta);
		static void Render();
		static void RenderSprite(void* _pSprite);
		static void UpdateSettings(int _iWords, int _iTimer, int _iDelay);
		static void GetSettings(int& _riWords, int& _riTimer, int& _riDelay);

	protected:

	private:
		//void GenerateLayout();
		static unsigned int WINAPI GenerateLayout(void* _vpData);
		static bool IsConnected(CScrabbleWord* _pWord1, CScrabbleWord* _pWord2);
		static int FindWordScore(CScrabbleWord* _pNewWord, CScrabbleWord* _pPlacedWord, bool _bConnected);
		static int FindBoardScore(std::vector<CScrabbleWord> _vecBoard);

	// Class Members
	public:

	protected:

	private:
		static ID3D10Device* m_pDevice;

		static float m_fWidth;
		static float m_fHeight;
		HWND m_hWnd;

		// classes used
		static CDirectX* m_pDirectXClass;
		static CCamera* m_pCameraClass;
		static CSpriteShader* m_pSpriteShader;
		static ID3DX10Font* m_pFont;

		// Game objects
		static CSprite* m_pLetters[26];
		static std::vector<CScrabbleWord> m_vecWords;
		static float m_fDelta;

		// Threading data
		static CThreadManager* m_pThreadManager;
		static CRITICAL_SECTION m_criticalSection;

		// Settings
		static int m_iWords;
		static int m_iTimer;
		static int m_iDelay;
};

#endif // _CGAME_H_