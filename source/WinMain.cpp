#include"Framework\Application\CWindowsApplication.h"
#include"Framework\Graphics\D3D11\CD3D11Graphics.h"
#include"Framework\Resource\Model\CGeometoryCreater.h"
#include"Framework\Resource\CD3D11ResourceManager.h"
#include"Framework\Game\Object\CGameObject3D.h"
#include"Framework\Resource\Model\D3D11\CD3D113DModelLoader.h"
#include"../CD3D11FBXModelLoader.h"

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	CWindowsApplication application(L"テストでござる");
	CD3D11Graphics graphics;

	application.SetWindowsCommandLine(hInstance, lpCmdLine, nCmdShow);
	application.SetWindowSize(WINDOW_SIZE_TYPE::SIZE_1280x720);

	application.Initialize();
	graphics.Initialize(&application);

	CGeometoryCreater gCreater(graphics.GetD3D11Device());
	std::shared_ptr<CD3D11DrawModel3D>	pPlaneMesh;
	std::shared_ptr<CGameObject3D> planeObject = std::make_shared<CGameObject3D>();

	CD3D113DModelLoader loader;
	CD3D11FBXModelLoader loaderFbx;
	std::shared_ptr<CD3D11DrawModel3D>	model = std::make_shared<CD3D11DrawModel3D>();
	loaderFbx.Load(graphics.GetD3D11Device(), L"resource/sword/sword.fbx", model);
	//bool result = loader.Load(graphics.GetD3D11Device(), L"resource/rapi/rapi.pmd", model);
	planeObject->SetDrawObject(model);

	pPlaneMesh = gCreater.CreatePlanePolygon(100, 2);
	//planeObject->SetDrawObject(pPlaneMesh);
	graphics.RegistObject2StaticRenerer(planeObject);

	// ビュー行列とプロジェクション行列用意
	using namespace DirectX;
	const RECT_SIZE& clientSize = application.GetClientSize();
	XMMATRIX viewMatrix = XMMatrixTranspose(XMMatrixLookAtLH(XMVectorSet(0, 2, 15, 0), XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 1, 0, 0)));
	XMMATRIX projectionMatrix = XMMatrixTranspose((XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)clientSize.width / clientSize.height, 1.0f, 200.0f)));
	XMFLOAT4X4 viewMatrixVelue;
	XMStoreFloat4x4(&viewMatrixVelue, viewMatrix);
	s_cbChangeEveryFrame changeEveryFrame{ viewMatrixVelue, XMFLOAT4(0.5f, 0.5f, 0.5f, 0), XMFLOAT4(1, 1, 1, 1) };
	D3D11ResourceManager.CompileVertexShader( graphics.GetD3D11Device(),L"vs.fx", "vsMain");
	D3D11ResourceManager.CompilePixelShader(graphics.GetD3D11Device(),L"vs.fx", "psMain");

	while (!application.DispatchedWMQuitMessage()) {
		graphics.Update(projectionMatrix, changeEveryFrame);
		//graphics.Update(projectionMatrix, viewMatrix);
		graphics.Render();
	}

	graphics.Finalize();
	application.Finalize();

	return 0;
}
