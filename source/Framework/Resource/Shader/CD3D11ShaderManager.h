#pragma once
#include<string>
#include<vector>
#include<memory>
#include<d3dcompiler.h>
#include<d3d11.h>
#include"Framework\Utility\SafeDelete.h"
//#include"Framework\Resource\Resource.h"

#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "dxguid.lib")

// TODO:文字列とシェーダファイルの関連付
enum SHADER_MODEL
{
	SHADER_MODEL_3_0 = 0,
	SHADER_MODEL_4_0,
	SHADER_MODEL_4_1,
	SHADER_MODEL_5_0,
    SHADER_MODEL_COUNT, // 列挙型の数取得のため定義
};

enum class D3D11_SHADER_TYPE
{
	VERTEX_SHADER = 0,
	PIXEL_SHADER,
	GEOMETRY_SHADER,
	HULL_SHADER,
	DOMAIN_SHADER,
	COMPUTE_SHADER,
};

// シェーダ情報
struct s_shaderFileInfo
{
    std::wstring    m_fileName;
    std::string     m_entryPoint;

public:
	s_shaderFileInfo(){}

    s_shaderFileInfo( std::wstring _filePath, std::string _entryPoint){
        m_fileName = _filePath;
        m_entryPoint = _entryPoint;
    }
};

template< class T>
struct s_shader
{
	//ID3DBlob*	m_pCompile;	
	//ID3D11VertexShader*	m_pShader;
    T m_pShader;
    s_shaderFileInfo m_fileInfo;

	s_shader()
	{
		//m_pCompile = nullptr;
		m_pShader = nullptr;
	}

	s_shader(std::wstring _filePath, std::string _entryPoint)
		:m_fileInfo( _filePath, _entryPoint)
	{
		m_pShader = nullptr;
	}

	~s_shader()
	{
		m_pShader = nullptr;
		//SafeRelease( m_pCompile);
		//SafeRelease( m_pShader);
	}
};

// インプットレイアウトのために頂点シェーダだけ構造体分ける
struct s_vertexShader
{
	s_shader<std::shared_ptr<ID3D11VertexShader>> m_pShader;
	int m_inputLayoutIndex;

	s_vertexShader(){
		m_inputLayoutIndex = 0;
	}

	s_vertexShader(std::wstring _filePath, std::string _entryPoint)
		:m_pShader( _filePath, _entryPoint)
	{
		m_inputLayoutIndex = 0;
	}
};

// インプットレイアウト構造体
struct s_InputLayout
{
	std::shared_ptr<ID3D11InputLayout>	m_pInputLayout;
	std::shared_ptr<D3D11_INPUT_ELEMENT_DESC>	m_pInputElementDesc;
    UINT	m_numElements;

    s_InputLayout()
    {
    }

    ~s_InputLayout()
    {
        //SafeRelease( m_pInputLayout);
        //SafeDelete( m_pInputElementDesc);
    }
};

/*!
 * @brief    シェーダを管理するクラス
 * @note     シェーダのロードと管理をする。インプットレイアウトはシェーダリフレクションで取ってる
 * @author   N.kaji
 * @date     2016/05/22
 */
class CD3D11ShaderManager
{
private:
    const wchar_t SHADER_FILE_PATH[24] = L"resource/shader/";
	//std::shared_ptr<ID3D11Device> m_pD3dDevice;

	SHADER_MODEL m_shaderModel = SHADER_MODEL_5_0;	// プロファイル文字列のインデックスとしても使用

	// プロファイル文字列。存在しないプロファイルはnull
	std::string m_shaderProfileVS[SHADER_MODEL_COUNT]; 
	std::string m_shaderProfilePS[SHADER_MODEL_COUNT]; 
	std::string m_shaderProfileGS[SHADER_MODEL_COUNT]; 
	std::string m_shaderProfileHS[SHADER_MODEL_COUNT]; 
	std::string m_shaderProfileDS[SHADER_MODEL_COUNT]; 
	std::string m_shaderProfileCS[SHADER_MODEL_COUNT];

	// シェーダ配列
    std::vector<std::shared_ptr<s_vertexShader>>					m_vertexShaders;
    std::vector<std::shared_ptr<s_shader<std::shared_ptr<ID3D11PixelShader>>>>		m_pixelShaders;
	std::vector<std::shared_ptr<s_shader<std::shared_ptr<ID3D11GeometryShader>>>>	m_geometryShaders;
	std::vector<std::shared_ptr<s_shader<std::shared_ptr<ID3D11HullShader>>>>		m_hullShaders;
	std::vector<std::shared_ptr<s_shader<std::shared_ptr<ID3D11DomainShader>>>>		m_domainShaders;
	std::vector<std::shared_ptr<s_shader<std::shared_ptr<ID3D11ComputeShader>>>>	m_computeShaders;

    // インプットレイアウト
    std::vector<std::shared_ptr<s_InputLayout>> m_inputLayoutStructs;

public:
	CD3D11ShaderManager();
	~CD3D11ShaderManager();

	void	SetShaderModel( SHADER_MODEL _shaderModel);

	// シェーダコンパイル
	int	CompileVS(std::shared_ptr<ID3D11Device> pDevice, std::wstring _filePath, std::string _entryPoint);
	int	CompilePS(std::shared_ptr<ID3D11Device> pDevice, std::wstring _filePath, std::string _entryPoint);

	// ゲッターいろいろ
	// s_shader<ID3D11VertexShader*>	GetVertexShaderStruct( std::string _entryPointName)const;
	std::shared_ptr<ID3D11InputLayout> GetInputLayout( UINT _vsIndex)const;
	std::shared_ptr<ID3D11InputLayout> GetInputLayout( std::wstring _shaderName, std::string _entryPoint)const;
	std::shared_ptr<ID3D11VertexShader>	GetVertexShader( UINT _index)const;
	std::shared_ptr<ID3D11VertexShader>	GetVertexShader( std::wstring _shaderName, std::string _entryPoint)const;
	std::shared_ptr<ID3D11PixelShader>	GetPixelShader( UINT _index)const;
	std::shared_ptr<ID3D11PixelShader>	GetPixelShader( std::wstring _shaderName, std::string _entryPoint)const;

	void	DestroyAll();

private:
	HRESULT ShaderCompile( std::wstring _filePath, std::string _entryPoint, std::string _profile, ID3DBlob** _pOut);
	//HRESULT ShaderRecompile( std::vector<s_shaderFileInfo> _shaderFileInfos);

    // インプットレイアウト
    int CreateAndAddInputLayout(std::shared_ptr<ID3D11Device> pDevice, ID3D10Blob* _pBlob);
    DXGI_FORMAT GetDXGIFormat(D3D10_REGISTER_COMPONENT_TYPE _componentType, BYTE _mask);
    int CheckExistInputLayout(D3D11_INPUT_ELEMENT_DESC * _pElementDesc, UINT _numElement);
	HRESULT CreateInputLayoutByInputElement(std::shared_ptr<ID3D11Device> pDevice, ID3D10Blob * _pBlob, D3D11_INPUT_ELEMENT_DESC * _pElement, UINT _numElement, ID3D11InputLayout* _pOut);
};
