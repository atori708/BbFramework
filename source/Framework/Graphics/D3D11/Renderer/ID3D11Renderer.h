#pragma once
#include<d3d11.h>
#include"Framework\Resource\Model\CDrawModelBase.h"
#include"Framework\Utility\SafeDelete.h"
#include<memory>

enum class D3D11_SHADER_TYPE;

struct s_d3d11RenderTarget
{
	std::shared_ptr<ID3D11RenderTargetView> m_pRTV = nullptr;
	std::shared_ptr<ID3D11ShaderResourceView> m_pSRV = nullptr;
};

// ShaderManagerから各シェーダ取ってくるための情報
struct s_shaderLabel
{
	int m_shaderIndex;
	D3D11_SHADER_TYPE	m_shaderType;
};


class ID3D11Renderer
{
public:
	ID3D11Renderer() {};
	virtual ~ID3D11Renderer() {};

// TODO: 2D3Dで分けるか、それかレンダラの単位で2D3Dを分けるか
	virtual int ResistDrawObject(std::shared_ptr<CDrawModelBase> pDrawObject)abstract;
	virtual void Render(std::shared_ptr<ID3D11DeviceContext> pDebiceContext)abstract;

	virtual void ResistShader(D3D11_SHADER_TYPE shaderType, int shaderIndex);
};

