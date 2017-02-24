#include "BoidController.h"
#include "GameData.h"
#include "Boid.h"

BoidController::BoidController(int num, std::string _fileName, ID3D11Device * _pd3dDevice, IEffectFactory * _EF)
{
	boids.reserve(num);

	for (int i = 0; i < num; i++)
	{
		boid = new Boid(_fileName, _pd3dDevice, _EF);
		boids.push_back(boid);
	}
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
	for (int i = 0; i < boids.size(); i++)
	{
		if (boids[i]->GetActive() != true)
		{
			boids[i]->SetActive(true);

			switch (tag)
			{
			case 1:
				boids[i]->SetTag(1);
				break;
			case 2:
				boids[i]->SetTag(2);
				break;
			}
		}
		return;
	}
}




