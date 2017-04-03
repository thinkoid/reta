// -*- mode: c++; -*-

#ifndef RETA_NFA_HPP
#define RETA_NFA_HPP

#include <string>
#include <vector>

using namespace std;

struct nfa_t {
    using  int_type = int;
    using size_type = size_t;

    vector< vector< pair< int_type, size_type > > > states;
    vector< size_type > accept;
    size_type start;

    static constexpr int_type epsilon = -1;
};

string postfix (const string&);
nfa_t make_nfa (const string&);

istream& operator>> (istream&, nfa_t&);
ostream& operator<< (ostream&, const nfa_t&);

#endif // RETA_NFA_HPP
