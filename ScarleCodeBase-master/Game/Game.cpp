#include "Game.h"
//DXTK headers
#include "SimpleMath.h"

//system headers
#include <windows.h>
#include <time.h>
#include <iostream>
#include <AntTweakBar.h>

//our headers
#include "ObjectList.h"
#include "GameData.h"
#include "drawdata.h"
#include "DrawData2D.h"
#include "BoidController.h"
#include "BoidData.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

//callback functions for tweak bar buttons
void TW_CALL GroupCB(void * m_boidData)
{
	Game *game = (Game*)m_boidData;

	game->groupBoids();
}

void TW_CALL UnGroupCB(void * m_boidData)
{
	Game *game = (Game*)m_boidData;

	game->ungroupBoids();
}

void TW_CALL FightCB(void * m_boidData)
{
	Game *game = (Game*)m_boidData;

	game->fightBoids();
}

void TW_CALL PassiveCB(void * m_boidData)
{
	Game *game = (Game*)m_boidData;

	game->passiveBoids();
}

void TW_CALL SpawnCB(void * m_boidData)
{
	Game *game = (Game*)m_boidData;

	game->controllerSpawn(false);
}

void TW_CALL SpawnRandCB(void * m_boidData)
{
	Game *game = (Game*)m_boidData;

	game->controllerSpawn(true);
}

void TW_CALL ResetCB(void * m_boidData)
{
	Game *game = (Game*)m_boidData;

	game->controllerReset();
}

Game::Game(ID3D11Device* _pd3dDevice, HWND _hWnd, HINSTANCE _hInstance) 
{
	//set up audio
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif
	m_audioEngine.reset(new AudioEngine(eflags));

	//Create DirectXTK spritebatch stuff
	ID3D11DeviceContext* pd3dImmediateContext;
	_pd3dDevice->GetImmediateContext(&pd3dImmediateContext);
	m_DD2D = new DrawData2D();
	m_DD2D->m_Sprites.reset(new SpriteBatch(pd3dImmediateContext));
	m_DD2D->m_Font.reset(new SpriteFont(_pd3dDevice, L"..\\Assets\\italic.spritefont"));

	//seed the random number generator
	srand((UINT)time(NULL));

	//Direct Input Stuff
	m_hWnd = _hWnd;
	m_pKeyboard = nullptr;
	m_pDirectInput = nullptr;

	HRESULT hr = DirectInput8Create(_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);
	hr = m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL);
	hr = m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = m_pKeyboard->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
	hr = m_pMouse->SetDataFormat(&c_dfDIMouse);
	hr = m_pMouse->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	//create GameData struct and populate its pointers
	m_GD = new GameData;
	m_GD->m_keyboardState = m_keyboardState;
	m_GD->m_prevKeyboardState = m_prevKeyboardState;
	m_GD->m_GS = GS_PLAY_MAIN_CAM;
	m_GD->m_mouseState = &m_mouseState;

	//set up DirectXTK Effects system
	m_fxFactory = new EffectFactory(_pd3dDevice);

	//Tell the fxFactory to look to the correct build directory to pull stuff in from
#ifdef DEBUG
	((EffectFactory*)m_fxFactory)->SetDirectory(L"../Debug");
#else
	((EffectFactory*)m_fxFactory)->SetDirectory(L"../Release");
#endif

	// Create other render resources here
	m_states = new CommonStates(_pd3dDevice);

	//init render system for VBGOs
	VBGO::Init(_pd3dDevice);

	//find how big my window is to correctly calculate my aspect ratio
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	float AR = (float)width / (float)height;

	//create a base camera
	m_cam = new Camera(0.25f * XM_PI, AR, 1.0f, 10000.0f, Vector3::UnitY, Vector3(300, 0, 300));
	m_cam->SetPos(Vector3(200.0f, 200.0f, 850.0f));
	m_cam->SetPos(m_cam->GetPos() - Vector3(0, 0, -15));
	m_GameObjects.push_back(m_cam);

	//create a base light
	m_light = new Light(Vector3(300.0f, 100.0f, 300.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.4f, 0.1f, 0.1f, 1.0f));
	m_GameObjects.push_back(m_light);

	//create a base floor of 12 tiles
	int posX = 0;
	int posZ = 0;

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			FileVBGO* terrainBox = new FileVBGO("../Assets/terrainTex.txt", _pd3dDevice);
			terrainBox->SetScale(3, 0, 3);
			m_GameObjects.push_back(terrainBox);
			terrainBox->SetPos(Vector3(posX, -10, posZ));
			posX += 110;
		}
		posZ += 110;
		posX = 0;
	}

	//create point for camera to look at in centre
	FileVBGO* cameraPoint = new FileVBGO("../Assets/terrainTex.txt", _pd3dDevice);
	cameraPoint->SetScale(0, 0, 0);
	m_GameObjects.push_back(cameraPoint);
	cameraPoint->SetPos(cameraPoint->GetPos() + Vector3(100, 0, 100));

	//add a secondary camera
	m_TPScam = new TPSCamera(0.25f * XM_PI, AR, 1.0f, 10000.0f, cameraPoint, Vector3::UnitY, Vector3(50.0f, 10.0f, 100.0f));
	m_GameObjects.push_back(m_TPScam);

	//create DrawData struct and populate its pointers
	m_DD = new DrawData;
	m_DD->m_pd3dImmediateContext = nullptr;
	m_DD->m_states = m_states;
	m_DD->m_cam = m_cam;
	m_DD->m_light = m_light;

	//create boid data struct and assign values
	m_boidData =  new BoidData;
	m_boidData->braveryWeight = -2.0f;

	TwInit(TW_DIRECT3D11, _pd3dDevice);
	TwWindowSize(width, height);

	//create tweak bar
	TwBar *myBar;
	myBar = TwNewBar("Boid parameters");
	int barSize[2] = { 250, 225 };
	TwSetParam(myBar, NULL, "size", TW_PARAM_INT32, 2, barSize);

	//define values and buttons
	TwAddVarRW(myBar, "Seperation", TW_TYPE_FLOAT, &m_boidData->seperationWeight, "");
	TwAddVarRW(myBar, "Cohesion", TW_TYPE_FLOAT, &m_boidData->cohesionWeight, "");
	TwAddVarRW(myBar, "Alignment", TW_TYPE_FLOAT, &m_boidData->alignmentWeight, "");
	TwAddVarRW(myBar, "Bravery", TW_TYPE_FLOAT, &m_boidData->braveryWeight, "");
	TwAddVarRW(myBar, "Neighbour Distance", TW_TYPE_FLOAT, &m_boidData->neighbourDis, "");
	TwAddButton(myBar, "Enable Grouping", GroupCB, this, " label='Enable Grouping' ");
	TwAddButton(myBar, "Disable Grouping", UnGroupCB, this, " label='Disable Grouping' ");
	TwAddButton(myBar, "Enable Fighting", FightCB, this, " label='Enable Fighting' ");
	TwAddButton(myBar, "Disable Fighting", PassiveCB, this, " label='Disable Fighting' ");
	TwAddButton(myBar, "Spawn Boids", SpawnCB, this, " label='Spawn Boids' ");
	TwAddButton(myBar, "Spawn Boids (Random)", SpawnRandCB, this, " label='Spawn Boids (Random)' ");
	TwAddButton(myBar, "Reset Boids", ResetCB, this, " label='Reset Boids' ");

	//how many boids to spawn
	maxBoidSpawn = 800;

	//create boid manager
	controller = std::make_unique<BoidController>(maxBoidSpawn, _pd3dDevice, m_fxFactory, m_boidData);
};

Game::~Game() 
{
	//delete Game Data & Draw Data
	delete m_GD;
	delete m_DD;

	//tidy up VBGO render system
	VBGO::CleanUp();

	//tidy away Direct Input Stuff
	if (m_pKeyboard)
	{
		m_pKeyboard->Unacquire();
		m_pKeyboard->Release();
	}
	if (m_pMouse)
	{
		m_pMouse->Unacquire();
		m_pMouse->Release();
	}
	if (m_pDirectInput)
	{
		m_pDirectInput->Release();
	}

	//get rid of the game objects here
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		delete (*it);
	}
	m_GameObjects.clear();

	//and the 2D ones
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		delete (*it);
	}
	m_GameObject2Ds.clear();

	//clear away CMO render system
	delete m_states;
	delete m_fxFactory;

	delete m_DD2D;

	delete m_boidData;
};

bool Game::Tick() 
{
	//tick audio engine
	if (!m_audioEngine->Update())
	{
		// No audio device is active
		if (m_audioEngine->IsCriticalError())
		{
			//something has gone wrong with audio so QUIT!
			return false;
		}
	}

	//Poll Keyboard & Mouse
	ReadInput();

	//Upon pressing escape QUIT
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return false;
	}

	//lock the cursor to the centre of the window
	RECT window;
	GetWindowRect(m_hWnd, &window);

	//calculate frame time-step dt for passing down to game objects
	DWORD currentTime = GetTickCount();
	m_GD->m_dt = min((float)(currentTime - m_playTime) / 1000.0f, 0.1f);
	m_playTime = currentTime;

	//start to a VERY simple FSM
	switch (m_GD->m_GS)
	{
	case GS_ATTRACT:
		break;
	case GS_PAUSE:
		break;
	case GS_GAME_OVER:
		break;
	case GS_PLAY_MAIN_CAM:
		PlayTick();
	case GS_PLAY_TPS_CAM:
		PlayTick();
		break;
	}

	return true;
};

void Game::PlayTick()
{
	//upon space bar switch camera state
	if ((m_keyboardState[DIK_SPACE] & 0x80) && !(m_prevKeyboardState[DIK_SPACE] & 0x80))
	{
		if (m_GD->m_GS == GS_PLAY_MAIN_CAM)
		{
			m_GD->m_GS = GS_PLAY_TPS_CAM;
		}
		else
		{
			m_GD->m_GS = GS_PLAY_MAIN_CAM;
		}
	}

	//update all objects
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		(*it)->Tick(m_GD);
	}
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		(*it)->Tick(m_GD);
	}

	//tick boid manager
	controller->Tick(m_GD);
}

void Game::Draw(ID3D11DeviceContext* _pd3dImmediateContext) 
{
	//set immediate context of the graphics device
	m_DD->m_pd3dImmediateContext = _pd3dImmediateContext;

	//set which camera to be used
	m_DD->m_cam = m_cam;
	if (m_GD->m_GS == GS_PLAY_TPS_CAM)
	{
		m_DD->m_cam = m_TPScam;
	}

	//update the constant buffer for the rendering of VBGOs
	VBGO::UpdateConstantBuffer(m_DD);

	//draw boids using manager
	controller->DrawBoids(m_DD);

	//draw all objects
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		(*it)->Draw(m_DD);
	}

	// Draw sprite batch stuff 
	m_DD2D->m_Sprites->Begin();
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		(*it)->Draw(m_DD2D);
	}

	m_DD2D->m_Sprites->End();

	//drawing text screws up the Depth Stencil State, this puts it back again!
	_pd3dImmediateContext->OMSetDepthStencilState(m_states->DepthDefault(), 0);

	//draw tweak bar
	TwDraw();
}

//functions for previous callback functions to call
void Game::groupBoids()
{
	controller->groupBoids();
}

void Game::ungroupBoids()
{
	controller->ungroupBoids();
}

void Game::fightBoids()
{
	controller->enableFighting();
}

void Game::passiveBoids()
{
	controller->disableFighting();
}

void Game::controllerSpawn(bool _rand)
{
	controller->SpawnBoid(_rand);
}

void Game::controllerReset()
{
	controller->resetBoids();
}

bool Game::ReadInput()
{
	//copy over old keyboard state
	memcpy(m_prevKeyboardState, m_keyboardState, sizeof(unsigned char) * 256);

	//clear out previous state
	ZeroMemory(&m_keyboardState, sizeof(unsigned char) * 256);
	ZeroMemory(&m_mouseState, sizeof(DIMOUSESTATE));

	// Read the keyboard device.
	HRESULT hr = m_pKeyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(hr))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pKeyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	// Read the Mouse device.
	hr = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(hr))
	{
		// If the Mouse lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pMouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}