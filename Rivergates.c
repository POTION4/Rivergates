#include <stdio.h>
#include <stdlib.h>
#include "Rivergates.h"

int main() 
{
	////////////////////////////////////
	// Map test
	////////////////////////////////////
    RS *rs = RS_create();
    RS_pushRP(rs, produceRPNothing());
    for (int y = 0; y < MAPH; y++)
    {
        for (int x = 0; x < MAPW; x++)
        {
            printf("%c", RS_getMapRepr(rs, x, y));
        }
        printf("\n");
    }

    ////////////////////////////////////
    // Event test
    ////////////////////////////////////
    RE *re = RE_create(NULL, WALK, NULL, 0, 3, 2);
    RE *re1 = RE_create(NULL, CHOP, NULL, 0, 1, 2);
    re = RE_attach(re, re1);
    while (re)
    {
        printf("%d %d\n", re->x, re->y);
        re = re->next;
    }
    
    ////////////////////////////////////
    // Gear test
    ////////////////////////////////////
    for (int i = 0; i < MAXITEMLEN; i++)
    {
        printf("%s\n", RS_getRPIndex(rs, 0)->gears[i]->name);
    }
    return 0; 
};