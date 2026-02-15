# MUD Documentation Generator

## Preface

<img src="./doc/mud.png" width="200px"/>

> Omne tulit punctum qui miscuit utile dulci,
> lectorem delectando pariterque monendo.
> He who blends usefulness and pleasantness wins every
> vote, at once delighting and instructing the reader.
> Horace - Ars Poetica (343-344)

When Horace postulates in his "Ars Poetica" (19 B.C.) that one of the
qualifications of a poet is to mix the usefulness with sweetness in order to
both delight and instruct the reader, little would he have known that this
would not only apply to the written art of poetry, but to the art of
programming as well. Taken from verse 343 of this poem is the Latin phrase
_miscere utile dulci_ that captures this principle. Abbreviated to MUD,
is the project name that is being uses as the basis for naming various
projects by the same Author.

## Overview

The MUD Documentation generator is an application that scans through a project
and extracts Doxygen comments. It uses these comments together with the
programming language signature of the interface to create comprehensive and
useful documentation, primarily for developers and users of said project.

## Installation

The MUD Documentation library is utilising clang for the parsing and analysing
of C/C++ code, and to extract context-specific comments from the source code.
The list of build dependencies are

* autoconf
* autoconf-archive
* automake
* libtool
* clang/llvm development package

## License

Unless explictely stated otherwise, this project and all its source code is
provided under the MIT license. See the LICENSE file for more information.

