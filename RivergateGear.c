#include "RivergateGear.h"

#include <string.h>

RG* RG_create(int id, char* name, int dmg, int stac, int mpc, int hpc, int amount)
{
    RG *rg = malloc(sizeof(RG));
    strcpy(rg->name, name);
    rg->dmg = dmg;
    rg->staminaCost = stac;
    rg->magicCost = mpc;
    rg->hpCost = hpc;
    rg->id = id;
    rg->amount = amount;

    return rg;
}

RG* RG_clone(RG* rg)
{
    RG *mem = malloc(sizeof(RG));
    memcpy(mem, rg, sizeof(RG));

    return mem;
}

RG *produceRGNothing()
{
    RG *rg = RG_create(NOTHING, "None", 0, 0, 0, 0, 0);
    strcpy(rg->name, "None");
    
    return rg;
}

void RG_modifyGear(RG* rg, int id, char* name, int stac, int mpc, int hpc, int dmg, int amount)
{
    strcpy(rg->name, name);
    rg->id = id;
    rg->staminaCost = stac;
    rg->magicCost = mpc;
    rg->hpCost = hpc;
    rg->dmg = dmg;
    rg->amount = amount;
}

RG *RG_getGear(int id) {
    RG *rg = produceRGNothing();
    
    switch (id)
    {
    case NOTHING:
        break;

    case SWORD:
        RG_modifyGear(rg, SWORD, "Sword", 30, 0, 0, 35, 1);
        break;

	case DAGGER:
        RG_modifyGear(rg, DAGGER, "Dagger", 15,
                      0, 0, 25, 5);
        break;
        
    case SPEAR:
        RG_modifyGear(rg, SPEAR, "Spear", 45, 0, 0, 40, 1);
        break;
        
    case LONGSWORD:
        RG_modifyGear(rg, LONGSWORD, "Longsword", 55, 0, 0, 50, 1);
        break;
        
    case CLUB:
        RG_modifyGear(rg, CLUB, "Club", 50, 0, 0, 40, 1);
        break;
        
    default:
        break;
    }

    return rg;
}