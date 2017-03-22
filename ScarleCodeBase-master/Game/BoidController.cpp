#include "BoidController.h"
#include "GameData.h"
#include "Boid.h"
#include "BoidData.h"

#include <iostream>

BoidController::BoidController(int _numBoids, ID3D11Device * _pd3dDevice, IEffectFactory * _EF, BoidData* _boidData)
{
	//set vector
	maxBoids = _numBoids;

	boids.reserve(maxBoids);

	//create boids
	for (int i = 0; i < (maxBoids); i++)
	{
		boid = new Boid(_pd3dDevice, i, _boidData, spawnTag, _numBoids);
		boids.push_back(boid);

		//1 type at a time
		spawnTag++;

		if (spawnTag > 4)
		{
			spawnTag = 1;
		}
	}
}

BoidController::~BoidController()
{
	//delete boids
	for (int i = 0; i < boids.size(); i++)
	{
		delete boids[i];
	}

	boids.clear();
}

void BoidController::Tick(GameData* _GD)
{
	//tick boids if active
	for (int i = 0; i < boids.size(); i++)
	{
		if (boids[i]->GetActive())
		{
			//only tick 100 at a time
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
	//draw all boids if active
	for (int i = 0; i < boids.size(); i++)
	{
		if (boids[i]->GetActive())
		{
			boids[i]->Draw(_DD);
		}
	}
}

void BoidController::SpawnBoid(bool _rand)
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

				//if spawning randomly, assign random spawn points 
				if (_rand)
				{
					float point1 = -49 + (rand() % (int)(559 - -49 + 1));
					float point2 = -49 + (rand() % (int)(559 - -49 + 1));

					boids[i]->SetPos(Vector3(point1, 1, point2));
				}

				//one type at a time
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
	//set all boids to group up
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
	//set all boids to no longer group up
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
	//set all boids to fight
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
	//set all boids to not fight
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
	//call reset on each boid
	for (int i = 0; i < boids.size(); i++)
	{
		boids[i]->resetBoid();
	}
}




