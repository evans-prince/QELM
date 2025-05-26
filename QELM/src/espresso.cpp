#include "espresso.hpp"
#include "utils.hpp"
#include <map>
#include <set>
#include <algorithm>
#include <iostream>
#include <random>
#include <ctime>

using namespace std;

// === Heuristic EXPAND with randomization ===
vector<Term> expand(const vector<Term>& onSet, const vector<Term>& dcSet, int numVars) {
    vector<Term> expanded = onSet;
    bool merged;
    //This line creates a random number generator (rng) using the Mersenne Twister 19937 algorithm
    mt19937 rng(static_cast<unsigned int>(time(nullptr)));

    do {
        merged = false;
        vector<Term> newTerms;

        vector<size_t> indices(expanded.size());
        iota(indices.begin(), indices.end(), 0);
        shuffle(indices.begin(), indices.end(), rng);

        for (size_t iIdx = 0; iIdx < indices.size(); iIdx++) {
            for (size_t jIdx = iIdx + 1; jIdx < indices.size(); jIdx++) {
                size_t i = indices[iIdx];
                size_t j = indices[jIdx];
                if (expanded[i].canCombineWith(expanded[j])) {
                    Term combined = expanded[i].combineWith(expanded[j]);
                    if (find(newTerms.begin(), newTerms.end(), combined) == newTerms.end()) {
                        newTerms.push_back(combined);
                        merged = true;
                    }
                }
            }
        }

        for (const Term& dc : dcSet) {
            for (size_t i = 0; i < expanded.size(); i++) {
                if (expanded[i].canCombineWith(dc)) {
                    Term combined = expanded[i].combineWith(dc);
                    if (find(newTerms.begin(), newTerms.end(), combined) == newTerms.end()) {
                        newTerms.push_back(combined);
                        merged = true;
                    }
                }
            }
        }

        for (const Term& t : newTerms) {
            if (find(expanded.begin(), expanded.end(), t) == expanded.end()) {
                expanded.push_back(t);
            }
        }
    } while (merged);

    return expanded;
}

// === Heuristic REDUCE: literal count based cost ===
vector<Term> reduce(const vector<Term>& expanded, const vector<Term>& onSet, int numVars) {
    vector<Term> reduced = expanded;

    for (auto it = reduced.begin(); it != reduced.end(); ) {
        vector<Term> testCover = reduced;
        testCover.erase(remove(testCover.begin(), testCover.end(), *it), testCover.end());

        set<int> coveredMinterms;
        for (const Term& t : testCover) {
            for (int m : t.getCoveredMinterms()) {
                coveredMinterms.insert(m);
            }
        }

        bool isEssential = false;
        for (int m : it->getCoveredMinterms()) {
            if (coveredMinterms.find(m) == coveredMinterms.end()) {
                isEssential = true;
                break;
            }
        }

        if (isEssential) {
            it++;
        } else {
            it = reduced.erase(it);
        }
    }

    return reduced;
}

vector<Term> extractEssential(const vector<Term>& reduced, const vector<Term>& onSet, int numVars) {
    vector<Term> essential;
    map<int, vector<Term>> coverageChart;

    for (const auto& term : reduced) {
        for (int minterm : term.getCoveredMinterms()) {
            coverageChart[minterm].push_back(term);
        }
    }

    set<Term> selected;

    for (const auto& [minterm, terms] : coverageChart) {
        if (terms.size() == 1 && selected.find(terms[0]) == selected.end()) {
            essential.push_back(terms[0]);
            selected.insert(terms[0]);
        }
    }

    set<int> covered;
    for (const Term& t : essential) {
        for (int m : t.getCoveredMinterms()) {
            covered.insert(m);
        }
    }

    for (const Term& t : reduced) {
        bool addsCoverage = false;
        for (int m : t.getCoveredMinterms()) {
            if (covered.find(m) == covered.end()) {
                addsCoverage = true;
                break;
            }
        }

        if (addsCoverage && selected.find(t) == selected.end()) {
            essential.push_back(t);
            for (int m : t.getCoveredMinterms()) {
                covered.insert(m);
            }
            selected.insert(t);
        }
    }

    return essential;
}

vector<Term> runEspressoOnce(const vector<Term>& onSet, const vector<Term>& dcSet, int numVars) {
    vector<Term> expanded = expand(onSet, dcSet, numVars);
    vector<Term> reduced = reduce(expanded, onSet, numVars);
    return extractEssential(reduced, onSet, numVars);
}



vector<Term> runEspressoMultiple(const vector<Term>& onSet, const vector<Term>& dcSet, int numVars, int passes) {
    vector<Term> best = runEspressoOnce(onSet, dcSet, numVars);
    int minLiterals = countLiterals(best);

    for (int i = 1; i < passes; i++) {
        vector<Term> current = runEspressoOnce(onSet, dcSet, numVars);
        int currentLiterals = countLiterals(current);

        if (currentLiterals < minLiterals) {
            best = current;
            minLiterals = currentLiterals;
        }
    }

    return best;
}
