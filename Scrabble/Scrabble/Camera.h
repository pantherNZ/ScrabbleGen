#pragma once

#ifndef _CCAMERACLASS_H_
#define _CCAMERACLASS_H_

// Library Includes:

// Local Includes:
#include "Utility.h"

// Class Declaration:
class CCamera
{
	// class functions
	public:
		CCamera();
		~CCamera();

		void Initialise(int _iWidth, int _iHeight);
		bool Process(float _fDelta);

		// get methods
		D3DXVECTOR2 GetPosition() const;
		D3DXMATRIX GetViewMatrix() const;
		D3DXMATRIX GetProjectionMatrix() const;

	protected:

	private:

	// class members
	public:

	protected:

	private:
		float m_fWidth;
		float m_fHeight;
		float m_fAspectRatio;

		// vectors to create view matrix
		D3DXVECTOR2 m_vec2Position;
		float m_fRotation;

		// view matrix (camera class revolves around this)
		D3DXMATRIX m_matView;
		D3DXMATRIX m_matProjection;

		float m_fMoveSpeed;
};

#endif // _CCAMERACLASS_H_