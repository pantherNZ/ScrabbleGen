// Self Include:
#include "Sprite.h"

// Local Includes:

// Library Includes:

// Static variables
ID3D10Device* CSprite::m_spDevice = nullptr;
ID3D10Buffer* CSprite::m_spIndexBuffer = nullptr;
ID3D10Buffer* CSprite::m_spVertexBuffer = nullptr;
int CSprite::m_siFaceCount = 2;

// Implementation:
CSprite::CSprite() : m_pTexture(nullptr), m_vec2SpriteSize(D3DXVECTOR2(0, 0)), m_iTextureWidth(0), m_iTextureHeight(0), m_fAnimationInterval(0.0f), m_bAnimationEnabled(false), 
					m_fTimer(0.0f), m_iAnimationRow(0), m_bAnimationLoop(true), m_bDifferentEndings(false)
{										  
	D3DXMatrixIdentity(&m_matWorld);
	m_vec2TexIncr = D3DXVECTOR2(0.0f, 0.0f);
	m_vec4TexCoord = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
	m_vec2Position = D3DXVECTOR2(0.0f, 0.0f);
	m_vec2Scale    = D3DXVECTOR2(0.0f, 0.0f);
	m_fRotation	   = 0.0f;
}

CSprite::~CSprite()
{
	ReleaseCOM(m_pTexture);
}

void CSprite::Initialise(LPCWSTR _pstrTexture, D3DXVECTOR2 _vec2Position, float _fRotation, D3DXVECTOR2 _vec2Scale)
{
	Initialise(_pstrTexture, _vec2Position, _fRotation, _vec2Scale, D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));
}

void CSprite::Initialise(LPCWSTR _pstrTexture, D3DXVECTOR2 _vec2Position, float _fRotation, D3DXVECTOR2 _vec2Scale, D3DXVECTOR4 _vec4TexCoord)
{
	SetPosition(_vec2Position);
	SetScale(_vec2Scale);
	m_fRotation = _fRotation;
	m_vec4TexCoord = _vec4TexCoord;

	if( m_pTexture != nullptr )
	{
		m_pTexture->Release();
	}

	if(FAILED(D3DX10CreateShaderResourceViewFromFile(m_spDevice, _pstrTexture, 0, 0, &m_pTexture, 0)))
	{
		MessageBox(NULL, L"ERROR: Initialising Sprite Texture Failed", _pstrTexture, MB_OK);	
		PostQuitMessage(0);
		return;
	}

	ID3D10Resource* pResource;
	m_pTexture->GetResource(&pResource);

	D3D10_TEXTURE2D_DESC desc2D;
	((ID3D10Texture2D*)pResource)->GetDesc(&desc2D);

	m_iTextureWidth = desc2D.Width;
	m_iTextureHeight = desc2D.Height;

	m_vec2Scale.x *= m_iTextureWidth;
	m_vec2Scale.y *= m_iTextureHeight;
   
	m_bAnimationEnabled = false;

	pResource->Release();

	BuildWorldMatrix();
	BuildTextureMatrix();

	m_vec2ScaleOrig = m_vec2Scale;
}

void CSprite::Initialise(LPCWSTR _pstrTexture, D3DXVECTOR2 _vec2Position, float _fRotation, D3DXVECTOR2 _vec2Scale, float _fAnimationInterval, D3DXVECTOR2 _vec2SpriteSize)
{
	Initialise(_pstrTexture, _vec2Position, _fRotation, _vec2Scale);

	m_bAnimationEnabled = true;
	m_fAnimationInterval = _fAnimationInterval / 1000.0f;
	m_vec2SpriteSize = _vec2SpriteSize;

	m_vec2TexIncr.x = (1.0f / m_iTextureWidth) * m_vec2SpriteSize.x;
	m_vec2TexIncr.y = (1.0f / m_iTextureHeight) * m_vec2SpriteSize.y;

	SetTexCoord(D3DXVECTOR4(0.0f, 0.0f, m_vec2TexIncr.x, m_vec2TexIncr.y));

	m_vec2Scale.x /= m_iTextureWidth;
	m_vec2Scale.y /= m_iTextureHeight;

	m_vec2Scale.x *= _vec2SpriteSize.x;
	m_vec2Scale.y *= _vec2SpriteSize.y;

	BuildWorldMatrix();
	BuildTextureMatrix();

	m_vec2ScaleOrig = m_vec2Scale;
}

void CSprite::Initialise(LPCWSTR _pstrTexture, D3DXVECTOR2 _vec2Position, float _fRotation, D3DXVECTOR2 _vec2Scale, float _fAnimationInterval, D3DXVECTOR2 _vec2SpriteSize, std::vector<int> _vecEndings)
{
	Initialise(_pstrTexture, _vec2Position, _fRotation, _vec2Scale, _fAnimationInterval, _vec2SpriteSize);
	m_bDifferentEndings = true;

	int iMaxAnimations = int(m_iTextureWidth / _vec2SpriteSize.x);

	for(unsigned int i = 0; i < _vecEndings.size(); ++i)
	{
		m_vecAnimationEndings.push_back(float(_vecEndings[i]) / float(iMaxAnimations));
	}
}

void CSprite::CreateBuffers(ID3D10Device* _pDevice)
{
	m_spDevice = _pDevice;

	int iNumVertices = 4;

	// Indices
	DWORD* pIndices = new DWORD[m_siFaceCount * 3];

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 2;
	pIndices[4] = 1;
	pIndices[5] = 3;

	// buffer desc
	D3D10_BUFFER_DESC indexBufferDesc;

	indexBufferDesc.Usage = D3D10_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * m_siFaceCount * 3;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA indexInitialData;
	indexInitialData.pSysMem = pIndices;

	if(FAILED(m_spDevice->CreateBuffer(&indexBufferDesc, &indexInitialData, &m_spIndexBuffer)))
	{
		MessageBox(NULL, L"Initialising Index Buffer Failed", L"Error", MB_OK);	
		PostQuitMessage(0);
		return;
	}

	Vertex* pVertices = new Vertex[iNumVertices];

	pVertices[0].vecPosition = D3DXVECTOR2(-0.5f, -0.5f);
	pVertices[0].vecTexCoord = D3DXVECTOR2(0.0f, 1.0f);

	pVertices[1].vecPosition = D3DXVECTOR2(0.5f, -0.5f);
	pVertices[1].vecTexCoord = D3DXVECTOR2(1.0f, 1.0f);

	pVertices[2].vecPosition = D3DXVECTOR2(-0.5f, 0.5f);
	pVertices[2].vecTexCoord = D3DXVECTOR2(0.0f, 0.0f);

	pVertices[3].vecPosition = D3DXVECTOR2(0.5f, 0.5f);
	pVertices[3].vecTexCoord = D3DXVECTOR2(1.0f, 0.0f);

	// buffer settings (desc)
	D3D10_BUFFER_DESC vertexBufferDesc;

	vertexBufferDesc.Usage = D3D10_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * iNumVertices;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA vertexInitialData;
	vertexInitialData.pSysMem = pVertices;

	if(FAILED(m_spDevice->CreateBuffer(&vertexBufferDesc, &vertexInitialData, &m_spVertexBuffer)))
	{
		MessageBox(NULL, L"Initialising Vertex Buffer Failed", L"Error", MB_OK);	
		PostQuitMessage(0);
		return;
	}

	delete[] pIndices;
	delete[] pVertices;
}

void CSprite::ReleaseBuffers()
{
	ReleaseCOM(m_spVertexBuffer);
	ReleaseCOM(m_spIndexBuffer);
}

int CSprite::GetFaceCount()
{
	return(m_siFaceCount);
}

void CSprite::ApplyRenderOptions()
{
	UINT uiStride = sizeof(Vertex);
	UINT uiOffset = 0;

	// set buffers to be used
	m_spDevice->IASetVertexBuffers(0, 1, &m_spVertexBuffer, &uiStride, &uiOffset);
	m_spDevice->IASetIndexBuffer(m_spIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void CSprite::Process(float _fDelta)
{
	if(m_bAnimationEnabled)
	{
		m_fTimer += _fDelta;

		if(m_fTimer > m_fAnimationInterval)
		{
			m_fTimer = 0.0f;	

			float fEnding = (!m_bDifferentEndings ? 1.0f : m_vecAnimationEndings[m_iAnimationRow]);

			if(m_vec4TexCoord.z + m_vec2TexIncr.x > fEnding)
			{
				if(m_bAnimationLoop)
				{
					float fY = float(m_vec2SpriteSize.y / m_iTextureHeight) * m_iAnimationRow;
					SetTexCoord(D3DXVECTOR4(0.0f, fY, m_vec2TexIncr.x, fY + m_vec2TexIncr.y));
				}
				/* MULTILINE ANIMATION CODE 
				if(m_vec4TexCoord.w + m_vec2TexIncr.y >= 1.0f)
				{
					SetTexCoord(D3DXVECTOR4(0.0f, 0.0f, m_vec2TexIncr.x, m_vec2TexIncr.y));
				}
				else
				{
					SetTexCoord(D3DXVECTOR4(0.0f, m_vec4TexCoord.y + m_vec2TexIncr.y, m_vec2TexIncr.x, m_vec4TexCoord.w + m_vec2TexIncr.y));
				} */
			}
			else
			{
				TranslateTexCoord(D3DXVECTOR4(m_vec2TexIncr.x, 0.0f, m_vec2TexIncr.x, 0.0f));
			}
		}
	}
}

ID3D10ShaderResourceView* CSprite::GetTexture() const
{
	return(m_pTexture);
}

void CSprite::SetPosition(D3DXVECTOR2 _vec2Position)
{
	m_vec2Position = _vec2Position;
	BuildWorldMatrix();
}

D3DXVECTOR2 CSprite::GetPosition() const
{
	return(m_vec2Position);
}

void CSprite::SetRotation(float _fRotation)
{
	m_fRotation = _fRotation;
	BuildWorldMatrix();
}

void CSprite::Move(D3DXVECTOR2 _vec2Position)
{
	m_vec2Position += _vec2Position;
	BuildWorldMatrix();
}

void CSprite::Rotate(float _fRotation)
{
	m_fRotation += _fRotation;
	BuildWorldMatrix();
}

void CSprite::Scale(D3DXVECTOR2 _vec2Scale)
{
	m_vec2Scale.x += _vec2Scale.x;
	m_vec2Scale.y += _vec2Scale.y;
	BuildWorldMatrix();
}

float CSprite::GetRotation() const
{
	return(m_fRotation);
}

void CSprite::SetScale(D3DXVECTOR2 _vec2Scale)
{
	m_vec2Scale = _vec2Scale;
}

D3DXVECTOR2 CSprite::GetScale() const
{
	return(m_vec2Scale);
}

D3DXMATRIX CSprite::GetWorldMatrix() const
{
	return(m_matWorld);
}

D3DXMATRIX CSprite::GetTextureMatrix() const
{
	return(m_matTexture);
}

int CSprite::GetWidth() const
{
	return(m_iTextureWidth);
}

int CSprite::GetHeight() const
{
	return(m_iTextureHeight);
}

void CSprite::SetTexCoord(D3DXVECTOR4 _vec4TexCoord)
{
	m_vec4TexCoord = D3DXVECTOR4(Round(_vec4TexCoord.x, 1000), Round(_vec4TexCoord.y, 1000), Round(_vec4TexCoord.z, 1000), Round(_vec4TexCoord.w, 1000));
	BuildTextureMatrix();
}

D3DXVECTOR4 CSprite::GetTexCoord() const
{
	return(m_vec4TexCoord);
}

void CSprite::TranslateTexCoord(D3DXVECTOR4 _vec4TexCoord)
{
	m_vec4TexCoord += _vec4TexCoord;
	m_vec4TexCoord = D3DXVECTOR4(Round(m_vec4TexCoord.x, 1000), Round(m_vec4TexCoord.y, 1000), Round(m_vec4TexCoord.z, 1000), Round(m_vec4TexCoord.w, 1000));
	BuildTextureMatrix();
}

void CSprite::LoopAnimation(bool _bLoop)
{
	m_bAnimationLoop = _bLoop;
}

void CSprite::AnimationEnabled(bool _bEnabled)
{
	m_bAnimationEnabled = _bEnabled;
}

void CSprite::SetAnimationRow(int _iRow, bool _bResetAnimation)
{
	m_iAnimationRow = _iRow;
	float fY = float(m_vec2SpriteSize.y / m_iTextureHeight) * m_iAnimationRow;

	if(_bResetAnimation)
	{
		SetTexCoord(D3DXVECTOR4(0.0f, fY, m_vec2TexIncr.x, fY + m_vec2TexIncr.y));
	}
	else
	{
		float fX = m_vec4TexCoord.x;
		float fZ = m_vec4TexCoord.z;

		if(m_vec4TexCoord.z > m_vecAnimationEndings[m_iAnimationRow])
		{
			fX = m_vecAnimationEndings[m_iAnimationRow] - m_vec2TexIncr.x;
			fZ = m_vecAnimationEndings[m_iAnimationRow];
		}

		SetTexCoord(D3DXVECTOR4(fX, fY, fZ, fY + m_vec2TexIncr.y));
	}

	m_fTimer = 0.0f;
}

int CSprite::GetAnimationRow() const
{
	return(m_iAnimationRow);
}

void CSprite::SetAnimationInterval(float _fInterval)
{
	m_fAnimationInterval = _fInterval / 1000.0f;
}

void CSprite::BuildWorldMatrix()
{
	D3DXMATRIX matTranslation = *D3DXMatrixTranslation(&matTranslation, m_vec2Position.x, m_vec2Position.y, 0.0f);
	D3DXMATRIX matRotation = *D3DXMatrixRotationYawPitchRoll(&matRotation, 0.0f, 0.0f, m_fRotation);
	D3DXMATRIX matScaling = *D3DXMatrixScaling(&matScaling, m_vec2Scale.x, m_vec2Scale.y, 0.0f);

	m_matWorld = matScaling * matRotation * matTranslation;
}

void CSprite::BuildTextureMatrix()
{
	D3DXMATRIX matTranslation = *D3DXMatrixTranslation(&matTranslation, m_vec4TexCoord.x, m_vec4TexCoord.y, 0.0f);
	D3DXMATRIX matScaling = *D3DXMatrixScaling(&matScaling, m_vec4TexCoord.z - m_vec4TexCoord.x, m_vec4TexCoord.w - m_vec4TexCoord.y, 1.0f);

	m_matTexture = matScaling * matTranslation;
}

void CSprite::ResetScale()
{
	m_vec2Scale = m_vec2ScaleOrig;
}