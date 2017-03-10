// -*- mode: c++; -*-

#include <cassert>
#include <cmath>

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

template< typename T >
inline size_t size_cast (T c) {
    return size_t (typename make_unsigned< T >::type (c));
}

/* static */ const auto first_less = [](const auto& lhs, const auto& rhs) {
    return lhs.first < rhs.first;
};

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

////////////////////////////////////////////////////////////////////////

struct nfa_t {
    using  int_type = int;
    using size_type = size_t;

    vector< vector< pair< int_type, size_type > > > states;
    vector< size_type > accept;
    size_type start;

    static constexpr int_type epsilon = -1;
};

/* static */ constexpr nfa_t::int_type nfa_t::epsilon /* = -1 */;

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

static nfa_t
make_nfa (const string& s) {
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

////////////////////////////////////////////////////////////////////////

struct dfa_t {
    using  int_type = int;
    using size_type = size_t;

    vector< vector< pair< int_type, size_type > > > states;
    vector< size_type > accept;
};

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

static dfa_t
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
        sort (t.begin (), t.end (), first_less);

    dfa.accept = dfa_state.accept;

    auto& a = dfa.accept;
    sort (a.begin (), a.end ());

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
            const auto& transitions = nfa.states [i];

            for (const auto& t : transitions) {
                ss << "    q" << i << " -> q" << t.second << "[label=\"";

                if (0 > t.first)
                    ss << "ϵ";
                else
                    ss << char (t.first);

                ss << "\"];\n";
            }
        }

        ss << "    q" << nfa.accept.front () << "[shape=doublecircle;rank="
           << nfa.states.size () << ";];\n";

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
            const auto& transitions = dfa.states [i];

            for (const auto& t : transitions)
                ss << "    q" << i << " -> q" << t.second << "[label=\""
                   << char (t.first) << "\"];\n";
        }

        for (const auto state : dfa.accept)
            ss << "    q" << state << "[shape=doublecircle;rank="
               << dfa.states.size () << ";];\n";

        ss << "}\n";
        ss << "#+END_SRC\n\n";

        return ss.str ();
    }

private:
    string value_;
};

////////////////////////////////////////////////////////////////////////

#if 0

#include <benchmark/benchmark.h>

static const vector< string > test_data {
    "a",
    "a*",
    "a*b",
    "ab*",
    "a|b",
    "(a*|b)",
    "(a|b*)",
    "(a|b)*"
    "(((a|b)*)a)",
    "((((a|b)*)a)(a|b))",
    "(((((a|b)*)a)(a|b))(a|b))",
    "((((((a|b)*)a)(a|b))(a|b))(a|b))",
    "(((((((a|b)*)a)(a|b))(a|b))(a|b))(a|b))",
    "((((((((a|b)*)a)(a|b))(a|b))(a|b))(a|b))(a|b))",
    "(((((((((a|b)*)a)(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))",
    "((((((((((a|b)*)a)(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))",
    "(((((((((((a|b)*)a)(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))",
    "((((((((((((a|b)*)a)(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))",
    "(((((((((((((a|b)*)a)(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))",
    "((((((((((((((a|b)*)a)(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))(a|b))"
};

static void
BM_nfa (benchmark::State& state) {
    const auto s = postfix (test_data [state.range (0)]);

    while (state.KeepRunning ())
        benchmark::DoNotOptimize (make_nfa (s));
}

BENCHMARK (BM_nfa)->DenseRange (0, test_data.size () - 1);

static void
BM_dfa (benchmark::State& state) {
    const auto s = postfix (test_data [state.range (0)]);
    const auto nfa = make_nfa (s);

    while (state.KeepRunning ())
        benchmark::DoNotOptimize (make_dfa (nfa));
}

BENCHMARK (BM_dfa)->DenseRange (0, test_data.size () - 1);

BENCHMARK_MAIN();

#elif 0

int main (int, char** argv) {
    size_t ignore, i;
    cin >> ignore;

    for (string s; cin >> s >> i;) {
        cout << "# -->     regex : " << s << endl;

        s = postfix (s);
        cout << "# --> postfixed : " << s << endl;

        const auto nfa = make_nfa (s);
        cout << dot_graph_t (nfa).value () << "\n\n";

        const auto dfa = make_dfa (nfa);
        cout << dot_graph_t (dfa).value () << "\n\n";
    }

    return 0;
}

#elif 1

int main (int, char** argv) {
    string s (argv [1]);
    cout << "# -->     regex : " << s << endl;

    s = postfix (s);
    cout << "# --> postfixed : " << s << endl;

    const auto nfa = make_nfa (s);
    cout << dot_graph_t (nfa).value () << "\n\n";

    const auto dfa = make_dfa (nfa);
    cout << dot_graph_t (dfa).value () << "\n\n";

    return 0;
}

#endif
