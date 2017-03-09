#include "VBGO.h"
#include "vertex.h"
#include <vector>
#include <memory>

class Boid : public VBGO
{

public:
	Boid(ID3D11Device* _pd3dDevice, int _id);
	~Boid() = default;

	void Tick(GameData* _GD) override;

	void runBoid(std::vector<Boid*> boids, GameData* _GD);
	void applyForce(Vector3 force);
	void flock(std::vector<Boid*> boids);
	void updateBoid();
	Vector3 seek(Vector3 target);
	void boundingBox();
	Vector3 separate(std::vector<Boid*> boids);
	Vector3 align(std::vector<Boid*> boids);
	Vector3 cohesion(std::vector<Boid*> boids);
	bool checkColour(Boid b, Boid c);
	Vector3 GetVelocity();
	int GetTag();
	void SetTag(int num);
	void SetActive(bool act);
	bool GetActive();
	int GetID();

protected:
	myVertex* m_vertices;
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	Vector3 maxSpeedV;
	Vector3 maxForceV;
	float r;
	float maxforce;    
	float maxspeed;   
	int tag = 1;
	bool active = false;
	int ID = 0;
	Vector4 colour = Vector4(1, 0, 0, 1);
};