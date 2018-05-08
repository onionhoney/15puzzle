#ifndef PDB_H
#define PDB_H
#include "grid.h"
#include "constants.h"
#include "premove.h"
#include "heur.h"
#include "heur_lc.h"
#include "wd.h"

class PatternDB : public Heuristics {
    typedef std::vector<std::vector<std::vector<int> > > vvvi;
    vvvi g_555 = {
        {
            {2, 3, 6, 7, 11},
            {0, 1, 4, 8, 12},
            {5, 9, 10, 13, 14}
        },
        {
            {8, 12, 9, 13, 14},
            {0, 4, 1, 2, 3},
            {5, 6, 10, 7, 11}
        }  
    };
    vvvi g_663 = {
        {
            {0, 1, 2, 3, 4, 5}, 
            {6, 7, 11},
            {8, 9, 10, 12, 13, 14}
        }
    };
    vvvi& g;
    Premover* premovers[2][3];
    CacheManager& manager;
    // LinearConflict lc; 
    string mode;

    bool initialized = false;


public:

    PatternDB(CacheManager& manager, string mode="663") 
    : manager(manager), g(decide_db(mode)), mode(mode) {
    }

    vvvi& decide_db(string mode) {
        if (mode == "555") return g_555;
        if (mode == "663") return g_663;
        return g_555;
    }
    void init() {
        if (initialized) return;
        initialized = true;
        for (int k = 0; k < g.size(); k++) {
        for (int i = 0; i < g[k].size(); i++) {
            sort(g[k][i].begin(), g[k][i].end(), greater<int>());
            vector<int> group(g[k][i].begin(), g[k][i].end());
            premovers[k][i] = &(manager.getPremover(group, mode+"-"+to_string(k)+"-"+to_string(i)+".txt"));
        }
        }


    }

    int shortHash(long long pos, const vector<int>& arr) {
        int sh = 0;
        for (int d : arr) {
            sh = (sh << 4) | ((pos >> (d << 2)) & 15);
        }
        return sh;
    }

    int h_pdb(long long pos, int k) {
        char totalCount = 0;
        for (int i = 0; i < g[k].size(); i++) {
            //int sh = shortHash(pos, g[k][i]);
            int partialCount = premovers[k][i]->getMove(pos);
            totalCount += partialCount;
        }
        return totalCount;
    }
    // #define max(a, b, c, d) max(max( (a), (b) ), max( (c), (d) ))
    
    int h(Grid& grid) {
        long long pos = grid.posHash();
        //char maxHeur = h_pdb(pos, 0);
        char maxHeur = max(h_pdb(pos, 0), h_pdb(pos, 1));
        /*
        char maxHeur = max( max(h_pdb(pos, 0), h_pdb(getSym(pos), 0)) , 
                            max(
                                max(h_pdb(getHoriz(pos), 0) - 3, h_pdb(getHoriz(getSym(pos)), 0) - 3), 
                                max(h_pdb(getVert(pos), 0) - 3, h_pdb(getVert(getSym(pos)), 0) - 3))
                            );
        */ 
        //char maxHeur = h_pdb(pos, 1);
        return maxHeur;
    }


    ll getSym(ll pos) {
        ll symPos = 0;
        for (int i = 15; i >= 0; i--) {
            symPos = (symPos << 4) | MIRROR[getpos(MIRROR[i])];
        }
        return symPos;
    }

    ll getHoriz(ll pos) {
        const char reflmap[16] = {
            3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12
        };
        const char hatmap[16] = {
            3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 14, 13, 12, 15
        };
        ll symPos = 0;
        for (int i = 15; i >= 0; i--) {
            char p = reflmap[getpos(hatmap[i])];
            symPos = (symPos << 4) | p;
        }
        return symPos;
    }

    ll getVert(ll pos) {
        const char reflmap[16] = {
            12, 13, 14, 15, 8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3
        };
        const char hatmap[16] = {
            12, 13, 14, 11, 8, 9, 10, 7, 4, 5, 6, 3, 0, 1, 2, 15
        };
        ll symPos = 0;
        for (int i = 15; i >= 0; i--) {
            char p = reflmap[getpos(hatmap[i])];
            symPos = (symPos << 4) | p;
        }
        return symPos;
    }
};



#endif
