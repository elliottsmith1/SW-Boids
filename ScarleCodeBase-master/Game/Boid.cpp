#include "Boid.h"
#include "GameData.h"

Boid::Boid(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	GameObject.FindGameObjectWithTag("Control").GetComponent<ControlScript>().addBoid(this);

	acceleration = new Vector3(0, 0, 0);

	float angle = Random.Range(0.1f, 359.0f);
	velocity = new Vector3(cos(angle), 0, sin(angle));

	position = m_pos;
	r = 2.0f;
	maxspeed = 0.2f;
	maxforce = 0.03f;
}

Boid::~Boid()
{
	//
}


void Boid::Tick(GameData* _GD)
{


	CMOGO::Tick(_GD);
}

void Boid::runBoid(vector<std::unique_ptr<Boid>> boids)
{
	flock(boids);
	updateBoid();
}

void Boid::applyForce(Vector3 force)
{
	// We could add mass here if we want A = F / M
	acceleration += force;
}

void Boid::flock(vector<std::unique_ptr<Boid>> boids)
{
	Vector3 sep = separate(boids);   // Separation
	Vector3 ali = align(boids);      // Alignment
	Vector3 coh = cohesion(boids);   // Cohesion
	Vector3 boun = Vector3(0, 0, 0);

	for (int i = 0; i < boids.size; i++)
	{
		boun = boundingBox(boids[i]);
	}

	// Arbitrarily weight these forces
	sep *= 1.5f;
	ali *= 1.0f;
	coh *= 1.0f;
	boun *= 1.0f;
	// Add the force vectors to acceleration
	applyForce(boun);
	applyForce(sep);
	applyForce(ali);
	applyForce(coh);
}

void Boid::updateBoid()
{
	// Update velocity
	velocity += acceleration;
	// Limit speed
	velocity = Vector3.ClampMagnitude(velocity, maxspeed);
	m_pos += velocity;
	// Reset accelertion to 0 each cycle
	acceleration *= 0;
}

Vector3 Boid::seek(Vector3 target)
{
	Vector3 desired = (target - m_pos);  // A vector pointing from the position to the target
													  // Scale to maximum speed
	desired.Normalize();
	desired *= maxspeed;

	// Steering = Desired minus Velocity
	Vector3 steer = (desired - velocity);
	steer = Vector3.ClampMagnitude(steer, maxforce);  // Limit to maximum steering force
	return steer;
}

Vector3 Boid::boundingBox(Boid b)
{
	int Xmin = 1, Xmax = 25, Ymin = -1, Ymax = 1, Zmin = 1, Zmax = 25;
	Vector3 v = Vector3(0, 0, 0);

	if (b.m_pos.x < Xmin)
	{
		v.x += 0.5f;
	}

	else if (b.m_pos.x > Xmax)
	{
		v.x += -0.5f;

	}
	if (b.m_pos.y < Ymin)
	{
		v.y += 0.1f;
	}

	else if (b.m_pos.y > Ymax)
	{
		v.y += -0.1f;
	}

	if (b.m_pos.z < Zmin)
	{
		v.z += 0.5f;
	}

	else if (b.m_pos.z > Zmax)
	{
		v.z += -0.5f;
	}

	return v;
}

Vector3 Boid::separate(vector<std::unique_ptr<Boid>> boids)
{
	float desiredseparation = 1.0f;
	Vector3 steer = Vector3(0, 0, 0);
	int count = 0;
	// For every boid in the system, check if it's too close
	for (int i = 0; i < boids.size; i++)
	{
		if (checkColour(this, boids[i]))
		{
			float d = Vector3.Distance(m_pos, boids[i].m_pos);
			// If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
			if ((d > 0) && (d < desiredseparation))
			{
				// Calculate vector pointing away from neighbor
				Vector3 diff = (m_pos - boids[i].m_pos);
				diff.Normalize();
				diff /= d;        // Weight by distance
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
	if (steer.magnitude > 0)
	{
		// Implement Reynolds: Steering = Desired - Velocity
		steer.Normalize();
		steer *= maxspeed;
		steer -= velocity;
		steer = Vector3.ClampMagnitude(steer, maxforce);
	}
	return steer;
}

Vector3 Boid::align(vector<std::unique_ptr<Boid>> boids)
{
	float neighbordist = 2.5f;
	Vector3 sum = Vector3(0, 0);
	int count = 0;
	for (int i = 0; i < boids.size; i++)
	{
		if (checkColour(this, boids[i]))
		{
			float d = Vector3.Distance(m_pos, boids[i].m_pos);
			if ((d > 0) && (d < neighbordist))
			{
				sum += boids[i].velocity;
				count++;
			}
		}
	}
	if (count > 0)
	{
		sum /= ((float)count);

		// Implement Reynolds: Steering = Desired - Velocity
		sum.Normalize();
		sum *= maxspeed;
		Vector3 steer = (sum - velocity);
		steer = Vector3.ClampMagnitude(steer, maxforce);
		return steer;
	}
	else
	{
		return Vector3(0, 0, 0);
	}
}

Vector3 Boid::cohesion(vector<std::unique_ptr<Boid>> boids)
{
	float neighbordist = 2.5f;
	Vector3 sum = Vector3(0, 0, 0);   
	int count = 0;
	for (int i = 0; i < boids.size; i++)
	{
		if (checkColour(this, boids[i]))
		{
			float d = Vector3.Distance(m_pos, boids[i].m_pos);
			if ((d > 0) && (d < neighbordist))
			{
				sum += boids[i]->GetPos(); // Add position
				count++;
			}
		}
	}

	if (count > 0)
	{
		sum /= count;
		return seek(sum);  // Steer towards the position
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
