#include "Boid.h"
#include "BoidController.h"
#include "GameData.h"
#include <iostream>

Boid::Boid(ID3D11Device* _pd3dDevice, int _id)
{
	ID = _id;
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

	//in each loop create the two traingles for the matching sub-square on each of the six faces
	int vert = 0;

	if (ID % 2 == 0)
	{
		colour = Vector4(1, 0, 0, 1);
		tag = 1;
	}

	else
	{
		colour = Vector4(0, 1, 0, 1);
		tag = 2;
	}

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

	float angle = 0.1 + (rand() % (int)(359 - 0.1 + 1));

	SetYaw(angle);

	velocity = Vector3(cos(angle), 0, tan(angle));

	float pos1 = -50 + (rand() % (int)(660 - -50 + 1));
	float pos2 = -50 + (rand() % (int)(660 - -50 + 1));

	/*std::cout << pos1 << ", ";
	std::cout << pos2 << std::endl;*/

	m_pos = Vector3(pos1, 1, pos2);
	position = m_pos;
	r = 2.0f;

	maxforce = 0.03f;
	maxspeed = 0.5f;

	maxSpeedV = Vector3(maxspeed, maxspeed, maxspeed);
	maxForceV = Vector3(maxforce, maxforce, maxforce);

	m_scale *= 3;
}

void Boid::Tick(GameData* _GD)
{
	VBGO::Tick(_GD);
}

void Boid::runBoid(std::vector<Boid*> boids, GameData* _GD)
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

void Boid::flock(std::vector<Boid*> boids)
{
	Vector3 sep = separate(boids, 10);   // Separation
	Vector3 ali = align(boids);      // Alignment
	Vector3 coh = cohesion(boids);   // Cohesion
	Vector3 rep = separate(boids, 100);
		
	// Arbitrarily weight these forces
	sep *= 5.0f;
	ali *= 2.0f;
	coh *= 1.0f;
	rep *= 2.0f;

	// Add the force vectors to acceleration
	applyForce(sep);
	applyForce(ali);
	applyForce(coh);
	applyForce(rep);
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
	int Xmin = -50, Xmax = 600, Zmin = -50, Zmax = 600;

	if (m_pos.x < Xmin)
	{
		m_pos.x += 600;
	}

	else if (m_pos.x > Xmax)
	{
		m_pos.x -= 600;
	}

	if ((m_pos.y < 0) || (m_pos.y > 0))
	{
		m_pos.y = 0;
	}

	if (m_pos.z < Zmin)
	{
		m_pos.z += 600;
	}

	else if (m_pos.z > Zmax)
	{
		m_pos.z -= 600; 
	}
}

Vector3 Boid::separate(std::vector<Boid*> boids, int _sep)
{
	float desiredseparation = _sep;
	Vector3 steer = Vector3(0, 0, 0);
	int count = 0;
	// For every boid in the system, check if it's too close
	for (int i = 0; i < boids.size(); i++)
	{
		float dis = Vector3::Distance(m_pos, boids[i]->GetPos());

		if ((dis > 0) && (dis < desiredseparation))
		{			
			if (desiredseparation > 99)
				{
					if (!checkColour(this, boids[i]))
					{
						// Calculate vector pointing away from neighbor
						Vector3 diff = (m_pos - boids[i]->GetPos());
						diff = XMVector3Normalize(diff);
						diff = (diff / dis);        // Weight by distance
						steer += diff;
						count++;            // Keep track of how many
					}
				}
			else 
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

Vector3 Boid::repel(std::vector<Boid*> boids)
{
	float desiredseparation = 100.0f;
	Vector3 steer = Vector3(0, 0, 0);
	int count = 0;
	// For every boid in the system, check if it's too close
	for (int i = 0; i < boids.size(); i++)
	{
		float dis = Vector3::Distance(m_pos, boids[i]->GetPos());

		if ((dis > 0) && (dis < desiredseparation))
		{
			if (!checkColour(this, boids[i]))
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

Vector3 Boid::align(std::vector<Boid*> boids)
{
	float neighbordist = 30.0f;
	Vector3 sum = Vector3(0, 0, 0);
	int count = 0;
	for (int i = 0; i < boids.size(); i++)
	{
		if (boids[i] != this)		
		{
			if (checkColour(this, boids[i]))
			{
				float dis = Vector3::Distance(m_pos, boids[i]->GetPos());

				if ((dis > 0) && (dis < neighbordist))
				{
					sum += boids[i]->GetVelocity();
					count++;
				}
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

Vector3 Boid::cohesion(std::vector<Boid*> boids)
{
	float neighbordist = 30.0f;
	Vector3 sum = Vector3(0, 0, 0);   
	int count = 0;
	for (int i = 0; i < boids.size(); i++)
	{
		if (boids[i] != this)		
		{
			if (checkColour(this, boids[i]))
			{
				if (Vector3::Distance(m_pos, boids[i]->GetPos()) < neighbordist)
				{
					sum += boids[i]->GetPos();
					count++;
				}
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

bool Boid::checkColour(Boid* b, Boid* c)
{
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

void Boid::SetTag(int num)
{
	tag = num; 

	if (tag == 1)
	{
		colour = Vector4(1, 0, 0, 0);
	}

	else if (tag == 2)
	{
		colour = Vector4(0, 0, 0, 1);
	}
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
