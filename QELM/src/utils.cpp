#include "utils.hpp"

using namespace std;

map<int, vector<Term>> groupByOnes(const vector<Term>& terms) {
    map<int, vector<Term>> grouped;
    for (const Term& t : terms) {
        int ones = t.countOnes();
        grouped[ones].push_back(t);
    }
    return grouped;
}


int countLiterals(const vector<Term>& terms) {
    int total = 0;
    for (const Term& t : terms) {
        string bin = t.getBinary();
        for (char c : bin) {
            if (c != '-') {
                total++;
            }
        }
    }
    return total;
}
