#pragma once

#ifndef _CSPRITE_H_
#define _CSPRITE_H_

// Library Includes:

// Local Includes:
#include "Utility.h"

// Class Declaration:
class CSprite
{
	// class functions
	public:
		CSprite();
		~CSprite();

		static void CreateBuffers(ID3D10Device* _pDevice);
		static void ReleaseBuffers();
		static int GetFaceCount();
		static void ApplyRenderOptions();

		void Initialise(LPCWSTR _pstrTexture, D3DXVECTOR2 _vec2Position, float _fRotation, D3DXVECTOR2 _vec2Scale);
		void Initialise(LPCWSTR _pstrTexture, D3DXVECTOR2 _vec2Position, float _fRotation, D3DXVECTOR2 _vec2Scale, D3DXVECTOR4 _vec4TexCoord);
		void Initialise(LPCWSTR _pstrTexture, D3DXVECTOR2 _vec2Position, float _fRotation, D3DXVECTOR2 _vec2Scale, float _fAnimationInterval, D3DXVECTOR2 _vec2SpriteSize);
		void Initialise(LPCWSTR _pstrTexture, D3DXVECTOR2 _vec2Position, float _fRotation, D3DXVECTOR2 _vec2Scale, float _fAnimationInterval, D3DXVECTOR2 _vec2SpriteSize, std::vector<int> _vecEndings);

		void Process(float _fDelta);
		void LoopAnimation(bool _bLoop);
		void AnimationEnabled(bool _bEnabled);

		void SetAnimationRow(int _iRow, bool _bResetAnimation);
		int GetAnimationRow() const;

		void SetAnimationInterval(float _fInterval);

		ID3D10ShaderResourceView* GetTexture() const;
	
		void SetPosition(D3DXVECTOR2 _vec2Position);
		D3DXVECTOR2 GetPosition() const;

		void SetRotation(float _fRotation);
		float GetRotation() const;

		void SetScale(D3DXVECTOR2 _vec2Scale);
		D3DXVECTOR2 GetScale() const;

		void Move(D3DXVECTOR2 _vec2Position);
		void Rotate(float _fRotation);

		D3DXMATRIX GetTextureMatrix() const;
		D3DXMATRIX GetWorldMatrix() const;

		void SetTexCoord(D3DXVECTOR4 _vec4TexCoord);
		D3DXVECTOR4 GetTexCoord() const;
		void TranslateTexCoord(D3DXVECTOR4 _vec4TexCoord);

		int GetWidth() const;
		int GetHeight() const;

		void ResetScale();
		void Scale(D3DXVECTOR2 _vec2Scale);

	protected:

	private:
		void BuildWorldMatrix();
		void BuildTextureMatrix();

	// class members
	public:

	protected:
		D3DXVECTOR4 m_vec4TexCoord;
		ID3D10ShaderResourceView* m_pTexture;
		D3DXVECTOR2 m_vec2Position;

	private:
		// Buffers for rendering
		static ID3D10Device* m_spDevice;
		static ID3D10Buffer* m_spVertexBuffer;
		static ID3D10Buffer* m_spIndexBuffer; 
		static int m_siFaceCount;
	
		// Transformations
		D3DXVECTOR2 m_vec2ScaleOrig;
		D3DXVECTOR2 m_vec2Scale;
		float m_fRotation;
	
		// Matrices
		D3DXMATRIX m_matWorld;
		D3DXMATRIX m_matTexture;

		// Texture data
		int m_iTextureWidth;
		int m_iTextureHeight;
		
		// Animation data
		bool m_bAnimationLoop;
		bool m_bAnimationEnabled;
		float m_fAnimationInterval;
		int m_iAnimationRow;
		float m_fTimer;
		D3DXVECTOR2 m_vec2SpriteSize;
		D3DXVECTOR2 m_vec2TexIncr;
		std::vector<float> m_vecAnimationEndings;
		bool m_bDifferentEndings;
};

#endif // _CSPRITE_H_