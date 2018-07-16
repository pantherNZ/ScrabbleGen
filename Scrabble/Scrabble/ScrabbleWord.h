#pragma once

#ifndef _CSCRABBLEWORD_H_
#define _CSCRABBLEWORD_H_

// Library Includes:

// Local Includes:
#include "Utility.h"

// Class Declaration:
class CScrabbleWord
{
	// class functions
	public:
		CScrabbleWord(std::string _strWord);
		CScrabbleWord& operator=(const CScrabbleWord& _word);
		~CScrabbleWord();

		std::string GetWord() const;
		void Move(D3DXVECTOR2 _vec2Pos);

		void SetPosition(D3DXVECTOR2 _vec2Pos);
		D3DXVECTOR2 GetPosition(int _i) const;

		void SetHorizontal(bool _b);
		bool IsHorizontal() const;

		int GetLength() const;

	protected:

	private:

	// class members
	public:

	protected:

	private:
		int m_iLength;
		std::string m_strWord;
		bool m_bHorizontal;
		D3DXVECTOR2 m_vec2Position;
};		



#endif // _CSCRABBLEWORD_H_