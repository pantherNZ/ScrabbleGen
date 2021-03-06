#pragma once

#ifndef _CDIRECTXCLASS_H_
#define _CDIRECTXCLASS_H_

// Library Includes:

// Local Includes:
#include "Utility.h"

// Class Declaration:
class CDirectX
{
	// class functions
	public:
		CDirectX();
		~CDirectX();

		bool Initialise(HWND _hWnd, int _iWidth, int _iHeight);
	
		void BeginScene(D3DXCOLOR _cClearColour);
		void EndScene();

		ID3D10Device* GetDevice();

	protected:

	private:


	// class members
	public:

	protected:

	private:
		// window members
		int m_iWidth;
		int m_iHeight;
		HWND m_hWnd;

		// DirectX members
		ID3D10Device*    m_pD3DDevice;
		IDXGISwapChain*  m_pSwapChain;
		ID3D10Texture2D* m_pDepthStencilBuffer;
		ID3D10RenderTargetView* m_pRenderTargetView;
		ID3D10DepthStencilView* m_pDepthStencilView;
		ID3D10RasterizerState* m_pRasterState;
};

#endif // _CDIRECTXCLASS_H_