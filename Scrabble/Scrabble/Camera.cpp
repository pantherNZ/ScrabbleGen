// Self Include:
#include "Camera.h"

// Local Includes:

// Library Includes:

// Implementation:
CCamera::CCamera() : m_fMoveSpeed(2.0f), m_fWidth(0.0f), m_fHeight(0.0f), m_fRotation(0.0f)
{
	m_vec2Position = D3DXVECTOR2(0.0f, 0.0f);
}

CCamera::~CCamera()
{
	
}

void CCamera::Initialise(int _iWidth, int _iHeight)
{
	m_fWidth = (float)_iWidth;
	m_fHeight = (float)_iHeight;
	m_fAspectRatio = m_fWidth / m_fHeight;

	m_matView = *D3DXMatrixTranslation(&m_matView, m_vec2Position.x, m_vec2Position.y, 0.0f);
	//D3DXMatrixIdentity(&m_matView);
	D3DXMatrixOrthoLH(&m_matProjection, m_fWidth, m_fHeight, -1.0f, 1.0f);
}

bool CCamera::Process(float _fDelta)
{
	// WASD Camera Movement
	int iMoveUp = ((GetAsyncKeyState(0x53) & 0x8000) + (GetAsyncKeyState(0x57) & 0x8000) * -1) / 32768;
	int iMoveAcross = ((GetAsyncKeyState(0x41) & 0x8000) + (GetAsyncKeyState(0x44) & 0x8000) * -1) / 32768;

	if(iMoveUp || iMoveAcross)
	{
		m_vec2Position += (D3DXVECTOR2(iMoveAcross * CAMERA_MOVE_SPEED * _fDelta, iMoveUp * CAMERA_MOVE_SPEED * _fDelta));
		m_matView = *D3DXMatrixTranslation(&m_matView, m_vec2Position.x, m_vec2Position.y, 0.0f);
		return(true);
	}

	return(false);
}

D3DXVECTOR2 CCamera::GetPosition() const
{
	return(m_vec2Position);
}

D3DXMATRIX CCamera::GetViewMatrix() const
{
	return(m_matView);
}

D3DXMATRIX CCamera::GetProjectionMatrix() const
{
	return(m_matProjection);
}