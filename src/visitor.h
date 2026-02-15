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

#ifndef _MUDDOC_VISITOR_H_
#define _MUDDOC_VISITOR_H_

#include <filesystem>
#include <memory>
#include <iostream>
#include <clang-c/Index.h>

namespace muddoc {

static CXChildVisitResult __visit(CXCursor, CXCursor, CXClientData);

class Filter
{
public:
    /**
     * @brief Clone this object.
     *
     * @return A new instance that is an idential copy of this instance.
     */
    virtual Filter* clone() const { return nullptr; }

    /**
     * @brief Destructor.
     */
    virtual ~Filter() = default;

    /**
     * @brief verify if the cursor matches the filter condition.
     *
     * @details
     * Match the language construct at the @p cursor and return true if the
     * match is successful.
     * @return True if the match is successful.
     */
    virtual bool match(const CXCursor& cursor) const = 0;

protected:
    
    Filter() = default;
private:
};

/**
 * Create a filter that allows everything.
 */
class AnyFilter: public Filter
{
public:
    /**
     * @brief Create a filter that matches everthing.
     */
    AnyFilter() = default;

    /**
     * @brief Clone this object.
     *
     * @return A new instance that is an idential copy of this instance.
     */
    Filter* clone() const override { return new AnyFilter(); }

    /**
     * @brief verify if the cursor matches the filter condition.
     *
     * @details
     * Match the language construct at the @p cursor and return true if the
     * match is successful.
     * @return True if the match is successful.
     */
    bool match(const CXCursor& cursor) const override { return true; }
};

/**
 * Create a filter for a file name. Only elements that occur in the specified
 * filename are matching.
 */
class FileFilter: public Filter
{
public:
    /**
     * @brief Create a filter for a file path.
     *
     * @param path The file patch to match against.
     */
    FileFilter(const std::filesystem::path& path);

    /**
     * @brief Clone this object.
     *
     * @return A new instance that is an idential copy of this instance.
     */
    Filter* clone() const override { return new FileFilter(_path); }

    /**
     * @brief verify if the cursor matches the filter condition.
     *
     * @details
     * Match the language construct at the @p cursor and return true if the
     * match is successful.
     * @return True if the match is successful.
     */
    bool match(const CXCursor& cursor) const override;

private: 
    /** The path to filter on. */
    const std::filesystem::path& _path;
};

/**
 * @brief Class to visit elementsi in a translation unit.
 *
 * @details
 * The visitor object can generate an XML representation of a translation
 * unit by iterating over all of its elements and generating their
 * representation when each element is visited.
 */
class Visitor
{
public:
    /**
     * @brief Create a visitor of language constructs.
     *
     *
     * @param unit The translation unit to visit.
     */
    Visitor(CXTranslationUnit unit);

    /**
     * @brief Generate a representation of the translation unit.
     *
     * @details
     * Iterate over all the elements of the translation unit and output the
     * XML representation. The elements can be filtered in such a manner that
     * only the elements that pass the filter will appear in the output.
     *
     * @param output The output stream to push the 
     * @param filter The filter to apply.
     */
    void generate(std::ostream& output, const Filter& filter);

    /**
     * @brief Generate a representation of all the children from a particular
     * cursor location.
     *
     * @details
     * Visit all the children of the cursor location and construct the
     * XML representation.
     *
     * @param cursor The location to visit the children of.
     * @return The XML result of the descriptions of the cursor.
     */
    std::string generate(CXCursor cursor) const;

private:
    friend CXChildVisitResult __visit(CXCursor, CXCursor, CXClientData);
    struct ClientData;

    /**
     * @brief Visitor function when iterating through the language elements.
     *
     * @details
     * The clang visitation callback routine when iterating through the elements
     * of the translation unit. For each element that is encountered, it may
     * generate the XML representation and either continue to the next sibling
     * or iterate through its children recursively.
     *
     * @param cursor The location for the language element.
     * @param parent The location for the language element's parent.
     * @param data The custom client data.
     * @return Either @c CXChildVisit_Continue to move to the next sibling, or
     * @c CXChildVisit_Recurse to move to this elements children recursively.
     */
    CXChildVisitResult visit(CXCursor cursor, CXCursor parent,
            struct ClientData* data) const;

    /**
     * @brief Generate the output for the language element.
     *
     * @details
     * When a language construct of a particular kind is encountred, generate
     * the output in an XML representation.
     * 
     * @param cursor The location for the language element.
     * @param decl The language element declaration.
     * @param data The custom client data.
     */
    void generate(const CXCursor& cursor,
            const clang::NamespaceDecl* decl,
            struct ClientData* data) const;
    void generate(const CXCursor& cursor,
            const clang::CXXRecordDecl* decl,
            struct ClientData* data) const;
    void generate(const CXCursor& cursor,
            const clang::ClassTemplateDecl* decl,
            struct ClientData* data) const;
    void generate(const CXCursor& cursor,
            const clang::ClassTemplatePartialSpecializationDecl* decl,
            struct ClientData* data) const;
    void generate(const CXCursor& cursor,
            const clang::FunctionTemplateDecl* decl,
            struct ClientData* data) const;
    void generate(const CXCursor& cursor,
            const clang::EnumDecl* decl,
            struct ClientData* data) const;
    void generate(const CXCursor& cursor,
            const clang::EnumConstantDecl* decl,
            struct ClientData* data) const;
    void generate(const CXCursor& cursor,
            const clang::CXXMethodDecl* decl,
            struct ClientData* data) const;
    void generate(const CXCursor& cursor,
            const clang::CXXConstructorDecl* decl,
            struct ClientData* data) const;
    void generate(const CXCursor& cursor,
            const clang::CXXDestructorDecl* decl,
            struct ClientData* data) const;
    void generate(const CXCursor& cursor,
            const clang::FieldDecl* decl,
            struct ClientData* data) const;
    void generate(const CXCursor& cursor,
            const clang::TypedefDecl* decl,
            struct ClientData* data) const;

    /** The translation unit to visit */
    CXTranslationUnit _unit;

    /** The filter to apply while generating. */
    std::shared_ptr<Filter> _filter;

};

} // namespace muddoc

#endif /* _MUDDOC_VISITOR_H_ */
