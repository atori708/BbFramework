#include "CD3D11ShaderManager.h"
#include<cassert>
//#include"Framework\Debug\CDebugLog.h"
#include"Framework\Utility\D3d11Utility.h"

CD3D11ShaderManager::CD3D11ShaderManager()
{
	m_shaderModel = SHADER_MODEL_5_0;

	// �v���t�@�C�������񏉊���
	m_shaderProfileVS[0] = "vs_3_0";
	m_shaderProfileVS[1] = "vs_4_0";
	m_shaderProfileVS[2] = "vs_4_1";
	m_shaderProfileVS[3] = "vs_5_0";

	m_shaderProfilePS[0] = "ps_3_0";
	m_shaderProfilePS[1] = "ps_4_0";
	m_shaderProfilePS[2] = "ps_4_1";
	m_shaderProfilePS[3] = "ps_5_0";

	m_shaderProfileGS[0] = "";
	m_shaderProfileGS[1] = "gs_4_0";
	m_shaderProfileGS[2] = "gs_4_1";
	m_shaderProfileGS[3] = "gs_5_0";

	m_shaderProfileDS[0] = "";
	m_shaderProfileDS[1] = "";
	m_shaderProfileDS[2] = "";
	m_shaderProfileDS[3] = "ds_5_0";

	m_shaderProfileHS[0] = "";
	m_shaderProfileHS[1] = "";
	m_shaderProfileHS[2] = "";
	m_shaderProfileHS[3] = "hs_5_0";

	m_shaderProfileCS[0] = "";
	m_shaderProfileCS[1] = "";
	m_shaderProfileCS[2] = "";
	m_shaderProfileCS[3] = "cs_5_0";
}

CD3D11ShaderManager::~CD3D11ShaderManager()
{
//	m_vertexShaders[0] = nullptr;
	this->DestroyAll();
}

void	CD3D11ShaderManager::SetShaderModel(SHADER_MODEL _shaderModel)
{
	m_shaderModel = _shaderModel;
}

//HRESULT CD3D11ShaderManager::ShaderRecompile(std::vector<s_shaderFileInfo> _shaderFileInfos)
//{
//	HRESULT hr;
//
//	for (unsigned int i = 0; i < _shaderFileInfos.size(); ++i) {
//		hr = this->CompileVS(_shaderFileInfos[i].m_fileName, _shaderFileInfos[i].m_entryPoint);
//		if (FAILED(hr)) {
//			return hr;
//		}
//	}
//
//	return hr;
//}

// ���_�V�F�[�_�̃R���p�C��
int	CD3D11ShaderManager::CompileVS(std::shared_ptr<ID3D11Device> pDevice, std::wstring _filePath, std::string _entryPoint)
{
	assert(pDevice);

	if (_filePath.empty() || _entryPoint.empty()) {
		return -1;
	}

	HRESULT hr;
	ID3DBlob* pCompile;

	// �V�F�[�_�R���p�C��
	hr = this->ShaderCompile(_filePath, _entryPoint, m_shaderProfileVS[m_shaderModel], &pCompile);
	if (FAILED(hr)) {
		//SafeDelete(pVs);
		return -1;
	}

	// ���_�V�F�[�_�쐬�A�\���̂ɓ����
	std::shared_ptr< s_vertexShader> pVertexShaderStruct = std::make_shared<s_vertexShader>();
	ID3D11VertexShader* pVertexShader;
	hr = pDevice->CreateVertexShader(pCompile->GetBufferPointer(), pCompile->GetBufferSize(), nullptr, &pVertexShader);
	if (FAILED(hr)) {
		pVertexShaderStruct = nullptr;	// �Q�ƃJ�E���^���炵�ĉ�����Ă���
		return -1;
	}
	pVertexShaderStruct->m_pShader.m_pShader.reset(pVertexShader, D3DComDeleter());	// �f���[�^�ݒ�
	pVertexShaderStruct->m_inputLayoutIndex = this->CreateAndAddInputLayout(pDevice, pCompile);
	m_vertexShaders.push_back(pVertexShaderStruct);
	SafeRelease(pCompile);

	return m_vertexShaders.size() - 1;
}

// �s�N�Z���V�F�[�_�̃R���p�C��
int	CD3D11ShaderManager::CompilePS(std::shared_ptr<ID3D11Device> pDevice, std::wstring _filePath, std::string _entryPoint)
{
	assert(pDevice);

	if (_filePath.empty() || _entryPoint.empty()) {
		return -1;
	}

	HRESULT hr;
	ID3DBlob* pCompile;

	hr = this->ShaderCompile(_filePath, _entryPoint, m_shaderProfilePS[m_shaderModel], &pCompile);
	if (FAILED(hr)) {
		return -1;
	}

	std::shared_ptr<s_shader<std::shared_ptr<ID3D11PixelShader>>> pShader = std::make_shared<s_shader<std::shared_ptr<ID3D11PixelShader>>>();
	ID3D11PixelShader* pPixelShader;
	hr = pDevice->CreatePixelShader(pCompile->GetBufferPointer(), pCompile->GetBufferSize(), nullptr, &pPixelShader);
	if (FAILED(hr)) {
		pShader = nullptr;	// �Q�ƃJ�E���^���炵�ĉ�����Ă���
		return -1;
	}
	pShader->m_pShader.reset(pPixelShader, D3DComDeleter());
	m_pixelShaders.push_back(pShader);
	SafeRelease(pCompile);

	return m_vertexShaders.size() - 1;
}

// �V�F�[�_�̃R���p�C��
HRESULT	CD3D11ShaderManager::ShaderCompile(std::wstring _filePath, std::string _entryPoint, std::string _profile, ID3DBlob** _ppOut)
{
	if (_ppOut == nullptr) {
		return E_FAIL;
	}

	HRESULT hr;
	ID3DBlob* pError = nullptr;
	_filePath = this->SHADER_FILE_PATH + _filePath;

	hr = D3DCompileFromFile(_filePath.c_str(), nullptr, nullptr, _entryPoint.c_str(), _profile.c_str(),
		0, 0,
		_ppOut, &pError);

	if (FAILED(hr)) {
		if (pError) {
			MessageBoxA(nullptr, (char*)pError->GetBufferPointer(), "Compile shader error", MB_OK);
		}
		return hr;
	}

	SafeRelease(pError);

	return hr;
}

// �C���v�b�g���C�A�E�g���V�F�[�_���t���N�V��������擾�A�V�����G�������g�\���Ȃ�ǉ�
int CD3D11ShaderManager::CreateAndAddInputLayout(std::shared_ptr<ID3D11Device> pDevice, ID3D10Blob* _pBlob)
{
	if (_pBlob == nullptr) {
		return -1;
	}

	HRESULT hr;
	ID3D11ShaderReflection* pShaderReflection;

	// ShaderRefrection����C���v�b�g���C�A�E�g���擾
	hr = D3DReflect(_pBlob->GetBufferPointer(), _pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pShaderReflection);
	if (FAILED(hr))
		return -1;

	D3D11_SHADER_DESC shaderDesc;
	pShaderReflection->GetDesc(&shaderDesc);
	int elementNum = shaderDesc.InputParameters;
	D3D11_INPUT_ELEMENT_DESC* pElementDescs = new D3D11_INPUT_ELEMENT_DESC[elementNum];
	//D3D11_INPUT_ELEMENT_DESC* pElementDescs = (D3D11_INPUT_ELEMENT_DESC*)malloc( sizeof(D3D11_INPUT_ELEMENT_DESC) * elementNum);

	for (int i = 0; i < elementNum; ++i) {
		D3D11_SIGNATURE_PARAMETER_DESC parameterDesc;
		DXGI_FORMAT format;

		pShaderReflection->GetInputParameterDesc(i, &parameterDesc);
		format = this->GetDXGIFormat(parameterDesc.ComponentType, parameterDesc.Mask);

		D3D11_INPUT_ELEMENT_DESC elementDesc = {
			parameterDesc.SemanticName, // Semantic��
			parameterDesc.SemanticIndex, // POSITION0�Ƃ��̐����B�������0
			format, // DXGI_FORMAT
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0 // ���ߑł�
		};

		pElementDescs[i] = elementDesc;
	}

	// �g���܂킹��C���v�b�g���C�A�E�g�����邩���ׂ�
	int existInputLayoutIndex = this->CheckExistInputLayout(pElementDescs, elementNum);
	if (existInputLayoutIndex == -1) {
		// �V�����G�������g�Ȃ̂�InputLayout�쐬
		ID3D11InputLayout* pInputLayout = nullptr;
		std::shared_ptr<s_InputLayout> pInputLayoutStruct = std::make_shared<s_InputLayout>();

		hr = pDevice->CreateInputLayout(pElementDescs, elementNum, _pBlob->GetBufferPointer(), _pBlob->GetBufferSize(), &pInputLayout);
		if (FAILED(hr)) {
			return -1;
		}

		pInputLayoutStruct->m_pInputLayout.reset(pInputLayout, D3DComDeleter());
		pInputLayoutStruct->m_pInputElementDesc.reset(pElementDescs);
		pInputLayoutStruct->m_numElements = elementNum;
		m_inputLayoutStructs.push_back(pInputLayoutStruct);
		SafeRelease(pShaderReflection);	// �C���v�b�g���C�A�E�g���O�ɉ�������x64�̃����[�X�r���h�ŃC���v�b�g���C�A�E�g�����Ȃ�(�Ȃ��H)

		return m_inputLayoutStructs.size() - 1;
	}

	SafeRelease(pShaderReflection);	// �C���v�b�g���C�A�E�g���O�ɉ�������x64�̃����[�X�r���h�ŃC���v�b�g���C�A�E�g�����Ȃ�(�Ȃ��H)
	// SafeDelete(pElementDescs);
	 //SafeFree(pElementDescs);

	return existInputLayoutIndex;
}

// �C���v�b�g���C�A�E�g����t�H�[�}�b�g�擾
DXGI_FORMAT	CD3D11ShaderManager::GetDXGIFormat(D3D10_REGISTER_COMPONENT_TYPE _componentType, BYTE _mask)
{
	if (_mask == 1) {
		if (_componentType == D3D_REGISTER_COMPONENT_UINT32)	return DXGI_FORMAT_R32_UINT;
		else if (_componentType == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32_SINT;
		else if (_componentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32_FLOAT;
	} else if (_mask <= 3) {
		if (_componentType == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32G32_UINT;
		else if (_componentType == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32G32_SINT;
		else if (_componentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32_FLOAT;
	} else if (_mask <= 7) {
		if (_componentType == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32G32B32_UINT;
		else if (_componentType == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32G32B32_SINT;
		else if (_componentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32B32_FLOAT;
	} else if (_mask <= 15) {
		if (_componentType == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32G32B32A32_UINT;
		else if (_componentType == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32G32B32A32_SINT;
		else if (_componentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}

	return DXGI_FORMAT_UNKNOWN;
}

// ���łɑ��݂��Ă���C���v�b�g���C�A�E�g���`�F�b�N
int	CD3D11ShaderManager::CheckExistInputLayout(D3D11_INPUT_ELEMENT_DESC* _pElementDesc, UINT _numElement)
{
	for (UINT countInputLayout = 0; countInputLayout < m_inputLayoutStructs.size(); ++countInputLayout) {
		D3D11_INPUT_ELEMENT_DESC* pElement = m_inputLayoutStructs[countInputLayout]->m_pInputElementDesc.get();
		UINT numElement = m_inputLayoutStructs[countInputLayout]->m_numElements > _numElement ? m_inputLayoutStructs[countInputLayout]->m_numElements : _numElement;
		//	if( numElement != _numElement )
		//		continue;

		bool isMatch = true;
		for (UINT i = 0; i < numElement; ++i) {
			//for( UINT j = 0; j < _numElement; ++j )
			if (!CompareD3D11InputElementDesc(&_pElementDesc[i], &pElement[i])) {
				isMatch = false;
			}
		}

		if (isMatch) {
			return countInputLayout;	// ��v���Ă������_�ŕԂ�
		}
	}

	return -1;
}

// �C���v�b�g�G�������g����C���v�b�g���C�A�E�g���쐬����
HRESULT	CD3D11ShaderManager::CreateInputLayoutByInputElement(std::shared_ptr<ID3D11Device> pDevice, ID3D10Blob* _pBlob, D3D11_INPUT_ELEMENT_DESC* _pElement, UINT _numElement, ID3D11InputLayout* _pOut)
{
	assert(pDevice);

	if (_pBlob == nullptr || _pElement == nullptr ) {
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;

	hr = pDevice->CreateInputLayout(_pElement, _numElement, _pBlob->GetBufferPointer(), _pBlob->GetBufferSize(), &_pOut);

	if (FAILED(hr)) {
		return hr;
	}

	return hr;
}

// �C���v�b�g���C�A�E�g�擾
std::shared_ptr<ID3D11InputLayout> CD3D11ShaderManager::GetInputLayout(UINT _vsIndex) const
{
	if (_vsIndex >= m_vertexShaders.size()) {
		return nullptr;
	}
	 
	return m_inputLayoutStructs[m_vertexShaders[_vsIndex]->m_inputLayoutIndex]->m_pInputLayout;
}

// �C���v�b�g���C�A�E�g�擾(���_�V�F�[�_�̎�ނ���)
std::shared_ptr<ID3D11InputLayout> CD3D11ShaderManager::GetInputLayout(std::wstring _shaderName, std::string _entryPoint) const
{
	for each (auto vertexShader in m_vertexShaders)
	{
		s_shaderFileInfo fileInfo = vertexShader->m_pShader.m_fileInfo;
		if (fileInfo.m_fileName == _shaderName && fileInfo.m_entryPoint == _entryPoint) {
			return m_inputLayoutStructs[vertexShader->m_inputLayoutIndex]->m_pInputLayout;
		}
	}
	return nullptr;
}

// ���_�V�F�[�_�擾
std::shared_ptr<ID3D11VertexShader>	CD3D11ShaderManager::GetVertexShader(UINT _index)const
{
	if (_index >= m_vertexShaders.size()) {
		return nullptr;
	}

	return m_vertexShaders[_index]->m_pShader.m_pShader;
}

// ���_�V�F�[�_�擾(�V�F�[�_�t�@�C�����ƃG���g���|�C���g������)
std::shared_ptr<ID3D11VertexShader> CD3D11ShaderManager::GetVertexShader(std::wstring _shaderName, std::string _entryPoint) const
{
	for each (auto vertexShader in m_vertexShaders)
	{
		s_shaderFileInfo fileInfo = vertexShader->m_pShader.m_fileInfo;
		if (fileInfo.m_fileName == _shaderName && fileInfo.m_entryPoint == _entryPoint) {
			return vertexShader->m_pShader.m_pShader;
		}
	}

	return nullptr;
}

// �s�N�Z���V�F�[�_�擾
std::shared_ptr<ID3D11PixelShader>	CD3D11ShaderManager::GetPixelShader(UINT _index)const
{
	if (_index >= m_pixelShaders.size()) {
		return nullptr;
	}

	return m_pixelShaders[_index]->m_pShader;
}

// �s�N�Z���V�F�[�_�擾(�V�F�[�_�t�@�C�����ƃG���g���|�C���g������)
std::shared_ptr<ID3D11PixelShader>	CD3D11ShaderManager::GetPixelShader(std::wstring _shaderName, std::string _entryPoint) const
{
	for each (auto pixelShader in m_pixelShaders)
	{
		s_shaderFileInfo fileInfo = pixelShader->m_fileInfo;
		if (fileInfo.m_fileName == _shaderName && fileInfo.m_entryPoint == _entryPoint) {
			return pixelShader->m_pShader;
		}
	}

	return nullptr;
}

// �V�F�[�_�֘A�̑S�Ẵ��\�[�X��j��
void	CD3D11ShaderManager::DestroyAll()
{
	using namespace std;

	m_inputLayoutStructs.clear();
	m_vertexShaders.clear();

	for (shared_ptr <s_shader<shared_ptr<ID3D11PixelShader>>> pPixelShader : m_pixelShaders) {
		pPixelShader = nullptr;
	}
	m_pixelShaders.clear();


}
