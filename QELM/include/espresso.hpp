
#ifndef espresso_hpp
#define espresso_hpp

#include <stdio.h>

#include <vector>
#include "term.hpp"

using namespace std;

// Structure to represent a PLA table (input-output pairs)

struct PLACube {
    Term term;
    string outputBits;

    PLACube(const Term& t, const string& out)
        : term(t), outputBits(out) {}
};

// Reads PLA format and returns cubes for each output function

vector<vector<PLACube>> groupByOutput(const vector<PLACube>& cubes, int numOutputs);

// Core Espresso function: takes grouped cubes and returns minimized result

vector<Term> runEspressoOnce(const vector<Term>& onSet, const vector<Term>& dcSet, int numVars);

vector<Term> runEspressoMultiple(const vector<Term>& onSet, const vector<Term>& dcSet, int numVars, int passes) ;

// Utility function to convert minimized cubes to SOP string

vector<string> espressoTermsToSOP(const vector<vector<Term>>& result, int numVars);

// Core functions

vector<Term> expand(const vector<Term>& onSet, const vector<Term>& dcSet, int numVars);

vector<Term> reduce(const vector<Term>& expanded, const vector<Term>& onSet, int numVars);

vector<Term> extractEssential(const vector<Term>& reduced, const vector<Term>& onSet, int numVars);

#endif /* espresso_hpp */
