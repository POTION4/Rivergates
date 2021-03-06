#include "Rivergates.h"

void announce(RS* rs, char* msg)
{
    RO *observer = rs->observers;
    
    while (observer)
    {
        observer->message(msg);
        observer = observer->next;
    }
}

// Create a new RivergateSession instance.
RS *RS_create()
{
    RS *rs;
    rs = malloc(sizeof(RS));
    rs->map = malloc(0);
    rs->mapLen = 0;
    rs->rps = malloc(0);
    rs->rpLen = 0;
    rs->observers = NULL;

    for (int y = 0; y < MAPH; y++)
    {
        for (int x = 0; x < MAPW; x++)
        {
            RT rt = produceRTNothing();
            rt.repr = '.';
            rt.pos = cpos(x, y);
            RS_pushRT(rs, rt);
        }
    }

    return rs;
}

// Get the terrain; if blank, create it.
RT* RS_getRT(RS* rs, pos p)
{
    for (int i = 0; i < rs->mapLen; i++)
    {
    	
    	// TODO: give chunk POS! 
        if (pos_pequals(rs->map[i].pos, p))
        {
            return &(rs->map[i]);
        }
    }
    // Not found! Create it.
    RT rt = produceRTNothing();
    rt.pos = p;
    RS_pushRT(rs, rt);
    return &(rs->map[rs->mapLen - 1]);
}

// Get the map's repr.
char RS_getMapRepr(RS *rs, pos p)
{
    for (int i = 0; i < rs->rpLen; i++)
    {
        if (pos_pequals(rs->rps[i].pos, p))
        {
            return rs->rps[i].repr;
        }
    }

    return RS_getRT(rs, p)->repr;
}

// Push back a new terrain.
void RS_pushRT(RS* rs, RT rt)
{
    RT *mem = malloc(sizeof(RT) * (rs->mapLen + 1));
    memcpy(mem, rs->map, sizeof(RT) * rs->mapLen);
    mem[rs->mapLen] = rt;
    rs->mapLen++;
    rs->map = mem;
}

// Push back a new player.
void RS_pushRP(RS* rs, RP rp)
{
    RP *mem = malloc(sizeof(RP) * (rs->rpLen + 1));
    memcpy(mem, rs->rps, sizeof(RP) * rs->rpLen);
    free(rs->rps);
    mem[rs->rpLen] = rp;
    rs->rpLen++;
    rs->rps = mem;
}

// Return the player based on index.
RP* RS_getRPIndex(RS* rs, int i)
{
	if (i < 0 && i >= rs->rpLen)
    {
        return NULL;
    }
    return &(rs->rps[i]);
}

// Return the player based on position.
RP *RS_getRPPos(RS *rs, pos p)
{
    for (int i = 0; i < rs->rpLen; i++)
    {
        if (pos_pequals(rs->rps[i].pos, p))
        {
            return &(rs->rps[i]);
        }
    }

    return NULL;
}

void RS_processRE(RS *rs, RE *re)
{
    char msg[256];
	while (re)
    {
        RET type = re->type;
        pos p;
        RP *affected;
        RP *player = re->player;
        RG *gear = player->thoughts.gear;
        bool pass = TRUE;
        int hitCount = 0;

        if (type != WALK && type != REST)
        {
            // Check here first.
            if (RP_check(STAMINA, player, gear->staminaCost) && RP_check(HP, player, gear->hpCost) 
                && RP_check(MP, player, gear->magicCost))
            {
                // If all of them suites the situation, remove them 
                RP_remove(STAMINA, player, gear->staminaCost);
                RP_remove(HP, player, gear->hpCost);
                RP_remove(MP, player, gear->magicCost);
            }
            else
            {
                sprintf(msg, "Player %s tried to use %s, but didn't have enough stamina...", player->name, gear->name);
                announce(rs, msg);
                pass = FALSE;
            }
        }
        else if (type == WALK)
        {
            if (!RP_checkAndRemove(STAMINA, player, WALKSTA))
            {
                sprintf(msg, "Player %s tried to walk, but is too tired...", player->name);
                announce(rs, msg);
                pass = FALSE;
            }
        }

        if (!pass)
        {
            return;
        }
        
        switch (type)
        {
        case REST:
            player->stamina += RESTSTA;
            if (player->stamina > 100)
            {
                player->stamina = 100;
            }
            sprintf(msg, "Player %s took a rest.", player->name);
            announce(rs, msg);
            break;

        case WALK:
            p = RP_getDirectionPos(re->player);
            if (!RS_getRPPos(rs, p))
            {
                player->pos = p;
                sprintf(msg, "Player %s moved to (%d, %d)", player->name,
                        player->pos.x, player->pos.y);
                announce(rs, msg);
            }
            // BLOCKED
            else
            {
                sprintf(msg, "Player %s tried to walk, but is blocked...",
                        player->name);
                announce(rs, msg);
            }
            break;

        case CHOP:
            affected = RS_getRPPos(
                rs,
                pos_getAffectedPos(player->pos, player->thoughts.direction, 1));
            if (affected)
            {
                RP_damage(player, affected, player->thoughts.gear);
                sprintf(msg, "%s chopped %s!\n", re->player->name,
                        affected->name);
                announce(rs, msg);
                hitCount++;
            }
            else
            {
                sprintf(msg, "%s chopped through thin air.\n",
                        re->player->name);
                announce(rs, msg);
            }
            break;
            
        case STAB:
        	affected = RS_getRPPos(
                rs,
                pos_getAffectedPos(player->pos, player->thoughts.direction, 1));
            if (affected)
            {
                RP_damage(player, affected, player->thoughts.gear);
                sprintf(msg, "%s stabbed %s.\n", re->player->name,
                        affected->name);
                announce(rs, msg);
                hitCount++;
            }
            else
            {
                sprintf(msg, "%s stabbed through thin air.\n",
                        re->player->name);
                announce(rs, msg);
            }
            break;
            
        case PIERCE:
        	for (int i = 1; i <= 2; i++)
            {
                p = pos_getAffectedPos(re->player->pos,
                                       re->player->thoughts.direction, i);
                affected = RS_getRPPos(rs, p);
                if (affected)
                {
                    RP_damage(re->player, affected, re->player->thoughts.gear);
                    sprintf(msg, "%s pierced %s.", re->player->name,
                            affected->name);
                    announce(rs, msg);
                    hitCount++;
                }
            }
            
            if (hitCount == 0)
            {
                sprintf(msg, "%s pierced through thin air.", re->player->name);
                announce(rs, msg);
            }
            break;
            
        case CLOB:
        	affected = RS_getRPPos(
                rs,
                pos_getAffectedPos(player->pos, player->thoughts.direction, 1));
            if (affected)
            {
                RP_damage(player, affected, player->thoughts.gear);
                sprintf(msg, "%s clobbed %s.\n", re->player->name,
                        affected->name);
                announce(rs, msg);
                hitCount++;
            }
            else
            {
                sprintf(msg, "%s clobbed through thin air.\n",
                        re->player->name);
                announce(rs, msg);
            }
            break;
            
        }
        
        re = re->next;
    }
}