
struct BoidData
{
	float maxSpeed = 0.4f;
	float maxForce = 0.04f;

	float seperationWeight = 4.0f;
	float cohesionWeight = 1.0f;
	float alignmentWeight = 1.0f;
	float braveryWeight = -2.0f;

	float neighbourDis = 50.0f;
};

