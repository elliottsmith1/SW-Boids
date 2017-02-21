#include "CMOGO.h"
#include <list>

using std::list;

class Boid : public CMOGO
{

public:
	Boid(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF);
	~Boid();

	virtual void Tick(GameData* _GD) override;

	void runBoid(vector<std::unique_ptr<Boid>> boids);
	void applyForce(Vector3 force);
	void flock(vector<std::unique_ptr<Boid>> boids);
	void updateBoid();
	Vector3 seek(Vector3 target);
	Vector3 boundingBox(Boid b);
	Vector3 separate(vector<std::unique_ptr<Boid>> boids);
	Vector3 align(vector<std::unique_ptr<Boid>> boids);
	Vector3 cohesion(vector<std::unique_ptr<Boid>> boids);
	bool checkColour(Boid b, Boid c);

	Vector3 GetVelocity();
	int GetTag();

protected:

	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	float r;
	Vector3 maxforce;    
	float maxspeed;   
	int tag = 1;
};