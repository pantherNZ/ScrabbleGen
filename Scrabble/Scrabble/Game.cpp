// Self Include:
#include "Game.h"

// Local Includes:
#include "System.h"

// Library Includes:

// Statics:
ID3D10Device* CGame::m_pDevice = nullptr;
float CGame::m_fWidth = 0.0f;
float CGame::m_fHeight = 0.0f;
std::vector<CScrabbleWord> CGame::m_vecWords;
CSprite* CGame::m_pLetters[26];
CDirectX* CGame::m_pDirectXClass = nullptr;
CCamera* CGame::m_pCameraClass = nullptr;
CSpriteShader* CGame::m_pSpriteShader = nullptr;
ID3DX10Font* CGame::m_pFont = nullptr;
float CGame::m_fDelta = 0.0f;

int CGame::m_iWords = 20;  // Number of words the crossword will contain in a perfect solution
int CGame::m_iTimer = 5;   // (Seconds) Time program will contiue creating and rating puzzles before picking the best
int CGame::m_iDelay = 0;   // (Milliseconds) Time delay between adding a new word

// Threading statics
CThreadManager* CGame::m_pThreadManager;
CRITICAL_SECTION CGame::m_criticalSection;

// Implementation:
CGame::CGame() : m_hWnd(NULL)
{	

}

CGame::~CGame()
{
	// Terminate active threads
	while(m_pThreadManager->GetActiveThreadCount() != 0)
	{
		m_pThreadManager->Update(100);
	}

	//bool bStopped = m_pThreadManager->TerminateThreads();

	// Delete classes used
	DeleteOBJ(m_pDirectXClass);
	DeleteOBJ(m_pCameraClass);
	DeleteOBJ(m_pSpriteShader);
	DeleteOBJ(m_pThreadManager);

	// Release sprite buffers
	CSprite::ReleaseBuffers();

	for(int i = 0; i < 26; ++i)
	{
		DeleteOBJ(m_pLetters[i]);
	}

	// Delete critical section
	DeleteCriticalSection(&m_criticalSection);

	// Release font
	m_pFont->Release();
}

bool CGame::Initialise(HINSTANCE _hInstance, HWND _hWnd, int _iWidth, int _iHeight)
{
	m_fWidth = (float)_iWidth;
	m_fHeight = (float)_iHeight;
	m_hWnd = _hWnd;

	// Create dx10 class
	m_pDirectXClass = new CDirectX;

	// Initalise Dx10 class
	if(!m_pDirectXClass->Initialise(_hWnd, _iWidth, _iHeight))
	{
		MessageBox(NULL, L"Initialising DirectX10 Failed", L"Error", MB_OK);	
		return(false);
	}

	// Store created device
	m_pDevice = m_pDirectXClass->GetDevice(); 

	// Create camera class
	m_pCameraClass = new CCamera;

	// Initialise camera
	m_pCameraClass->Initialise(_iWidth, _iHeight);

	// Initialise sprite buffers
	CSprite::CreateBuffers(m_pDevice);
	CSprite::ApplyRenderOptions();

	// Create sprite shader class
	m_pSpriteShader = new CSpriteShader;

	// Initialise sprite shader class
	if(!m_pSpriteShader->Initialise(m_pDevice, _hWnd))
	{
		MessageBox(NULL, L"Initialising Sprite Shader Failed", L"Error", MB_OK);	
		return(false);
	}
	
	// Create Game Sprites
	for(int i = 0; i < 26; ++i)
	{
		m_pLetters[i] = new CSprite;
		std::wstringstream ss;
		ss << L"Data\\Sprites\\" << i << L".bmp";

		m_pLetters[i]->Initialise(ss.str().c_str(), D3DXVECTOR2(0.0f, 0.0f), 0.0f, D3DXVECTOR2(SCALE, SCALE));
	}

	// Initialise critical section interface
	InitializeCriticalSection(&m_criticalSection);

	// Create thread manager class
	m_pThreadManager = new CThreadManager;

	// Initialise class
	m_pThreadManager->Initialise();

	// Create font
	if(FAILED(D3DX10CreateFont(m_pDevice, 20, 10, 300, 0, false, 0, 0, 0, 0, L"Times New Roman", &m_pFont)))
	{
		MessageBox(NULL, L"Initialising DX10Font Failed", L"Error", MB_OK);	
		return(false);
	}

	// Create board
	m_pThreadManager->CreateThread(&GenerateLayout, nullptr);

	return(true);
}

unsigned int WINAPI CGame::GenerateLayout(void* _vpData)
{
	srand((unsigned int)(time(0)));
	//srand(345);

	// Enter critical section
	EnterCriticalSection(&m_criticalSection);

	// Update shared data of words
	m_vecWords.clear();

	// Leave critical section
	LeaveCriticalSection(&m_criticalSection);

	// Load all words
	std::vector<std::string> vecAllWords;
	std::ifstream fsInput;
	fsInput.open("Data\\Words.txt");

	while(!fsInput.eof())
	{
		std::string str;
		std::getline(fsInput, str);
		vecAllWords.push_back(str);
	}

	fsInput.close();
	
	// Pick a random number of these words to create layout from
	std::vector<std::string> vecNewWords;
	
	for(int i = 0; i < m_iWords; ++i)
	{
		int iRand = int(RandomFloat() * (vecAllWords.size() - 1));
		vecNewWords.push_back(vecAllWords[iRand]);
		vecAllWords.erase(vecAllWords.begin() + iRand);
	}

	// Order by length
	std::sort(vecNewWords.begin(), vecNewWords.end(), compareLen);
	
	std::vector<CScrabbleWord> vecNewWords2;

	for(unsigned int i = 0; i < vecNewWords.size(); ++i)
	{
		CScrabbleWord newWord(vecNewWords[i]);
		vecNewWords2.push_back(newWord);
	}
	
	// Create multiple crosswords and rate each one, keeping the best at the end
	double dTimer = (double)clock();
	int iBestBoardScore = 0;
	std::vector<CScrabbleWord> vecBestBoard;
	std::vector<CScrabbleWord> vecCurrentBoard;
	
	while((double)clock() <= (dTimer + m_iTimer * 1000.0f))
	{
		// For each word to be placed
		for(unsigned int i = 0; i < vecNewWords2.size(); ++i)
		{
			// If first word, place on top left of board
			if(i == 0)
			{
				vecNewWords2[i].SetPosition(D3DXVECTOR2(-vecNewWords2[i].GetLength() * SQUARE_SIZE * 0.5f, 0.0f));
				//CScrabbleWord newWord(vecNewWords2[i].GetWord());
				vecCurrentBoard.push_back(vecNewWords2[i]);

				// Enter critical section
				EnterCriticalSection(&m_criticalSection);

				// Update shared data of words
				m_vecWords.push_back(vecNewWords2[i]);

				// Leave critical section
				LeaveCriticalSection(&m_criticalSection);
			}
			else
			{ 
				int iBestScore = 0;
				D3DXVECTOR2 vec2Pos;
				bool bHorizontal;

				// For every word already placed
				for(unsigned int j = 0; j < vecCurrentBoard.size(); ++j)
				{			
					vecNewWords2[i].SetHorizontal(!vecCurrentBoard[j].IsHorizontal());

					// For every letter in new word
					for(int k = 0; k < vecNewWords2[i].GetLength(); ++k)
					{
						// For every letter in placed word
						for(int l = 0; l < vecCurrentBoard[j].GetLength(); ++l)
						{
							// If there is a common letter
							if(vecNewWords2[i].GetWord()[k] == vecCurrentBoard[j].GetWord()[l])
							{
								int iScore = 1;
								float fX = -(vecNewWords2[i].IsHorizontal() ? k * SQUARE_SIZE : 0);
								float fY = (vecNewWords2[i].IsHorizontal() ? 0 : k * SQUARE_SIZE);
								D3DXVECTOR2 vec2NewPos = vecCurrentBoard[j].GetPosition(l) + D3DXVECTOR2(fX, fY);
								vecNewWords2[i].SetPosition(vec2NewPos);

								// Loop through each word on the board to see if it breaks the board
								for(unsigned int m = 0; m < vecCurrentBoard.size(); ++m)
								{
									if(j != m)
									{
										//bool bConnected = IsConnected(&vecCurrentBoard[j], &vecCurrentBoard[m]);
										int iScoreNew = FindWordScore(&vecNewWords2[i], &vecCurrentBoard[m], false);
									
										if(iScoreNew == 0)
										{
											iScore = 0;
											break;
										}

										iScore += iScoreNew - 1;
									}
								}

								// If the board is fine, place the new word
								if(iScore != 0)		
								{
									if((iScore > iBestScore) || (iScore == iBestScore && RandomFloat() >= 0.7f))
									{
										iBestScore = iScore;
										vec2Pos = vec2NewPos;
										bHorizontal = !vecCurrentBoard[j].IsHorizontal();
									}
								}
							}
						}
					}
				}

				if(iBestScore != 0)
				{
					vecNewWords2[i].SetHorizontal(bHorizontal);
					vecNewWords2[i].SetPosition(vec2Pos);
					vecCurrentBoard.push_back(vecNewWords2[i]);

					// Enter critical section
					EnterCriticalSection(&m_criticalSection);

					m_vecWords = vecCurrentBoard;

					// Leave critical section
					LeaveCriticalSection(&m_criticalSection);

					Sleep(m_iDelay);
				}
			}
		}
		
		// Score board and then continue with creating new crossword
		int iScore = FindBoardScore(vecCurrentBoard);

		if(iScore > iBestBoardScore)
		{
			vecBestBoard.clear();
			vecBestBoard = vecCurrentBoard;
			iBestBoardScore = iScore;
		}

		vecCurrentBoard.clear();
	}
	
	// Enter critical section
	EnterCriticalSection(&m_criticalSection);

	m_vecWords = vecBestBoard;

	// Leave critical section
	LeaveCriticalSection(&m_criticalSection);
	
	return(0);
}

bool CGame::IsConnected(CScrabbleWord* _pWord1, CScrabbleWord* _pWord2)
{
	// For every letter in new word
	for(int j = 0; j < _pWord1->GetLength(); ++j)
	{
		// For every letter in placed word
		for(int k = 0; k < _pWord2->GetLength(); ++k)
		{
			if(_pWord1->GetPosition(j) == _pWord2->GetPosition(k) && _pWord1->GetWord()[j] == _pWord2->GetWord()[k])
			{
				return(true);
			}
		}
	}

	return(false);
}

int CGame::FindWordScore(CScrabbleWord* _pNewWord, CScrabbleWord* _pPlacedWord, bool _bConnected)
{
	int iScore = 1;
	bool bInside = false;

	// For every letter in new word
	for(int j = 0; j < _pNewWord->GetLength(); ++j)
	{
		// For every letter in placed word
		for(int k = 0; k < _pPlacedWord->GetLength(); ++k)
		{
			float fX = abs(_pNewWord->GetPosition(j).x - _pPlacedWord->GetPosition(k).x);
			float fY = abs(_pNewWord->GetPosition(j).y - _pPlacedWord->GetPosition(k).y);

			if(fX == 0.0f && fY == 0.0f)
			{
				if((_pNewWord->GetWord()[j] != _pPlacedWord->GetWord()[k]) || (_pNewWord->IsHorizontal() == _pPlacedWord->IsHorizontal()))
				{
					return(0);
				}
				
				iScore++;
			}
			else if(_bConnected) 
			{
				if((fX < SQUARE_SIZE) && (fY < SQUARE_SIZE))
				{
					return(0);
				}

				if((fX == SQUARE_SIZE) && (fY == SQUARE_SIZE))
				{
 					//iScore += 100;
				}
			}
			else if((fX <= SQUARE_SIZE * 2.0f) && (fY <= SQUARE_SIZE * 2.0f))
			{
				bInside = true;
			}
		}
	}

	if(bInside && iScore <= 1)
	{
		return(0);
	}

	return(iScore);
}

int CGame::FindBoardScore(std::vector<CScrabbleWord> _vecBoard)
{
	int iScore = 0;

	// Higher score for more words placed
	iScore += _vecBoard.size() * 2;
	
	D3DXVECTOR2 vec2Smallest = D3DXVECTOR2(99999.0f, 99999.0f);
	D3DXVECTOR2 vec2Largest = D3DXVECTOR2(-99999.0f, -99999.0f);

	// Higher score for denser board
	for(unsigned int i = 0; i < _vecBoard.size(); ++i)
	{
		D3DXVECTOR2 vec2StartPos = _vecBoard[i].GetPosition(0);
		D3DXVECTOR2 vec2EndPos = _vecBoard[i].GetPosition(_vecBoard[i].GetLength() - 1);

		if(vec2StartPos.x < vec2Smallest.x) vec2Smallest.x = vec2StartPos.x;
		if(vec2StartPos.y < vec2Smallest.y) vec2Smallest.y = vec2StartPos.y;
		if(vec2EndPos.x > vec2Largest.x) vec2Largest.x = vec2EndPos.x;
		if(vec2EndPos.y > vec2Largest.y) vec2Largest.y = vec2EndPos.y;
	}

	int iDiffX = int(vec2Largest.x - vec2Smallest.x);
	int iDiffY = int(vec2Largest.y - vec2Smallest.y);

	iScore += int(40000 / (iDiffX == 0 ? 1 : iDiffX));
	iScore += int(40000 / (iDiffY == 0 ? 1 : iDiffY));

	// Higher score for squarer board
	int iDiffSides = abs(iDiffY - iDiffX);
	iScore += int(100 / (iDiffSides == 0 ? 1 : iDiffSides));

	return(iScore);
}

void CGame::Process(float _fDelta)
{
	m_pCameraClass->Process(_fDelta);

	// PLUS / MINUS Camera Zooming
	int iZoom = ((GetAsyncKeyState(VK_OEM_MINUS) & 0x8000) + (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000) * -1) / 32768;

	if(iZoom)
	{
		if((iZoom < 0 && m_fWidth >= 50.0f) || (iZoom > 0 && m_fWidth <= 3000.0f))
		{
			float fVal = m_fWidth / m_fHeight;
			m_fWidth += iZoom * fVal * CAMERA_ZOOM_SPEED * _fDelta;
			m_fHeight += iZoom * CAMERA_ZOOM_SPEED * _fDelta;
			m_pCameraClass->Initialise(int(m_fWidth), int(m_fHeight));
		}
	}

	// Update thread manager
	if(m_pThreadManager->GetActiveThreadCount() != 0)
	{
		m_pThreadManager->Update(0);
	}

	// Space generates board
	if(GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		// Create board
		if(m_pThreadManager->GetActiveThreadCount() == 0)
		{
			m_pThreadManager->CreateThread(&GenerateLayout, nullptr);
		}
	}

	m_fDelta = _fDelta;
}

void CGame::Render()
{
	// Begin scene
	m_pDirectXClass->BeginScene(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	CSprite::ApplyRenderOptions();

	// Enter critical section
	EnterCriticalSection(&m_criticalSection);

	// Render letters
	for(unsigned int i = 0; i < m_vecWords.size(); ++i)
	{
		const std::string pcWord = m_vecWords[i].GetWord();

		for(int j = 0; j < m_vecWords[i].GetLength(); ++j)
		{
			int iVal = pcWord[j] - 'a';

			if(iVal >= 0 && iVal < 26)
			{
				m_pLetters[iVal]->SetPosition(m_vecWords[i].GetPosition(j));
				RenderSprite(m_pLetters[iVal]);
			}
		}
	}
	
	// Leave critical section
	LeaveCriticalSection(&m_criticalSection);

	std::wstringstream ss;
	ss << L"FPS:   " << int(1.0f / m_fDelta);
	RECT r = {10, 10, 0, 0};
	m_pFont->DrawText(nullptr, ss.str().c_str(), -1, &r, DT_NOCLIP, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));

	// End scene
	m_pDirectXClass->EndScene();
}

void CGame::RenderSprite(void* _pSprite)
{
	CSprite* pSprite = static_cast<CSprite*>(_pSprite);

	m_pSpriteShader->Render(pSprite->GetWorldMatrix(), m_pCameraClass->GetViewMatrix(), m_pCameraClass->GetProjectionMatrix(),
							pSprite->GetTextureMatrix(), pSprite->GetTexture(), pSprite->GetFaceCount());
}

void CGame::UpdateSettings(int _iWords, int _iTimer, int _iDelay)
{
	m_iWords = _iWords;
	m_iTimer = _iTimer;
	m_iDelay = _iDelay;
}

void CGame::GetSettings(int& _riWords, int& _riTimer, int& _riDelay)
{
	_riWords = m_iWords;
	_riTimer = m_iTimer;
	_riDelay = m_iDelay;
}