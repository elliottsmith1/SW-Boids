#pragma once

#include "gameobject.h"
#include "Model.h"
#include <d3d11_1.h>
#include <vector>
#include <string>

struct GameData;
struct DrawData;
class Boid;

class BoidController
{
public:
	BoidController(int numBoids, std::string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF);
	~BoidController() = default; 

	void Tick(GameData* _GD);
	void DrawBoids(DrawData* _DD);
	void SpawnBoid(int tag);

	std::vector<Boid*> boids;

protected:

	Boid* boid;

};

