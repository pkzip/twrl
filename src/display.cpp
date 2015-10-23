#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include "autopilot.h"
#include "combat.h"
#include "crt.h"
#include "display.h"
#include "types.h"

static void CheckQuit()
{
    cout << endl;
    TextColor(White);
    TextBackground(Red);
    cout << " QUIT GAME: ";
    TextBackground(Black);
    cout << " Are you sure (y/N)? ";
    char ch = toupper(ReadKey());
    if (ch != 'Y') {
        ch = 'N';
    }
    TextColor(Yellow);
	cout << ch << endl;
	if (ch == 'Y') {
		GameOver();
    }
}

static string ReadVal()
{
	string res;
	while (1) {
        char ch;
        do {
            ch = toupper(ReadKey());
        } while (ch != 8 && ch != '\r' && ch != '\n' && (ch < 32 || ch > 127));
        if (ch == 8 || ch == 127) {
            if (!res.empty()) {
        	    res = res.substr(0, res.length()-1);
        	    Backspace();
       	    }
       	} else if (ch == '\r' || ch == '\n') {
       		return res;
        } else {
            if (isdigit(ch)) {
            	cout << ch;
                res += ch;
            } else {
            	if (res.empty()) {
            		res += ch;
            	}
          		return res;
            }
        }
    }
}

static int ReadInt(const int def = 0)
{
	string res;
	while (1) {
        char ch;
        do {
            ch = toupper(ReadKey());
        } while (ch != 8 && ch != 127 && ch != '\r' && ch != '\n' && !isdigit(ch));
        if (ch == 8 || ch == 127) {
            if (!res.empty()) {
        	    res = res.substr(0, res.length()-1);
        	    Backspace();
       	    }
       	} else if (ch == '\r' || ch == '\n') {
       		if (res.empty()) {
       			cout << def << endl;
       			return def;
       		} else {
       			cout << endl;
       			return atoi(res.c_str());
       		}
        } else {
           	cout << ch;
            res += ch;
        }
    }
}

static void DisplayHelp()
{
	cout << endl;
	TextColor(Green);
	cout << "Enter sector number to warp, or press command key:" << endl << endl;
	cout << "<D> Display sector" << endl;
	cout << "<I> display ship's Inventory" << endl;
	cout << "<P> dock at Port or starbase" << endl;
	cout << "<R> display Regional logs" << endl;
	cout << "<T> display ship's Tactical report" << endl;
	cout << "<Q> Quit game" << endl;
}

static void DisplayInventory()
{
	cout << endl;
	TextColor(Yellow);
	cout << "Retrieving current ship's manifest" << endl << endl;
	TextColor(Green);
	if (player.ore > 0) {
		cout << "Ore:       ";
		TextColor(LightGreen);
		cout << setw(5) << player.ore;
		TextColor(Green);
		cout << " cargo holds" << endl;
	}
	if (player.org > 0) {
		cout << "Organics:  ";
		TextColor(LightGreen);
		cout << setw(5) << player.org;
		TextColor(Green);
		cout << " cargo holds" << endl;
	}
	if (player.eqp > 0) {
		cout << "Equipment: ";
		TextColor(LightGreen);
		cout << setw(5) << player.eqp;
		TextColor(Green);
		cout << " cargo holds" << endl;
	}
	const int emptyHolds = player.holds - (player.ore + player.org + player.eqp);
	cout << "Empty:     ";
	TextColor(LightGreen);
	cout << setw(5) << emptyHolds;
	TextColor(Green);
	cout << " cargo holds" << endl;
	cout << endl << "TOTAL:     ";
	TextColor(LightGreen);
	cout << setw(5) << player.holds;
	TextColor(Green);
	cout << " cargo holds" << endl << endl;
	cout << "Credits: ";
	TextColor(LightCyan);
	cout << player.credits << endl;
}

static void DisplayTactical()
{
	cout << endl;
	TextColor(Yellow);
	cout << "Retrieving current ship's tactical status" << endl << endl;

	TextColor(Green);
	cout << "Shields: ";
	TextColor(LightGreen);
	cout << player.shields;
	TextColor(Green);
	cout << " of ";
	TextColor(LightGreen);
	cout << player.maxShields;
	TextColor(Green);
	cout << " emitters online" << endl;

	TextColor(Green);
	cout << "Phasers: ";
	TextColor(LightGreen);
	cout << player.phasers;
	TextColor(Green);
	cout << " banks online" << endl;
}

static void WriteBS(const bool selling) {
	if (selling) {
		cout << "S";
	} else {
		cout << "B";
	}
}

static void DisplayRegion()
{
	const int cluster = sectors[player.sector].cluster;
	cout << endl;
	TextColor(Yellow);
	cout << "Retrieving current regional logs" << endl << endl;
	TextColor(Green);
	cout << "Region ";
	TextColor(LightCyan);
	cout << cluster+1;
	TextColor(Green);
	cout << ": ";
	TextColor(LightGreen);
	cout << clusters[cluster].name << endl << endl;
	TextColor(Green);
	cout << "Sector  Port  Warps to" << endl;
	cout << "------  ----  --------" << endl;
	for (int i=0; i < clusters[cluster].sectors.size(); i++) {
		int sector = clusters[cluster].sectors[i];
		if (sectors[sector].visited) {
			if (sector == player.sector) {
				TextColor(White);
				cout << " * ";
			} else {
				cout << "   ";
			}
			TextColor(LightGreen);
			cout << setw(3) << sector+1;
			TextColor(Green);
			cout << "  ";
			int port = sectors[sector].port;
			if (port < 0) {
				cout << "none";
			} else if (ports[port].starbase) {
				TextColor(LightMagenta);
				cout << "base";
				TextColor(Green);
			} else {
				cout << " ";
				WriteBS(ports[port].sellOre);
				WriteBS(ports[port].sellOrg);
				WriteBS(ports[port].sellEqp);
			}
			cout << "  ";
			for (int j=0; j < sectors[sector].links.size(); j++) {
				if (j > 0) {
					TextColor(Green);
					cout << ", ";
				}
				int link = sectors[sector].links[j];
				if (sectors[link].cluster != cluster) {
					TextColor(White);
					cout << "[";
				}
				if (sectors[link].visited) {
					TextColor(LightCyan);
					cout << link+1;
				} else {
					TextColor(LightRed);
					cout << "(" << link+1 << ")";
				}
				if (sectors[link].cluster != cluster) {
					TextColor(White);
					cout << "]";
				}
			}
			cout << endl;
		}
	}
}

static void SellCommodity(const string &commodity, int &units, int &avail, const int basePrice)
{
	if (avail < 1) {
		TextColor(Green);
		cout << endl << "We don't need any more " << commodity << " today." << endl;
		return;
	}
	int sell;
	int def = units;
	if (def > avail) { 
		def = avail;
	}
	do {
		TextColor(Green);
		cout << endl << "Sell how many holds of " << commodity << " [";
		TextColor(LightGreen);
		cout << def;
		TextColor(Green);
		cout << "]? ";
		TextColor(Yellow);
		sell = ReadInt(def);
		if (sell > units) {
			DisplayError("You don't have that many");
		}
		if (sell > avail) {
			TextColor(Green);
			cout << endl << "We can only buy up to " << avail << " holds today." << endl;
		}
	} while (sell > units || sell > avail);
	if (sell > 0) {
		int price = (int)(sell * basePrice * 1.10);
		TextColor(Green);
		cout << "We'll buy ";
		TextColor(LightGreen);
		cout << sell;
		TextColor(Green);
		cout << " holds for ";
		TextColor(LightCyan);
		cout << price;
		TextColor(Green);
		cout << " credits, OK? (Y/n) ";
		TextColor(Yellow);
		char ch;
		do {
			ch = toupper(ReadKey());
		} while (ch != 'Y' && ch != 'N' && ch != '\r' && ch != '\n');
		if (ch != 'N') {
			cout << "Y" << endl;
			units -= sell;
			avail -= sell;
			player.credits += price;
			TextColor(LightGreen);
			cout << sell;
			TextColor(Green);
			cout << " holds of " << commodity << " offloaded, ";
			TextColor(LightGreen);
			cout << units;
			TextColor(Green);
			cout << " remain." << endl;
			TextColor(LightCyan);
			cout << price;
			TextColor(Green);
			cout << " credits transfered.  You now have ";
			TextColor(LightCyan);
			cout << player.credits;
			TextColor(Green);
			cout << " credits." << endl;
		} else {
			cout << "N" << endl;
		}
	}
}

static void BuyCommodity(const string &commodity, int &units, int &avail, const int basePrice)
{
	if (avail < 1) {
		TextColor(Green);
		cout << endl << "We are sold out of " << commodity << " today." << endl;
		return;
	}
	const int emptyHolds = player.holds - (player.ore + player.org + player.eqp);
	int buy;
	int def = emptyHolds;
	if (def > avail) { 
		def = avail;
	}
	do {
		TextColor(Green);
		cout << endl << "Buy how many holds of " << commodity << " [";
		TextColor(LightGreen);
		cout << def;
		TextColor(Green);
		cout << "]? ";
		TextColor(Yellow);
		buy = ReadInt(def);
		if (buy > emptyHolds) {
			DisplayError("You don't have that many empty holds");
		}
		if (buy > avail) {
			TextColor(Green);
			cout << endl << "We only have " << avail << " holds left today." << endl;
		}
	} while (buy > emptyHolds || buy > avail);
	if (buy > 0) {
		int price = (int)(buy * basePrice * 0.90);
		TextColor(Green);
		cout << "We'll sell ";
		TextColor(LightGreen);
		cout << buy;
		TextColor(Green);
		cout << " holds for ";
		TextColor(LightCyan);
		cout << price;
		TextColor(Green);
		cout << " credits, OK? (Y/n) ";
		TextColor(Yellow);
		char ch;
		do {
			ch = toupper(ReadKey());
		} while (ch != 'Y' && ch != 'N' && ch != '\r' && ch != '\n');
		if (ch != 'N') {
			cout << "Y" << endl;
			if (player.credits < price) {
				DisplayError("You do not have enough credits to cover your purchase, transaction cancelled");
				return;
			}
			units += buy;
			avail -= buy;
			player.credits -= price;
			TextColor(LightGreen);
			cout << buy;
			TextColor(Green);
			cout << " holds of " << commodity << " loaded, ";
			TextColor(LightGreen);
			cout << emptyHolds - buy;
			TextColor(Green);
			cout << " empty holds remain." << endl;
			TextColor(LightCyan);
			cout << price;
			TextColor(Green);
			cout << " credits transfered.  You now have ";
			TextColor(LightCyan);
			cout << player.credits;
			TextColor(Green);
			cout << " credits." << endl;
		} else {
			cout << "N" << endl;
		}
	}
}

static void RepairShip()
{
	if (player.shields == player.maxShields) {
		TextColor(Yellow);
		cout << endl << "Nothing to repair on this ship!" << endl;
		return;
	}
	int needRepair = player.maxShields - player.shields;
	int cost = needRepair * SHIELD_REPAIR_CREDITS;
	TextColor(Green);
	cout << endl << "Repairing ";
	TextColor(LightGreen);
	cout << needRepair;
	TextColor(Green);
	cout << " shield emitters will cost ";
	TextColor(LightCyan);
	cout << cost;
	TextColor(Green);
	cout << " credits." << endl;
	if (cost > player.credits) {
		needRepair = player.credits / SHIELD_REPAIR_CREDITS;
		TextColor(Green);
		cout << "You only have enough credits to repair ";
		TextColor(LightGreen);
		cout << needRepair;
		TextColor(Green);
		cout << " emitters." << endl;
	}
	TextColor(Green);
	cout << "Repair how many shield emitters [";
	TextColor(LightGreen);
	cout << needRepair;
	TextColor(Green);
	cout << "] ? ";
	TextColor(Yellow);
	int num = ReadInt(needRepair);
	if (num > needRepair) {
		DisplayError("You cannot repair that many");
	} else if (num > 0) {
		cost = num * SHIELD_REPAIR_CREDITS;
		player.shields += num;
		player.credits -= cost;
		TextColor(Green);
		cout << "Repairs completed and ";
		TextColor(LightCyan);
		cout << cost;
		TextColor(Green);
		cout << " credits transferred." << endl;
	}
}

static void Upgrade(const string &item, const int upgradeCost, int &units)
{
	TextColor(Green);
	cout << endl << "You have ";
	TextColor(LightCyan);
	cout << player.credits;
	TextColor(Green);
	cout << " credits." << endl;
	cout << "Each " << item << " costs ";
	TextColor(LightCyan);
	cout << upgradeCost;
	TextColor(Green);
	cout << " credits." << endl;
	int canBuy = player.credits / upgradeCost;
	if (canBuy < 1) {
		TextColor(Yellow);
		cout << endl << "You don't have enough credits" << endl;
		return;
	}
	cout << "Purchase how many [";
	TextColor(LightGreen);
	cout << canBuy;
	TextColor(Green);
	cout << "]? ";
	TextColor(Yellow);
	int buy = ReadInt(canBuy);
	if (buy < 1) return;
	if (buy > canBuy) {
		DisplayError("You don't have enough credits");
		return;
	}
	int cost = buy * upgradeCost;
	player.credits -= cost;
	units += buy;
	TextColor(Green);
	cout << "Installed ";
	TextColor(LightGreen);
	cout << buy;
	TextColor(Green);
	cout << " " << item << "s." << endl;
	TextColor(LightCyan);
	cout << cost;
	TextColor(Green);
	cout << " credits transferred." << endl;
}

static void PhaserUpgrades()
{
	Upgrade("phaser bank", PHASER_UPGRADE_COST, player.phasers);
}

static void ShieldUpgrades()
{
	if (player.shields < player.maxShields) {
		TextColor(Yellow);
		cout << endl << "Your existing shield emitters must be in working order before upgrading." << endl;
		return;
	}
	Upgrade("shield emitter", SHIELD_UPGRADE_COST, player.maxShields);
	player.shields = player.maxShields;
}

static void Starbase()
{
	const int port = sectors[player.sector].port;
	TextColor(Green);
	cout << endl << "Welcome to starbase ";
	TextColor(LightGreen);
	cout << ports[port].name << endl;
	bool done = false;
	while (!done) {
		TextColor(Green);
		cout << endl;
		cout << "<T> Display ship's tactical status" << endl;
		cout << "<R> Repair ship" << endl;
		cout << "<P> Phaser upgrades" << endl;
		cout << "<S> Shield upgrades" << endl;
		cout << "<Q> Quit and leave starbase" << endl;
		TextColor(Cyan);
		cout << endl << "Starbase command: ";
		TextColor(Yellow);
		char ch;
		do {
			ch = toupper(ReadKey());
		} while (ch != 'T' && ch != 'R' && ch != 'P' && ch != 'S' && ch != 'Q');
		cout << ch << endl;
		switch (ch) {
			case 'R':
				RepairShip();
				break;
			case 'P':
				PhaserUpgrades();
				break;
			case 'S':
				ShieldUpgrades();
				break;
			case 'T':
				DisplayTactical();
				break;
			case 'Q':
				done = true;
				break;
		}
	}
}

static void TradingPort()
{
	const int port = sectors[player.sector].port;
	TextColor(Yellow);
	bool traded = false;
	if (player.ore > 0 && !ports[port].sellOre) {
		SellCommodity("ore", player.ore, ports[port].ore, ORE_PRICE);
		traded = true;
	}
	if (player.org > 0 && !ports[port].sellOrg) {
		SellCommodity("organics", player.org, ports[port].org, ORG_PRICE);
		traded = true;
	}
	if (player.eqp > 0 && !ports[port].sellEqp) {
		SellCommodity("equipment", player.eqp, ports[port].eqp, EQP_PRICE);
		traded = true;
	}
	int emptyHolds;
	emptyHolds = player.holds - (player.ore + player.org + player.eqp);
	TextColor(Green);
	cout << endl << "You have ";
	TextColor(LightGreen);
	cout << emptyHolds;
	TextColor(Green);
	cout << " empty holds." << endl;
	if (emptyHolds > 0 && ports[port].sellOre) {
		BuyCommodity("ore", player.ore, ports[port].ore, ORE_PRICE);
		traded = true;
	}
	emptyHolds = player.holds - (player.ore + player.org + player.eqp);
	if (emptyHolds > 0 && ports[port].sellOrg) {
		BuyCommodity("organics", player.org, ports[port].org, ORG_PRICE);
		traded = true;
	}
	emptyHolds = player.holds - (player.ore + player.org + player.eqp);
	if (emptyHolds > 0 && ports[port].sellEqp) {
		BuyCommodity("equipment", player.eqp, ports[port].eqp, EQP_PRICE);
		traded = true;
	}
	if (!traded) {
		TextColor(Yellow);
		cout << endl << "There is nothing for you to buy or sell at this port." << endl;
	}
}

static void DockAtPort()
{
	const int port = sectors[player.sector].port;
	if (port < 0) {
		DisplayError("No port in this sector");
		return;
	}
	player.turns++;
	TextColor(Magenta);
	cout << endl << "Docking clamps engaged" << endl;
	TextColor(Green);
	cout << "You have ";
	TextColor(LightCyan);
	cout << player.credits;
	TextColor(Green);
	cout << " credits." << endl;
	if (!ports[port].visited) {
		TextColor(Yellow);
		int pts = 1;
		if (ports[port].starbase) {
			pts = 5;
		}
		cout << endl << "For discovering this ";
		if (ports[port].starbase) {
			cout << "starbase";
		} else {
			cout << "port";
		}
		cout << ", you gain " << pts << " experience." << endl;
		player.xp += pts;
		ports[port].visited = true;
	}
	FerrengiMove();
	if (ports[port].starbase) {
		Starbase();
	} else {
		TradingPort();
	}
	TextColor(Magenta);
	cout << endl << "Docking clamps disengaged" << endl;
	FerrengiMove();
}

void DisplayError(const string &msg)
{
    cout << endl;
    TextBackground(Red);
    TextColor(Yellow);
    cout << " " << msg << " ";
    TextBackground(Black);
    cout << endl;
}

void DisplayIntro()
{
    CrtInit();
    TextBackground(Black);
    TextColor(Green);
    ClrScr();
    cout << "+--------------------------+" << endl;
    cout << "|  ";
    TextColor(White);
    cout << "TradewarsRL 2015";
    TextColor(Green);
    cout << "        |" << endl;
    cout << "|  ";
    TextColor(LightGreen);
    cout << "a 7drl by Mad Sorcerer";
    TextColor(Green);
    cout << "  |" << endl;
    cout << "+--------------------------+" << endl;
    cout << endl << endl;
    TextColor(LightGray);
    cout << "Your mission is to journey into deep space and" << endl;
    cout << "destroy the Ferrengi Mothership Yendor." << endl << endl;
}

void DisplayMenu()
{
    while (1)
    {
        TextColor(Cyan);
        cout << endl << "Command (?=Help) [D]: ";
        TextColor(Yellow);
        string s = ReadVal();
        if (s.empty()) {
        	s = "D";
        }
        if (isdigit(s[0])) {
        	int sector = atoi(s.c_str());
            cout << endl;
            if (sector < 1 || sector > NUM_SECTORS) {
                DisplayError("Invalid Sector");
            } else {
            	CheckMove(sector-1);
            }
        } else {
        	cout << s[0] << endl;
            switch (s[0]) {
            	case 'A':
            		Attack();
            		break;
                case 'D':
                    DisplaySector(player.sector);
                    break;
                case 'I':
                	DisplayInventory();
                	break;
                case 'P':
                    DockAtPort();
                    break;
                case 'Q':
                    CheckQuit();
                    break;
                case 'R':
                	DisplayRegion();
                	break;
                case 'T':
                	DisplayTactical();
                	break;
                case '?':
                	DisplayHelp();
                	break;
                default:
                    DisplayError("Unknown Command");
                    break;
            }
        }
    }
}

static void WriteBuySell(const bool sells)
{
	if (sells) {
		cout << "sell";
	} else {
		cout << "buy";
	}
}

static void DisplayPort(int port)
{
    TextColor(Magenta);
	if (ports[port].starbase) {
		cout << "Starbase: ";
	} else {
	    cout << "Port: ";
	}
    TextColor(LightCyan);
    cout << ports[port].name;
    if (!ports[port].starbase) {
	    TextColor(Green);
	    cout << " (";
	    WriteBuySell(ports[port].sellOre);
	    cout << "/";
	    WriteBuySell(ports[port].sellOrg);
	    cout << "/";
	    WriteBuySell(ports[port].sellEqp);
	    cout << ")";
	}
	cout << endl;
}

static void DisplayFerrengi(int f)
{
	TextColor(Brown);
	cout << "Ship: ";
	TextColor(LightCyan);
	cout << ferrengi[f].make << " " << ferrengi[f].model;
	TextColor(LightRed);
	cout << " " << ferrengi[f].name;
	TextColor(Green);
	cout << " with ";
	TextColor(LightGreen);
	cout << ferrengi[f].phasers;
	TextColor(Green);
	cout << " phaser banks" << endl;
}

void DisplaySector(int sector)
{
    TextColor(Green);
    cout << endl << "Sector ";
    TextColor(LightCyan);
    cout << sector+1;
    TextColor(Green);
    cout << " in ";
    TextColor(LightGreen);
    cout << clusters[sectors[sector].cluster].name << endl;
    if (sectors[sector].port >= 0) {
		DisplayPort(sectors[sector].port);
    }
    for (int i=0; i < ferrengi.size(); i++) {
    	if (ferrengi[i].sector == sector) {
    		DisplayFerrengi(i);
    	}
    }
    TextColor(Green);
    cout << "Warps lead to: ";
    for (int i = 0; i < sectors[sector].links.size(); i++) {
        if (i > 0) {
            TextColor(Green);
            cout << ", ";
        }
        int link = sectors[sector].links[i];
        if (sectors[link].cluster != sectors[sector].cluster) {
        	TextColor(White);
        	cout << "[";
        }
        if (sectors[link].visited) {
            TextColor(LightCyan);
            cout << link+1;
        } else {
            TextColor(LightRed);
            cout << "(" << link+1 << ")";
        }
        if (sectors[link].cluster != sectors[sector].cluster) {
        	TextColor(White);
        	cout << "]";
        }
    }
    cout << endl;
}

void DisplayUniverse()
{
	cout << endl;
    for (int i=0; i < clusters.size(); i++) {
        TextColor(Green);
        cout << i+1 << ": ";
        TextColor(LightGreen);
        cout << "[" << clusters[i].downSector+1 << "," << clusters[i].upSector+1 << "] ";
        for (int j = 0; j < clusters[i].sectors.size(); j++) {
            int sector = clusters[i].sectors[j];
            TextColor(Yellow);
            cout << sector;
            TextColor(Cyan);
            cout << "->(";
            TextColor(LightCyan);
            for (int k=0; k < sectors[sector].links.size(); k++) {
                cout << sectors[sector].links[k]+1 << ",";
            }
            TextColor(Cyan);
            cout << "),";
        }
        cout << endl;
    }
}
