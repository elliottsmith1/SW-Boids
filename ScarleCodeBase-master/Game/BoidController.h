#pragma once

#include "gameobject.h"
#include "Model.h"

#include <d3d11_1.h>
#include <vector>
#include <string>

struct GameData;
struct DrawData;
struct BoidData;
class Boid;

class BoidController
{
public:
	BoidController(int _numBoids, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, BoidData* _boidData);
	~BoidController(); 

	void Tick(GameData* _GD);
	void DrawBoids(DrawData* _DD);
	void SpawnBoid();
	void groupBoids();
	void ungroupBoids();
	void enableFighting();
	void disableFighting();
	void resetBoids();

	std::vector<Boid*> boids;

protected:

	Boid* boid;
	int numBoids = 0;
	int updateNum = 0;
	int maxBoids = 0;
	int spawnTag = 1;
};

