// -*- mode: c++; -*-

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

#include <reta/nfa.hpp>
#include <reta/dfa.hpp>

int main (int, char** argv) {
    string s (argv [1]);
    cout << "# --> regex   : " << s << endl;

    s = postfix (s);
    cout << "# --> postfix : " << s << endl;

    const auto nfa = make_nfa (s);

    {
        stringstream ss;
        ss << nfa;

        cout << ss.str () << endl;

        nfa_t other;
        ss >> other;

        cout << other << endl;
    }

    const auto dfa = make_dfa (nfa);

    {
        stringstream ss;
        ss << dfa;

        cout << ss.str () << endl;

        dfa_t other;
        ss >> other;

        cout << other << endl;
    }

    return 0;
}
