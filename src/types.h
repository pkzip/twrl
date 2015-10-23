#ifndef _TYPES_H_
#define _TYPES_H_

#include <string>
#include <vector>
using namespace std;

#if LONG_MAX==9223372036854775807 && LONG_MIN==(-9223372036854775807-1)
typedef long tw_int64;
#elif defined(_MSC_VER)
typedef __int64 tw_int64;
#else
typedef long long tw_int64;
#endif


const int NUM_SECTORS = 999;
const int MIN_CLUSTER_SIZE = 10;
const int EXTRA_CLUSTER_SIZE = 10;
const int PERCENT_SECTORS_WITH_PORT = 40;
const int STARTING_CREDITS = 500;
const int STARTING_HOLDS = 10;
const int STARTING_ORE = 5;
const int STARTING_ORG = 0;
const int STARTING_EQP = 0;
const int STARTING_PHASERS = 10;
const int STARTING_SHIELDS = 10;
const int ORE_PRICE = 7;
const int ORG_PRICE = 13;
const int EQP_PRICE = 27;
const int SHIELD_REPAIR_CREDITS = 5;
const int PHASER_UPGRADE_COST = 120;
const int SHIELD_UPGRADE_COST = 100;
const int SHIELD_REGEN_CHANCE = 20;

class Cluster {
public:
    Cluster(const int num) :
        num(num),
        downSector(-1),
        upSector(-1) {}
    int num;
    vector<int> sectors;
    int downSector;
    int upSector;
    string name;
};

class Ferrengi {
public:
	Ferrengi() :
		sector(0),
		phasers(0),
		shields(0),
		dest(-1),
		endGame(false) {}
	int sector;
	int phasers;
	int shields;
	int dest;
	string make;
	string model;
	string name;
	bool endGame;
	int ore;
	int org;
	int eqp;
};

class Player {
public:
	Player() :
		sector(0),
		turns(0),
		credits(STARTING_CREDITS),
		holds(STARTING_HOLDS),
		ore(STARTING_ORE),
		org(STARTING_ORG),
		eqp(STARTING_EQP),
		xp(0),
		phasers(STARTING_PHASERS),
		shields(STARTING_SHIELDS),
		maxShields(STARTING_SHIELDS),
		abortAutoPilot(false) {}
	int sector;
	int turns;
	tw_int64 credits;
	int holds;
	int ore;
	int org;
	int eqp;
	int xp;
	int phasers;
	int shields;
	int maxShields;
	int abortAutoPilot;
};

class Port {
public:
	Port() :
		sector(0),
		starbase(false),
		sellOre(false),
		sellOrg(false),
		sellEqp(false),
		visited(false) {}
	int sector;
	bool starbase;
	bool sellOre;
	bool sellOrg;
	bool sellEqp;
	int ore;
	int org;
	int eqp;
	string name;
	bool visited;
};

class Sector {
public:
    Sector(const int num) : 
        num(num),
        visited(false),
        cluster(-1),
        port(-1) {}
    int num;
    bool visited;
    int cluster;
    int port;
    vector<int> links;
};


extern vector<Cluster> clusters;
extern vector<Ferrengi> ferrengi;
extern Player player;
extern vector<Port> ports;
extern vector<Sector> sectors;

#endif
