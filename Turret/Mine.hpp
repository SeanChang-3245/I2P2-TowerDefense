#ifndef MiNEHPP
#define MiNEHPP
#include "Turret.hpp"

class Mine: public Turret {
protected:
    int damage;
    int initializetime;
public:
    Mine(std::string Minebase, std::string Mineimg, int price, float x, float y);
	void Update(float deltaTime) override;
    
    virtual void Explode() = 0;
    void CreateBullet() override;
    int GetInitialTime();
};

#endif // MiNEHPP
