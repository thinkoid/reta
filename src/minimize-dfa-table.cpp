// -*- mode: c++; -*-

#include <cassert>

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

#include <reta/dfa.hpp>

static inline bool
distinct (vector< vector< bool > >& t, size_t i, size_t j) {
    return i == j ? false : i > j ? t [j][i - j - 1] : t [i][j - i - 1];
}

static bool
distinct (const dfa_t& dfa, size_t i, size_t j, int c,
          vector< vector< bool > >& t) {
    const auto& states = dfa.states;

    const auto cmp = [](int c) {
        return [c](const auto& arg) { return c == arg.first; };
    };

    const auto p1 = find_if (states [i].begin (), states [i].end (), cmp (c));
    const auto p2 = find_if (states [j].begin (), states [j].end (), cmp (c));

    const auto b1 = p1 == states [i].end ();
    const auto b2 = p2 == states [j].end ();

    return (b1 ^ b2) || (!b1 && distinct (t, p1->second, p2->second));
}

static inline vector< bool >
final_states_of (const dfa_t& dfa) {
    vector< bool > v (dfa.states.size ());

    for (const auto s : dfa.accept)
        v [s] = true;

    return v;
}

static vector< vector< bool > >
make_minimization_table (const dfa_t& dfa) {
    const auto n = dfa.states.size ();
    assert (n > 1);

    vector< vector< bool > > t;
    t.reserve (n - 1);

    for (size_t i = 0; i < n - 1; ++i)
        t.emplace_back (vector< bool > (n - i - 1));

    assert (t.front ().size () == n - 1);
    assert (t.back  ().size () == 1);

    const auto f = final_states_of (dfa);

    for (size_t i = 0; i < n - 1; ++i)
        for (size_t j = i + 1; j < n; ++j)
            t [i][j - i - 1] = f [i] ^ f [j];

    for (bool changed = true; changed; ) {
        changed = false;

        for (size_t i = 0; i < n - 1; ++i) {
            for (size_t j = i + 1; j < n; ++j) {

                if (t [i][j - i - 1])
                    continue;

                for (int c = 'a'; c <= 'z'; ++c)
                    if (distinct (dfa, i, j, c, t))
                        t [i][j - i - 1] = changed = true;
            }
        }
    }

    return t;
}

static inline vector< size_t >
flatten (const vector< vector< size_t > >& v) {
    vector< size_t > u;

    for (const auto& w : v)
        u.insert (u.end (), w.begin (), w.end ());

    return u;
}

template< typename T >
inline T unique_sorted (T arg) {
    sort (arg.begin (), arg.end ());

    auto last = unique (arg.begin (), arg.end ());
    arg.resize (distance (arg.begin (), last));

    return arg;
}

static vector< size_t >
distinguishable_states (
    const dfa_t& dfa, const vector< vector< size_t > >& indistinct) {
    const auto n = dfa.states.size ();

    auto lhs = vector< size_t > ();
    lhs.reserve (n);

    size_t g = 0;
    generate_n (back_inserter (lhs), n, [g]() mutable { return g++; });

    auto rhs = unique_sorted (flatten (indistinct));

    vector< size_t > diff;

    set_difference (
        lhs.begin (), lhs.end (),
        rhs.begin (), rhs.end (),
        back_inserter (diff));

    return diff;
}

static vector< tuple< size_t, size_t > >
indistinguishable_states (const vector< vector< bool > >& t) {
    const auto n = t.size () + 1;

    vector< tuple< size_t, size_t > > v;
    v.reserve (n);

    for (size_t i = 0; i < n - 1; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (!t [i][j - i - 1])
                v.emplace_back (i, j);
        }
    }

    static const auto pair_less = [](const auto& lhs, const auto& rhs) {
        return
            (get< 0 > (lhs)  < get< 0 > (rhs)) ||
            (get< 0 > (lhs) == get< 0 > (rhs)  && get< 1 > (lhs) < get< 1 > (rhs));
    };

    sort (v.begin (), v.end (), pair_less);

    return v;
}

////////////////////////////////////////////////////////////////////////

static vector< int >
make_color_table (const vector< tuple< size_t, size_t > >& v) {
    vector< int > u;

    for (const auto& t : v) {
        size_t a, b;
        tie (a, b) = t;

        const auto c = (max) (a, b);

        if (size_t (c) >= u.size ())
            u.resize (c + 1, -1);

        u [a] = u [b] = 0;
    }

    return u;
}

static inline void
recolor (vector< int >& v, int from, int to) {
    if (from == to)
        return;

    for (auto& c : v)
        if (c == from) c = to;
}

static vector< vector< size_t > >
cluster (const vector< tuple< size_t, size_t > >& v) {
    size_t counter = 0;

    auto u = make_color_table (v);

    for (auto& p : v) {
        size_t a, b;
        tie (a, b) = p;

        const unsigned mask = (u [a] ? 2: 0) | (u [b] ? 1 : 0);

        switch (mask) {
        case 0:
            u [a] = u [b] = ++counter;
            break;

        case 1:
            u [a] = u [b];
            break;

        case 2:
            u [b] = u [a];
            break;

        case 3: {
            auto lhs = u [a], rhs = u [b];

            if (lhs != rhs) {
                if (lhs > rhs)
                    swap (lhs, rhs);

                recolor (u, rhs, lhs);
            }
        }
            break;

        default:
            assert (0);
            break;
        }
    }

    for (auto& c : u)
        if (0 > c) c = ++counter;

    map< size_t, vector< size_t > > m;

    for (size_t i = 0; i < u.size (); ++i)
        m [u [i]].emplace_back (i);

    vector< vector< size_t > > w;

    for (auto& p : m)
        w.emplace_back (move (p.second));

    return w;
}

static inline vector< int >
collect_input (const dfa_t& dfa, const vector< size_t >& states) {
    vector< int > v;

    for (const auto s : states)
        for (const auto& p : dfa.states [s])
            v.emplace_back (p.first);

    return unique_sorted (v);
}

static inline vector< int >
collect_input (const dfa_t& dfa) {
    vector< int > v;

    for (const auto& t : dfa.states)
        transform (
            t.begin (), t.end (), back_inserter (v),
            [](const auto& arg) { return arg.first; });

    return unique_sorted (v);
}

static map< size_t, size_t >
make_state_map (
    const vector< vector< size_t > >& a,
    const vector< size_t >& b) {

    map< size_t, size_t > m;

    size_t i = 0;

    for (const auto& v : a) {
        for (const auto u : v)
            m.emplace (u, i);

        ++i;
    }

    for (const auto q : b)
        m.emplace (q, i++);

    return m;
}

static inline vector< size_t >
collect_output_states (
    const dfa_t& src, const vector< size_t >& states, int i) {

    vector< size_t > v;

    for (const auto s : states)
        for (const auto& p : src.states [s])
            if (i == p.first)
                v.push_back (p.second);

    return unique_sorted (v);
}

static size_t
map_source_state (
    const vector< size_t >& states, const map< size_t, size_t >& m) {

    vector< size_t > v;
    v.reserve (states.size ());

    for (const auto s : states) {
        const auto iter = m.find (s);
        assert (iter != m.end ());

        v.push_back (iter->second);
    }

    v = unique_sorted (v);
    assert (1 == v.size ()); // TODO

    return v [0];
}

static size_t
map_destination_state (
    const vector< size_t >& states, const map< size_t, size_t >& m) {

    vector< size_t > v;

    for (const auto s : states) {
        const auto iter = m.find (s);
        assert (iter != m.end ());

        v.push_back (iter->second);
    }

    v = unique_sorted (v);
    assert (1 == v.size ());

    return v [0];
}

static inline dfa_t
make_minimal_dfa (
    const dfa_t& src,
    const vector< vector< size_t > >& indistinct,
    const vector< size_t >& distinct) {

    map< size_t, size_t > m = make_state_map (indistinct, distinct);

    dfa_t dst { };
    dst.states.resize (indistinct.size () + distinct.size ());

    for (const auto& states : indistinct) {
        const auto from = map_source_state (states, m);

        for (const auto i : collect_input (src, states)) {
            const auto to = map_destination_state (
                collect_output_states (src, states, i), m);
            dst.states [from].emplace_back (i, to);
        }
    }

    for (const auto s : distinct) {
        const auto from = m [s];

        for (const auto& p : src.states [s]) {
            const auto to = m [p.second];
            dst.states [from].emplace_back (p.first, to);
        }
    }

    for (const auto s : src.accept)
        dst.accept.emplace_back (m [s]);

    dst.accept = unique_sorted (dst.accept);

    dst.start = m [src.start];

    return dst;
}

////////////////////////////////////////////////////////////////////////

dfa_t
minimize_dfa_table (const dfa_t& src) {
    if (src.states.size () < 2)
        return src;

    const auto table = make_minimization_table (src);

    const auto indistinct = cluster (indistinguishable_states (table));
    const auto distinct = distinguishable_states (src, indistinct);

    return make_minimal_dfa (src, indistinct, distinct);
}
