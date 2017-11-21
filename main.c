#include "heis.h"
#include "elev.h"
#include "queue.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


typedef int bool;
#define true 1
#define false 0

//Hovedprogrammet. While løkken kjøres så lenge programmet er på.
int main(){
	slaPaHeis();
	bool run = true;
	while(run){
		velgEtasje();
		bestillHeis();	
		stoppHeis();
		stoppHeisOBS();
		kjorHeis();
		if ((elev_get_floor_sensor_signal() >= 0) && (elev_get_floor_sensor_signal() != getEtasjeTilstand())){
			ankomstEtasje();
			if (testEtasjeIQueue()) {
				printf("Etasjen er i Queue.\n");
				avstigning();
			}
		}
	}
    return 0;   
}


