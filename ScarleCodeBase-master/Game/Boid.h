#include "CMOGO.h"
#include <vector>

class Boid : public CMOGO
{

public:
	Boid(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF);
	~Boid();

	virtual void Tick(GameData* _GD) override;

	void runBoid(vector<Boid*> boids, GameData* _GD);
	void applyForce(Vector3 force);
	void flock(vector<Boid*> boids);
	void updateBoid();
	Vector3 seek(Vector3 target);
	Vector3 boundingBox();
	Vector3 separate(vector<Boid*> boids);
	Vector3 align(vector<Boid*> boids);
	Vector3 cohesion(vector<Boid*> boids);
	bool checkColour(Boid b, Boid c);
	Vector3 GetVelocity();
	int GetTag();
	void SetTag(int num);
	void SetActive(bool act);
	bool GetActive();

protected:

	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	float r;
	float maxforce;    
	float maxspeed;   
	int tag = 1;
	bool active = false;
};