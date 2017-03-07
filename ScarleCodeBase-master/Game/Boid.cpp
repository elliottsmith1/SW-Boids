#include "Boid.h"
#include "BoidController.h"
#include "GameData.h"
#include <iostream>

Boid::Boid(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	acceleration = Vector3(0, 0, 0);

	float angle = 0.1 + (rand() % (int)(359 - 0.1 + 1));
	velocity = Vector3(cos(angle), 0, tan(angle));

	float pos1 = -50 + (rand() % (int)(160 - -50 + 1));
	float pos2 = -50 + (rand() % (int)(160 - -50 + 1));

	/*std::cout << pos1 << ", ";
	std::cout << pos2 << std::endl;*/

	m_pos = Vector3(pos1, 1, pos2);
	position = m_pos;
	r = 2.0f;

	maxforce = 0.03f;
	maxspeed = 0.2f;

	maxSpeedV = Vector3(maxspeed, maxspeed, maxspeed);
	maxForceV = Vector3(maxforce, maxforce, maxforce);
}

void Boid::Tick(GameData* _GD)
{
	CMOGO::Tick(_GD);
}

void Boid::runBoid(vector<Boid*> boids, GameData* _GD)
{
	flock(boids);
	updateBoid();
	boundingBox();

	Tick(_GD);
}

void Boid::applyForce(Vector3 force)
{
	// We could add mass here if we want A = F / M
	acceleration += force;
}

void Boid::flock(vector<Boid*> boids)
{
	Vector3 sep = separate(boids);   // Separation
	Vector3 ali = align(boids);      // Alignment
	Vector3 coh = cohesion(boids);   // Cohesion

	// Arbitrarily weight these forces
	sep *= 1.3f;
	ali *= 1.0f;
	coh *= 1.0f;

	// Add the force vectors to acceleration
	applyForce(sep);
	applyForce(ali);
	applyForce(coh);
}

void Boid::updateBoid()
{
	// Update velocity
	velocity += acceleration;

	velocity = XMVector3Normalize(velocity);

	// Limit speed
	//velocity = XMVector3ClampLength(velocity, 0, maxspeed);
	velocity = XMVector3ClampLengthV(velocity, Vector3::Zero, maxSpeedV);

	m_pos += velocity;

	// Reset acceleration to 0 each cycle
	acceleration *= 0;
}

Vector3 Boid::seek(Vector3 target)
{
	Vector3 desired = (target - m_pos);  // A vector pointing from the position to the target
	
	// Scale to maximum speed
	desired = XMVector3Normalize(desired);
	desired *= maxspeed;

	// Steering = Desired minus Velocity
	Vector3 steer = (desired - velocity);
	steer = XMVector3ClampLengthV(steer, Vector3::Zero, maxForceV);

	return steer;
}

void Boid::boundingBox()
{
	int Xmin = -50, Xmax = 160, Zmin = -50, Zmax = 160;

	if (m_pos.x < Xmin)
	{
		m_pos.x += 220;
	}

	else if (m_pos.x > Xmax)
	{
		m_pos.x -= 220;
	}

	if ((m_pos.y < 0) || (m_pos.y > 0))
	{
		m_pos.y = 0;
	}

	if (m_pos.z < Zmin)
	{
		m_pos.z += 220;
	}

	else if (m_pos.z > Zmax)
	{
		m_pos.z -= 220; 
	}
}

Vector3 Boid::separate(vector<Boid*> boids)
{
	float desiredseparation = 7.0f;
	Vector3 steer = Vector3(0, 0, 0);
	int count = 0;
	// For every boid in the system, check if it's too close
	for (int i = 0; i < boids.size(); i++)
	{
		float dis = Vector3::Distance(m_pos, boids[i]->GetPos());

		//if (checkColour(this, boids[i]))
		{
			if ((dis > 0) && (dis < desiredseparation))
			{
				// Calculate vector pointing away from neighbor
				Vector3 diff = (m_pos - boids[i]->GetPos());
				diff = XMVector3Normalize(diff);
				diff = (diff / dis);        // Weight by distance
				steer += diff;
				count++;            // Keep track of how many
			}
		}
	}
	// Average -- divide by how many
	if (count > 0)
	{
		steer /= ((float)count);
	}

	// As long as the vector is greater than 0
	if (steer.Length() > 0)
	{
		steer = XMVector3Normalize(steer);
		steer *= maxspeed;
		steer -= velocity;
		steer = XMVector3ClampLengthV(steer, Vector3::Zero, maxForceV);
	}
	return steer;
}

Vector3 Boid::align(vector<Boid*> boids)
{
	float neighbordist = 10.0f;
	Vector3 sum = Vector3(0, 0, 0);
	int count = 0;
	for (int i = 0; i < boids.size(); i++)
	{
		//if (checkColour(this, boids[i]))
		if (boids[i] != this)
		{
			float dis = Vector3::Distance(m_pos, boids[i]->GetPos());

			if ((dis > 0) && (dis < neighbordist))
			{
				sum += boids[i]->GetVelocity();
				count++;
			}
		}
	}

	if (count > 0)
	{
		sum /= ((float)count);

		sum = XMVector3Normalize(sum);
		sum *= maxspeed;
		Vector3 steer = (sum - velocity);
		steer = XMVector3ClampLengthV(steer, Vector3::Zero, maxForceV);
		return steer;
	}

	else
	{
		return Vector3(0, 0, 0);
	}
}

Vector3 Boid::cohesion(vector<Boid*> boids)
{
	float neighbordist = 10.0f;
	Vector3 sum = Vector3(0, 0, 0);   
	int count = 0;
	for (int i = 0; i < boids.size(); i++)
	{
		//if (checkColour(this, boids[i]))
		if (boids[i] != this)
		{
			if (Vector3::Distance(m_pos, boids[i]->GetPos()) < neighbordist)
			{
				sum += boids[i]->GetPos(); 
				count++;
			}
		}
	}

	if (count > 0)
	{
		sum /= count;
		return seek(sum);  
	}
	else
	{
		return Vector3(0, 0, 0);
	}
}

bool Boid::checkColour(Boid b, Boid c)
{
	if (b.tag == c.tag)
	{
		return true;
	}

	else
	{
		return false;
	}
}

Vector3 Boid::GetVelocity()
{
	return velocity;
}

int Boid::GetTag()
{
	return tag;
}

void Boid::SetTag(int num)
{
	tag = num; 
}

void Boid::SetActive(bool act)
{
	active = act;
}

bool Boid::GetActive()
{
	return active;
}
