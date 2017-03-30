// -*- mode: c++; -*-

#ifndef RETA_DFA_HPP
#define RETA_DFA_HPP

#include <vector>

using namespace std;

#include "nfa.hpp"

struct dfa_t {
    using  int_type = int;
    using size_type = size_t;

    vector< vector< pair< int_type, size_type > > > states;
    vector< size_type > accept;
};

dfa_t make_dfa (const nfa_t&);

#endif // RETA_DFA_HPP
