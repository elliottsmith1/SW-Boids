#include "VBGO.h"
#include "vertex.h"

#include <vector>
#include <memory>

struct BoidData;

class Boid : public VBGO
{

public:
	Boid(ID3D11Device* _pd3dDevice, int _id, BoidData* _boidData, int _type, int _numBoids);
	~Boid() = default;

	void Tick(GameData* _GD) override;

	void checkNearBoids(std::vector<Boid*> boids);
	void runBoid(std::vector<Boid*> boids, GameData* _GD);
	void applyForce(Vector3 force);
	void flock(std::vector<Boid*> boids);
	void updateBoid(GameData* _GD);
	Vector3 seek(Vector3 target);
	void boundingBox();
	Vector3 separate(int _sep);
	Vector3 align();
	Vector3 cohesion();	
	void applyGrouping();
	bool checkColour(Boid* b, Boid* c);
	Vector3 GetVelocity();
	int GetTag();
	void SetActive(bool act);
	bool GetActive();
	int GetID();
	void setGrouping(bool _group);
	bool getGrouping();
	void attackEnemy(Boid* _enemy);
	void setHealth(int _health);
	int getHealth();
	void setAlive(bool _alive);
	bool getAlive();
	void fall();
	void setFighting(bool _fighting);
	bool getFighting();
	void attractEnemy();
	void resetBoid();

protected:
	myVertex* m_vertices;
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	Vector3 maxSpeedV;
	Vector3 maxForceV;
	Vector3 spawnPoint;
	float r; 
	int tag = 1;
	bool active = false;
	bool grouping = false;
	bool fighting = false;
	int ID = 0;
	Vector4 colour = Vector4(1, 0, 0, 1);
	BoidData* m_boidData;
	int health = 150;
	bool alive = true; 

	std::vector<Boid*> nearBoids;
};