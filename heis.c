#include "elev.h"
#include <stdio.h>
#include <time.h>
#include "heis.h"
#include "queue.h"
#include <assert.h>

typedef int bool;
#define true 1
#define false 0

static int speed = 0;
static int retning = 0;		//Angir hvilken retning heisen er pa vei mot, 1 opp, 0 i ro, -1 ned
static int etasjeTilstand = 0;	//Angir etasjen som heisen sist var i.
static int stoppLys = 0;		//Angir om stopplyset er på(1) eller av(0)
static const double DELAY = 3;	//Angir hvor lenge døren skal være åpen.
static bool doorOpen = false;
static int stoppOppstart = 0;	//Er 1 dersom stoppknappen har blitt trykt inn og ingen panellys ikke har blitt trykkt.


//slaPaHeisen skal kjøres når heisen slåes på. Dersom heisen ikke er i noen etasje 
//kjøres den til den nærmeste etasjen over dens nåverende posisjon.
void slaPaHeis(void){
	bool startup = true;
	if(!elev_init()){
		printf("Feil i oppstart. Kontakt support.\n");
		assert(!elev_init());
	}
	resetTabell();
	//Starter så snart OBS er av.
	while(startup){
		if(!elev_get_obstruction_signal()){
			startup = false;
		}
	}
	//Stopper med nærmeste etasje over nåverende posisjon.
	if (elev_get_floor_sensor_signal() == -1){
		speed = MAX_SPEED;
		elev_set_speed(speed);
		while (speed > 0) {
			if (elev_get_floor_sensor_signal() >= 0){
				bremsHeis();
			}
		}
	}
	etasjeTilstand = elev_get_floor_sensor_signal();
	elev_set_floor_indicator(etasjeTilstand);
}

//StoppHeis sjekker om stoppknappen er trykket inn og hvis så er tilfelle stoppes heisen
void stoppHeis(void){
	if(elev_get_stop_signal() && stoppLys == 0){
		printf("stoppHeis\n");
//slår på stopplyset	
		stoppLys = 1;
		elev_set_stop_lamp(stoppLys);
		stoppOppstart = 1;
		if(speed != 0){
			bremsHeis();
		}
		resetTabell();
//Slår av bestill opp, ned og velg lysene
		int i;
		for (i = 0; i < N_FLOORS; i++) {
		    if (i != 0) elev_set_button_lamp(1, i, 0);
		    if (i != N_FLOORS-1) elev_set_button_lamp(0, i, 0);
		    elev_set_button_lamp(2, i, 0);	    
		}

	}
}

//Funksjoen kontrollerer om en velg-etasje-panelknapp er trykket inn og hvis så legger 
//den i begge tabellene. Hvis stopp-knappen har vært trykt inn, slåes lysbryteren av.
void velgEtasje(void){
	int floor;
	for(floor = 0; floor < N_FLOORS; floor++){
		if(!(etasjeTilstand == floor && doorOpen)){
			if(elev_get_button_signal(2,floor)){
				elev_set_button_lamp(2,floor,1);
				setTabellOpp(floor,1);
				setTabellNed(floor,1);
				
				if(stoppLys){
					stoppLys = 0;
					elev_set_stop_lamp(stoppLys);
				}
			}
		}
	}
}

//Funksjoen kontrollerer om noen av bestill(opp eller ned) er trykket inn og hvis så er 
//tilfelle (og stoppknappen ikke har vært trykket inn), legger den bestillingen i de respektive tabellene.
void bestillHeis(void){
	if (!stoppLys){
		int floor;
		for(floor = 0; floor < 3; floor++){
			if(!(etasjeTilstand == floor && doorOpen)){
				if (elev_get_button_signal(0,floor)){
					elev_set_button_lamp(0,floor,1);
					setTabellOpp(floor, 1);
				}
			}
			if(!(etasjeTilstand == floor + 1 && doorOpen)){
				if (elev_get_button_signal(1,floor + 1)){
					elev_set_button_lamp(1,floor+ 1,1);
					setTabellNed(floor + 1, 1);
				}
			}
		}
	}
}

//Funksjonen stopper heisen, åpner døren, slår av alle bestilling og panellys i etasjen,
// sletter oppføringene i kø listen. Deretter starter den en timer som etter DELAY sekunder lukke døren. 
//Brukeren kan legge inn bestillinger ect mens timeren går. Funksjoen avsluttes når døren er lukket.
void avstigning(void){
    printf("Avstigning i etasje %d. \n", etasjeTilstand);
	bremsHeis();
	if(etasjeTilstand >= 0){
		elev_set_door_open_lamp(1);
		doorOpen = true;
		elev_set_button_lamp(2,etasjeTilstand,0);
		if(etasjeTilstand>=3){
			elev_set_button_lamp(1,etasjeTilstand,0);
		}else if(etasjeTilstand==0){
			elev_set_button_lamp(0,etasjeTilstand,0);
		}else{
			elev_set_button_lamp(0,etasjeTilstand,0);
			elev_set_button_lamp(1,etasjeTilstand,0);
		}
		setTabellOpp(etasjeTilstand,0);
		setTabellNed(etasjeTilstand,0);
	}
	clock_t startTime = clock();
	while(!testTid(DELAY, startTime) || elev_get_obstruction_signal()){
		velgEtasje();
		bestillHeis();	
		stoppHeis();
	}
	elev_set_door_open_lamp(0);
	doorOpen = false;
}


//Stopper heisen. For at heisen skal stoppe fortest mulig settes motoren i motsatt retning for en kort tid. 
//Må tilpasses heisen. Heisen kan motta bestillinger ect mens heisen bremses.
void bremsHeis(void){
	clock_t startTime = clock();
	speed = speed/-10;
	elev_set_speed(speed);
	while(!testTid(0.4, startTime)){
		velgEtasje();
		bestillHeis();	
		if(stoppLys == 0){stoppHeis();}	
	}
	speed = 0;
	elev_set_speed(speed);
}

//Funksjoen returnerer 1 dersom det er gått "delay" sekunder, 0 hvis ikke.
int testTid(double delay, clock_t startTime){
	clock_t clockTicksTaken = clock() - startTime;
	double timeInSeconds = clockTicksTaken/(double) CLOCKS_PER_SEC;
	if(timeInSeconds>delay){
		return 1;
	}
	else{
		return 0;
	}
}

//Funksjoen setter lysindikatoren til rett etasje og etasjetilstanden.
void ankomstEtasje(void){
    if(elev_get_floor_sensor_signal()>= 0){
	    etasjeTilstand = elev_get_floor_sensor_signal();
	    elev_set_floor_indicator(etasjeTilstand);
	    printf("Ankomst i etasje %d \n", etasjeTilstand);
    }
}

//Tester om heis skal stoppe i nåvaerende etasje. Returnerer 0 for stopp og 1 for stopp.
int testEtasjeIQueue(void){
	int i;
	int stopp = 0;
	//Test om det er flere etasjer i Queue-retning
	int test = 0;
//Hvis heisen er pa vei oppover, sjekk om nåværende etasje ligger i Queue-tabell-oppover (return 1) 
//eller om det finnes noen i tabellen over denne etasjen (0).
	if (retning == 1){
	//Tester om det finnes flere i queue som skal opp eller ned over etasjeTilstand.
		for (i = etasjeTilstand + 1; i<N_FLOORS; i++){
			if (getTabellOpp(i) == 1 || getTabellNed(i) == 1){
				test = 1;
			}
		}
		if (getTabellOpp(etasjeTilstand)){
			stopp = 1;
		}
		else if(getTabellNed(etasjeTilstand) && !test){
			stopp = 1;
		}
	}

//Samme som over, men sjekker i retning nedover.
	else if (retning == -1){
		for (i = 0; i<etasjeTilstand; i++){
			if (getTabellOpp(i) == 1 || getTabellNed(i) == 1){
				test = 1;
			}
		}
		if (getTabellNed(etasjeTilstand)){
			stopp = 1;
		}
		else if(getTabellOpp(etasjeTilstand) && !test){
			stopp = 1;
		}
	}
//Hvis heisen skal stoppe i etasjen pga at noen skal nedover eller oppover.
	return stopp;
}

//Bruker finnRetning til å undersøker om det finnes oppføringer i queue og hvilke 
//retning heisen må kjøre for å komme til disse etasjene. Starter heisen i den oppgitte retningen.
void kjorHeis(void){
    if(stoppLys != 1){
    	retning = finnRetning(etasjeTilstand,retning,stoppOppstart);
    	stoppOppstart = 0;
		if(retning == 1){
			speed = MAX_SPEED;
			elev_set_speed(speed);
		}
		else if(retning == -1){
			speed = -MAX_SPEED;
			elev_set_speed(speed);
		}
		else if(retning == 2){
			retning = 0;
			printf("Bestilling i nåværende etasje.\n");
			avstigning();
		}
	}
}

int getEtasjeTilstand(void){return etasjeTilstand;}

//Sjekker om OBS er på mens heisen kjører eller er i ro.
//Dersom OBS slåes på vil heisen stoppes intill OBS slåes av.
void stoppHeisOBS(void){
	while(elev_get_obstruction_signal()){
		if (speed != 0) {bremsHeis();}
		velgEtasje();
		bestillHeis();	
		stoppHeis();
	}
}
