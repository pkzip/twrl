#include <algorithm>
#include <iostream>
#include "autopilot.h"
#include "combat.h"
#include "crt.h"
#include "display.h"
#include "types.h"

class Scan
{
public:
	int sector;
	int parent;
	int warps;
};

static void WarpTo(const int sector)
{
    TextColor(Yellow);
    cout << endl << "Warping to sector " << sector+1 << endl;
    LeaveSector(player.sector, sector);
    player.sector = sector;
    player.turns++;
    EnterSector();
    sectors[player.sector].visited = true;
    DisplaySector(player.sector);
    FerrengiMove();
}

void CheckMove(const int sector)
{
	if (sector < 0 || sector >= NUM_SECTORS) return;
	const int s = player.sector;
	if (sector == s) {
		DisplayError("Already in that sector");
		return;
	}
	bool found = false;
	for (int i=0; i < sectors[s].links.size(); i++) {
		if (sectors[s].links[i] == sector) {
			found = true;
			break;
		}
	}
	if (found) {
		WarpTo(sector);
	} else {
		AutoPilot(sector);
	}
}

void AutoPilot(const int dest)
{
	int src = player.sector;
	if (dest < 0 || dest >= NUM_SECTORS) {
		DisplayError("Invalid sector");
		return;
	}
	bool unsetVisited = false;
	if (sectors[dest].visited == false) {
		bool found = false;
		for (int i=0; i < sectors[dest].links.size(); i++) {
			int s = sectors[dest].links[i];
			if (sectors[s].visited) {
				found = true;
			}
		}
		if (!found) {
			DisplayError("Unknown sector, cannot plot course");
			return;
		}
		unsetVisited = true;
		sectors[dest].visited = true;
	}
	if (src == dest) {
		DisplayError("Already in that sector");
		return;
	}

	TextColor(Yellow);
	cout << endl << "Navigation computer activated, plotting course" << endl;

	vector<Scan> openList;
	vector<Scan> closedList;
	Scan start;
	start.sector = src;
	start.parent = -1;
	start.warps = 0;
	openList.push_back(start);
	while (1) {
		if (openList.empty()) {
			DisplayError("Failed to plot course");
			return;
		}
		vector<Scan>::iterator lowest = openList.begin();
		for (vector<Scan>::iterator i=openList.begin(); i < openList.end(); i++) {
			if (i->warps < lowest->warps) {
				lowest = i;
			}
		}
		int sector = lowest->sector;
		closedList.push_back(*lowest);
		if (sector == dest) break;
        Scan lowestCopy(*lowest);
		openList.erase(lowest);
		for (int i=0; i < sectors[sector].links.size(); i++) {
			int link = sectors[sector].links[i];
			if (!sectors[link].visited) continue;
			bool closed = false;
			for (int j=0; j < closedList.size(); j++) {
				if (closedList[j].sector == link) {
					closed = true;
				}
			}
			if (closed) continue;
			bool open = false;
			for (int j=0; j < openList.size(); j++) {
				if (openList[j].sector == link) {
					if (lowestCopy.warps + 1 < openList[j].warps) {
						openList[j].parent = lowestCopy.sector;
						openList[j].warps = lowestCopy.warps + 1;
					}
					open = true;
					break;
				}
			}
			Scan add;
			add.sector = link;
			add.parent = sector;
			add.warps = (lowestCopy.warps) + 1;
			openList.push_back(add);
		}
	}
	vector<int> course;
	int s = dest;
	while (s != src) {
		course.push_back(s);
		for (int i=0; i < closedList.size(); i++) {
			if (closedList[i].sector == s) {
				s = closedList[i].parent;
				break;
			}
		}
	}
	course.push_back(s);
	reverse(course.begin(), course.end());
	cout << endl;
	if (unsetVisited) {
		sectors[dest].visited = false;
	}
	for (int i=0; i < course.size(); i++) {
		if (i > 0) {
			TextColor(Green);
			cout << " -> ";
		}
		TextColor(LightCyan);
		cout << course[i]+1;
	}
	cout << endl;
	TextColor(Green);
	cout << endl << "Course laid in, engage (Y/n)? ";
	TextColor(Yellow);
	char ch;
	do {
		ch = toupper(ReadKey());
	} while (ch != 'Y' && ch != 'N' && ch != '\r' && ch != '\n');
	if (ch == 'N') {
		cout << "N" << endl;
		return;
	}
	cout << "Y" << endl;
	player.abortAutoPilot = false;
	for (int i=1; i < course.size(); i++) {
		int warp = course[i];
		WarpTo(warp);
		if (player.abortAutoPilot) {
			TextColor(LightRed);
			cout << endl << "Auto-pilot disengaged" << endl;
			break;
		}
	}
}
