// -*- mode: c++; -*-

#include <iostream>
#include <string>

using namespace std;

#include <reta/nfa.hpp>
#include <reta/dfa.hpp>
#include <reta/dot-graph.hpp>

int main (int, char** argv) {
    string s (argv [1]);
    cout << "# --> regex   : " << s << endl;

    s = postfix (s);
    cout << "# --> postfix : " << s << endl;

    const auto nfa = make_nfa (s);
    cout << dot_graph_t (nfa).value () << endl;

    const auto dfa = make_dfa (nfa);
    cout << dot_graph_t (dfa).value () << endl;

    const auto dfa2 = minimize_dfa_table (dfa);
    cout << dot_graph_t (dfa2, "min-dfa").value () << endl;

    return 0;
}
