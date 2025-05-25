#include "quine.hpp"
#include "utils.hpp"
#include "term.hpp"
#include "combine.hpp"

#include <set>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

//Quine McCluskey algorithm
vector<Term> runQuine(const vector<Term>& minterms, const vector<Term>& dontCares) {
    vector<Term> allTerms = minterms;
    allTerms.insert(allTerms.end(), dontCares.begin(), dontCares.end());

    vector<Term> current = allTerms;
    vector<Term> nextRound;
    vector<Term> primeImplicants;

    //  Keep combining until no more combinations possible
    while (true) {
        nextRound = combineTerms(current);

        //  If nothing changed, we're done
        if (nextRound.size() == current.size()) {
            break;
        }

        current = nextRound;
    }
    

    primeImplicants=nextRound;
    //  Filter only those prime implicants that cover original minterms
    // some primeImplicants cover those minterms which are not needed as covered by other so filter those and left them
    vector<Term> essentialPIs;
    map<int, vector<Term>> chart;

    for (const Term& t : primeImplicants) {
        for (int m : t.getCoveredMinterms()) {
            bool isCovered = false;

            // Loop through original minterms to check if any of them cover 'm'
            for (const Term& mt : minterms) {
                if (mt.getCoveredMinterms().count(m)) {
                    isCovered = true;
                    break;
                }
            }

            if (isCovered) {
                chart[m].push_back(t);
            }
        }
    }

    set<Term> added;

    for (auto it = chart.begin(); it != chart.end(); it++) {
        const vector<Term>& terms = it->second;

        if (terms.size() == 1) {
            const Term& epi = terms[0];

            // Check if we’ve already added this essential prime implicant
            if (added.find(epi) == added.end()) {
                essentialPIs.push_back(epi);
                added.insert(epi);
            }
        }
    }
    
    // Using Petrick's method
    // final answer is of the form F= EPI + P
    // Where P is prime implicants so we need to find P
    
    // Step 1: Identify uncovered minterms
    set<int> covered;
    for (const Term& epi : essentialPIs) {
        for (int m : epi.getCoveredMinterms()) {
            covered.insert(m);
        }
    }

    set<int> uncovered;
    for (const auto& [m, _] : chart) {
        if (!covered.count(m)) {
            uncovered.insert(m);
        }
    }
    
    if(uncovered.size()==0){
        return essentialPIs;
    }

    // Step 2: Build Petrick expression
    vector<set<set<int>>> petrickProduct;  // Each inner set is a term (product of implicants)

    map<Term, int> piToIndex;
    vector<Term> piIndexList;
    int index = 0;
    for (const Term& pi : primeImplicants) {
        if (piToIndex.find(pi) == piToIndex.end()) {
            piToIndex[pi] = index++;
            piIndexList.push_back(pi);
        }
    }

    for (int m : uncovered) {
        set<set<int>> clause;
        for (const Term& pi : chart[m]) {
            clause.insert({ piToIndex[pi] });
        }
        petrickProduct.push_back(clause);
    }
    

    // Step 3: Expand the product
    
    if (petrickProduct.empty()) {
        return essentialPIs;  // Nothing to expand
    }
    
    set<set<int>> petrickResult = petrickProduct[0];// error in this line
    for (size_t i = 1; i < petrickProduct.size(); i++) {
        set<set<int>> newResult;
        for (const auto& a : petrickResult) {
            for (const auto& b : petrickProduct[i]) {
                set<int> merged = a;
                merged.insert(b.begin(), b.end());
                newResult.insert(merged);
            }
        }
        petrickResult = newResult;
    }

    // Step 4: Choose minimal term
    size_t minSize = SIZE_MAX;
    for (const auto& term : petrickResult) {
        if (term.size() < minSize) {
            minSize = term.size();
        }
    }

    for (const auto& term : petrickResult) {
        if (term.size() == minSize) {
            for (int i : term) {
                const Term& selected = piIndexList[i];
                if (added.find(selected) == added.end()) {
                    essentialPIs.push_back(selected);
                    added.insert(selected);
                }
            }
            break;
        }
    }

    // Final cleanup of redundant terms
    essentialPIs = combineTerms(essentialPIs);
    return essentialPIs;
}

string termsToSOP(const vector<Term>& terms, int numVars) {
    string result;

    for (size_t i = 0; i < terms.size(); i++) {
        const string& bin = terms[i].getBinary();

        for (int j = 0; j < numVars; j++) {
            if (bin[j] == '-') continue;

            char var = 'A' + j;
            if (bin[j] == '0') {
                result += var;
                result += '\'';
                
            }else {
                result += var;
            }
        }

        if (i != terms.size() - 1) result += " + ";
    }

    return result;
}
