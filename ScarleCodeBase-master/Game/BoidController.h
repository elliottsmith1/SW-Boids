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
	BoidController(int _numBoids, std::string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, BoidData* _boidData);
	~BoidController(); 

	void Tick(GameData* _GD);
	void DrawBoids(DrawData* _DD);
	void SpawnBoid(int tag);
	void groupBoids();
	void ungroupBoids();

	std::vector<Boid*> boids;

protected:

	Boid* boid;
	int numBoids;
	int updateNum = 0;

};

