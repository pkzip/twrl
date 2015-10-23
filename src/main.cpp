#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#ifdef _WIN32
#include <time.h>
#endif
#include "bigbang.h"
#include "crt.h"
#include "display.h"
#include "types.h"

int main()
{
    srand(time(NULL));
    DisplayIntro();
    BigBang();

    DisplaySector(player.sector);
    DisplayMenu();

    TextColor(LightGray);
    CrtClose();
    return 0;	
}
