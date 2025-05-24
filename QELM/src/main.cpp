#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "term.hpp"
#include "quine.hpp"

using namespace std;

// Helper to parse PLA format
bool parsePLA(const string& filename, int& numVars, vector<Term>& minterms, vector<Term>& dontCares) {
    ifstream fin(filename);
    if (!fin) {
        cerr << "Error opening input file\n";
        return false;
    }

    string line;
    numVars = 0;
    minterms.clear();
    dontCares.clear();

    while (getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue; // skip comments & empty lines

        if (line.substr(0, 3) == ".i ") {
            numVars = stoi(line.substr(3));
        } else if (line.substr(0, 2) == ".e") {
            break; // end of PLA
        } else if (line[0] == '0' || line[0] == '1' || line[0] == '-') {
            istringstream iss(line);
            string inputBits, outputBits;
            iss >> inputBits >> outputBits;

            // Convert binary string to decimal
            int decimal = 0;
            for (char c : inputBits) {
                decimal <<= 1;
                if (c == '1') decimal |= 1;
            }

            set<int> covered = {decimal};
            Term t(inputBits, covered, outputBits == "-");

            if (outputBits == "1") {
                minterms.push_back(t);
            } else if (outputBits == "-") {
                dontCares.push_back(t);
            }
        }
    }

    fin.close();
    return true;
}





int main() {
    const string inputFile = "./data/input.txt";
    const string outputFile = "./data/output.txt";

    int numVars;
    vector<Term> minterms;
    vector<Term> dontCares;

    if (!parsePLA(inputFile, numVars, minterms, dontCares)) {
        cerr << "Failed to parse PLA input\n";
        return 1;
    }

    ofstream fout(outputFile);
    if (!fout) {
        cerr << "Error opening output file\n";
        return 1;
    }

    if (numVars > 10) {
        fout << "Too many variables (" << numVars << ") to run Quine-McCluskey minimization.\n";
        cout << "Variables > 10, skipping minimization.\n";
    } else {
        vector<Term> essentialPIs = runQuine(minterms, dontCares);
        string sop = termsToSOP(essentialPIs, numVars);
        fout << sop << "\n";
        cout << "Minimization done! Check output.txt\n";
    }

    fout.close();
    return 0;
}
