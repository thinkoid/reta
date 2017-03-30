// -*- mode: c++; -*-

#include <cassert>

#include <functional>
#include <limits>
#include <stack>
#include <vector>

using namespace std;

#include "nfa.hpp"
#include "util.hpp"

/* static */ constexpr nfa_t::int_type nfa_t::epsilon /* = -1 */;

////////////////////////////////////////////////////////////////////////

string
postfix (const string& r) {
    string s;

    size_t alt = 0, exp = 0;

    vector< pair< size_t, size_t > > nests { { } };
    nests.reserve (16);

    for (const auto c : r) {
        switch (c) {
        case '(':
            if (exp > 1) {
                --exp;
                s += '.';
            }

            nests.emplace_back (alt, exp);
            alt = exp = 0;

            break;

        case '|':
            assert (exp);

            while (--exp > 0)
                s += '.';

            ++alt;
            break;

        case ')':
            while (--exp)
                s += '.';

            while (alt--)
                s += '|';

            tie (alt, exp) = nests.back ();
            nests.pop_back ();

            ++exp;
            break;

        case '*': case '+': case '?':
            assert (exp);
            s += c;
            break;

        default:
            if (exp > 1) {
                --exp;
                s += '.';
            }

            s += c;
            exp++;
            break;
        }
    }

    while (--exp > 0)
        s += '.';

    for (; alt > 0; --alt)
        s += '|';

    return s;
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
nfa_consume_optional (nfa_state_t& state) {
    auto& nfa = state.nfa;

    const auto n = state.nfa.states.size ();
    nfa.states.resize (n + 2);

    auto& st = state.st;
    assert (1 < st.size ());

    const size_t b = st.top (); st.pop ();
    const size_t a = st.top (); st.pop ();

    nfa.states [n].emplace_back (nfa_t::epsilon, a);
    nfa.states [n].emplace_back (nfa_t::epsilon, n + 1);

    nfa.states [b].emplace_back (nfa_t::epsilon, n + 1);

    st.push (n);
    st.push (n + 1);
}

static void
nfa_consume_multiple (nfa_state_t& state) {
    auto& nfa = state.nfa;

    const auto n = state.nfa.states.size ();
    nfa.states.resize (n + 2);

    auto& st = state.st;
    assert (1 < st.size ());

    const size_t b = st.top (); st.pop ();
    const size_t a = st.top (); st.pop ();

    nfa.states [n].emplace_back (nfa_t::epsilon, a);

    nfa.states [b].emplace_back (nfa_t::epsilon, a);
    nfa.states [b].emplace_back (nfa_t::epsilon, n + 1);

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
    static const int alphabet [] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 5, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 3,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 6, 0, 0, 0
    };

    detail::nfa_state_t state;

    for (const auto c : s) {
        assert (0 <= c && c <= (numeric_limits< char >::max) ());

        switch (alphabet [size_cast (c)]) {
        case 1:
            nfa_consume_literal (c, state);
            break;

        case 2:
            nfa_consume_kleene_closure (state);
            break;

        case 3:
            nfa_consume_optional (state);
            break;

        case 4:
            nfa_consume_multiple (state);
            break;

        case 5:
            nfa_consume_concatenation (state);
            break;

        case 6:
            nfa_consume_alternation (state);
            break;

        default:
            assert (0);
            break;
        }
    }

    auto& nfa = state.nfa;

    nfa.accept.emplace_back (state.st.top ());
    state.st.pop ();

    nfa.start = state.st.top ();

    return move (nfa);
}
