#include "elev.h"
#include <stdio.h>

// To tabeller. tabellOpp holder orden på om heisen er bestillt til noen som skal opp 
// og motsatt med tabellNed. Panelbestillinger lagres i begge tabeller
static int tabellOpp[N_FLOORS] = {0};
static int tabellNed[N_FLOORS] = {0};

void setTabellOpp(int etasje,int verdi){tabellOpp[etasje] = verdi;}
void setTabellNed(int etasje,int verdi){tabellNed[etasje] = verdi;}
int getTabellOpp(int etasje){return tabellOpp[etasje];}
int getTabellNed(int etasje){return tabellNed[etasje];}

void resetTabell(void){
	int i;
	for(i = 0; i < N_FLOORS ;i++){
		tabellOpp[i] = 0;
		tabellNed[i] = 0;
	} 
}

//Funksjonen undersøker hvilke retning heisen skal kjøre for å betjene neste købestilling. 
//Returnerer 0 for ingen ko, 1 for ko opp, -1 for ko ned, 2 for a apne dor.
//stoppOppstart er 1 dersom noen har trykket inn stoppknappen. 
//Den sørger for at personen i heisen kan returnere tilbake til den etasjen han kom fra.
int finnRetning(int etasjeTilstand,int retning,int stoppOppstart){
	int j;

//Dersom heisen er i ro, undersøkes det om det er kommet et nytt innlegg i køtabellene og returnerer evt rettning heisen må kjøre.
	if(retning == 0){
		if((tabellNed[etasjeTilstand]||tabellOpp[etasjeTilstand]) && elev_get_floor_sensor_signal() >= 0){
			return 2;
		}
		for(j = etasjeTilstand+1; j < N_FLOORS; j++){
			if(tabellOpp[j] == 1 || tabellNed[j] == 1){
				return 1;
			}
		}
		for(j = 0; j < etasjeTilstand; j++){
			if(tabellNed[j] == 1 || tabellOpp[j] == 1){
				return -1;
			}
		}	
	}
//Dersom heisen er på vei oppover undersøkes det først om det finnes flere bestillinger opp og hvis ikke, om det finnes noen nedover.
	else if(retning == 1){
		if (tabellNed[etasjeTilstand] && elev_get_floor_sensor_signal() == -1 && stoppOppstart) {
			return -1;
		}
		for(j = etasjeTilstand+1; j < N_FLOORS; j++){
			if(tabellOpp[j] == 1 || tabellNed[j] == 1){
				return 1;
			}
		}
		for(j = 0; j < etasjeTilstand; j++){
			if(tabellOpp[j] == 1 || tabellNed[j] == 1){
				return -1;
			}
		}
	}
//Som over men motsatt med rettningen nedover.
	else if(retning == -1){
		if (tabellOpp[etasjeTilstand] && elev_get_floor_sensor_signal() == -1 && stoppOppstart) {return 1;}
		for(j = 0; j < etasjeTilstand; j++){
			if(tabellOpp[j] == 1 || tabellNed[j] == 1){
				return -1;
			}
		}
		for(j = etasjeTilstand+1; j < N_FLOORS; j++){
			if(tabellOpp[j] == 1 || tabellNed[j] == 1){
				return 1;
			}
		}
	}
	return 0;
}

