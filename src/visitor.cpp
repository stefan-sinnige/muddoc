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

#include <functional>
#include <sstream>
#include <clang-c/Documentation.h>
#include <clang-c/Index.h>
#include <clang/AST/Comment.h>
#include <clang/AST/CommentCommandTraits.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>
#include "descriptor.h"
#include "visitor.h"
#include "utility.h"
#include "warn_error.h"

namespace muddoc {

FileFilter::FileFilter(const std::filesystem::path& path)
    : _path(path)
{
}

bool
FileFilter::match(const CXCursor& cursor) const
{
    // Only consider the elements of the specified path , not the elements
    // of any other file, like included files from '#include' statements.
    CXSourceLocation location = clang_getCursorLocation(cursor);
    CXString filename;
    unsigned int line, column;
    clang_getPresumedLocation(location, &filename, &line, &column);
    return strcmp(clang_getCString(filename), _path.c_str()) == 0;
}

struct Visitor::ClientData
{
    const Visitor& visitor;
    std::ostream& ostr;
};

static CXChildVisitResult
__visit(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    auto data = (struct Visitor::ClientData*)client_data;
    return data->visitor.visit(cursor, parent, data);
}

Visitor::Visitor(CXTranslationUnit unit)
    : _unit(unit)
{
    _filter.reset(new AnyFilter());
}

void
Visitor::generate(std::ostream& ostr, const Filter& filter)
{
    _filter.reset(filter.clone());
    ClientData data { *this, ostr };
    ostr << "<doc>";
    CXCursor cursor = clang_getTranslationUnitCursor(_unit);
    clang_visitChildren(cursor, __visit, (CXClientData)&data);
    ostr << "</doc>";
}

std::string
Visitor::generate(CXCursor cursor) const
{
    std::stringstream ostr;
    ClientData data { *this, ostr};
    clang_visitChildren(cursor, __visit, (CXClientData)&data);
    return ostr.str();
}

CXChildVisitResult
Visitor::visit(CXCursor cursor, CXCursor parent, struct ClientData* data) const
{
    // Only process if it passes the filter
    if (!_filter->match(cursor)) {
        return CXChildVisit_Continue;
    }

    // Consider declarations we care to document about.
    CXCursorKind kind = clang_getCursorKind(cursor);
    switch (kind) {
        case CXCursor_PreprocessingDirective:
        case CXCursor_MacroDefinition:
        case CXCursor_MacroExpansion:
        case CXCursor_InclusionDirective:
            // Ignore preprocessing directives
            break;
        case CXCursor_Namespace:
            generate(cursor,
                  static_cast<const clang::NamespaceDecl *>(cursor.data[0]),
                  data);
            break;
            // Recurse into a namespace
            return CXChildVisit_Recurse;
        case CXCursor_StructDecl:
        case CXCursor_UnionDecl:
        case CXCursor_ClassDecl:
            generate(cursor,
                  static_cast<const clang::CXXRecordDecl *>(cursor.data[0]),
                  data);
            break;
        case CXCursor_ClassTemplate:
            generate(cursor,
                  static_cast<const clang::ClassTemplateDecl *>(cursor.data[0]),
                  data);
            break;
        case CXCursor_ClassTemplatePartialSpecialization:
            generate(cursor,
                  static_cast<const clang::ClassTemplatePartialSpecializationDecl *>(cursor.data[0]),
                  data);
            break;
        case CXCursor_EnumDecl:
            generate(cursor,
                  static_cast<const clang::EnumDecl *>(cursor.data[0]),
                  data);
            break;
        case CXCursor_EnumConstantDecl:
            generate(cursor,
                  static_cast<const clang::EnumConstantDecl *>(cursor.data[0]),
                  data);
            break;
        case CXCursor_TypedefDecl:
            generate(cursor,
                  static_cast<const clang::TypedefDecl *>(cursor.data[0]),
                  data);
            break;
        case CXCursor_Constructor:
            generate(cursor,
                  static_cast<const clang::CXXConstructorDecl *>(cursor.data[0]),
                  data);
            break;
        case CXCursor_Destructor:
            generate(cursor,
                  static_cast<const clang::CXXDestructorDecl *>(cursor.data[0]),
                  data);
            break;
        case CXCursor_CXXMethod:
            generate(cursor,
                  static_cast<const clang::CXXMethodDecl *>(cursor.data[0]),
                  data);
            break;
        case CXCursor_FunctionTemplate:
            generate(cursor,
                  static_cast<const clang::FunctionTemplateDecl *>(cursor.data[0]),
                  data);
            break;
        case CXCursor_FieldDecl:
            generate(cursor,
                  static_cast<const clang::FieldDecl *>(cursor.data[0]),
                  data);
            break;
        case CXCursor_CXXAccessSpecifier:
        case CXCursor_FriendDecl:
            // Ignore
            break;
        default: {
           std::stringstream sstr;
           sstr << "Unsupported declaration type "
                << str(clang_getCursorKindSpelling(kind)) << " for '"
                << str(clang_getCursorSpelling(cursor)) << "'";
           warn(cursor, sstr.str());
           break; }
    }
    return CXChildVisit_Continue;
}

void
Visitor::generate(const CXCursor& cursor,
            const clang::NamespaceDecl* decl,
            struct ClientData* data) const
{
    // Do not consider private declarations
    if (decl->getAccess() == clang::AccessSpecifier::AS_private) {
        return;
    }
    NamespaceDescriptor descriptor(cursor, decl, *this);
    descriptor.generate();
    data->ostr << descriptor;
}

void
Visitor::generate(const CXCursor& cursor,
            const clang::CXXRecordDecl* decl,
            struct ClientData* data) const
{
    // Do not consider private declarations
    if (decl->getAccess() == clang::AccessSpecifier::AS_private) {
        return;
    }
    // Do not consider forward declaration
    if (!decl->isThisDeclarationADefinition()) {
        return;
    }
    ClassDescriptor descriptor(cursor, decl, *this);
    descriptor.generate();
    data->ostr << descriptor;
}

void
Visitor::generate(const CXCursor& cursor,
            const clang::ClassTemplateDecl* decl,
            struct ClientData* data) const
{
}

void
Visitor::generate(const CXCursor& cursor,
            const clang::ClassTemplatePartialSpecializationDecl* decl,
            struct ClientData* data) const
{
}

void
Visitor::generate(const CXCursor& cursor,
            const clang::FunctionTemplateDecl* decl,
            struct ClientData* data) const
{
}

void
Visitor::generate(const CXCursor& cursor,
            const clang::EnumDecl* decl,
            struct ClientData* data) const
{
    // Do not consider private declarations
    if (decl->getAccess() == clang::AccessSpecifier::AS_private) {
        return;
    }
    EnumDescriptor descriptor(cursor, decl, *this);
    descriptor.generate();
    data->ostr << descriptor;
}

void
Visitor::generate(const CXCursor& cursor,
            const clang::EnumConstantDecl* decl,
            struct ClientData* data) const
{
    // Do not consider private declarations
    if (decl->getAccess() == clang::AccessSpecifier::AS_private) {
        return;
    }
    EnumConstantDescriptor descriptor(cursor, decl);
    descriptor.generate();
    data->ostr << descriptor;
}

void
Visitor::generate(const CXCursor& cursor,
            const clang::CXXMethodDecl* decl,
            struct ClientData* data) const
{
    // Do not consider private declarations
    if (decl->getAccess() == clang::AccessSpecifier::AS_private) {
        return;
    }
    MethodDescriptor descriptor(cursor, decl);
    descriptor.generate();
    data->ostr << descriptor;
}

void
Visitor::generate(const CXCursor& cursor,
            const clang::CXXConstructorDecl* decl,
            struct ClientData* data) const
{
    // Do not consider private declarations
    if (decl->getAccess() == clang::AccessSpecifier::AS_private) {
        return;
    }
    ConstructorDescriptor descriptor(cursor, decl);
    descriptor.generate();
    data->ostr << descriptor;
}

void
Visitor::generate(const CXCursor& cursor,
            const clang::CXXDestructorDecl* decl,
            struct ClientData* data) const
{
    // Do not consider private declarations
    if (decl->getAccess() == clang::AccessSpecifier::AS_private) {
        return;
    }
    DestructorDescriptor descriptor(cursor, decl);
    descriptor.generate();
    data->ostr << descriptor;
}

void
Visitor::generate(const CXCursor& cursor,
            const clang::FieldDecl* decl,
            struct ClientData* data) const
{
}

void
Visitor::generate(const CXCursor& cursor,
            const clang::TypedefDecl* decl,
            struct ClientData* data) const
{
}

} // namespace muddoc
