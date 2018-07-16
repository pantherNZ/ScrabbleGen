// Self Include:
#include "ScrabbleWord.h"

// Local Includes:
#include "Game.h"
#include "System.h"

// Library Includes:

// Statics:

// Implementation:
CScrabbleWord::CScrabbleWord(std::string _strWord) : m_iLength(_strWord.length()), m_strWord(_strWord)
{
	m_vec2Position = D3DXVECTOR2(-99999.0f, -99999.0f);
	m_bHorizontal = true;
}

CScrabbleWord& CScrabbleWord::operator=(const CScrabbleWord& _word)
{
	m_iLength = _word.GetLength();
	m_strWord = _word.GetWord();
	m_bHorizontal = _word.IsHorizontal();
	m_vec2Position = _word.GetPosition(0);

	return(*this);
}

CScrabbleWord::~CScrabbleWord()
{
	
}

std::string CScrabbleWord::GetWord() const
{
	return(m_strWord);
}

void CScrabbleWord::SetPosition(D3DXVECTOR2 _vec2Pos)
{
	m_vec2Position = _vec2Pos;
}

D3DXVECTOR2 CScrabbleWord::GetPosition(int _i) const
{
	D3DXVECTOR2 vec2Offset(m_bHorizontal ? SQUARE_SIZE : 0, m_bHorizontal ? 0 : -SQUARE_SIZE);
	return(m_vec2Position + vec2Offset * (float)_i);
}

void CScrabbleWord::SetHorizontal(bool _b)
{
	m_bHorizontal = _b;
}

bool CScrabbleWord::IsHorizontal() const
{
	return(m_bHorizontal);
}

int CScrabbleWord::GetLength() const
{
	return(m_iLength);
}

void CScrabbleWord::Move(D3DXVECTOR2 _vec2Pos)
{
	m_vec2Position += _vec2Pos;
}