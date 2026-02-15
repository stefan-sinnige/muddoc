#
# ++ start-license-description ++
#
# Copyright (c) 2026 Stefan Sinnige.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# ++ end-license-description ++
#

AC_PREREQ([2.69])
AC_INIT([muddoc],[0.1.0])
AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT

AC_PROG_CXX
AC_PROG_INSTALL
PKG_PROG_PKG_CONFIG

# Add flag to indicate we are building the mudlib library
AC_DEFINE([MUDLIB_LIBRARY])

# We only use C++17 (or above) features
AX_CXX_COMPILE_STDCXX(17, , mandatory)

# Check for clang/llvm development library
AC_CHECK_PROGS([LLVM_CONFIG],[llvm-config-devel llvm-config-mp-devel])
llvm_devel_libdir=$("$LLVM_CONFIG" --libdir)
LDFLAGS="$LDFLAGS -L$llvm_devel_libdir"
AC_SEARCH_LIBS([clang_Comment_getKind],[clang],[],[
  AC_MSG_ERROR([clang development library not found])
])
AC_SEARCH_LIBS([__clang_Interpreter_SetValueNoAlloc],[clang-cpp],[],[
  AC_MSG_ERROR([clang-cpp development library not found])
])

# Create the configuration files.
AC_CONFIG_FILES([
    Makefile
    src/Makefile
])
AC_OUTPUT
