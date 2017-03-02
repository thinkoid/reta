// -*- mode: c++; -*-

#include <cassert>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

struct nfa_t {
    struct state_t {
        vector< size_t > a [2], e;
        bool accept;
    };

    vector< state_t > states;
    size_t start;
};

const auto default_nfa_state = nfa_t::state_t { };

struct nfa_state_t {
    nfa_t nfa;
    stack< int > st;
};

////////////////////////////////////////////////////////////////////////

static string
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

static void
nfa_consume_literal (int c, nfa_state_t& state) {
    auto& nfa = state.nfa;

    nfa.states.push_back (default_nfa_state);
    nfa.states.push_back (default_nfa_state);

    const auto n = state.nfa.states.size ();
    const auto beg = n - 2, end = n - 1;

    nfa.states [beg].a [c - 'a'].push_back (end);

    auto& st = state.st;

    st.push (beg);
    st.push (end);
}

static void
nfa_consume_concatenation (nfa_state_t& state) {
    auto& st = state.st;
    assert (3 < st.size ());

    int d = st.top (); st.pop ();
    int c = st.top (); st.pop ();
    int b = st.top (); st.pop ();
    int a = st.top (); st.pop ();

    auto& nfa = state.nfa;

    nfa.states [b].e.push_back (c);

    st.push (a);
    st.push (d);
}

static void
nfa_consume_kleene_closure (nfa_state_t& state) {
    auto& nfa = state.nfa;

    nfa.states.push_back (default_nfa_state);
    nfa.states.push_back (default_nfa_state);

    const auto n = state.nfa.states.size ();
    const auto beg = n - 2, end = n - 1;

    auto& st = state.st;
    assert (1 < st.size ());

    int b = st.top (); st.pop ();
    int a = st.top (); st.pop ();

    nfa.states [beg].e.push_back (a);
    nfa.states [beg].e.push_back (end);

    nfa.states [b].e.push_back (a);
    nfa.states [b].e.push_back (end);

    st.push (beg);
    st.push (end);
}

static void
nfa_consume_alternation (nfa_state_t& state) {
    auto& nfa = state.nfa;

    nfa.states.push_back (default_nfa_state);
    nfa.states.push_back (default_nfa_state);

    const auto n = state.nfa.states.size ();
    const auto beg = n - 2, end = n - 1;

    auto& st = state.st;
    assert (3 < st.size ());

    int d = st.top (); st.pop ();
    int c = st.top (); st.pop ();
    int b = st.top (); st.pop ();
    int a = st.top (); st.pop ();

    nfa.states [beg].e.push_back (a);
    nfa.states [beg].e.push_back (c);

    nfa.states [b].e.push_back (end);
    nfa.states [d].e.push_back (end);

    st.push (beg);
    st.push (end);
}

static nfa_t
make_nfa (const string& s) {
    nfa_state_t state;

    for (const auto c : s) {
        switch (c) {
        case 'a': case 'b':
            nfa_consume_literal (c, state);
            break;

        case '*':
            nfa_consume_kleene_closure (state);
            break;

        case '.':
            nfa_consume_concatenation (state);
            break;

        case '|':
            nfa_consume_alternation (state);
            break;

        default:
            assert (0);
            break;
        }
    }

    const auto last = state.st.top (); state.st.pop ();
    state.nfa.states [last].accept = true;

    state.nfa.start = state.st.top ();

    return state.nfa;
}

////////////////////////////////////////////////////////////////////////

static constexpr auto npos = size_t (-1);

struct dfa_t {
    struct state_t {
        size_t a [2] = { npos, npos };
        bool accept = false;
    };

    vector< state_t > states;
};

struct dfa_state_t {
    map< set< size_t >, size_t > closures;
    map< size_t, dfa_t::state_t > states;
};

static void
do_epsilon_closure (const nfa_t& nfa, size_t state, set< size_t >& closure) {
    for (const auto i : nfa.states [state].e) {
        const auto iter = closure.find (i);

        if (iter == closure.end ()) {
            closure.insert (i);
            do_epsilon_closure (nfa, i, closure);
        }
    }
}

static set< size_t >
epsilon_closure (const nfa_t& nfa, size_t n) {
    set< size_t > closure { n };
    do_epsilon_closure (nfa, n, closure);
    return closure;
}

namespace std {

static ostream&
operator<< (ostream& s, const set< size_t >& arg) {
    s << "{ ";
    copy (arg.begin (), arg.end (), ostream_iterator< size_t > (s, " "));
    return s << "}";
}

static ostream&
operator<< (ostream& s, const set< set< size_t > >& arg) {
    s << "{ ";
    copy (arg.begin (), arg.end (), ostream_iterator< set< size_t > > (s, " "));
    return s << "}";
}

}

static dfa_t
make_dfa (const nfa_t& nfa) {
    size_t state_counter = 0;

    dfa_state_t dfa_state { };

    //
    // Start from the epsilon closure of the start state:
    //
    const auto initial_closure = epsilon_closure (nfa, nfa.start);
    set< set< size_t > > closures { initial_closure };

    //
    // Update the closure-to-state-number map:
    //
    dfa_state.closures.emplace (initial_closure, state_counter++);

    while (!closures.empty ()) {
        cout << " --> " << closures << "\n";

        //
        // Accumulate all new closures as they are computed:
        //
        set< set< size_t > > accum;

        for (const auto& closure : closures) {
            const auto from = dfa_state.closures [closure];

            for (auto i : { 0, 1 }) {
                set< size_t > s;

                for (const auto state : closure) {
                    //
                    // Start from the sorted set of states that are direct
                    // transitions under `i':
                    //
                    set< size_t > t {
                        nfa.states [state].a [i].begin (),
                        nfa.states [state].a [i].end ()
                    };

                    //
                    // Merge all epsilon closures of all states that are
                    // reachable under input `i':
                    //
                    for (const auto state : t) {
                        const auto tmp = epsilon_closure (nfa, state);
                        s.insert (tmp.begin (), tmp.end ());
                    }

                    //
                    // Finally, merge the states reachable under `i':
                    //
                    s.insert (t.begin (), t.end ());
                }

                if (s.empty ())
                    continue;

                size_t to = 0;

                //
                // Store the newly computed state in the state object of
                // the DFA construction algorithm:
                //
                const auto
                    iter = dfa_state.closures.find (s),
                    last = dfa_state.closures.end ();

                if (iter == last) {
                    to = state_counter++;
                    dfa_state.closures.emplace (s, to);

                    //
                    // Compute the acceptance state:
                    //
                    dfa_state.states [to].accept = any_of (
                        s.begin (), s.end (),
                        [&](const auto i) {
                            return nfa.states [i].accept;
                        });

                    //
                    // Collect this new state for the next proceessing
                    // iteration:
                    //
                    accum.insert (s);
                }
                else
                    to = iter->second;

                //
                // Store the transition:
                //
                dfa_state.states [from].a [i] = to;
            }
        }

        closures = accum;
    }

    dfa_t dfa { };

    transform (
        dfa_state.states.begin (),
        dfa_state.states.end (),
        back_inserter (dfa.states),
        [](const auto& arg) { return arg.second; });

    return dfa;
}

////////////////////////////////////////////////////////////////////////

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
    static string
    make_dot (const nfa_t& nfa) {
        stringstream ss;

        ss << "#+BEGIN_SRC dot :file nfa.png :cmdline -Kdot -Tpng\n";
        ss << "digraph nfa {\n";

        ss << "    start [shape=none;rank=0;];\n";
        ss << "    start -> q" << nfa.start << ";\n";

        for (size_t i = 0; i < nfa.states.size (); ++i) {
            for (size_t j = 0; j < nfa.states [i].a [0].size (); ++j) {
                ss << "    q" << i << " -> q" << nfa.states [i].a [0][j]
                   << "[label=\"a\"];\n";
            }

            for (size_t j = 0; j < nfa.states [i].a [1].size (); ++j) {
                ss << "    q" << i << " -> q" << nfa.states [i].a [1][j]
                   << "[label=\"b\"];\n";
            }

            for (size_t j = 0; j < nfa.states [i].e.size (); j++) {
                ss << "    q" << i << " -> q" << nfa.states [i].e [j]
                   << "[label=\"ϵ\";];\n";
            }

            if (nfa.states [i].accept) {
                ss << "    q" << i << "[shape=doublecircle;rank="
                   << nfa.states.size () << ";];\n";
            }
        }

        ss << "}\n";
        ss << "#+END_SRC\n\n";

        return ss.str ();
    }

    static string
    make_dot (const dfa_t& dfa) {
        stringstream ss;

        ss << "#+BEGIN_SRC dot :file dfa.png :cmdline -Kdot -Tpng\n";
        ss << "digraph dfa {\n";

        ss << "    start [shape=none;rank=0;];\n";
        ss << "    start -> q0;\n";

        for (size_t i = 0; i < dfa.states.size (); ++i) {
            const auto& state = dfa.states [i];

            for (size_t j = 0; j < 2; ++j) {
                if (npos != state.a [j]) {
                    ss << "    q" << i << " -> q" << state.a [j]
                       << "[label=\"" << char ('a' + j) << "\"];\n";
                }
            }

            if (state.accept) {
                ss << "    q" << i << "[shape=doublecircle;rank="
                   << dfa.states.size () << ";];\n";
            }
        }

        ss << "}\n";
        ss << "#+END_SRC\n\n";

        return ss.str ();
    }

private:
    string value_;
};

////////////////////////////////////////////////////////////////////////

int main (int, char** argv) {
    const string r (argv [1]);

    const string s = postfix (r);
    cout << "#\n# postfixed expression : " << r << "  -->  "
         << s << "\n#\n\n";

    const auto nfa = make_nfa (s);
    cout << dot_graph_t (nfa).value () << "\n\n";

    const auto dfa = make_dfa (nfa);
    cout << dot_graph_t (dfa).value () << "\n\n";

    return 0;
}
