#include "BoidController.h"
#include "GameData.h"
#include "Boid.h"
#include "BoidData.h"

#include <iostream>

BoidController::BoidController(int _numBoids, ID3D11Device * _pd3dDevice, IEffectFactory * _EF, BoidData* _boidData)
{
	maxBoids = _numBoids;

	int maxAllBoids = maxBoids * 4;

	boids.reserve(maxBoids);

	for (int i = 0; i < (maxBoids); i++)
	{
		boid = new Boid(_pd3dDevice, i, _boidData, spawnTag);
		boids.push_back(boid);

		spawnTag++;

		if (spawnTag > 4)
		{
			spawnTag = 1;
		}
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
			if ((boids[i]->GetID() >= updateNum) && (boids[i]->GetID() < (updateNum + 100)))
			{
				boids[i]->runBoid(boids, _GD);
			}
		}
	}

	updateNum += 100;

	if (updateNum > maxBoids)
	{
		updateNum = 0;
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

void BoidController::SpawnBoid()
{
	// Loop through all boids
	for (int i = 0; i < boids.size(); i++)
	{
		//spawn 1 type at a time
		if (boids[i]->GetTag() == spawnTag)
		{
			//Check if boid is active,
			if (boids[i]->GetActive() == false)
			{
				// If not, spawn a new one and set tag
				boids[i]->SetActive(true);

				spawnTag++;

				if (spawnTag > 4)
				{
					spawnTag = 1;
				}

				//return;
			}
		}
	}
}

void BoidController::groupBoids()
{
	for (int i = 0; i < boids.size(); i++)
	{
		if (!boids[i]->getGrouping())
		{
			boids[i]->setGrouping(true);
		}
	}
}

void BoidController::ungroupBoids()
{
	for (int i = 0; i < boids.size(); i++)
	{
		if (boids[i]->getGrouping())
		{
			boids[i]->setGrouping(false);
		}
	}
}

void BoidController::enableFighting()
{
	for (int i = 0; i < boids.size(); i++)
	{
		if (!boids[i]->getGrouping())
		{
			boids[i]->setFighting(true);
		}
	}
}

void BoidController::disableFighting()
{
	for (int i = 0; i < boids.size(); i++)
	{
		if (!boids[i]->getGrouping())
		{
			boids[i]->setFighting(false);
		}
	}
}

void BoidController::resetBoids()
{
	for (int i = 0; i < boids.size(); i++)
	{
		boids[i]->resetBoid();
	}
}




