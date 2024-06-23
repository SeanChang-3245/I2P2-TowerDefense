#ifndef TELEPORTMiNEHPP
#define TELEPORTMiNEHPP
#include "Mine.hpp"

class TeleportMine: public Mine {
public:
    static const int dmg;
    static const int Price;
    TeleportMine(float x, float y);    
    void Explode() override;
};

#endif // TELEPORTMiNEHPP
