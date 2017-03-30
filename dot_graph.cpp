// -*- mode: c++; -*-

#include <sstream>
#include <string>
#include <vector>

using namespace std;

#include "dot_graph.hpp"

/* static */ string
dot_graph_t::make_dot (const nfa_t& nfa) {
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

/* static */  string
dot_graph_t::make_dot (const dfa_t& dfa) {
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
