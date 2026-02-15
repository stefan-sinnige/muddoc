/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

#include <clang-c/Documentation.h>
#include <clang-c/Index.h>
#include <clang/AST/Comment.h>
#include <clang/AST/CommentCommandTraits.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "visitor.h"
#include "warn_error.h"

#include "descriptor.h"

using muddoc::warn;

// The stream to write the XML documentation to.
std::ostream* xml;

// The stream to write any other output to (like warnings, errors).
std::ostream* out;

void
help(const char* msg = nullptr)
{
    if (msg != nullptr) {
        std::cerr << msg << std::endl;
    }
    std::cout <<
R"EOF(OVERVIEW: MUD documentation extractor

Extract comments from FILE to an XML representation. This can then be used for
further analysis or formatting.

USAGE:: muddoc [options] FILE

OPTIONS:
    --help, -h          Show this help.
    --base, -b DIR      The include folder containing FILE. The DIR/FILE is the
                        source file being processed and all references in the
                        documentation will be refered to as FILE. Defaults to
                        the current directory.
    --output, -o FILE   Write the XML representation to FILE.
    --diagnostics, -d   Show clang diagnostic output.

Recognised clang OPTIONS:
    -DMACRO=VALUE       Add an implicit #define macro definition.
    -UMACRO             Add an implicit #undef macro.
    -IDIR               Add DIR to the include path. Note that muddoc does not
                        add the default inlcude paths from your compiler, so
                        it is necessary to supply them explicitly.
    -std=LANG           Add the C++ language standard (defaults to c++17).
)EOF";
    ::exit(msg == nullptr ? 0 : 1);
}

int
main(int argc, char** argv)
{
    std::vector<const char*> clang_args = {
        "-x", "c++",
        "-fsyntax-only",
        "-std=c++17"
    };
    char *infile = nullptr;
    char *outfile = nullptr;
    std::filesystem::path base = std::filesystem::current_path();
    bool diagnostics = false;
    while (--argc && (*++argv)[0] == '-') {
        if (::strcmp(*argv, "--help") == 0 || ::strcmp(*argv, "-h") == 0) {
            help();
            return 0;
        }
        else
        if (::strcmp(*argv, "--base") == 0 || ::strcmp(*argv, "-b") == 0) {
            if (argc <= 2) {
                help("Option --base,-b requires an argument."); 
            }
            --argc, ++argv;
            base = std::filesystem::path(*argv);
        }
        else
        if (::strcmp(*argv, "--output") == 0 || ::strcmp(*argv, "-o") == 0) {
            if (argc <= 2) {
                help("Option --output,-o requires an argument."); 
            }
            --argc, ++argv;
            outfile = *argv;
        }
        else
        if (::strcmp(*argv, "--diagnostics") == 0 || ::strcmp(*argv, "-d") == 0) {
            diagnostics = true;
        }
        else
        if (::strcmp(*argv, "-I") > 0) {
            clang_args.push_back(*argv);
        }
        else
        if (::strcmp(*argv, "-D") > 0) {
            clang_args.push_back(*argv);
        }
        else
        if (::strcmp(*argv, "-U") > 0) {
            clang_args.push_back(*argv);
        }
        else
        if (::strcmp(*argv, "-std=") > 0) {
            clang_args.push_back(*argv);
        }
        else {
            std::stringstream sstr;
            sstr << "Unknown option '" << *argv << "'";
            help(sstr.str().c_str());
        }
    }

    // There should be one argument left.
    if (argc != 1) {
        help("Missing input file");
    }
    infile = argv[0];

    // Define the input path and check if it exists
    std::filesystem::path input = base / infile;
    if (!std::filesystem::exists(input)) {
        std::cerr << "Error opening input file " << input
                  << std::endl;
        return 1;
    }

    // If there is no output file defined, use stdout/stderr. Otherwise use the
    // file/stdout.
    if (outfile == nullptr) {
        xml = &std::cout;
        out = &std::cerr;
    }
    else {
        xml = new std::ofstream(outfile);
        if (!*xml) {
            std::cerr << "Error opening output file " << outfile << std::endl;
            return 1;
        }
        out = &std::cout;
    }

    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit unit = clang_parseTranslationUnit(
        index, input.c_str(),
        &(clang_args[0]), clang_args.size(),
        nullptr, 0,
        CXTranslationUnit_DetailedPreprocessingRecord);
    if (unit == nullptr) {
        std::cerr << "Unable to parse translation unit. Quitting." << std::endl;
        ::exit(-1);
    }

    // show any warnings that the compiler produced
    if (diagnostics) {
        size_t n = clang_getNumDiagnostics(unit);
        for (int i = 0; i != n; ++i) {
            CXDiagnostic diag = clang_getDiagnostic(
                unit, static_cast<unsigned>(i));
            CXString string = clang_formatDiagnostic(
                diag, clang_defaultDiagnosticDisplayOptions());
            std::cerr << "[clang]: " << clang_getCString(string) << std::endl;
            clang_disposeString(string);
            clang_disposeDiagnostic(diag);
        }
    }

    // Visit all nodes in the parsing tree
    muddoc::Visitor visitor(unit);
    muddoc::FileFilter filter(input);
    visitor.generate(*xml, filter);

    // Clean-up. If there is an output file, close it by destructing it.
    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
    if (outfile != nullptr) {
        delete xml;
    }

    return 0;
}
