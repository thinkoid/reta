// -*- mode: c++; -*-

#include <iostream>
#include <string>

using namespace std;

#include <reta/nfa.hpp>
#include <reta/dfa.hpp>
#include <reta/dot-graph.hpp>

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

static void
BM_min_dfa (benchmark::State& state) {
    const auto s = postfix (test_data [state.range (0)]);

    const auto nfa = make_nfa (s);
    const auto dfa = make_dfa (nfa);

    while (state.KeepRunning ())
        benchmark::DoNotOptimize (minimize_dfa_table (dfa));
}

BENCHMARK (BM_min_dfa)->DenseRange (0, test_data.size () - 1);

BENCHMARK_MAIN();
