// -*- mode: c++; -*-

#ifndef RETA_DFA_HPP
#define RETA_DFA_HPP

#include <vector>

using namespace std;

#include <reta/nfa.hpp>

struct dfa_t {
    using  int_type = int;
    using size_type = size_t;

    vector< vector< pair< int_type, size_type > > > states;
    vector< size_type > accept;
    size_type start;
};

dfa_t make_dfa (const nfa_t&);
dfa_t minimize_dfa_table (const dfa_t&);

istream& operator>> (istream&, dfa_t&);
ostream& operator<< (ostream&, const dfa_t&);

#endif // RETA_DFA_HPP
