// -*- mode: c++; -*-

#include <cassert>

#include <functional>
#include <string>
#include <vector>

using namespace std;

string
postfix (const string& r) {
    size_t a = 0, x = 0;

    vector< pair< size_t, size_t > > st;
    st.reserve (16);

    string s;

    for (const auto c : r) {
        switch (c) {
        case '(':
            if (x > 1) {
                --x;
                s += '.';
            }

            st.emplace_back (a, x);
            a = x = 0;

            break;

        case '|':
            assert (x);

            while (--x > 0)
                s += '.';

            ++a;
            break;

        case ')':
            while (--x)
                s += '.';

            while (a--)
                s += '|';

            tie (a, x) = st.back ();
            st.pop_back ();

            ++x;
            break;

        case '*':
            assert (x);
            s += c;
            break;

        default:
            if (x > 1) {
                --x;
                s += '.';
            }

            s += c;
            x++;
            break;
        }
    }

    while (--x > 0)
        s += '.';

    for (; a > 0; --a)
        s += '|';

    return s;
}
