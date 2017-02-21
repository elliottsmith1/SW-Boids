#include "gameobject.h"
#include "Boid.h"
#include <vector>

using std::list;

//=================================================================
//Terrian Game Object Class (i.e. an Object that just sits there)
//=================================================================

struct GameData;

class BoidController : public GameObject
{
public:
	BoidController() = default; 
	~BoidController() = default; 

	void Tick(GameData* _GD) override {};
	void addBoid(GameObject* b);

	//list<GameObject *> boids;
	std::vector<std::unique_ptr<Boid>> boids;

protected:

};

