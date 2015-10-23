#include <iostream>
#include <sstream>
#include <stdlib.h>
#include "bigbang.h"
#include "crt.h"
#include "types.h"

using namespace std;

string MakeName()
{
    const string vowels = "aeiou";
    const string consonants = "bcdfghjklmnpqrstvwxyz";
    int mod = 0;
    int len = (rand()%8) + 3;
    string name;
    for (int i=0; i < len; i++) {
        if ((rand()%6) + mod < 3) {
            name += vowels[rand()%vowels.size()];
            mod = 2;
        } else {
            name += consonants[rand()%consonants.size()];
            mod = -2;
        }
    }
    name[0] = toupper(name[0]);
    return name;
}

static void VisitLinks(const int sector)
{
    if (sectors[sector].visited) return;
    sectors[sector].visited = true;
    for (int i=0; i < sectors[sector].links.size(); i++) {
        VisitLinks(sectors[sector].links[i]);
    }
}

static void Join(vector<int> &cluster)
{
    bool redo = true;
    while (redo) {
        for (int i=0; i < cluster.size(); i++) {
            sectors[cluster[i]].visited = false;
        }
        VisitLinks(cluster[0]);
        redo = false;
        for (int i=0; i < cluster.size(); i++) {
            if (sectors[cluster[i]].visited == false) {
                redo = true;
                int s1 = cluster[i];
                int linkto = i;
                while (linkto == i) {
                    linkto = rand() % cluster.size();
                }
                int s2 = cluster[linkto];
                bool exists = false;
                for (int k = 0; k < sectors[s1].links.size(); k++) {
                    if (sectors[s1].links[k] == s2) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    sectors[s1].links.push_back(s2);
                    sectors[s2].links.push_back(s1);
                }
            }
        }
    }
}

static void BuildStarports()
{
    Port terra;
    terra.sector = 0;
    terra.name = "Terra";
    terra.starbase = true;
    ports.push_back(terra);
    sectors[0].port = 0;

    for (int i=1; i < clusters.size(); i++) {
        int sector;
        do {
            int idx = rand() % clusters[i].sectors.size();
            sector = clusters[i].sectors[idx];
        } while (sector == clusters[i].upSector || sector == clusters[i].downSector);
        Port ds;
        ds.sector = sector;
        stringstream ss;
        ss << "Deep Space " << i;
        ds.name = ss.str();
        ds.starbase = true;
        sectors[sector].port = ports.size();
        ports.push_back(ds);
    }
}

static void BuildPorts()
{
    for (int i=0; i < NUM_SECTORS; i++) {
        if (sectors[i].port == -1 && rand() % 100 < PERCENT_SECTORS_WITH_PORT) {
            Port p;
            p.sector = i;
            p.name = MakeName();
            switch(rand()%3) {
                case 0:
                    p.sellOre = true;
                    break;
                case 1:
                    p.sellOrg = true;
                    break;
                case 2:
                    p.sellEqp = true;
                    break;
            }
            p.ore = (rand() % 60) + 40;
            p.org = (rand() % 60) + 40;
            p.eqp = (rand() % 60) + 40;
            sectors[i].port = ports.size();
            ports.push_back(p);
        }
    }
}

void BigBang()
{
    TextColor(LightMagenta);
    cout << endl << "Generating universe" << endl;
    TextColor(Yellow);

	vector<int> sectorNums;
    for (int i=NUM_SECTORS-1; i >= 0; i--) {
        sectors.push_back(Sector(i));
        sectorNums.push_back(i);
    }

    while (!sectorNums.empty()) {
        int size = (rand()%EXTRA_CLUSTER_SIZE) + MIN_CLUSTER_SIZE;
        if (sectorNums.size() - size < MIN_CLUSTER_SIZE) {
            size = sectorNums.size();
        }
        vector<int> nodes;
        for (int i = 0; i < size; i++) {
            if (sectorNums.empty()) break;
            nodes.push_back(sectorNums.back());
            sectorNums.pop_back();
        }
        for (int i = 0; i < nodes.size(); i++) {
            int linksWanted = rand()%2 + 1;
            int s1 = nodes[i];
            sectors[s1].cluster = clusters.size();
            for (int j = 0; j < linksWanted; j++) {
                if (sectors[s1].links.size() >= linksWanted) break;
                int linkto = i;
                while (linkto == i) {
                    linkto = rand() % nodes.size();
                }
                int s2 = nodes[linkto];
                bool exists = false;
                for (int k = 0; k < sectors[s1].links.size(); k++) {
                    if (sectors[s1].links[k] == s2) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    sectors[s1].links.push_back(s2);
                    sectors[s2].links.push_back(s1);
                }
            }
        }
        Join(nodes);
        clusters.push_back(Cluster(clusters.size()));
        clusters[clusters.size()-1].sectors = nodes;
    }

    for (int i=0; i < clusters.size(); i++) {
        int downIdx = rand() % clusters[i].sectors.size();
        int downSector = clusters[i].sectors[downIdx];
        if (i == 0) {
            downSector = 0;  // ensure upsector in fedspace is not sector 0
        }
        clusters[i].downSector = downSector;
        // try to find an exit sector that's neither identical or adjacent
        // this can infinite loop so we limit the number of attempts
        bool found;
        int upSector;
        int attempt = 0;
        do {
            do {
                int upIdx = rand() % clusters[i].sectors.size();
                upSector = clusters[i].sectors[upIdx];
            } while (downSector == upSector);
            found = false;
            for (int k=0; k < sectors[downSector].links.size(); k++) {
                if (sectors[downSector].links[k] == upSector) {
                    found = true;
                    break;
                }
            }
            attempt++;
        } while (found && attempt < 10);
        clusters[i].upSector = upSector;
        if (i > 0) {
            int prevUp = clusters[i-1].upSector;
            sectors[downSector].links.push_back(prevUp);
            sectors[prevUp].links.push_back(downSector);
        }
    }

    for (int i=0; i < sectors.size(); i++) {
        sectors[i].visited = false;
    }
    sectors[0].visited = true;

    clusters[0].name = "Federation space";
    clusters[clusters.size()-1].name = "uncharted space";
    for (int i=1; i < clusters.size() - 1; i++) {
        clusters[i].name = MakeName() + " system";
    }

    BuildStarports();
    BuildPorts();
}
