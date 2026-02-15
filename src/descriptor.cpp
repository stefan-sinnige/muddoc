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

#include <sstream>
#include <clang-c/Index.h>
#include <clang/AST/ASTContext.h>
#include "descriptor.h"
#include "visitor.h"
#include "utility.h"
#include "warn_error.h"

using muddoc::warn;

namespace muddoc{

CXChildVisitResult
visit(CXCursor cursor, CXCursor parent, CXClientData data) {
    return CXChildVisit_Continue;
}

/* ========================================================================
 * Descriptor
 * ======================================================================== */

Descriptor::Descriptor(const CXCursor& cursor, const clang::Decl* decl)
    : _decl(decl), _cursor(cursor), _description(&_detailed)
{
}

void
Descriptor::traverse()
{
    const clang::comments::FullComment* fc =
        _decl->getASTContext().getLocalCommentForDeclUncached(_decl);
    if (fc != nullptr) {
        _detailed = traverse(fc);
    }
    else {
        warn(_cursor, "No comment for declaration.");
    }
}

std::string
Descriptor::traverse(const clang::comments::BlockCommandComment* comment)
{
    /* If nothing to do, bail out. */
    if (comment == nullptr) {
        return std::string();
    }

    /* Handle the block type. */
    std::string result;
    switch (comment->getCommandID()) {
        case clang::comments::CommandTraits::KCI_a:
        case clang::comments::CommandTraits::KCI_abstract:
        case clang::comments::CommandTraits::KCI_addindex:
        case clang::comments::CommandTraits::KCI_addtogroup:
        case clang::comments::CommandTraits::KCI_anchor:
        case clang::comments::CommandTraits::KCI_arg:
        case clang::comments::CommandTraits::KCI_attention:
        case clang::comments::CommandTraits::KCI_author:
        case clang::comments::CommandTraits::KCI_authors:
        case clang::comments::CommandTraits::KCI_b:
            break;
        case clang::comments::CommandTraits::KCI_brief:
            _brief = traverse(comment->child_begin(), comment->child_end());
            return std::string();
        case clang::comments::CommandTraits::KCI_bug:
        case clang::comments::CommandTraits::KCI_c:
        case clang::comments::CommandTraits::KCI_callgraph:
        case clang::comments::CommandTraits::KCI_callback:
        case clang::comments::CommandTraits::KCI_callergraph:
        case clang::comments::CommandTraits::KCI_category:
        case clang::comments::CommandTraits::KCI_cite:
        case clang::comments::CommandTraits::KCI_class:
        case clang::comments::CommandTraits::KCI_classdesign:
        case clang::comments::CommandTraits::KCI_coclass:
        case clang::comments::CommandTraits::KCI_code:
            result += "<verbatim>";
            result += traverse(comment->child_begin(), comment->child_end());
            result += "</verbatim>";
            break;
        case clang::comments::CommandTraits::KCI_endcode:
        case clang::comments::CommandTraits::KCI_concept:
        case clang::comments::CommandTraits::KCI_cond:
        case clang::comments::CommandTraits::KCI_const:
        case clang::comments::CommandTraits::KCI_constant:
        case clang::comments::CommandTraits::KCI_copybrief:
        case clang::comments::CommandTraits::KCI_copydetails:
        case clang::comments::CommandTraits::KCI_copydoc:
        case clang::comments::CommandTraits::KCI_copyright:
        case clang::comments::CommandTraits::KCI_date:
        case clang::comments::CommandTraits::KCI_def:
        case clang::comments::CommandTraits::KCI_defgroup:
        case clang::comments::CommandTraits::KCI_dependency:
        case clang::comments::CommandTraits::KCI_deprecated:
        case clang::comments::CommandTraits::KCI_details:
        case clang::comments::CommandTraits::KCI_diafile:
        case clang::comments::CommandTraits::KCI_dir:
        case clang::comments::CommandTraits::KCI_discussion:
        case clang::comments::CommandTraits::KCI_docbookinclude:
        case clang::comments::CommandTraits::KCI_docbookonly:
        case clang::comments::CommandTraits::KCI_enddocbookonly:
        case clang::comments::CommandTraits::KCI_dontinclude:
        case clang::comments::CommandTraits::KCI_dot:
        case clang::comments::CommandTraits::KCI_enddot:
        case clang::comments::CommandTraits::KCI_dotfile:
        case clang::comments::CommandTraits::KCI_e:
        case clang::comments::CommandTraits::KCI_else:
        case clang::comments::CommandTraits::KCI_elseif:
        case clang::comments::CommandTraits::KCI_em:
        case clang::comments::CommandTraits::KCI_emoji:
        case clang::comments::CommandTraits::KCI_endcond:
        case clang::comments::CommandTraits::KCI_endif:
        case clang::comments::CommandTraits::KCI_enum:
        case clang::comments::CommandTraits::KCI_example:
        case clang::comments::CommandTraits::KCI_exception:
        case clang::comments::CommandTraits::KCI_extends:
        case clang::comments::CommandTraits::KCI_flbrace:
        case clang::comments::CommandTraits::KCI_frbrace:
        case clang::comments::CommandTraits::KCI_flsquare:
        case clang::comments::CommandTraits::KCI_frsquare:
        case clang::comments::CommandTraits::KCI_fdollar:
        case clang::comments::CommandTraits::KCI_flparen:
        case clang::comments::CommandTraits::KCI_frparen:
        case clang::comments::CommandTraits::KCI_file:
        case clang::comments::CommandTraits::KCI_fn:
        case clang::comments::CommandTraits::KCI_function:
        case clang::comments::CommandTraits::KCI_functiongroup:
        case clang::comments::CommandTraits::KCI_headerfile:
        case clang::comments::CommandTraits::KCI_helper:
        case clang::comments::CommandTraits::KCI_helperclass:
        case clang::comments::CommandTraits::KCI_helps:
        case clang::comments::CommandTraits::KCI_hidecallgraph:
        case clang::comments::CommandTraits::KCI_hidecallergraph:
        case clang::comments::CommandTraits::KCI_hideinitializer:
        case clang::comments::CommandTraits::KCI_hiderefby:
        case clang::comments::CommandTraits::KCI_hiderefs:
        case clang::comments::CommandTraits::KCI_htmlinclude:
        case clang::comments::CommandTraits::KCI_htmlonly:
        case clang::comments::CommandTraits::KCI_endhtmlonly:
        case clang::comments::CommandTraits::KCI_idlexcept:
        case clang::comments::CommandTraits::KCI_if:
        case clang::comments::CommandTraits::KCI_ifnot:
        case clang::comments::CommandTraits::KCI_image:
        case clang::comments::CommandTraits::KCI_implements:
        case clang::comments::CommandTraits::KCI_include:
        case clang::comments::CommandTraits::KCI_ingroup:
        case clang::comments::CommandTraits::KCI_instancesize:
        case clang::comments::CommandTraits::KCI_interface:
        case clang::comments::CommandTraits::KCI_internal:
        case clang::comments::CommandTraits::KCI_endinternal:
        case clang::comments::CommandTraits::KCI_invariant:
        case clang::comments::CommandTraits::KCI_latexinclude:
        case clang::comments::CommandTraits::KCI_latexonly:
        case clang::comments::CommandTraits::KCI_endlatexonly:
        case clang::comments::CommandTraits::KCI_li:
        case clang::comments::CommandTraits::KCI_line:
        case clang::comments::CommandTraits::KCI_link:
        case clang::comments::CommandTraits::KCI_slashlink:
        case clang::comments::CommandTraits::KCI_mainpage:
        case clang::comments::CommandTraits::KCI_maninclude:
        case clang::comments::CommandTraits::KCI_manonly:
        case clang::comments::CommandTraits::KCI_endmanonly:
        case clang::comments::CommandTraits::KCI_memberof:
        case clang::comments::CommandTraits::KCI_method:
        case clang::comments::CommandTraits::KCI_methodgroup:
        case clang::comments::CommandTraits::KCI_msc:
        case clang::comments::CommandTraits::KCI_endmsc:
        case clang::comments::CommandTraits::KCI_mscfile:
        case clang::comments::CommandTraits::KCI_n:
        case clang::comments::CommandTraits::KCI_name:
        case clang::comments::CommandTraits::KCI_namespace:
        case clang::comments::CommandTraits::KCI_noop:
        case clang::comments::CommandTraits::KCI_nosubgrouping:
        case clang::comments::CommandTraits::KCI_note:
        case clang::comments::CommandTraits::KCI_overload:
        case clang::comments::CommandTraits::KCI_ownership:
        case clang::comments::CommandTraits::KCI_p:
        case clang::comments::CommandTraits::KCI_page:
        case clang::comments::CommandTraits::KCI_par:
        case clang::comments::CommandTraits::KCI_parblock:
        case clang::comments::CommandTraits::KCI_endparblock:
        case clang::comments::CommandTraits::KCI_paragraph:
        case clang::comments::CommandTraits::KCI_param:
        case clang::comments::CommandTraits::KCI_performance:
        case clang::comments::CommandTraits::KCI_post:
        case clang::comments::CommandTraits::KCI_pre:
        case clang::comments::CommandTraits::KCI_private:
        case clang::comments::CommandTraits::KCI_privatesection:
        case clang::comments::CommandTraits::KCI_property:
        case clang::comments::CommandTraits::KCI_protected:
        case clang::comments::CommandTraits::KCI_protectedsection:
        case clang::comments::CommandTraits::KCI_protocol:
        case clang::comments::CommandTraits::KCI_public:
        case clang::comments::CommandTraits::KCI_publicsection:
        case clang::comments::CommandTraits::KCI_pure:
        case clang::comments::CommandTraits::KCI_ref:
        case clang::comments::CommandTraits::KCI_refitem:
        case clang::comments::CommandTraits::KCI_related:
        case clang::comments::CommandTraits::KCI_relatedalso:
        case clang::comments::CommandTraits::KCI_relates:
        case clang::comments::CommandTraits::KCI_relatesalso:
        case clang::comments::CommandTraits::KCI_remark:
        case clang::comments::CommandTraits::KCI_remarks:
        case clang::comments::CommandTraits::KCI_result:
        case clang::comments::CommandTraits::KCI_return:
        case clang::comments::CommandTraits::KCI_returns:
        case clang::comments::CommandTraits::KCI_retval:
        case clang::comments::CommandTraits::KCI_rtfinclude:
        case clang::comments::CommandTraits::KCI_rtfonly:
        case clang::comments::CommandTraits::KCI_endrtfonly:
        case clang::comments::CommandTraits::KCI_sa:
        case clang::comments::CommandTraits::KCI_secreflist:
        case clang::comments::CommandTraits::KCI_endsecreflist:
        case clang::comments::CommandTraits::KCI_section:
        case clang::comments::CommandTraits::KCI_security:
        case clang::comments::CommandTraits::KCI_see:
        case clang::comments::CommandTraits::KCI_seealso:
        case clang::comments::CommandTraits::KCI_short:
        case clang::comments::CommandTraits::KCI_showinitializer:
        case clang::comments::CommandTraits::KCI_showrefby:
        case clang::comments::CommandTraits::KCI_showrefs:
        case clang::comments::CommandTraits::KCI_since:
        case clang::comments::CommandTraits::KCI_skip:
        case clang::comments::CommandTraits::KCI_skipline:
        case clang::comments::CommandTraits::KCI_snippet:
        case clang::comments::CommandTraits::KCI_static:
        case clang::comments::CommandTraits::KCI_struct:
        case clang::comments::CommandTraits::KCI_subpage:
        case clang::comments::CommandTraits::KCI_subsection:
        case clang::comments::CommandTraits::KCI_subsubsection:
        case clang::comments::CommandTraits::KCI_superclass:
        case clang::comments::CommandTraits::KCI_tableofcontents:
        case clang::comments::CommandTraits::KCI_template:
        case clang::comments::CommandTraits::KCI_templatefield:
        case clang::comments::CommandTraits::KCI_test:
        case clang::comments::CommandTraits::KCI_textblock:
        case clang::comments::CommandTraits::KCI_slashtextblock:
        case clang::comments::CommandTraits::KCI_throw:
        case clang::comments::CommandTraits::KCI_throws:
        case clang::comments::CommandTraits::KCI_todo:
        case clang::comments::CommandTraits::KCI_tparam:
        case clang::comments::CommandTraits::KCI_typedef:
        case clang::comments::CommandTraits::KCI_startuml:
        case clang::comments::CommandTraits::KCI_enduml:
        case clang::comments::CommandTraits::KCI_union:
        case clang::comments::CommandTraits::KCI_until:
        case clang::comments::CommandTraits::KCI_var:
        case clang::comments::CommandTraits::KCI_verbinclude:
        case clang::comments::CommandTraits::KCI_verbatim:
        case clang::comments::CommandTraits::KCI_endverbatim:
        case clang::comments::CommandTraits::KCI_version:
        case clang::comments::CommandTraits::KCI_warning:
        case clang::comments::CommandTraits::KCI_weakgroup:
        case clang::comments::CommandTraits::KCI_xrefitem:
        case clang::comments::CommandTraits::KCI_xmlinclude:
        case clang::comments::CommandTraits::KCI_xmlonly:
        case clang::comments::CommandTraits::KCI_endxmlonly:
        default:
            break;
    }
    return result;
}

std::string
Descriptor::traverse(const clang::comments::HTMLEndTagComment* comment)
{
    /* If nothing to do, bail out. */
    if (comment == nullptr) {
        return std::string();
    }
    return traverse(comment->child_begin(), comment->child_end());
}

std::string
Descriptor::traverse(const clang::comments::HTMLStartTagComment* comment)
{
    /* If nothing to do, bail out. */
    if (comment == nullptr) {
        return std::string();
    }
    return traverse(comment->child_begin(), comment->child_end());
}

std::string
Descriptor::traverse(const clang::comments::InlineCommandComment* comment)
{
    /* If nothing to do, bail out. */
    if (comment == nullptr) {
        return std::string();
    }
    return std::string();
}

std::string
Descriptor::traverse(const clang::comments::FullComment* comment)
{
    /* If nothing to do, bail out. */
    if (comment == nullptr) {
        return std::string();
    }
    return traverse(comment->child_begin(), comment->child_end());
}

std::string
Descriptor::traverse(const clang::comments::ParamCommandComment* comment)
{
    /* If nothing to do, bail out. */
    if (comment == nullptr) {
        return std::string();
    }
    return traverse(comment->child_begin(), comment->child_end());
}

std::string
Descriptor::traverse(const clang::comments::ParagraphComment* comment)
{
    /* If nothing to do, bail out. */
    if (comment == nullptr) {
        return std::string();
    }
    return traverse(comment->child_begin(), comment->child_end());
}

std::string
Descriptor::traverse(const clang::comments::TextComment* comment)
{
    /* If nothing to do, bail out. */
    if (comment == nullptr) {
        return std::string();
    }
    if (comment->isWhitespace()) {
        return std::string();
    }
    return escape(str(comment->getText()));
}

std::string
Descriptor::traverse(const clang::comments::TParamCommandComment* comment)
{
    /* If nothing to do, bail out. */
    if (comment == nullptr) {
        return std::string();
    }
    return traverse(comment->child_begin(), comment->child_end());
}

std::string
Descriptor::traverse(const clang::comments::VerbatimBlockComment* comment)
{
    /* If nothing to do, bail out. */
    if (comment == nullptr) {
        return std::string();
    }
    std::string result;
    result += "<verbatim><![CDATA[";
    result += traverse(comment->child_begin(), comment->child_end());
    result += "]]></verbatim>";
    return result;
}

std::string
Descriptor::traverse(const clang::comments::VerbatimBlockLineComment* comment)
{
    std::string result;
    result += str(comment->getText()) + "\n";
    return result;
}

std::string
Descriptor::traverse(const clang::comments::VerbatimLineComment* comment)
{
    /* If nothing to do, bail out. */
    if (comment == nullptr) {
        return std::string();
    }
    return traverse(comment->child_begin(), comment->child_end());
}

std::string
Descriptor::traverse(const clang::comments::Comment* comment) 
{
    /* If nothing to do, bail out. */
    if (comment == nullptr) {
        return std::string();
    }

    auto kind = comment->getCommentKind();
    auto kindname = comment->getCommentKindName();
    switch (comment->getCommentKind())
    {
        case clang::comments::CommentKind::BlockCommandComment:
            return traverse(
                static_cast<const clang::comments::BlockCommandComment*>(
                    comment));
        case clang::comments::CommentKind::FullComment:
            return traverse(
                static_cast<const clang::comments::FullComment*>(
                    comment));
        case clang::comments::CommentKind::HTMLEndTagComment:
            return traverse(
                static_cast<const clang::comments::HTMLEndTagComment*>(
                    comment));
        case clang::comments::CommentKind::HTMLStartTagComment:
            return traverse(
                static_cast<const clang::comments::HTMLStartTagComment*>(
                    comment));
        case clang::comments::CommentKind::InlineCommandComment:
            return traverse(
                static_cast<const clang::comments::InlineCommandComment*>(
                    comment));
        case clang::comments::CommentKind::ParamCommandComment:
            return traverse(
                static_cast<const clang::comments::ParamCommandComment*>(
                    comment));
        case clang::comments::CommentKind::ParagraphComment:
            return traverse(
                static_cast<const clang::comments::ParagraphComment*>(
                    comment));
        case clang::comments::CommentKind::TextComment:
            return traverse(
                static_cast<const clang::comments::TextComment*>(
                    comment));
        case clang::comments::CommentKind::TParamCommandComment:
            return traverse(
                static_cast<const clang::comments::TParamCommandComment*>(
                    comment));
        case clang::comments::CommentKind::VerbatimBlockComment:
            return traverse(
                static_cast<const clang::comments::VerbatimBlockComment*>(
                    comment));
        case clang::comments::CommentKind::VerbatimBlockLineComment:
            return traverse(
                static_cast<const clang::comments::VerbatimBlockLineComment*>(
                    comment));
        case clang::comments::CommentKind::VerbatimLineComment:
            return traverse(
                static_cast<const clang::comments::VerbatimLineComment*>(
                    comment));
        default:
            // Unsupported kind
            break;
    }
    return std::string();
}

std::string
Descriptor::traverse(const clang::comments::Comment::child_iterator begin,
                  const clang::comments::Comment::child_iterator end)
{
    std::string result;
    for(auto iter = begin; iter != end; ++iter) {
        result += traverse(*iter);
    }
    return result;
}


std::ostream&
operator<<(std::ostream& ostr, const Descriptor& obj)
{
    return ostr;
}

/* ========================================================================
 * NamespaceDescriptor
 * ======================================================================== */

NamespaceDescriptor::NamespaceDescriptor(
        const CXCursor& cursor,
        const clang::NamespaceDecl* decl,
        const Visitor& visitor)
    : Descriptor(cursor, decl), _decl(decl), _visitor(visitor)
{
}

void
NamespaceDescriptor::generate()
{
    // Capture all descriptive information
    Descriptor::traverse();
    _name = _decl->getNameAsString();
    _qualified = _decl->getQualifiedNameAsString();
    _members = _visitor.generate(cursor());
}

std::ostream&
operator<<(std::ostream& ostr, const NamespaceDescriptor& obj)
{
    ostr << "<namespace"
         << " name=\"" << escape(obj._name) << "\""
         << " qualified=\"" << escape(obj._qualified) << "\""
         << ">";
    ostr << "<brief>" << obj.brief() << "</brief>";
    ostr << "<detailed>" << obj.detailed() << "</detailed>";
    ostr << obj._members;
    ostr << "</namespace>";
    return ostr;
}

/* ========================================================================
 * ClassDescriptor
 * ======================================================================== */

ClassDescriptor::ClassDescriptor(
        const CXCursor& cursor,
        const clang::CXXRecordDecl* decl,
        const Visitor& visitor)
    : Descriptor(cursor, decl), _decl(decl), _visitor(visitor)
{
}

void
ClassDescriptor::generate()
{
    // Capture all descriptive information
    Descriptor::traverse();
    _name = _decl->getNameAsString();
    _qualified = _decl->getQualifiedNameAsString();
    _usr = str(clang_getCursorUSR(cursor()));
    _pretty = "class " + _name;
    _members = _visitor.generate(cursor());

    // Match the description to the declaration and report any mismatch
    /*
    if (_params.size() != _decl->param_size()) {
        std::stringstream sstr;
        sstr << "Number of template parameters in declaration (" 
             << _decl->param_size() << ") does not match comment ("
             << _params.size() << ").";
        warn(cursor(), sstr.str());
    }
    else {
        auto desc_iter = _params.begin();
        auto decl_iter = _decl->param_begin();
        for (;
             desc_iter != _params.end() && decl_iter != _decl->param_end();
             ++desc_iter, ++decl_iter)
        {
            std::string desc_name = desc_iter->name();
            std::string decl_name = str((*decl_iter)->getName());
            if (desc_name != decl_name) {
                std::stringstream sstr;
                sstr << "Template parameter \"" << decl_name << "\" "
                     << "in declaration does not match "
                     << "parameter \"" << desc_name << "\" "
                     << "in the comment.";
                warn(cursor(), sstr.str());
            }
        }
    }
    */
}

std::string
ClassDescriptor::traverse(const clang::comments::BlockCommandComment* comment)
{
    std::string result;
    switch (comment->getCommandID()) {
        default:
            return Descriptor::traverse(comment);
    }
    return result;
}

std::string
ClassDescriptor::traverse(const clang::comments::TParamCommandComment* comment)
{
    /* If nothing to do, bail out. */
    if (comment == nullptr) {
        return std::string();
    }
    std::string description = Descriptor::traverse(
            comment->child_begin(), comment->child_end());
    if (comment->isPositionValid()) {
        _params.emplace_back(
                comment->getIndex(0),
                str(comment->getParamNameAsWritten()),
                description);
    }
    else {
        std::cout << str(comment->getParamNameAsWritten()) << std::endl;
        _params.emplace_back(
                unsigned(-1),
                str(comment->getParamNameAsWritten()),
                description);
    }
    return std::string();
}

std::ostream&
operator<<(std::ostream& ostr, const ClassDescriptor& obj)
{
    ostr << "<class"
         << " name=\"" << escape(obj._name) << "\""
         << " qualified=\"" << escape(obj._qualified) << "\""
         << ">";
    ostr << "<usr>" << escape(obj._usr) << "</usr>";
    ostr << "<declaration>" << escape(obj._pretty) << "</declaration>";
    if (!obj._params.empty()) {
        ostr << "<parameters>";
        for (const auto& param: obj._params) {
            ostr << "<param index=\"" << param.index() << "\">";
            ostr << "<name>" << escape(param.name()) << "</name>";
            ostr << "<brief>" << escape(param.description()) << "</brief>";
            ostr << "</param>";
        }
        ostr << "</parameters>";
    }
    ostr << "<brief>" << obj.brief() << "</brief>";
    ostr << "<detailed>" << obj.detailed() << "</detailed>";
    ostr << obj._members;
    ostr << "</class>";
    return ostr;
}

/* ========================================================================
 * ConstructorDescriptor
 * ======================================================================== */

ConstructorDescriptor::ConstructorDescriptor(
        const CXCursor& cursor,
        const clang::CXXConstructorDecl* decl)
    : Descriptor(cursor, decl), _decl(decl)
{
}

void
ConstructorDescriptor::generate()
{
    // Capture all descriptive information
    Descriptor::traverse();
    _name = _decl->getNameAsString();
    _usr = str(clang_getCursorUSR(cursor()));
    _pretty = str(clang_getCursorPrettyPrinted(cursor(), nullptr));

    // Match the description to the declaration and report any mismatch
    if (_params.size() != _decl->param_size()) {
        std::stringstream sstr;
        sstr << "Number of parameters in declaration (" 
             << _decl->param_size() << ") does not match comment ("
             << _params.size() << ").";
        warn(cursor(), sstr.str());
    }
    else {
        auto desc_iter = _params.begin();
        auto decl_iter = _decl->param_begin();
        for (;
             desc_iter != _params.end() && decl_iter != _decl->param_end();
             ++desc_iter, ++decl_iter)
        {
            std::string desc_name = desc_iter->name();
            std::string decl_name = str((*decl_iter)->getName());
            if (desc_name != decl_name) {
                std::stringstream sstr;
                sstr << "Parameter \"" << decl_name << "\" "
                     << "in declaration does not match "
                     << "parameter \"" << desc_name << "\" "
                     << "in the comment.";
                warn(cursor(), sstr.str());
            }
        }
    }
}

std::string
ConstructorDescriptor::traverse(const clang::comments::BlockCommandComment* comment)
{
    std::string result;
    switch (comment->getCommandID()) {
        default:
            return Descriptor::traverse(comment);
    }
    return result;
}

std::string
ConstructorDescriptor::traverse(const clang::comments::ParamCommandComment* comment)
{
    /* If nothing to do, bail out. */
    if (comment == nullptr) {
        return std::string();
    }
    std::string description = Descriptor::traverse(
            comment->child_begin(), comment->child_end());
    if (comment->isParamIndexValid()) {
        _params.emplace_back(
                comment->getParamIndex(),
                str(comment->getParamNameAsWritten()),
                description);
    }
    else {
        std::cout << str(comment->getParamNameAsWritten()) << std::endl;
        _params.emplace_back(
                unsigned(-1),
                str(comment->getParamNameAsWritten()),
                description);
    }
    return std::string();
}

std::ostream&
operator<<(std::ostream& ostr, const ConstructorDescriptor& obj)
{
    ostr << "<method"
         << " name=\"" << escape(obj._name) << "\""
         << ">";
    ostr << "<info constructor=\"true\" ";
    if (obj._decl->isCopyAssignmentOperator())
        ostr << " copy-assignment=\"true\"";
    if (obj._decl->isMoveAssignmentOperator())
        ostr << " move-assignment=\"true\"";
    if (obj._decl->isOverloadedOperator())
        ostr << " overloaded-operator=\"true\"";
    if (obj._decl->isStatic())
        ostr << " static=\"true\"";
    if (obj._decl->isConst())
        ostr << " const=\"true\"";
    if (obj._decl->isConstexpr())
        ostr << " const-expr=\"true\"";
    if (obj._decl->isConsteval())
        ostr << " const-eval=\"true\"";
    if (obj._decl->isVirtual())
        ostr << " virtual=\"true\"";
    if (obj._decl->isPureVirtual()) 
        ostr << " pure-virtual=\"true\"";
    if (obj._decl->isDefaulted())
        ostr << " default=\"true\"";
    if (obj._decl->isDeleted())
        ostr << " delete=\"true\"";
    if (obj._decl->isVariadic())
        ostr << " variadic=\"true\"";
    if (obj._decl->isGlobal())
        ostr << " global=\"true\"";
    if (obj._decl->isExternC())
        ostr << " extern-c=\"true\"";
    if (obj._decl->isInlined())
        ostr << " inline=\"true\"";
    switch (obj._decl->getAccess()) {
        case clang::AccessSpecifier::AS_private:
            ostr << " access=\"private\"";
            break;
        case clang::AccessSpecifier::AS_protected:
            ostr << " access=\"protected\"";
            break;
        default:
            ostr << " access=\"public\"";
            break;
    }
    ostr << "/>";
    ostr << "<usr>" << escape(obj._usr) << "</usr>";
    ostr << "<declaration>" << escape(obj._pretty) << "</declaration>";
    if (!obj._params.empty()) {
        ostr << "<parameters>";
        for (const auto& param: obj._params) {
            ostr << "<param index=\"" << param.index() << "\">";
            ostr << "<name>" << escape(param.name()) << "</name>";
            ostr << "<brief>" << escape(param.description()) << "</brief>";
            ostr << "</param>";
        }
        ostr << "</parameters>";
    }
    ostr << "<brief>" << obj.brief() << "</brief>";
    ostr << "<detailed>" << obj.detailed() << "</detailed>";
    ostr << "</method>";
    return ostr;
}

/* ========================================================================
 * DestructorDescriptor
 * ======================================================================== */

DestructorDescriptor::DestructorDescriptor(
        const CXCursor& cursor,
        const clang::CXXDestructorDecl* decl)
    : Descriptor(cursor, decl), _decl(decl)
{
}

void
DestructorDescriptor::generate()
{
    // Capture all descriptive information
    Descriptor::traverse();
    _name = _decl->getNameAsString();
    _usr = str(clang_getCursorUSR(cursor()));
    _pretty = str(clang_getCursorPrettyPrinted(cursor(), nullptr));

    // Match the description to the declaration and report any mismatch
    if (_params.size() != _decl->param_size()) {
        std::stringstream sstr;
        sstr << "Number of parameters in declaration (" 
             << _decl->param_size() << ") does not match comment ("
             << _params.size() << ").";
        warn(cursor(), sstr.str());
    }
    else {
        auto desc_iter = _params.begin();
        auto decl_iter = _decl->param_begin();
        for (;
             desc_iter != _params.end() && decl_iter != _decl->param_end();
             ++desc_iter, ++decl_iter)
        {
            std::string desc_name = desc_iter->name();
            std::string decl_name = str((*decl_iter)->getName());
            if (desc_name != decl_name) {
                std::stringstream sstr;
                sstr << "Parameter \"" << decl_name << "\" "
                     << "in declaration does not match "
                     << "parameter \"" << desc_name << "\" "
                     << "in the comment.";
                warn(cursor(), sstr.str());
            }
        }
    }
}

std::string
DestructorDescriptor::traverse(const clang::comments::BlockCommandComment* comment)
{
    std::string result;
    switch (comment->getCommandID()) {
        default:
            return Descriptor::traverse(comment);
    }
    return result;
}

std::string
DestructorDescriptor::traverse(const clang::comments::ParamCommandComment* comment)
{
    /* If nothing to do, bail out. */
    if (comment == nullptr) {
        return std::string();
    }
    std::string description = Descriptor::traverse(
            comment->child_begin(), comment->child_end());
    if (comment->isParamIndexValid()) {
        _params.emplace_back(
                comment->getParamIndex(),
                str(comment->getParamNameAsWritten()),
                description);
    }
    else {
        std::cout << str(comment->getParamNameAsWritten()) << std::endl;
        _params.emplace_back(
                unsigned(-1),
                str(comment->getParamNameAsWritten()),
                description);
    }
    return std::string();
}

std::ostream&
operator<<(std::ostream& ostr, const DestructorDescriptor& obj)
{
    ostr << "<method"
         << " name=\"" << escape(obj._name) << "\""
         << ">";
    ostr << "<info destructor=\"true\" ";
    if (obj._decl->isCopyAssignmentOperator())
        ostr << " copy-assignment=\"true\"";
    if (obj._decl->isMoveAssignmentOperator())
        ostr << " move-assignment=\"true\"";
    if (obj._decl->isOverloadedOperator())
        ostr << " overloaded-operator=\"true\"";
    if (obj._decl->isStatic())
        ostr << " static=\"true\"";
    if (obj._decl->isConst())
        ostr << " const=\"true\"";
    if (obj._decl->isConstexpr())
        ostr << " const-expr=\"true\"";
    if (obj._decl->isConsteval())
        ostr << " const-eval=\"true\"";
    if (obj._decl->isVirtual())
        ostr << " virtual=\"true\"";
    if (obj._decl->isPureVirtual()) 
        ostr << " pure-virtual=\"true\"";
    if (obj._decl->isDefaulted())
        ostr << " default=\"true\"";
    if (obj._decl->isDeleted())
        ostr << " delete=\"true\"";
    if (obj._decl->isVariadic())
        ostr << " variadic=\"true\"";
    if (obj._decl->isGlobal())
        ostr << " global=\"true\"";
    if (obj._decl->isExternC())
        ostr << " extern-c=\"true\"";
    if (obj._decl->isInlined())
        ostr << " inline=\"true\"";
    switch (obj._decl->getAccess()) {
        case clang::AccessSpecifier::AS_private:
            ostr << " access=\"private\"";
            break;
        case clang::AccessSpecifier::AS_protected:
            ostr << " access=\"protected\"";
            break;
        default:
            ostr << " access=\"public\"";
            break;
    }
    ostr << "/>";
    ostr << "<usr>" << escape(obj._usr) << "</usr>";
    ostr << "<declaration>" << escape(obj._pretty) << "</declaration>";
    if (!obj._params.empty()) {
        ostr << "<parameters>";
        for (const auto& param: obj._params) {
            ostr << "<param index=\"" << param.index() << "\">";
            ostr << "<name>" << escape(param.name()) << "</name>";
            ostr << "<brief>" << escape(param.description()) << "</brief>";
            ostr << "</param>";
        }
        ostr << "</parameters>";
    }
    ostr << "<brief>" << obj.brief() << "</brief>";
    ostr << "<detailed>" << obj.detailed() << "</detailed>";
    ostr << "</method>";
    return ostr;
}

/* ========================================================================
 * MethodDescriptorriptor
 * ======================================================================== */

MethodDescriptor::MethodDescriptor(
        const CXCursor& cursor,
        const clang::CXXMethodDecl* decl)
    : Descriptor(cursor, decl), _decl(decl)
{
}

void
MethodDescriptor::generate()
{
    // Capture all descriptive information
    Descriptor::traverse();
    _name = _decl->getNameAsString();
    _usr = str(clang_getCursorUSR(cursor()));
    _pretty = str(clang_getCursorPrettyPrinted(cursor(), nullptr));

    // Match the description to the declaration and report any mismatch
    if (_params.size() != _decl->param_size()) {
        std::stringstream sstr;
        sstr << "Number of parameters in declaration (" 
             << _decl->param_size() << ") does not match comment ("
             << _params.size() << ").";
        warn(cursor(), sstr.str());
    }
    else {
        auto desc_iter = _params.begin();
        auto decl_iter = _decl->param_begin();
        for (;
             desc_iter != _params.end() && decl_iter != _decl->param_end();
             ++desc_iter, ++decl_iter)
        {
            std::string desc_name = desc_iter->name();
            std::string decl_name = str((*decl_iter)->getName());
            if (desc_name != decl_name) {
                std::stringstream sstr;
                sstr << "Parameter \"" << decl_name << "\" "
                     << "in declaration does not match "
                     << "parameter \"" << desc_name << "\" "
                     << "in the comment.";
                warn(cursor(), sstr.str());
            }
        }
    }

    // Match the description of the return value
    auto type = _decl->getReturnType().getTypePtr();
    if (type && !type->isVoidType() && _return.empty()) {
        warn(cursor(), "Method has a non-void return type, but no return comment");
    }
}

std::string
MethodDescriptor::traverse(const clang::comments::BlockCommandComment* comment)
{
    std::string result;
    switch (comment->getCommandID()) {
        case clang::comments::CommandTraits::KCI_return:
        case clang::comments::CommandTraits::KCI_returns:
            _return = Descriptor::traverse(comment->child_begin(), comment->child_end());
            return std::string();
        default:
            return Descriptor::traverse(comment);
    }
    return result;
}

std::string
MethodDescriptor::traverse(const clang::comments::ParamCommandComment* comment)
{
    /* If nothing to do, bail out. */
    if (comment == nullptr) {
        return std::string();
    }
    std::string description = Descriptor::traverse(
            comment->child_begin(), comment->child_end());
    if (comment->isParamIndexValid()) {
        _params.emplace_back(
                comment->getParamIndex(),
                str(comment->getParamNameAsWritten()),
                description);
    }
    else {
        std::cout << str(comment->getParamNameAsWritten()) << std::endl;
        _params.emplace_back(
                unsigned(-1),
                str(comment->getParamNameAsWritten()),
                description);
    }
    return std::string();
}

std::ostream&
operator<<(std::ostream& ostr, const MethodDescriptor& obj)
{
    ostr << "<method"
         << " name=\"" << escape(obj._name) << "\""
         << ">";
    ostr << "<info";
    if (obj._decl->isCopyAssignmentOperator())
        ostr << " copy-assignment=\"true\"";
    if (obj._decl->isMoveAssignmentOperator())
        ostr << " move-assignment=\"true\"";
    if (obj._decl->isOverloadedOperator())
        ostr << " overloaded-operator=\"true\"";
    if (obj._decl->isStatic())
        ostr << " static=\"true\"";
    if (obj._decl->isConst())
        ostr << " const=\"true\"";
    if (obj._decl->isConstexpr())
        ostr << " const-expr=\"true\"";
    if (obj._decl->isConsteval())
        ostr << " const-eval=\"true\"";
    if (obj._decl->isVirtual())
        ostr << " virtual=\"true\"";
    if (obj._decl->isPureVirtual()) 
        ostr << " pure-virtual=\"true\"";
    if (obj._decl->isDefaulted())
        ostr << " default=\"true\"";
    if (obj._decl->isDeleted())
        ostr << " delete=\"true\"";
    if (obj._decl->isVariadic())
        ostr << " variadic=\"true\"";
    if (obj._decl->isGlobal())
        ostr << " global=\"true\"";
    if (obj._decl->isExternC())
        ostr << " extern-c=\"true\"";
    if (obj._decl->isInlined())
        ostr << " inline=\"true\"";
    switch (obj._decl->getAccess()) {
        case clang::AccessSpecifier::AS_private:
            ostr << " access=\"private\"";
            break;
        case clang::AccessSpecifier::AS_protected:
            ostr << " access=\"protected\"";
            break;
        default:
            ostr << " access=\"public\"";
            break;
    }
    ostr << "/>";
    ostr << "<usr>" << escape(obj._usr) << "</usr>";
    ostr << "<declaration>" << escape(obj._pretty) << "</declaration>";
    if (!obj._params.empty()) {
        ostr << "<parameters>";
        for (const auto& param: obj._params) {
            ostr << "<param index=\"" << param.index() << "\">";
            ostr << "<name>" << escape(param.name()) << "</name>";
            ostr << "<brief>" << escape(param.description()) << "</brief>";
            ostr << "</param>";
        }
        ostr << "</parameters>";
    }
    if (!obj._return.empty()) {
        ostr << "<return>" << obj._return << "</return>";
    }
    ostr << "<brief>" << obj.brief() << "</brief>";
    ostr << "<detailed>" << obj.detailed() << "</detailed>";
    ostr << "</method>";
    return ostr;
}

/* ========================================================================
 * EnumDescriptor
 * ======================================================================== */

EnumDescriptor::EnumDescriptor(
        const CXCursor& cursor,
        const clang::EnumDecl* decl,
        const Visitor& visitor)
    : Descriptor(cursor, decl), _decl(decl), _visitor(visitor)
{
}

void
EnumDescriptor::generate()
{
    // Capture all descriptive information
    Descriptor::traverse();
    _name = _decl->getNameAsString();
    _members = _visitor.generate(cursor());
}

std::ostream&
operator<<(std::ostream& ostr, const EnumDescriptor& obj)
{
    ostr << "<enum"
         << " name=\"" << escape(obj._name) << "\">";
    ostr << "<brief>" << obj.brief() << "</brief>";
    ostr << "<detailed>" << obj.detailed() << "</detailed>";
    ostr << "<values>" << obj._members << "</values>";
    ostr << "</enum>";
    return ostr;
}

/* ========================================================================
 * EnumConstantDescriptor
 * ======================================================================== */

EnumConstantDescriptor::EnumConstantDescriptor(
        const CXCursor& cursor,
        const clang::EnumConstantDecl* decl)
    : Descriptor(cursor, decl), _decl(decl)
{
}

void
EnumConstantDescriptor::generate()
{
    // Capture all descriptive information
    Descriptor::traverse();
    _name = _decl->getNameAsString();
}

std::ostream&
operator<<(std::ostream& ostr, const EnumConstantDescriptor& obj)
{
    ostr << "<value"
         << " name=\"" << escape(obj._name) << "\">";
    ostr << "<brief>" << obj.brief() << "</brief>";
    ostr << "<detailed>" << obj.detailed() << "</detailed>";
    ostr << "</value>";
    return ostr;
}

} // namespace muddoc

