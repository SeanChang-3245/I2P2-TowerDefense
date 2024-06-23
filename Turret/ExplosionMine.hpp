#ifndef EXPOLSIONMiNEHPP
#define EXPOLSIONMiNEHPP
#include "Mine.hpp"

class ExplosionMine: public Mine {
public:
    static std::string Minebase;
    static std::string Mineimg;
    static const int dmg;
    static const int Price;
    ExplosionMine(float x, float y);    
    void Explode() override;
};

#endif // EXPOLSIONMiNEHPP
