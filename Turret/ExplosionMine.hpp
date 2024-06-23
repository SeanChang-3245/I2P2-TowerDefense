#ifndef EXPOLSIONMiNEHPP
#define EXPOLSIONMiNEHPP
#include "Mine.hpp"

class ExplosionMine: public Mine {
public:
    static const int dmg;
    static const int Price;
    ExplosionMine(float x, float y);    
    void Explode() override;
};

#endif // EXPOLSIONMiNEHPP
