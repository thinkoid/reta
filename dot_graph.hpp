// -*- mode: c++; -*-

#ifndef RETA_DOT_GRAPH_HPP
#define RETA_DOT_GRAPH_HPP

#include <string>

using namespace std;

#include "nfa.hpp"
#include "dfa.hpp"

struct dot_graph_t {
    explicit dot_graph_t (const nfa_t& nfa)
        : value_ (make_dot (nfa))
        { }

    explicit dot_graph_t (const dfa_t& dfa)
        : value_ (make_dot (dfa))
        { }

    const string& value () const {
        return value_;
    }

private:
    static string  make_dot (const nfa_t&);
    static string  make_dot (const dfa_t&);

private:
    string value_;
};

#endif // RETA_DOT_GRAPH_HPP
