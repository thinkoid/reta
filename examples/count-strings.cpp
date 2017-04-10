// -*- mode: c++; -*-

#include <cassert>
#include <cmath>

#include <iostream>

using namespace std;

#include <reta/nfa.hpp>
#include <reta/dfa.hpp>

const size_t mod = 1000000007;

template< typename T >
struct matrix_t {
    using value_type = T;

    explicit matrix_t (size_t n)
        : value (n * n, value_type { }), size (n)
        { }

    value_type& at (size_t i, size_t j) {
        return value [i * size + j];
    }

    const value_type& at (size_t i, size_t j) const {
        return value [i * size + j];
    }

    vector< value_type > value;
    size_t size;
};

template< typename T >
ostream&
operator<< (ostream& s, const matrix_t< T >& m) {
    const auto n = m.size;

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j)
            cout << m.at (i, j) << " ";

        cout << "\n";
    }

    cout << "\n";

    return s;
}

template< typename T >
matrix_t< T >
operator* (const matrix_t< T >& lhs, const matrix_t< T >& rhs) {
    const auto n = lhs.size;

    matrix_t< T > m (n);

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            T accum { };

            for (size_t k = 0; k < n; ++k) {
                accum += (lhs.at (i, k) * rhs.at (k, j)) % mod;
                accum %= mod;
            }

            m.at (i, j) = accum;
        }
    }

    return m;
}

template< typename T >
matrix_t< T >
pow (matrix_t< T > m, size_t n) {
    assert (n);
    
    if (1 == n)
        return m;
    else if (n % 2)
        return m * pow (m, n - 1);
    else {
        m = pow (m, n/2);
        return m * m;
    }
}

static size_t
howmanys (const dfa_t& dfa, size_t k) {
    const auto n = dfa.states.size ();

    matrix_t< size_t > m (n);

    for (size_t i = 0; i < n; ++i) {
        const auto& state = dfa.states [i];

        for (size_t j = 0; j < n; ++j) {
            size_t n = 0;

            for (const auto& p : state)
                if (j == p.second)
                    ++n;

            assert (0 <= n && n <= 2);
            m.at (i, j) = n;
        }
    }

    m = pow (m, k);

    size_t result = 0;

    for (const auto i : dfa.accept) {
        result += m.at (0, i) % mod;
        result %= mod;
    }

    return result;
}

static size_t
test (const string& r, size_t k) {
    const string s = postfix (r);

    const auto nfa = make_nfa (s);

    const auto dfa = make_dfa (nfa);
    const auto dfa2 = minimize_dfa_table (dfa);

    return size_t (howmanys (dfa, k));
}

int main () {
    int ignore;
    cin >> ignore;

    string s;
    int k;

    for (; cin >> s >> k;)
        cout << test (s, k) << endl;

    return 0;
}
