#ifndef QUEUE_H
#define QUEUE_H

void setTabellOpp(int etasje,int verdi);
void setTabellNed(int etasje,int verdi);
int getTabellOpp(int etasje);
int getTabellNed(int etasje);
void resetTabell(void);

//Funksjonen undersøker hvilke retning heisen skal kjøre for å betjene neste købestilling. 
//Returnerer 0 for ingen ko, 1 for ko opp, -1 for ko ned, 2 for a apne dor.
//stoppOppstart er 1 dersom noen har trykket inn stoppknappen. 
//Den sørger for at personen i heisen kan returnere tilbake til den etasjen han kom fra.
int finnRetning(int etasjeTilstand,int retning,int stoppOppstart);

#endif
