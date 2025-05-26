#include "espresso.hpp"
#include <map>
#include <set>
#include <algorithm>
#include <iostream>

using namespace std;


vector<Term> expand(const vector<Term>& onSet, const vector<Term>& dcSet, int numVars) {
    vector<Term> expanded = onSet;
    bool merged;
        do {
            merged = false;
            vector<Term> newTerms;

            for (size_t i = 0; i < expanded.size(); i++) {
                for (size_t j = i + 1; j < expanded.size(); j++) {
                    if (expanded[i].canCombineWith(expanded[j])) {
                        Term combined = expanded[i].combineWith(expanded[j]);
                        if (find(newTerms.begin(), newTerms.end(), combined) == newTerms.end()) {
                            newTerms.push_back(combined);
                            merged = true;
                        }
                    }
                }

                for (const Term& dc : dcSet) {
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

vector<Term> reduce(const vector<Term>& expanded, const vector<Term>& onSet, int numVars) {
    vector<Term> reduced = expanded;
    // Remove redundant terms or literals
    for (auto it = reduced.begin(); it != reduced.end(); ) {
            vector<Term> testCover = reduced;
            testCover.erase(remove(testCover.begin(), testCover.end(), *it), testCover.end());

            // Build covered set with remaining terms
            set<int> coveredMinterms;
            for (const Term& t : testCover) {
                for (int m : t.getCoveredMinterms()) {
                    coveredMinterms.insert(m);
                }
            }

            // Check if current term is essential or redundant
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
            if (terms.size() == 1) {
                if (selected.find(terms[0]) == selected.end()) {
                    essential.push_back(terms[0]);
                    selected.insert(terms[0]);
                }
            }
        }

        // Add remaining non-redundant terms to cover full ON-set
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

// Step 1: Group cubes by output index (one vector per output bit)
vector<vector<PLACube>> groupByOutput(const vector<PLACube>& cubes, int numOutputs) {
    vector<vector<PLACube>> outputs(numOutputs);

    for (const auto& cube : cubes) {
        for (int i = 0; i < numOutputs; i++) {
            if (cube.outputBits[i] == '1') {
                outputs[i].push_back(cube);
            }
        }
    }

    return outputs;
}

// Step 2: Run Espresso on each output function independently
vector<vector<Term>> runEspresso(const vector<vector<PLACube>>& groupedCubes, int numVars) {
    vector<vector<Term>> results;

    for (const auto& outputCubes : groupedCubes) {
        // Separate 1(on) and dont care terms
        vector<Term> onSet;
        vector<Term> dcSet;

        for (const auto& cube : outputCubes) {
            if (cube.term.isDontCareTerm()) {
                dcSet.push_back(cube.term);
            } else {
                onSet.push_back(cube.term);
            }
        }

        //  Espresso logic: expand -> reduce -> essential
        vector<Term> expanded = expand(onSet, dcSet, numVars);
        vector<Term> reduced = reduce(expanded, onSet, numVars);
        vector<Term> essential = extractEssential(reduced, onSet, numVars);

        results.push_back(essential);
    }

    return results;
}

// Step 3: Convert final terms to SOP strings per output
vector<string> espressoTermsToSOP(const vector<vector<Term>>& minimizedPerOutput, int numVars) {
    vector<string> expressions;

    for (const auto& terms : minimizedPerOutput) {
        string sop;

        for (size_t i = 0; i < terms.size(); i++) {
            const string& bin = terms[i].getBinary();
            string term;

            for (int j = 0; j < numVars; j++) {
                if (bin[j] == '-') continue;
                term += (bin[j] == '1') ? ('A' + j) : ('A' + j);
                if (bin[j] == '0') term += '\'';
            }

            sop += term;
            if (i != terms.size() - 1) sop += " + ";
        }

        expressions.push_back(sop);
    }

    return expressions;
}


