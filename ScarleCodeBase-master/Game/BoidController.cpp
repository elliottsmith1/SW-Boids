#include "BoidController.h"
#include "GameData.h"

void BoidController::Tick(GameData* _GD)
{
	for (int i = 0; i < boids.size; i++)
	{
		boids[i]->runBoid(boids);
	}

	if (_GD->m_keyboardState[DIK_B] & 0x80)
	{
		//Instantiate(blueBoid, new Vector3(12, 0, 12), Quaternion.identity);
	}

	if (_GD->m_keyboardState[DIK_R] & 0x80)
	{
		//Instantiate(redBoid, new Vector3(12, 0, 12), Quaternion.identity);
	}
}

void BoidController::addBoid(GameObject* b)
{
	boids.push_back(b);
}
