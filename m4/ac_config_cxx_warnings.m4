dnl -*- Autoconf -*-

AC_DEFUN([AC_CONFIG_CXX_WARNINGS],[

WARNFLAGS=" -W -Wall "

CLANG_WARNFLAGS="\
-Wno-deprecated-register \
-Wno-logical-op-parentheses \
-Wno-parentheses "

GCC_WARNFLAGS="\
-Wno-parentheses \
-Wno-strict-aliasing \
-Wno-unused-function \
-Wno-unused-local-typedefs \
-Wno-unused-variable "

test -z "$OSNAME" && OSNAME=$( uname )

case $CXX in
    *clang++)
        WARNFLAGS+=$GCC_WARNFLAGS
        ;;
    *g++)
        if test "$OSNAME" = "Darwin"; then
            WARNFLAGS+=$CLANG_WARNFLAGS
        else
            WARNFLAGS+=$GCC_WARNFLAGS
        fi
        ;;
    *)
        ;;
esac

CXXFLAGS+=$WARNFLAGS
])
