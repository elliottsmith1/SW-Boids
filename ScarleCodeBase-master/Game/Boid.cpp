#include "Boid.h"
#include "BoidController.h"
#include "GameData.h"
#include "BoidData.h"

#include <iostream>

Boid::Boid(ID3D11Device* _pd3dDevice, int _id, BoidData* _boidData, int _type, int _numBoids)
{
	//max reserve local boids to max boids on screen
	nearBoids.reserve(_numBoids);

	ID = _id;
	m_boidData = _boidData;

	//for each of the 4 types, assign spawn point, tag and colour
	switch (_type)
	{
	case 1:
		colour = Vector4(1, 0, 0, 1);
		m_pos = Vector3(100, 1, 100);
		tag = 1;
		break;
	case 2:
		colour = Vector4(0, 1, 0, 1);
		m_pos = Vector3(100, 1, 500);
		tag = 2;
		break;
	case 3:
		colour = Vector4(0, 0, 1, 1);
		m_pos = Vector3(500, 1, 100);
		tag = 3;
		break;
	case 4:
		colour = Vector4(1, 1, 1, 1);
		m_pos = Vector3(500, 1, 500);
		tag = 4;
		break;
	}

	int numVerts = 24;
	m_numPrims = numVerts / 3;
	myVertex* m_vertices = new myVertex[numVerts];
	WORD* indices = new WORD[numVerts];

	//as using the standard VB shader set the tex-coords somewhere safe
	for (int i = 0; i<numVerts; i++)
	{
		indices[i] = i;
		m_vertices[i].texCoord = Vector2::One;
	}

	//in each loop create the two traingles for the matching sub-square on each face
	int vert = 0;

	//back			
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(1.0f, 0.0f, 1.0f);
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(0.0f, 0.0f, 0.0f);
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 0.0f);

	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(1.0f, 0.0f, 1.0f);
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 0.0f);
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(1.0f, 4.0f, 1.0f);

	//front
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(0.0f, 0.0f, 1.0f);
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(1.0f, 0.0f, 1.0f);
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 1.0f);

	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 1.0f);
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(1.0f, 0.0f, 1.0f);
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(1.0f, 4.0f, 1.0f);

	//side3
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 0.0f);
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(0.0f, 0.0f, 0.0f);
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(0.0f, 0.0f, 1.0f);

	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 1.0f);
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 0.0f);
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(0.0f, 0.0f, 1.0f);

	//top
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 0.0f);
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(0.0f, 4.0f, 1.0f);
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(1.0f, 4.0f, 1.0f);

	//bottom
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(1.0f, 0.0f, 1.0f);
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(0.0f, 0.0f, 1.0f);
	m_vertices[vert].Color = colour;
	m_vertices[vert++].Pos = Vector3(0.0f, 0.0f, 0.0f);


	//calculate the normals for the basic lighting in the base shader
	for (int i = 0; i<m_numPrims; i++)
	{
		WORD V1 = 3 * i;
		WORD V2 = 3 * i + 1;
		WORD V3 = 3 * i + 2;

		//build normals
		Vector3 norm;
		Vector3 vec1 = m_vertices[V1].Pos - m_vertices[V2].Pos;
		Vector3 vec2 = m_vertices[V3].Pos - m_vertices[V2].Pos;
		norm = vec1.Cross(vec2);
		norm.Normalize();

		m_vertices[V1].Norm = norm;
		m_vertices[V2].Norm = norm;
		m_vertices[V3].Norm = norm;
	}


	BuildIB(_pd3dDevice, indices);
	BuildVB(_pd3dDevice, numVerts, m_vertices);

	delete[] indices;    //this is no longer needed as this is now in the index Buffer
	delete[] m_vertices; //this is no longer needed as this is now in the Vertex Buffer
	m_vertices = nullptr;

	acceleration = Vector3(0, 0, 0);

	//give random starting rotation and velocity
	float angle = 0.1 + (rand() % (int)(359 - 0.1 + 1));

	SetYaw(angle);

	velocity = Vector3(cos(angle), 0, tan(angle));	

	spawnPoint = m_pos;

	position = m_pos;
	r = 2.0f;

	maxSpeedV = Vector3(m_boidData->maxSpeed, m_boidData->maxSpeed, m_boidData->maxSpeed);
	maxForceV = Vector3(m_boidData->maxForce, m_boidData->maxForce, m_boidData->maxForce);

	m_scale *= 3;
}

void Boid::Tick(GameData* _GD)
{
	VBGO::Tick(_GD);
}

void Boid::checkNearBoids(std::vector<Boid*> boids)
{
	//clear local boids
	nearBoids.clear();

	//assign any boids near enough to local vector
	for (int i = 0; i < boids.size(); i++)
	{
		float dis = Vector3::Distance(m_pos, boids[i]->GetPos());

		if ((dis > 0) && (dis < m_boidData->neighbourDis))
		{
			nearBoids.push_back(boids[i]);
		}
	}
}

void Boid::runBoid(std::vector<Boid*> boids, GameData* _GD)
{
	//move (using rules) when alive
	if (alive)
	{
		flock(boids);
		updateBoid(_GD);
		boundingBox();
	}

	//if standing when dead, fall
	else
	{
		if (m_pitch < 30)
		{
			fall();
		}
	}

	Tick(_GD);
}

void Boid::applyForce(Vector3 force)
{
	acceleration += force;
}

void Boid::flock(std::vector<Boid*> boids)
{
	//check which boids are near
	checkNearBoids(boids);

	//apply rules
	Vector3 sep = separate(15);			// Separation
	Vector3 ali = align();				// Alignment
	Vector3 coh = cohesion();			// Cohesion
	Vector3 bra = separate(100);		// Bravery	
		
	// Weightweight the forces
	sep *= m_boidData->seperationWeight;
	ali *= m_boidData->alignmentWeight;
	coh *= m_boidData->cohesionWeight;
	bra *= m_boidData->braveryWeight;	

	// Add the force vectors to acceleration
	applyForce(sep);
	applyForce(ali);
	applyForce(coh);
	applyForce(bra);
	
	//if fighting is enabled then call attack function(s)
	if (fighting)
	{
		attractEnemy();
	}

	//if grouping is enabled then call grouping function
	if (grouping)
	{
		applyGrouping();
	}
}

void Boid::updateBoid(GameData* _GD)
{
	//apply DT
	acceleration * _GD->m_dt;

	// Update velocity
	velocity += acceleration;

	velocity * _GD->m_dt / 60;

	velocity = XMVector3Normalize(velocity);

	// Limit speed
	velocity = XMVector3ClampLengthV(velocity, Vector3::Zero, Vector3(m_boidData->maxSpeed, m_boidData->maxSpeed, m_boidData->maxSpeed));

	//apply to position
	m_pos += velocity;

	// Reset acceleration to 0 each cycle
	acceleration *= 0;
}

Vector3 Boid::seek(Vector3 target)
{
	// A vector pointing from the position to the target
	Vector3 desired = (target - m_pos);  
	
	// Scale to maximum speed
	desired = XMVector3Normalize(desired);
	desired *= m_boidData->maxSpeed;

	// Steering = Desired minus Velocity
	Vector3 steer = (desired - velocity);
	steer = XMVector3ClampLengthV(steer, Vector3::Zero, Vector3(m_boidData->maxForce, m_boidData->maxForce, m_boidData->maxForce));

	return steer;
}

void Boid::boundingBox()
{
	int Xmin = 0, Xmax = 600, Zmin = 0, Zmax = 600;

	//if out of bounds then bounce back and reverse direction 
	if (m_pos.x < Xmin)
	{
		m_pos.x += 0.5f;
		velocity *= -1;
	}

	else if (m_pos.x > Xmax)
	{
		m_pos.x -= 0.5f;
		velocity *= -1;
	}

	if ((m_pos.y < 0) || (m_pos.y > 0))
	{
		m_pos.y = 0;
	}

	if (m_pos.z < Zmin)
	{
		m_pos.z += 0.5f;
		velocity *= -1;
	}

	else if (m_pos.z > Zmax)
	{
		m_pos.z -= 0.5f;
		velocity *= -1;
	}
}

Vector3 Boid::separate(int _sep)
{
	float desiredseparation = _sep;
	Vector3 steer = Vector3(0, 0, 0);
	int count = 0;

	// For every near boid, check if it's too close
	for (int i = 0; i < nearBoids.size(); i++)
	{
		float dis = Vector3::Distance(m_pos, nearBoids[i]->GetPos());

		if ((dis > 0) && (dis < desiredseparation))
		{			
			//if using function for bravery
			if (desiredseparation > 99)
			{
				//check other type
				if (!checkColour(this, nearBoids[i]))
				{
					if (nearBoids[i]->getAlive())
					{
						// Calculate vector pointing away from neighbor
						Vector3 diff = (m_pos - nearBoids[i]->GetPos());
						diff = XMVector3Normalize(diff);
						diff = (diff / dis);        // Weight by distance
						steer += (diff * -1);
						count++;            // Keep track of how many
					}
				}
			}

			//else normal separation 
			else 
			{
				// Calculate vector pointing away from neighbor
				Vector3 diff = (m_pos - nearBoids[i]->GetPos());
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
		steer *= m_boidData->maxSpeed;
		steer -= velocity;
		steer = XMVector3ClampLengthV(steer, Vector3::Zero, Vector3(m_boidData->maxForce, m_boidData->maxForce, m_boidData->maxForce));
	}

	return steer;
}

void Boid::applyGrouping()
{
	//for each type, force to move towards original spawn points to group up
	switch (tag)
	{
	case 1:
		applyForce(seek(Vector3(100, 1, 100)) * 2);
		break;
	case 2:
		applyForce(seek(Vector3(100, 1, 500)) * 2);
		break;
	case 3:
		applyForce(seek(Vector3(500, 1, 100)) * 2);
		break;
	case 4:
		applyForce(seek(Vector3(500, 1, 500)) * 2);
		break;
	}
}

Vector3 Boid::align()
{
	Vector3 sum = Vector3(0, 0, 0);
	int count = 0;

	//check all near boids
	for (int i = 0; i < nearBoids.size(); i++)
	{
		if (nearBoids[i] != this)
		{
			//check same type
			if (checkColour(this, nearBoids[i]))
			{
				float dis = Vector3::Distance(m_pos, nearBoids[i]->GetPos());

				if ((dis > 0) && (dis < m_boidData->neighbourDis))
				{
					if (nearBoids[i]->getAlive())
					{
						//get velocity 
						sum += nearBoids[i]->GetVelocity();
						count++;
					}
				}
			}
		}
	}

	if (count > 0)
	{
		//average
		sum /= ((float)count);

		sum = XMVector3Normalize(sum);
		sum *= m_boidData->maxSpeed;
		Vector3 steer = (sum - velocity);
		steer = XMVector3ClampLengthV(steer, Vector3::Zero, Vector3(m_boidData->maxForce, m_boidData->maxForce, m_boidData->maxForce));
		return steer;
	}

	else
	{
		return Vector3(0, 0, 0);
	}
}

Vector3 Boid::cohesion()
{
	Vector3 sum = Vector3(0, 0, 0);   
	int count = 0;

	//check all near boids
	for (int i = 0; i < nearBoids.size(); i++)
	{
		if (nearBoids[i] != this)
		{
			//check same type
			if (checkColour(this, nearBoids[i]))
			{
				if (Vector3::Distance(m_pos, nearBoids[i]->GetPos()) < m_boidData->neighbourDis)
				{
					if (nearBoids[i]->getAlive())
					{
						//get position
						sum += nearBoids[i]->GetPos();
						count++;
					}
				}
			}
		}
	}

	if (count > 0)
	{
		//average 
		sum /= count;
		return seek(sum);  
	}

	else
	{
		return Vector3(0, 0, 0);
	}	
}

void Boid::attractEnemy()
{
	float enemyDis = 10.0f;

	//check all near boids
	for (int i = 0; i < nearBoids.size(); i++)
	{
		if (nearBoids[i] != this)
		{
			//check other type
			if (!checkColour(this, nearBoids[i]))
			{
				if (Vector3::Distance(m_pos, nearBoids[i]->GetPos()) < enemyDis)
				{
					if (nearBoids[i]->getAlive())
					{
						//attack
						attackEnemy(nearBoids[i]);
						break;
					}
				}
			}
		}
	}
}

void Boid::resetBoid()
{
	//reset all variables to respawn
	m_pos = spawnPoint;
	alive = true;
	health = 100;
	grouping = false;
	fighting = false;
	m_pitch = 0;
	active = false;
}

bool Boid::checkColour(Boid* b, Boid* c)
{
	//check tag of other boid against own
	if ((*b).tag == (*c).tag)
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

void Boid::SetActive(bool act)
{
	active = act;
}

bool Boid::GetActive()
{
	return active;
}

int Boid::GetID()
{
	return ID;
}

void Boid::setGrouping(bool _group)
{
	grouping = _group;
}

bool Boid::getGrouping()
{
	return grouping;
}

void Boid::attackEnemy(Boid * _enemy)
{	
	//attack
	_enemy->setHealth(_enemy->getHealth() - 1);

	//kill
	if (_enemy->getHealth() <= 0)
	{
		_enemy->setAlive(false);
	}
}

void Boid::setHealth(int _health)
{
	health = _health;
}

int Boid::getHealth()
{
	return health;
}

void Boid::setAlive(bool _alive)
{
	alive = _alive;
}

bool Boid::getAlive()
{
	return alive;
}

void Boid::fall()
{
	m_pitch += (1.5f - m_pitch) * 0.15f;
}

void Boid::setFighting(bool _fighting)
{
	fighting = _fighting;
}

bool Boid::getFighting()
{
	return fighting;
}
