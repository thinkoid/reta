// -*- mode: c++; -*-

#include <cassert>

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <stack>
#include <vector>

using namespace std;

#include <reta/nfa.hpp>
#include <reta/util.hpp>

/* static */ constexpr nfa_t::int_type nfa_t::epsilon /* = -1 */;

////////////////////////////////////////////////////////////////////////

istream& operator>> (istream& ss, nfa_t& a) {
    ss >> a.start;

    size_t n;
    ss >> n;

    a.states.resize (n);

    ss >> n;

    for (size_t i = 0; i < n; ++i) {
        int from, c, to;
        ss >> from >> c >> to;

        a.states [from].emplace_back (c, to);
    }

    ss >> n;

    for (size_t i = 0, accept; i < n && ss >> accept; ++i)
        a.accept.emplace_back (accept);

    return ss;
}

ostream& operator<< (ostream& ss, const nfa_t& a) {
    size_t from = 0;

    ss << "0 " << a.states.size () << ' ' <<
        accumulate (
            a.states.begin (), a.states.end (), 0,
            [](const auto memo, const auto& arg) {
                return memo + arg.size ();
            })
       << ' ';

    for (auto s : a.states) {
        sort (s.begin (), s.end (), [](const auto& lhs, const auto& rhs) {
                return
                    lhs.first < rhs.first ||
                    lhs.first == rhs.first && lhs.second < rhs.second;
            });

        for (const auto& t : s) {
            const auto to = t.second;
            ss << from << ' ' << t.first << ' ' << to << ' ';
        }

        ++from;
    }

    ss << a.accept.size () << ' ';

    copy (
        a.accept.begin (), a.accept.end (),
        ostream_iterator< size_t > (ss, " "));

    return ss;
}

////////////////////////////////////////////////////////////////////////

namespace detail {

struct nfa_state_t {
    nfa_t nfa;
    stack< size_t > st;
};

static void
nfa_consume_literal (int c, nfa_state_t& state) {
    auto& nfa = state.nfa;

    const auto n = nfa.states.size ();

    nfa.states.resize (n + 2);
    nfa.states [n].emplace_back (c, n + 1);

    auto& st = state.st;

    st.push (n);
    st.push (n + 1);
}

static void
nfa_consume_concatenation (nfa_state_t& state) {
    auto& st = state.st;
    assert (3 < st.size ());

    const size_t d = st.top (); st.pop ();
    const size_t c = st.top (); st.pop ();
    const size_t b = st.top (); st.pop ();
    const size_t a = st.top (); st.pop ();

    auto& nfa = state.nfa;
    nfa.states [b].emplace_back (nfa_t::epsilon, c);

    st.push (a);
    st.push (d);
}

static void
nfa_consume_kleene_closure (nfa_state_t& state) {
    auto& nfa = state.nfa;

    const auto n = state.nfa.states.size ();
    nfa.states.resize (n + 2);

    auto& st = state.st;
    assert (1 < st.size ());

    const size_t b = st.top (); st.pop ();
    const size_t a = st.top (); st.pop ();

    auto& states = nfa.states;

    states [n].emplace_back (nfa_t::epsilon, a);
    states [n].emplace_back (nfa_t::epsilon, n + 1);

    states [b].emplace_back (nfa_t::epsilon, a);
    states [b].emplace_back (nfa_t::epsilon, n + 1);

    st.push (n);
    st.push (n + 1);
}

static void
nfa_consume_alternation (nfa_state_t& state) {
    auto& nfa = state.nfa;

    const auto n = state.nfa.states.size ();
    nfa.states.resize (n + 2);

    auto& st = state.st;
    assert (3 < st.size ());

    const size_t d = st.top (); st.pop ();
    const size_t c = st.top (); st.pop ();
    const size_t b = st.top (); st.pop ();
    const size_t a = st.top (); st.pop ();

    nfa.states [n].emplace_back (nfa_t::epsilon, a);
    nfa.states [n].emplace_back (nfa_t::epsilon, c);

    nfa.states [b].emplace_back (nfa_t::epsilon, n + 1);
    nfa.states [d].emplace_back (nfa_t::epsilon, n + 1);

    st.push (n);
    st.push (n + 1);
}

} // namespace detail

nfa_t
make_nfa (const string& s) {
    detail::nfa_state_t state;

    for (const auto c : s) {
        assert (0 <= c && c <= (numeric_limits< char >::max) ());

        if ('a' <= c && c <= 'z')
            nfa_consume_literal (c, state);
        else if ('.' == c)
            nfa_consume_concatenation (state);
        else if ('*' == c)
            nfa_consume_kleene_closure (state);
        else if ('|' == c)
            nfa_consume_alternation (state);
        else
            assert (0);
    }

    auto& nfa = state.nfa;

    nfa.accept.emplace_back (state.st.top ());
    state.st.pop ();

    nfa.start = state.st.top ();

    return move (nfa);
}
