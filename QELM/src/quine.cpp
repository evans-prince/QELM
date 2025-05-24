#include "quine.hpp"
#include "utils.hpp"
#include "term.hpp"
#include "combine.hpp"

#include <set>
#include <vector>
#include <map>
#include <iostream>

//Quine McCluskey algorithm
std::vector<Term> runQuine(const std::vector<Term>& minterms, const std::vector<Term>& dontCares) {
    std::vector<Term> allTerms = minterms;
    allTerms.insert(allTerms.end(), dontCares.begin(), dontCares.end());

    std::vector<Term> current = allTerms;
    std::vector<Term> nextRound;
    std::vector<Term> primeImplicants;

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
    std::vector<Term> essentialPIs;
    std::map<int, std::vector<Term>> chart;

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

    std::set<Term> added;

    for (auto it = chart.begin(); it != chart.end(); it++) {
        const std::vector<Term>& terms = it->second;

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
    
    // Step 1: Identify uncovered minterms
    std::set<int> covered;
    for (const Term& epi : essentialPIs) {
        for (int m : epi.getCoveredMinterms()) {
            covered.insert(m);
        }
    }

    std::set<int> uncovered;
    for (const auto& [m, _] : chart) {
        if (!covered.count(m)) {
            uncovered.insert(m);
        }
    }

    // Step 2: Build Petrick expression
    std::vector<std::set<std::set<int>>> petrickProduct;  // Each inner set is a term (product of implicants)

    std::map<Term, int> piToIndex;
    std::vector<Term> piIndexList;
    int index = 0;
    for (const Term& pi : primeImplicants) {
        if (piToIndex.find(pi) == piToIndex.end()) {
            piToIndex[pi] = index++;
            piIndexList.push_back(pi);
        }
    }

    for (int m : uncovered) {
        std::set<std::set<int>> clause;
        for (const Term& pi : chart[m]) {
            clause.insert({ piToIndex[pi] });
        }
        petrickProduct.push_back(clause);
    }

    // Step 3: Expand the product
    std::set<std::set<int>> petrickResult = petrickProduct[0];
    for (size_t i = 1; i < petrickProduct.size(); i++) {
        std::set<std::set<int>> newResult;
        for (const auto& a : petrickResult) {
            for (const auto& b : petrickProduct[i]) {
                std::set<int> merged = a;
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

    return essentialPIs;
}

std::string termsToSOP(const std::vector<Term>& terms, int numVars) {
    std::string result;

    for (size_t i = 0; i < terms.size(); i++) {
        const std::string& bin = terms[i].getBinary();

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
