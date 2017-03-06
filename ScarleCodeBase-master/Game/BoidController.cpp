#include "BoidController.h"
#include "GameData.h"
#include "Boid.h"

#include <iostream>

BoidController::BoidController(int numBoids, std::string _fileName, ID3D11Device * _pd3dDevice, IEffectFactory * _EF)
{
	boids.reserve(numBoids);

	for (int i = 0; i < numBoids; i++)
	{
		boid = new Boid(_fileName, _pd3dDevice, _EF);
		boids.push_back(boid);
	}
}

BoidController::~BoidController()
{
	for (int i = 0; i < boids.size(); i++)
	{
		delete boids[i];
	}

	boids.clear();
}

void BoidController::Tick(GameData* _GD)
{
	for (int i = 0; i < boids.size(); i++)
	{
		if (boids[i]->GetActive())
		{
			boids[i]->runBoid(boids, _GD);
		}
	}
}

void BoidController::DrawBoids(DrawData* _DD)
{
	for (int i = 0; i < boids.size(); i++)
	{
		if (boids[i]->GetActive())
		{
			boids[i]->Draw(_DD);
		}
	}
}

void BoidController::SpawnBoid(int tag)
{
	// Loop through all boids
	for (int i = 0; i < boids.size(); i++)
	{
		//Check if boid is active,
		if (boids[i]->GetActive() == false)
		{
			// If not, spawn a new one and set tag
			boids[i]->SetActive(true);
			switch (tag)
			{
			case 1:
				boids[i]->SetTag(1);
				break;
			case 2:
				boids[i]->SetTag(2);
				break;
			default:
				boids[i]->SetTag(1);
				break;
			}
			return;
		}
	}
}




