#ifndef HEIS_H
#define HEIS_H

#define MAX_SPEED 300;
#include <time.h>

void slaPaHeis(void);

void stoppHeis(void);

void velgEtasje(void);

void bestillHeis(void);

void avstigning(void);

int testTid(double delay, clock_t startTime);

void bremsHeis(void);

void ankomstEtasje(void);

void kjorHeis(void);

int testEtasjeIQueue(void);

int getEtasjeTilstand(void);

void stoppHeisOBS(void);
#endif
