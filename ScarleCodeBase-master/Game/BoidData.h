
struct BoidData
{
	float maxSpeed = 0.0f;
	float maxForce = 0.03f;

	float seperationWeight = 1.0f;
	float cohesionWeight = 1.0f;
	float alignmentWeight = 1.0f;
	float braveryWeight = 2.0f;

	float neighbourDis = 1.0f;
};

