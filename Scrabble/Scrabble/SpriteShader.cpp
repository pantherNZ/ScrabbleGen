// Self Include:
#include "SpriteShader.h"

// Local Includes:

// Library Includes:

// Implementation:
CSpriteShader::CSpriteShader()
{
	m_pEffect = nullptr;
	m_pTechnique = nullptr;
	m_pLayout = nullptr;

	m_pWorldMatrix = nullptr;
	m_pFinalMatrix = nullptr;
	m_pTextureMatrix = nullptr;
}

CSpriteShader::~CSpriteShader()
{
	m_pEffect = nullptr;
	m_pTechnique = nullptr;
	m_pLayout = nullptr;

	m_pWorldMatrix = nullptr;
	m_pFinalMatrix = nullptr;
	m_pTextureMatrix = nullptr;

	ReleaseCOM(m_pLayout);
	ReleaseCOM(m_pEffect);
}

bool CSpriteShader::Initialise(ID3D10Device* _pDevice, HWND _hWnd)
{
	m_pDevice = _pDevice;

	ID3D10Blob* pErrorMessage;
	LPCWSTR pstrShaderFile = L"Data\\SpriteShader.fx";

	if(FAILED(D3DX10CreateEffectFromFile(pstrShaderFile, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS,
										     0, m_pDevice, NULL, NULL, &m_pEffect, &pErrorMessage, NULL)))
	{
		// If the shader failed to compile it should have writen something to the error message
		if(pErrorMessage)
		{
			OutputShaderErrorMessage(pErrorMessage, _hWnd);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself
		else
		{
			MessageBox(_hWnd, L"ERROR: Missing Shader File", pstrShaderFile, MB_OK);
		}

		return(false);
	}

	// Get a pointer to the technique inside the shader
	m_pTechnique = m_pEffect->GetTechniqueByName("TextureTechnique");

	if(!m_pTechnique)
	{
		return(false);
	}

	// Get pointers to the data in the fx file
	m_pWorldMatrix       = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	m_pFinalMatrix       = m_pEffect->GetVariableByName("gFinalMatrix")->AsMatrix();
	m_pTextureMatrix     = m_pEffect->GetVariableByName("gTextureMatrix")->AsMatrix();
	m_pTexture			 = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	
	// Now setup the layout of the data that goes into the shader
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};

	// Get the description of the first pass described in the shader technique.
	D3D10_PASS_DESC passDesc;
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	// Create the input layout.
	if(FAILED(m_pDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pLayout)))
	{
		return(false);
	}

	return(true);
}

void CSpriteShader::Render(D3DXMATRIX _worldMatrix, D3DXMATRIX _viewMatrix, D3DXMATRIX _projMatrix, D3DXMATRIX _texMatrix, ID3D10ShaderResourceView* _pTexture, int _iNumFaces)
{
	m_pDevice->OMSetDepthStencilState(0, 0);
	m_pDevice->IASetInputLayout(m_pLayout);
	m_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// calculate final matrix
	D3DXMATRIX finalMatrix = _worldMatrix * _viewMatrix * _projMatrix;

	// set matrices
	m_pFinalMatrix->SetMatrix((float*)&finalMatrix);
	m_pWorldMatrix->SetMatrix((float*)&_worldMatrix);
	m_pTextureMatrix->SetMatrix((float*)&_texMatrix);

	m_pTexture->SetResource(_pTexture);

	// technique description
	D3D10_TECHNIQUE_DESC techniqueDesc;
	// Get the description structure of the technique from inside the shader so it can be used for rendering
	m_pTechnique->GetDesc(&techniqueDesc);

	// Go through each pass in the technique
	for(unsigned int i = 0; i < techniqueDesc.Passes; ++i)
	{
		m_pTechnique->GetPassByIndex(i)->Apply(0);

		// draw
		m_pDevice->DrawIndexed(_iNumFaces * 3, 0, 0);
	}
}

void CSpriteShader::OutputShaderErrorMessage(ID3D10Blob* _pErrorMessage, HWND _hWnd)
{
	// Get a pointer to the error message text buffer
	char* cCompileErrors = (char*)(_pErrorMessage->GetBufferPointer());

	// Get the length of the message
	unsigned long  ulBufferSize = _pErrorMessage->GetBufferSize();

	std::ofstream fout;

	// Open a file to write the error message to
	fout.open("Data\\shader-error.txt");

	// Write out the error message.
	for(unsigned int i = 0; i < ulBufferSize; i++)
	{
		fout << cCompileErrors[i];
	}

	// Close the file
	fout.close();

	// Release the error message
	ReleaseCOM(_pErrorMessage);

	// Pop a message up on the screen to notify the user to check the text file for compile errors
	MessageBox(_hWnd, L"ERROR: Compiling shader failed (See \"Data\\shader-error.txt\")", L"Error", MB_OK);
}