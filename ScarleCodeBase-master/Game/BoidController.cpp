#include "BoidController.h"
#include "GameData.h"
#include "Boid.h"

#include <iostream>

BoidController::BoidController(int _numBoids, std::string _fileName, ID3D11Device * _pd3dDevice, IEffectFactory * _EF, BoidData* _boidData)
{
	numBoids = _numBoids;

	boids.reserve(numBoids);	

	for (int i = 0; i < numBoids; i++)
	{
		boid = new Boid(_pd3dDevice, i, _boidData);
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
			if ((boids[i]->GetID() >= updateNum) && (boids[i]->GetID() < (updateNum + 100)))
			{
				boids[i]->runBoid(boids, _GD);
			}
		}
	}

	updateNum += 100;

	if (updateNum > numBoids)
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
			/*switch (tag)
			{
			case 1:
				boids[i]->SetTag(1);
				break;
			case 2:
				boids[i]->SetTag(2);
				break;
			default:
				break;
			}*/
			return;
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




