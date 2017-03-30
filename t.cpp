// -*- mode: c++; -*-

#include <iostream>
#include <string>

using namespace std;

#include "nfa.hpp"
#include "dfa.hpp"
#include "dot_graph.hpp"

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
