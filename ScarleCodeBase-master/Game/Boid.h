#include "VBGO.h"
#include "vertex.h"

#include <vector>
#include <memory>

struct BoidData;

class Boid : public VBGO
{

public:
	Boid(ID3D11Device* _pd3dDevice, int _id, BoidData* _boidData);
	~Boid() = default;

	void Tick(GameData* _GD) override;

	void runBoid(std::vector<Boid*> boids, GameData* _GD);
	void applyForce(Vector3 force);
	void flock(std::vector<Boid*> boids);
	void updateBoid();
	Vector3 seek(Vector3 target);
	void boundingBox();
	Vector3 separate(std::vector<Boid*> boids, int _sep);
	Vector3 align(std::vector<Boid*> boids);
	Vector3 cohesion(std::vector<Boid*> boids);
	Vector3 attractEnemy(std::vector<Boid*> boids);
	Vector3 repel(std::vector<Boid*> boids);
	void applyGrouping();
	bool checkColour(Boid* b, Boid* c);
	Vector3 GetVelocity();
	int GetTag();
	void SetTag(int num);
	void SetActive(bool act);
	bool GetActive();
	int GetID();
	void setGrouping(bool _group);
	bool getGrouping();
	void attackEnemy(Boid* _enemy);

protected:
	myVertex* m_vertices;
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	Vector3 maxSpeedV;
	Vector3 maxForceV;
	float r; 
	int tag = 1;
	bool active = false;
	bool grouping = false;
	int ID = 0;
	Vector4 colour = Vector4(1, 0, 0, 1);
	BoidData* m_boidData;
};