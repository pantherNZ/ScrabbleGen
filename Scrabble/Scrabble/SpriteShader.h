#pragma once

#ifndef _SPRITESHADERCLASS_H_
#define _SPRITESHADERCLASS_H_

// Library Includes:

// Local Includes:
#include "Utility.h"

// Class Declaration:
class CSpriteShader
{
	// class functions
	public:
		CSpriteShader();
		~CSpriteShader();

		bool Initialise(ID3D10Device* _pDevice, HWND _hWnd);
		void Render(D3DXMATRIX _worldMatrix, D3DXMATRIX _viewMatrix, D3DXMATRIX _projMatrix, D3DXMATRIX _texMatrix, ID3D10ShaderResourceView* _pTexture, int _iNumFaces);
		
	protected:

	private:
		void OutputShaderErrorMessage(ID3D10Blob* _pErrorMessage, HWND _hWnd);

	// class members
	public:

	protected:

	private:
		// store copy of device
		ID3D10Device* m_pDevice;

		// fx vars
		ID3D10Effect* m_pEffect;
		ID3D10EffectTechnique* m_pTechnique;
		ID3D10InputLayout* m_pLayout;

		ID3D10EffectMatrixVariable* m_pWorldMatrix;
		ID3D10EffectMatrixVariable* m_pFinalMatrix;
		ID3D10EffectMatrixVariable* m_pTextureMatrix;

		ID3D10EffectShaderResourceVariable* m_pTexture;
};

#endif  // _SPRITESHADERCLASS_H_