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

void BoidController::Tick(GameData* _GD, DrawData* _DD)
{
	for (int i = 0; i < boids.size(); i++)
	{
		boids[i]->runBoid(boids);
	}

	if (_GD->m_keyboardState[DIK_B] & 0x80)
	{
		for (int i = 0; i < boids.size(); i++)
		{
			if (boids[i]->GetActive() != true)
			{
				boids[i]->SetActive(true);
				boids[i]->SetTag(1);
				return;
			}
		}
	}

	if (_GD->m_keyboardState[DIK_R] & 0x80)
	{
		for (int i = 0; i < boids.size(); i++)
		{
			if (boids[i]->GetActive() != true)
			{
				boids[i]->SetActive(true);
				boids[i]->SetTag(2);
				return;
			}
		}
	}

	Draw(_DD);
}

void BoidController::Draw(DrawData* _DD)
{
	for (int i = 0; i < boids.size(); i++)
	{
		boids[i]->Draw(_DD);
	}

}

