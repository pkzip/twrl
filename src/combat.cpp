#include <stdlib.h>
#include <iostream>
#include "bigbang.h"
#include "combat.h"
#include "crt.h"
#include "display.h"
#include "types.h"

using namespace std;

static int CalcDamage(const int phasers)
{
	return (rand() % phasers) + 0;
}

void GameOver()
{
	TextColor(White);
	cout << endl << "Final score: " << player.xp << " experience points." << endl;
	TextColor(LightGray);
	cout << endl << "Thank you for playing TradewarsRL 2015" << endl << endl;

    TextColor(LightGray);
    CrtClose();
	exit(0);
}

static void FerrengiAttack(const int f)
{
	if (ferrengi[f].phasers < 1) return;
	player.abortAutoPilot = true;
	bool hit = (rand() % 100) < 50;
	if (!hit) {
		TextColor(Green);
		cout << endl << ferrengi[f].make << " " << ferrengi[f].model << " fires phasers and misses." << endl;
		return;
	}

	TextColor(Green);
	cout << endl << ferrengi[f].make << " " << ferrengi[f].model <<  " fires phasers and hits." << endl;
	int damage = CalcDamage(ferrengi[f].phasers);
	if (damage == 0) {
		cout << endl << "No damage!" << endl;
		return;
	}
	if (damage > player.shields) {
		damage = player.shields;
	}
	player.shields -= damage;
	int shieldPerc = (int) ( (float) player.shields / (float) player.maxShields * 100);
	TextColor(LightGreen);
	cout << damage;
	TextColor(Green);
	cout << " shield emitters damaged!  Shields at ";
	TextColor(LightGreen);
	cout << shieldPerc << "%";
	cout << endl;
	if (player.shields < 1) {
		TextColor(LightRed);
		cout << endl << "Your ship explodes!" << endl;
		TextColor(Yellow);
		cout << "Killed by: a " << ferrengi[f].make << " " << ferrengi[f].model <<
		  " in sector " << player.sector+1 << "." << endl;
	    GameOver();
	}
}

static void EndGame()
{
	TextColor(Yellow);
	cout << endl << "Congratulations!!!  Mission accomplished." << endl;
	cout << endl << "You have successfully won TradewarsRL 2015." << endl;
	GameOver();
}

static void Salvage(const int units, int &punits, const string &commodity)
{
	int emptyHolds = player.holds - (player.ore + player.org + player.eqp);
	if (units > 0) {
		int salvage = rand() % (units + 1);
		if (salvage > emptyHolds) {
			salvage = emptyHolds;
		}
		if (salvage > 0) {
			punits += salvage;
			TextColor(Green);
			cout << "You salvage ";
			TextColor(LightGreen);
			cout << salvage;
			TextColor(Green);
			cout << " holds of " << commodity << " from the debris." << endl;
		}
	}
}

void FerrengiMove()
{
	for (int i=0; i < ferrengi.size(); i++) {
		if (ferrengi[i].sector == player.sector) {
			FerrengiAttack(i);
		} else if (ferrengi[i].dest >= 0) {
			ferrengi[i].sector = ferrengi[i].dest;
			ferrengi[i].dest = -1;
			if (ferrengi[i].sector == player.sector) {
				TextColor(LightRed);
				cout << endl << ferrengi[i].make << " " << ferrengi[i].model << " " <<
				  ferrengi[i].name << " enters sector" << endl;
			}
		}
	}
}

void LeaveSector(const int src, const int dest)
{
	for (int i=0; i < ferrengi.size(); i++) {
		if (ferrengi[i].sector == src) {
			ferrengi[i].dest = dest;
		}
	}
}

void Attack()
{
	vector<int> nearby;
	for (int i=0; i < ferrengi.size(); i++) {
		if (ferrengi[i].sector == player.sector) {
			nearby.push_back(i);
		}
	}
	if (nearby.empty()) {
		DisplayError("No ships here to attack");
		return;
	}
	if (player.phasers < 1) {
		DisplayError("You have no phaser banks online");
		return;
	}
	// TODO multi-target select
	TextColor(Green);
	cout << endl << "Firing phasers!" << endl;
	const int f = nearby.front();
	bool hit = (rand() % 100) < 60;
	if (!hit) {
		TextColor(Green);
		cout << "You miss." << endl;
	} else {
		int damage = CalcDamage(player.phasers);
		if (damage > ferrengi[f].shields) {
			damage = ferrengi[f].shields;
		}
		if (damage == 0) {
			cout << "You hit but their shields absorb the damage." << endl;
		} else {
			cout << "You hit the " << ferrengi[f].make << " " << ferrengi[f].model <<
			 " destroying ";
			TextColor(LightGreen);
			cout << damage;
			TextColor(Green);
			cout << " shield emitters." << endl;
			ferrengi[f].shields -= damage;
			if (ferrengi[f].shields < 1) {
				TextColor(LightRed);
				cout << endl << "The " << ferrengi[f].make << " " <<
				  ferrengi[f].model << " explodes!" << endl;
				int cluster = sectors[player.sector].cluster;
				int xp = 10 + (cluster*5) + (ferrengi[f].phasers *3);
				player.xp += xp;
				TextColor(Yellow);
				cout << endl << "For destroying this ship, you receive " << xp << " experience." << endl;
				if (ferrengi[f].endGame) {
					EndGame();
				}
				Salvage(ferrengi[f].eqp, player.eqp, "equipment");
				Salvage(ferrengi[f].org, player.org, "organics");
				Salvage(ferrengi[f].ore, player.ore, "ore");
				vector<Ferrengi>::iterator i = ferrengi.begin();
				i += f;
				ferrengi.erase(i);
			}
		}
	}
	FerrengiMove();
}

void EnterSector()
{
	int cluster = sectors[player.sector].cluster;
	if (player.shields < player.maxShields) {
		if (rand() % 100 < SHIELD_REGEN_CHANCE) {
			int num = (rand() % (cluster + 1)) + 1;
			if (player.shields + num > player.maxShields) {
				num = player.maxShields - player.shields;
			}
			player.shields += num;
			TextColor(Yellow);
			cout << endl << num << " shield emitters have been repaired." << endl;
		}
	}

	int chance = 20;
	if (!sectors[player.sector].visited) chance += 20;
	chance += cluster;
	int roll = rand() % 100;
	if (roll < chance) {
		int maxLevel = cluster;
		int level = rand() % (maxLevel + 1);
        Ferrengi f;
        f.sector = player.sector;
        f.phasers = 1 + (level * 2) + (rand() % 2);
        f.shields = 1 + (level * 2) + (rand() % 3);
        f.ore = 1 + (level) + (rand() % 3);
        f.org = 1 + (level) + (rand() % 3);
        f.eqp = 1 + (level) + (rand() % 3);
        f.make = "Ferrengi";
        f.name = MakeName();
        switch(level) {
        	case 0: f.model = "Scout"; break;
        	case 1: f.model = "Maurauder"; break;
        	case 2: f.model = "Mine Sweeper"; break;
        	case 3: f.model = "Transport"; f.ore += 2; f.org += 2; f.eqp += 2; break;
        	case 4: f.model = "Cruiser"; break;
        	case 5: f.model = "Armed Transport"; f.ore += 4; f.org += 4; f.eqp += 4; break;
        	case 6: f.model = "Gunner"; break;
        	case 7: f.model = "Destroyer"; break;
        	default:
        		f.model = "Mothership";
        		f.phasers += 10;
        		f.shields += 20;
        		f.name = "Yendor";
        		f.endGame = true;
        		break;
        }
        ferrengi.push_back(f);
	}
}
