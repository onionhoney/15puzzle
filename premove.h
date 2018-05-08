#ifndef PREMOVE_H
#define PREMOVE_H

#include <algorithm>
#include <vector>
#include <unordered_map> 
#include <map>

#include <cassert>
#include <array>
#include <cmath>
#include <fstream>

#include "grid.h"
#include "move.h"
#include "wd.h"

using namespace std;
typedef unsigned long long ll;

class Premover {
    std::unordered_map<ll, ll> premoveMap;
    //google::dense_hash_map<ll, int, hash<ll> > sansBlankMap;
    std::unordered_map<ll, char>  sansBlankMap;
    std::vector<char> sansBlankArr;
    long long solvedMask, solvedHash;
    int otherFill = -1;
    bool toBeSolved[SIZE*SIZE] = {0};
    char depthCap;
    int statesCap;
    bool generated = false;
    bool anyBlank = false;
    bool moveOnly;
    vector<int> pieces;

    static const int PMAX = 17;
    int P[PMAX][PMAX];

    ll bitmask, bitmask1;
private:
    ll encode_move(vector<Move>& v) {
        if (moveOnly) {
            return v.size();
        }
        ll code = -1;
        for (int i = 0; i < v.size(); i++) {
            code = (code << 3) | ((int)v[i]);
        }
        return code;
    }
    vector<Move> decode_move(ll code);

    void decode_move(vector<Move>& sol, ll code) {
        vector<Move> nextMoves = decode_move(code);
        reverse(nextMoves.begin(), nextMoves.end());
        for (Move m : nextMoves) {
            sol.push_back(oppositeMove(m));
        }
    }
public:
    Premover(string filename) : sansBlankMap() {
        load(filename);
    }
    Premover(std::vector<int> pieces, int dCap=25, int sCap=1000000, int moveOnly=false)
    : sansBlankMap(), pieces(pieces) {
        this->moveOnly = moveOnly;
        
        cout << "Before constructor:  " ;
        for (int p : pieces) cout << p << " ";
        cout << endl;

        depthCap = dCap;
        statesCap = sCap;
        bitmask = 0;  
        bitmask1 = 0;
        for (int i = 0; i < SIZE * SIZE; i++) {
            if (std::find(pieces.begin(), pieces.end(), i) != pieces.end()) {
                toBeSolved[i] = 1;
            } else { 
                if (otherFill == -1 || otherFill == BLANK_VAL)
                    otherFill = i;
            }
        }
        for (int i = 15; i >= 0; i--) {
            if (toBeSolved[i] || i == BLANK_VAL) {
                bitmask = (bitmask << 4) | 15;
                bitmask1 = (bitmask1 << 4) | (toBeSolved[i] ? 15 : 0);
            } else {
                bitmask = (bitmask << 4) | 0;
                bitmask1 = (bitmask1 << 4) | 0;
            }

        }
        if (otherFill == BLANK_VAL) // if the only left is blank, lol
            toBeSolved[BLANK_VAL] = true;
        anyBlank = !toBeSolved[BLANK_VAL];

     
        cout << "Constructor: To be solved : " ;
        for (int i = 0; i < SIZE * SIZE; i++) {
            if (toBeSolved[i]) cout << i << " ";
        } cout << endl;
        sort(pieces.begin(), pieces.end(), greater<int>());

        init_partial_perm_nth();
    }

    char bit[32];
    const int n = 16;
    void clr();
    void upd(char pos);
    char query(char pos);

    int decodeHash(ll hash) {
        // cout << "orig hash is " << hex << hash << dec << hash << endl;
        // cout << "pieces are " ; for (int p : pieces) cout << p << " "; cout << endl;
        vector<char> arr;
        for (int p : pieces) arr.push_back(15 & (hash >> (p << 2LL)));

        // for (int x : arr) {
        //    cout << hex << (int)x << dec;
        // }cout << endl;
        int ret = nth(arr.data(), pieces.size(), SIZE * SIZE);

        // cout << "return nth is " << ret << endl;
        return ret;
    }
    int nth(char*, int , int);

    int getMove(Grid& g) {
        //cout << "hash requested " << hex << g.hash(bitmask1) << dec << endl;
        return sansBlankMap[g.hash(bitmask1)];
    }
    inline char getMove(long long hash) {
        return sansBlankMap[hash & bitmask1];
        // return sansBlankArr[decodeHash(hash)];
        //cout << "hash requested " << hex << (hash & bitmask1) << dec << 
        //" returning " << (int)ret << endl;
    }
    void setMove(long long hash, long long hash1, char move) {
        premoveMap[hash] = move;
        if (sansBlankMap.find(hash1) == sansBlankMap.end()) {
            sansBlankMap[hash1] = move;
        } else {
            sansBlankMap[hash1] = min( (char)sansBlankMap[hash1], move);
        }
        
    }
    bool visited(long long hash) {
        return premoveMap.find(hash) != premoveMap.end();
    }

    void expand(Grid& curr, vector<Grid>& next, int level) {
        ll hash = curr.hash(bitmask);
        if (visited(hash)) return;
        ll hash1 = curr.hash(bitmask1);
        setMove(hash, hash1, level);
        for (int i = 0; i < 4; i++) {
            Move m = (Move) i;
            if (curr.can_move(m)) {
                Grid newGrid(curr);
                newGrid.move_to(m);
                if (toBeSolved[curr.get_swap_content(m)]) { // really moved 
                    next.push_back(newGrid);
                } else {
                    expand(newGrid, next, level);
                }
            }
        }
    }
    void save(string filename) {
        generate();
        ofstream file;
        file.open(filename);

        vector<pair<ll, char> > v(sansBlankMap.begin(), sansBlankMap.end());
        sort(v.begin(), v.end());
        file << pieces.size() << " " << v.size() << endl;
        for (const auto& p : v) {
            file << p.second;
            sansBlankArr.push_back(p.second);
        }
        file.flush();
        file.close();
    }

    bool next_partial_perm(vector<ll>& arr, int k) {
        int n = arr.size();
        int edge = k - 1;

        int first_greater = k;
        while (first_greater < n && arr[first_greater] < arr[edge]) 
            first_greater++;
        if (first_greater < n) {
            swap(arr[edge], arr[first_greater]);
            return true;
        }
        // we got a peak followed by an ascending slope
        reverse(arr.begin() + k, arr.end());
        // now it's all descending
        // find first ascneding edge

        int first_ascending = k - 2;
        while (first_ascending >= 0 && arr[first_ascending] > arr[first_ascending + 1]) 
            first_ascending--;
        if (first_ascending < 0) return false;
        first_greater = n - 1;
        while (first_greater > first_ascending && arr[first_ascending] > arr[first_greater])
            first_greater--;
        swap(arr[first_ascending], arr[first_greater]);
        reverse(arr.begin() + first_ascending + 1, arr.end());
        return true;
    }

    void init_partial_perm_nth() {
        for (int i = 0; i < PMAX; i++) {
            P[i][i] = 1;
            for (int j = i - 1; j >= 0; j--) {
                P[i][j] = P[i][j + 1] * (j + 1);
            }
        }
    }

    int partial_perm_nth(vector<char>& arr, int n) {
            int k = arr.size();
            vector<char> rem;
            for (int i = 0; i < n; i++) rem.push_back(i);
            int sum = 0;
            // if the top digit is k, we add (k) * (n-1)! / (n - k) !
            // then for the next digit, we add l, l where is the # of numbers smaller than itself
            //
            //
            // (n-1)! / (n-k)! 
            for (int i = 0; i < k; i ++) {
                auto elem = find(rem.begin(), rem.end(), arr[i]);
                int rk = elem - rem.begin();
                //cerr << arr[i] << " with rank " << rk << endl;
                sum += rk * P[n - i - 1][n - k];
                /// how many left? (n - i - 1). how many to choose? (k - i - 1). 
                // so P(n - i - 1]
                rem.erase(elem);
            }
            return sum;
    }



    bool load(string& filename) {
        if (filename == "") return false;
        ifstream file;
        file.open(filename);
        if (!file.is_open()) return false;
        
        int num, tot, i, count;
        char ch;
        file >> num >> tot; file.ignore(10000, '\n');

        vector<ll> perm(SIZE * SIZE);
        for (i = 0; i < perm.size(); i++) perm[i] = i; // smallest

        vector<ll> mask;
        sort(pieces.begin(), pieces.end(), greater<ll>());

        count = 0;
        do {
            if (file.eof()) {
                break;
            }
            file >> noskipws >> ch;

            
            ll key = 0;
            for (int i = 0; i < pieces.size(); i++) {
                key |= perm[i] << ( (ll)pieces[i] << 2LL);
            }
            
            sansBlankMap[key] = ch;

            sansBlankArr.push_back(ch);
            if (sansBlankArr.size() % 1000000 == 0) 
                cout << "inserting nth " << sansBlankArr.size() << endl;
        }
        while (next_partial_perm(perm, num));

        file.close();
        return true;
    }
    void generate() {
        if (generated) return; generated=true;
        vector<Grid> levelGraph;

        vector<int> idxOfWildcards;
        for (int i = 0; i < SIZE * SIZE; i++) {
            if (!toBeSolved[i]) idxOfWildcards.push_back(i);
        }
        assert(!toBeSolved[BLANK_VAL]);
        if (toBeSolved[BLANK_VAL]) {
            idxOfWildcards.clear(); 
            idxOfWildcards.push_back(BLANK_VAL);
        }
        for (int blankPos : idxOfWildcards) {
            vector<int> states;
            for (int i = 0; i < SIZE * SIZE; i++) {
                if (i == blankPos) {
                    states.push_back(BLANK_VAL);
                } else if (toBeSolved[i]) {
                    states.push_back(i);
                } else {
                    states.push_back(otherFill);
                }
            }
            Grid root(states, false);
            cerr << "premove root includes " << root << endl;
            levelGraph.push_back(root);
        }
        assert(levelGraph.size() > 1);
        if (levelGraph.size() > 1) {
            assert(levelGraph[0].hash(bitmask1) 
               == levelGraph[1].hash(bitmask1));
        }

        for (char depth = 0; depth <= depthCap; depth++) {
            vector<Grid> nextLevelGraph;
            for (Grid& grid : levelGraph) {
                expand(grid, nextLevelGraph, depth);
            }
            cout << "generating"  << sansBlankMap.size() << "/" << premoveMap.size() << " cumu for depth " << (int)depth << endl;
            levelGraph = nextLevelGraph;
        }
        cerr << "generated " << premoveMap.size() << " many states" << endl;
        generated = true;
    }
};


class CacheManager {
    std::map<vector<int>, Premover*> premoverMap;
    WalkingDist* wd_inst;
    bool wd_used = false;

public:
    CacheManager() {}

    Premover& getPremover(vector<int> pieces, string filename="") {
        if (premoverMap.find(pieces) != premoverMap.end()) {
            return *premoverMap[pieces];
        } else {
            Premover* p = new Premover( pieces);
            if (!p->load(filename)) {
                p->generate();
                p->save(filename);
            }

            premoverMap[pieces] = p;
            //premoverMap.emplace(make_pair(pieces, Premover(pieces)));
            return *p;
        }
    }

    WalkingDist& getWD() {
        if (!wd_used) {
            wd_used = true;
            wd_inst = new WalkingDist();
        }
        return *wd_inst;
    }

    ~CacheManager() {
        for (auto p : premoverMap) {
            delete p.second;
        }
    }
    // TODO: implement LRU
};

#endif
