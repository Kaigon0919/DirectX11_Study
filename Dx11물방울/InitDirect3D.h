#pragma once
#include"d3dApp.h"

class InitDirect3DApp : public D3DApp
{
public:
	InitDirect3DApp(HINSTANCE hInstance);
	~InitDirect3DApp();

	bool init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

};