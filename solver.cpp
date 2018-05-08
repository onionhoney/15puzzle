#include "solver.h"
using namespace std;


vector<Move> ida_star(Grid& root, Heuristics& heuristics, Heuristics& heur_aux, vector<int>& pieces);

int global_expanded = 0;

bool toBeSolved[SIZE * SIZE];

std::vector<Move>* pathList;
Grid* node;
Heuristics* heur;
WalkingDist* heur_aux_gl;

int bound;

ll goalState;
ll goalMask;

string pdb_used = "555";

int search(
        int g_cost, 
        int cwc, 
        int ccwc, 
        Move prev_move,
        int wdidx1, 
        int wdidx2,
        int wd_h
) { 
    // Grid node = nodeList.back();
    // MISTAKE OF A LIFETIME: MISUSE OF REFS. 
    // NO!!!! It gets moooooved!!! 
    global_expanded++;
    int h_cost = heur->h(*node);

    if (heur_aux_gl != nullptr) {
        h_cost = max(h_cost, wd_h);
    }
    int f_cost = g_cost + h_cost;
    if (cwc > 4 || ccwc > 5) return 100000;
    if (f_cost > bound) return f_cost;

    if ( (node->hash(goalMask)) == goalState) {
        return FOUND;
    }

    int lowerBound = 100000;

    for (Move move= UP; move <= LEFT; move = (Move)((int)move + 1)) {
        if (oppositeMove(move) == prev_move) continue;
        if (node->can_move(move)) {
            char content = node->get_swap_content(move);
            int tidx1, tidx2;
            if (heur_aux_gl != nullptr) {
                tidx1 = wdidx1, tidx2 = wdidx2;
                wd_h = heur_aux_gl->h(*node, tidx1, tidx2, move, content);
            }

            node->move_to_with_confidence(move);


            pathList->push_back(move);
            int ncwc = 0, nccwc = 0;
            if (move == prev_move) {
            } else if (  (prev_move == UP && move == RIGHT) ||
                         (prev_move == RIGHT && move == DOWN) ||
                         (prev_move == DOWN && move == LEFT ) ||
                         (prev_move == LEFT && move == UP) ) {
                ncwc = cwc + 1; ncwc = 0;
            } else {
                ncwc = 0; nccwc = ccwc + 1;
            }

            int leastEstimate = search(g_cost + 1, ncwc, nccwc, move, tidx1, tidx2, wd_h);
            if (leastEstimate == FOUND) {
                return FOUND;
            }
            if (leastEstimate < lowerBound) {
                lowerBound = leastEstimate;
            }
            node->move_back(move);
            pathList->pop_back();
        }
    }
    return lowerBound;
}

vector<Move> ida_star(Grid& root, Heuristics& heuristics, WalkingDist& heur_aux, vector<int>& pieces) {
    vector<Move> pList;
    global_expanded = 0;

    goalState = 0, goalMask = 0;
    for (int i = SIZE * SIZE - 1; i>= 0; i--) { // i < SIZE * SIZE; i++) {
        if (find(pieces.begin(), pieces.end(), i) != pieces.end()) {
            goalState = (goalState << 4) | i;
            goalMask = (goalMask << 4) | 15;
        } else {
            goalState = (goalState << 4) | 0;
            goalMask=  (goalMask << 4)  | 0;
        }
    }

    bound = heuristics.h(root);
    int wdidx1, wdidx2;
    int wd_bound = heur_aux.h(root, wdidx1, wdidx2);
    bound = max(bound, wd_bound);

    for (;;) { 
        if (global_expanded > 0)
            cerr << " Nodes expanded cumulative = " << global_expanded << endl;
        cerr << "Trying bound " << bound << endl;

        pathList = &pList;
        node = &root;
        heur = &heuristics;
        heur_aux_gl = &heur_aux;

        int leastEstimate = search(0, 0, 0, OTHER, wdidx1, wdidx2, wd_bound);
        if (leastEstimate == FOUND) {
            cerr << "Nodes expanded cumulative = " << global_expanded << endl;
            return pList;
        }
        bound = max(leastEstimate, bound + 2);
    }
}


extern int global_manh[SIZE*SIZE][SIZE*SIZE];
string heur_used = "pdb";

extern "C" {
void solve(int* grid, int nPieces, int* pieces, int* sol) {
    vector<int> gridVec(grid, grid + SIZE * SIZE);

    cout << "SOLVING ";
    for (int g : gridVec) cout << g << " "; cout << endl << endl << endl;
    cout << "TARGET " ;
    for (int i = 0; i < nPieces; i++) cout << pieces[i] << " "; cout << endl;
    vector<int> piecesToSolve(pieces, pieces + nPieces);
    for (int i = 0; i < piecesToSolve.size(); i++) piecesToSolve[i] = orthogonalIndex(piecesToSolve[i]);

    bool precomputed = false;
    static CacheManager cacheManager; 

    Grid root(gridVec); 
    cout << "solving " << root << endl;

    static LinearConflict lc(piecesToSolve);
    static PatternDB pdb(cacheManager, pdb_used);
    Heuristics* heur = nullptr;

    // HOW TO DO THIS?
    if (heur_used == "lc") {
        lc.init();
        heur = &lc;
    } else {
        pdb.init();
        heur = &pdb;
    }
    WalkingDist& heur_aux = cacheManager.getWD();

    vector<Move> solution = ida_star(root, *heur, heur_aux, piecesToSolve);
    for (size_t i = 0; i < solution.size(); i++) {
        sol[i] = solution[i];
    }
    sol[solution.size()] = -1;
}
}

vector<Move> solveVec(vector<int> grid, vector<int> pieces) {
    const int MAXSOL = 80 + 1;
    int sol[MAXSOL] = {0};
    solve(grid.data(), pieces.size(), pieces.data(), sol);
    int n;
    for (n = 0; n < MAXSOL; n++)
        if (sol[n] == -1) break;
    vector<Move> solution(n);
    for (int i = 0 ;i < n; i++) 
        solution[i] = Move(sol[i]);
    return solution;
}


bool solveable(vector<int>& grid, int width=SIZE) {
    /* https://www.cs.bham.ac.uk/~mdr/teaching/modules04/java2/TilesSolvability.html
       The formula says:
       If the grid width is odd, then the number of inversions in a solvable situation is even.
       If the grid width is even, and the blank is on an even row counting from the bottom (second-last, fourth-last etc), then the number of inversions in a solvable situation is odd.
       If the grid width is even, and the blank is on an odd row counting from the bottom (last, third-last, fifth-last etc) then the number of inversions in a solvable situation is even.
    */
    
    int inversionParity = 0;
    for (int i = 0; i < width * width - 1; i++) {
        for (int j = i + 1; j < width * width; j++) {
            if (grid[i] == 0 || grid[j] == 0) continue;
            if (grid[i] > grid[j]) inversionParity++;
        }
    }
    inversionParity %= 2;

    int blankRowParity = 0;
    for (int i = 0; i < width * width; i++) {
        if (grid[i] == 0) {
            blankRowParity = ((SIZE ) - i / SIZE) % 2;
        }
    }
    if (width % 2 == 1) {
        return inversionParity == 0;
    } else {
        return (blankRowParity == 0 && inversionParity == 1) ||
               (blankRowParity == 1 && inversionParity == 0);
    }
}



int main(int argc, char** argv) {
    vector<int> allPieces(SIZE * SIZE);
    for (int i = 0; i < SIZE * SIZE; i++) allPieces[i] = i;

    vector<int> FLPieces = {1, 2, 3, 4, 5, 9, 13};
    srand(time(NULL));
    if (argc >= 3) {
        heur_used = argv[2];
    }
    if (argc >= 4) {
        pdb_used = argv[3];
    }

    if (argc >= 2 && strcmp(argv[1], "bench") == 0) {
        vector<int> gridArr(SIZE * SIZE);
        for (int i = 0; i < SIZE * SIZE; i++) {
            gridArr[i] = (i == SIZE * SIZE - 1) ? 0 : (i + 1);
        }

        double total_duration = 0;
        long long total_expanded = 0;

        long long total_mvc = 0;
        int cnt = 0;
        for (int i = 0; i < BENCH_SIZE; i++) {
            std::clock_t start = std::clock();
            do {
                std::random_shuffle(gridArr.begin(), gridArr.end());
            } while (!solveable(gridArr));

            Grid g(gridArr);
            auto sol = solveVec(gridArr, allPieces);

            if (i == 0) continue; // do not count initializiations
            cnt++;

            double dura = (std::clock() - start) / (double)CLOCKS_PER_SEC;
            total_expanded += global_expanded;
            total_duration += dura;

            total_mvc += sol.size();

            cout << "sol size = " << sol.size() << endl;
            cout << "Time = " << dura << endl;

            cout << "Avg = " << total_duration / cnt << endl;
            cout << "Avg expanded = " << total_expanded / cnt << endl;
            cout << "Avg movecount = " << 1.0 * total_mvc / cnt << endl;
        }

        
    } else if (argc >= 2 && strcmp(argv[1], "solve") == 0){
        vector<int> gridArr(SIZE * SIZE);
        for (int i = 0; i < SIZE * SIZE; i++)  {
            cin >> gridArr[i];
        }
        cout << solveVec(gridArr, allPieces) << endl;
    } else if (argc >= 2 && strcmp(argv[1], "fl") == 0){
        vector<int> gridArr(SIZE * SIZE);
        for (int i = 0; i < SIZE * SIZE; i++)  {
            cin >> gridArr[i];
        }
        cout << solveVec(gridArr, FLPieces) << endl;
    } else if (argc >= 2 && strcmp(argv[1], "all") == 0){
        vector<int> gridArr(SIZE * SIZE);
        for (int i = 0; i < SIZE * SIZE; i++)  {
            cin >> gridArr[i];
        }
        cout << solveVec(gridArr, FLPieces) << endl;
        cout << solveVec(gridArr, allPieces) << endl;
    }

}
