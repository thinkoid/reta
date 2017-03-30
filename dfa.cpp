// -*- mode: c++; -*-

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

#include "dfa.hpp"

namespace detail {

struct dfa_state_t {
    using int_type = int;
    using size_type = size_t;

    map< set< size_type >, size_type > closures;
    map< size_type, vector< pair< int_type, size_type > > > transitions;
    vector< size_type > accept;
};

static void
do_epsilon_closure (const nfa_t& nfa, size_t state, set< size_t >& closure) {
    for (const auto& t : nfa.states [state]) {
        const auto c = t.first;

        if (c == nfa_t::epsilon) {
            const auto dst = t.second;

            const auto iter = closure.find (dst);

            if (iter == closure.end ()) {
                closure.insert (dst);
                do_epsilon_closure (nfa, dst, closure);
            }
        }
    }
}

static set< size_t >
epsilon_closure (const nfa_t& nfa, size_t n) {
    set< size_t > closure { n };
    do_epsilon_closure (nfa, n, closure);
    return closure;
}

static inline bool
accepting (const nfa_t& nfa, const set< size_t >& c) {
    const auto& a = nfa.accept;

    return any_of (c.begin (), c.end (), [&](const auto n) {
        return a.end () == find (a.begin (), a.end (), n);
    });
}

} // namespace detail

dfa_t
make_dfa (const nfa_t& nfa) {
    detail::dfa_state_t dfa_state { };

    set< set< size_t > > closures { detail::epsilon_closure (nfa, nfa.start) };
    const auto& initial_closure = *closures.begin ();

    size_t state_counter = 0;
    dfa_state.closures.emplace (initial_closure, state_counter);

    if (detail::accepting (nfa, initial_closure))
        dfa_state.accept.emplace_back (state_counter);

    ++state_counter;

    while (!closures.empty ()) {
        set< set< size_t > > accum;

        for (const auto& closure : closures) {
            map< int, set< size_t > > transitions;

            const auto from = dfa_state.closures [closure];

            for (const auto state : closure) {
                for (const auto& t : nfa.states [state]) {
                    if (nfa_t::epsilon == t.first)
                        continue;

                    const auto s = detail::epsilon_closure (nfa, t.second);
                    transitions [t.first].insert (s.begin (), s.end ());
                }
            }

            for (const auto& t : transitions) {
                size_t to = 0;

                const auto iter = dfa_state.closures.find (t.second);

                if (iter == dfa_state.closures.end ()) {
                    to = state_counter++;
                    dfa_state.closures.emplace (t.second, to);

                    if (detail::accepting (nfa, t.second))
                        dfa_state.accept.emplace_back (to);

                    accum.insert (t.second);
                }
                else {
                    to = iter->second;
                }

                dfa_state.transitions [from].emplace_back (t.first, to);
            }
        }

        closures = accum;
    }

    dfa_t dfa;

    for (auto& t : dfa_state.transitions)
        dfa.states.emplace_back (move (t.second));

    for (auto& t : dfa.states)
        sort (t.begin (), t.end (), [](const auto& lhs, const auto& rhs) {
                return lhs.first < rhs.first;
            });

    dfa.accept = dfa_state.accept;

    auto& a = dfa.accept;
    sort (a.begin (), a.end ());

    return dfa;
}
