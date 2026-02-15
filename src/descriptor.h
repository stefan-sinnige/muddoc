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

#ifndef _MUDDOC_DESC_H_
#define _MUDDOC_DESC_H_

#include <iostream>
#include <string>
#include <vector>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang-c/Index.h>
#include <clang-c/CXString.h>
#include <clang/AST/Comment.h>
#include <llvm/ADT/StringRef.h>

namespace muddoc {

/** Forward declaration */
class Visitor;

/**
 * @brief Class to contain description information of a parameter.
 *
 * @details
 * A parameter (method or template) as described by the comment description. The
 * parameter will ultimately be matched against the actual declaration to
 * assert its correctness and completeness.
 */
class ParamDescriptor
{
public:
    /**
     * Construct a parameter description.
     */
    ParamDescriptor(unsigned index,
              const std::string& name,
              const std::string& description)
        : _index(index), _name(name), _description(description)
     {}

    /**
     * @brief Return the zero-based parameter index.
     * @return The parameter index.
     */
    unsigned index() const { return _index; }

    /**
     * @brief Return the name.
     * @return The parameter name.
     */
    const std::string& name() const { return _name; }

    /**
     * @brief Return the description.
     * @return The parameter description.
     */
    const std::string& description() const { return _description; }

private:
    /* The parameter index */
    unsigned _index;

    /* The parameter name */
    std::string _name;

    /* The parameter description */
    std::string _description;
};

/**
 * @brief Base class to hold the description of a declaration.
 *
 * @details
 * The description of a declaration (class, method, type-definition etc) as how
 * they appear in the source file. The comments may contain a (subset) of
 * doxygen commands as long as they are supported by the Clang comment parser.
 *
 * As there are many types of comments they will be grouped and presented in
 * a more logical form by one of its derived classes. For example, the
 * @c MethodComments derived class would separate the arguments and return
 * value comments from the general comment block and present them as separate
 * entities.
 *
 * Common description details mainatined by the base class will be:
 *  * brief: 
 *    A brief, single description
 *  * details:
 *    A complex block of comments that may contain various kinds, like examples
 *    and notes which have not been withheld by a derived class.
 */
class Descriptor
{
public:
    /**
     * Destructor.
     */
    virtual ~Descriptor() = default;

    /**
     * @brief Traverse over all the comment descriptions of the declaration.
     *
     * @details
     * Gathers all the comment descriptions by traversing though clang's parsed
     * comments. Each construct that is encountered will invoke the @c generate
     * function for declaration specific handling.
     *
     * At the end, a @c brief and/or @c detail documentation description might
     * have been created, depening on the specific handling in the derived
     * classes.
     */
    void traverse();

    /**
     * @brief Generate the description for the declaration.
     * @details
     * Generate the documentation for this declaration. This pure virtual
     * function would need to be implemented to provide declaration specific
     * descriptions.
     */
    virtual void generate() = 0;

    /**
     * @brief Get the cursor to the associate declaration.
     * @return The cursor to the declaration.
     */
    const CXCursor& cursor() const { return _cursor; }

    /**
     * @brief Get the brief description as XML.
     *
     * @details
     * Return the brief description. This is a small summary, usually just a
     * one line statement. It has been marked with the doxygen @c brief tag.
     * Any block text comment will usually be part of the @c detailed
     * description.
     *
     * @return The brief description as XML.
     */
    const std::string& brief() const { return _brief; }

    /**
     * @brief Get the detailed description as XML.
     *
     * @details
     * Return the detailed description. This is any block comment that is
     * considered to be part of the descriptive text and has not been marked
     * with the doxygen @c brief tag. Any comment that is not deemed to be
     * part of the common block text is excluded, for example method parameter
     * and return value comments. These may be captured by a derived
     * description class.
     *
     * @return The detailed description as XML.
     */
    const std::string& detailed() const { return _detailed; }

protected:
    /**
     * @brief Create parsed comments for a declaration.
     *
     * @details
     * The comments are not extracted as that will be done by the derived
     * class itself. For example, a derived class may withhold some declaration
     * specific comments from the @detail block to make it available as part
     * of their declaration context.
     *
     * @param cursor The cursor to the declaration.
     * @param decl The declaration to extract the descriptions for.
     */
    Descriptor(const CXCursor& cursor, const clang::Decl* decl);

    /**
     * @brief Traversal routines.
     *
     * @details Recursively traverse through all the comment constructs and
     * invoke the associated @c visit method. The default implementation would
     * render common text tags into two categories, brief and detailed.
     *
     * Each retuns the procesed contents of a construct in XML form, including
     * their children, recursively. These methods can be overriden to perform
     * specific handling for the comment constructs encountered.
     *
     * @param comment The comment construct encountered.
     * @return The string representing the description in XML form.
     */
    virtual std::string traverse(
            const clang::comments::BlockCommandComment* comment);
    virtual std::string traverse(
            const clang::comments::HTMLEndTagComment* comment);
    virtual std::string traverse(
            const clang::comments::HTMLStartTagComment* comment);
    virtual std::string traverse(
            const clang::comments::InlineCommandComment* comment);
    virtual std::string traverse(
            const clang::comments::FullComment* comment);
    virtual std::string traverse(
            const clang::comments::ParamCommandComment* comment);
    virtual std::string traverse(
            const clang::comments::ParagraphComment* comment);
    virtual std::string traverse(
            const clang::comments::TextComment* comment);
    virtual std::string traverse(
            const clang::comments::TParamCommandComment* comment);
    virtual std::string traverse(
            const clang::comments::VerbatimBlockComment* comment);
    virtual std::string traverse(
            const clang::comments::VerbatimBlockLineComment* comment);
    virtual std::string traverse(
            const clang::comments::VerbatimLineComment* comment);
    virtual std::string traverse(
            const clang::comments::Comment* comment) ;

    /**
     * @brief Traverse over each child.
     *
     * @details
     * Various comment constructs can have children. This function will
     * traverse over each child.
     *
     * @param begin The iterator to start from.
     * @param end The iterator to end with.
     */
    std::string traverse(
            const clang::comments::Comment::child_iterator begin,
            const clang::comments::Comment::child_iterator end);

private:
    /* Friend class */
    friend std::ostream& operator<<(std::ostream&, const Descriptor&);

    /* The cursor */
    const CXCursor& _cursor;

    /* The declaration */
    const clang::Decl* _decl;

    /* The brief description */
    std::string _brief;

    /* The detailed description */
    std::string _detailed;

    /* The description to use (refers to either @c brief or @c detailed) */
    std::string* _description;
};

/**
 * @brief Output the descriptions of a common declaration in XML.
 *
 * @details
 * Output the brief and detailed section of the declaration. The format will be
 * in XML as:
 *
 * @code
 * <brief>...</brief>
 * <detail>...</detail>
 * @endcode
 *
 * The @c brief section is usually a very short description, while the @c detail
 * section can be quite complex with embedded blocks.
 *
 * @param ostr The stream to output the comments to.
 * @param obj The object to output.
 * @return The @p ostr stream.
 */
std::ostream& operator<<(std::ostream& ostr, const Descriptor& obj);

/**
 * @brief Descriptor for a namespace declaration.
 *
 * @details
 * The namespace declaration has no specific descriptors other than the
 * standard @c brief and @c detailed blocks.
 */
class NamespaceDescriptor: public Descriptor
{
public:
    /**
     * @brief Create parsed comments for a namespcae.
     *
     * @details
     * Create comments by parsing the comment block for a namespace declaration.
     *
     * @param cursor The cursor to the declaration.
     * @param decl The namespace declaration to extract the comments for.
     * @param visitor The visitor object for its child constructs.
     */
    NamespaceDescriptor(const CXCursor& cursor,
                        const clang::NamespaceDecl* decl,
                        const Visitor& visitor);

    /**
     * @brief Destructor.
     */
    ~NamespaceDescriptor() = default;

    /**
     * @brief Generate the description for the namespace declaration.
     * @details
     * Generate the documentation for the namespace declaration. All
     * information is extracted from the declaration itself and its comments.
     */
    virtual void generate() override;

private:
    /* Friend class */
    friend std::ostream& operator<<(std::ostream&, const NamespaceDescriptor&);

    /* The declaration */
    const clang::NamespaceDecl* _decl;

    /* The visitor object */
    const Visitor& _visitor;

    /* The name of the class */
    std::string _name;

    /* The qualified name of the class */
    std::string _qualified;

    /* The XML representation of all its child member constructs */
    std::string _members;
};

/**
 * @brief Output the description of a namespace comment in XML.
 *
 * @details
 * Output the brief and detailed section of the comment. The format will be
 * in XML as:
 *
 * @code
 * <namespace name="NAME" qualified="QUALIFIED">
 *   <!-- From the Decl output: -->
 *   <brief/>
 *   <detail/>
 *   MEMBERS
 * </cclass>
 * @endcode
 *
 * The @c brief and @c detail sections are output from the common declaration
 * part but will not contain the comments that has been withheld and be made
 * accessible in different parts of the output.
 *
 * @param ostr The stream to output the comments to.
 * @param obj The object to output.
 * @return The @p ostr stream.
 */
std::ostream& operator<<(std::ostream& ostr, const NamespaceDescriptor& obj);

/**
 * @brief Descriptor for a class declaration.
 *
 * @details
 * The class declaration has separate accessors for constructor specific
 * attributes, like parameters. These are not part of the @c detail block but
 * presented separately.
 */
class ClassDescriptor: public Descriptor
{
public:
    /**
     * @brief Create parsed comments for a class.
     *
     * @details
     * Create comments by parsing the comment block for a class declaration.
     * The class-specific attributes as part of the comments are extracted
     * in their own accessors and are not made part of the @c detail block.
     *
     * @param cursor The cursor to the declaration.
     * @param decl The class declaration to extract the comments for.
     * @param visitor The visitor object for its child constructs.
     */
    ClassDescriptor(const CXCursor& cursor,
                    const clang::CXXRecordDecl* decl,
                    const Visitor& visitor);

    /**
     * @brief Destructor.
     */
    ~ClassDescriptor() = default;

    /**
     * @brief Generate the description for the class declaration.
     * @details
     * Generate the documentation for the class declaration. All information is
     * extracted from the declaration itself and its comments.
     */
    virtual void generate() override;

    /**
     * @brief Traversal routine overrides.
     *
     * @param comment The comment construct encountered.
     * @return The string representing the description in XML form.
     */
    virtual std::string traverse(
            const clang::comments::BlockCommandComment* comment) override;
    virtual std::string traverse(
            const clang::comments::TParamCommandComment* comment) override;

private:
    /* Friend class */
    friend std::ostream& operator<<(std::ostream&, const ClassDescriptor&);

    /* The declaration */
    const clang::CXXRecordDecl* _decl;

    /* The visitor object */
    const Visitor& _visitor;

    /* The name of the class */
    std::string _name;

    /* The qualified name of the class */
    std::string _qualified;

    /* The USR of the declaration */
    std::string _usr;

    /* The declaration in pretty-printed form */
    std::string _pretty;

    /* The template parameters */
    std::vector<ParamDescriptor> _params;

    /* The class members in XML serialised form. */
    std::string _members;
};

/**
 * @brief Output the description of a class comment in XML.
 *
 * @details
 * Output the brief and detailed section of the comment. The format will be
 * in XML as:
 *
 * @code
 * <class name="NAME" qualified="QUALIFIED">
 *   <usr>USR</usr>
 *   <templte-parameters>
 *     <param>
 *     </param>
 *   </templte-parameters>
 *   <!-- From the Decl output: -->
 *   <brief/>
 *   <detail/>
 * </cclass>
 * @endcode
 *
 * The @c brief and @c detail sections are output from the common declaration
 * part but will not contain the comments that has been withheld and be made
 * accessible in different parts of the output.
 *
 * @param ostr The stream to output the comments to.
 * @param obj The object to output.
 * @return The @p ostr stream.
 */
std::ostream& operator<<(std::ostream& ostr, const ClassDescriptor& obj);

/**
 * @brief Descriptor for a constructor declaration.
 *
 * @details
 * The constructor declaration has separate accessors for constructor specific
 * attributes, like parameters. These are not part of the @c detail block but
 * presented separately.
 */
class ConstructorDescriptor: public Descriptor
{
public:
    /**
     * @brief Create parsed comments for a constructor.
     *
     * @details
     * Create comments by parsing the comment block for a constructor
     * declaration. The constructor-specific attributes as part of the comments
     * are extracted in their own accessors and are not made part of the
     * @detail block.
     *
     * @param cursor The cursor to the declaration.
     * @param decl The constructor declaration to extract the comments for.
     */
    ConstructorDescriptor(const CXCursor& cursor,
                    const clang::CXXConstructorDecl* decl);

    /**
     * @brief Destructor.
     */
    ~ConstructorDescriptor() = default;

    /**
     * @brief Generate the description for the constructor declaration.
     * @details
     * Generate the documentation for the constructor declaration.
     * All information is extracted from the declaration itself and its
     * comments.
     */
    virtual void generate() override;

    /**
     * @brief Traversal routine overrides.
     *
     * @param comment The comment construct encountered.
     * @return The string representing the description in XML form.
     */
    virtual std::string traverse(
            const clang::comments::BlockCommandComment* comment) override;
    virtual std::string traverse(
            const clang::comments::ParamCommandComment* comment) override;

private:
    /* Friend class */
    friend std::ostream& operator<<(std::ostream&, const ConstructorDescriptor&);

    /* The declaration */
    const clang::CXXConstructorDecl* _decl;

    /* The name of the method */
    std::string _name;

    /* The USR of the declaration */
    std::string _usr;

    /* The declaration in pretty-printed form */
    std::string _pretty;

    /* The parameters */
    std::vector<ParamDescriptor> _params;
};

/**
 * @brief Output the description of a constructor comment in XML.
 *
 * @details
 * Output the brief and detailed section of the comment. The format will be
 * in XML as:
 *
 * @code
 * <constructor name="NAME">
 *   <info constructor="true" key=value/>
 *   <usr>USR</usr>
 *   <declaration>DECL</declaration>
 *   <parameters>
 *     <param>
 *     </param>
 *   </parameters>
 *   <!-- From the Decl output: -->
 *   <brief/>
 *   <detail/>
 * </constructor>
 * @endcode
 *
 * The @c brief and @c detail sections are output from the common declaration
 * part but will not contain the comments that has been withheld and be made
 * accessible in different parts of the output.
 *
 * @param ostr The stream to output the comments to.
 * @param obj The object to output.
 * @return The @p ostr stream.
 */
std::ostream& operator<<(std::ostream& ostr, const ConstructorDescriptor& obj);

/**
 * @brief Descriptor for a destructor declaration.
 *
 * @details
 * The destructor declaration has separate accessors for destructor specific
 * attributes, like parameters. These are not part of the @c detail block but
 * presented separately.
 */
class DestructorDescriptor: public Descriptor
{
public:
    /**
     * @brief Create parsed comments for a destructor.
     *
     * @details
     * Create comments by parsing the comment block for a destructor
     * declaration. The destructor-specific attributes as part of the comments
     * are extracted in their own accessors and are not made part of the
     * @detail block.
     *
     * @param cursor The cursor to the declaration.
     * @param decl The destructor declaration to extract the comments for.
     */
    DestructorDescriptor(const CXCursor& cursor,
                    const clang::CXXDestructorDecl* decl);

    /**
     * @brief Destructor.
     */
    ~DestructorDescriptor() = default;

    /**
     * @brief Generate the description for the destructor declaration.
     * @details
     * Generate the documentation for the destructor declaration.
     * All information is extracted from the declaration itself and its
     * comments.
     */
    virtual void generate() override;

    /**
     * @brief Traversal routine overrides.
     *
     * @param comment The comment construct encountered.
     * @return The string representing the description in XML form.
     */
    virtual std::string traverse(
            const clang::comments::BlockCommandComment* comment) override;
    virtual std::string traverse(
            const clang::comments::ParamCommandComment* comment) override;

private:
    /* Friend class */
    friend std::ostream& operator<<(std::ostream&, const DestructorDescriptor&);

    /* The declaration */
    const clang::CXXDestructorDecl* _decl;

    /* The name of the method */
    std::string _name;

    /* The USR of the declaration */
    std::string _usr;

    /* The declaration in pretty-printed form */
    std::string _pretty;

    /* The parameters */
    std::vector<ParamDescriptor> _params;
};

/**
 * @brief Output the description of a destructor comment in XML.
 *
 * @details
 * Output the brief and detailed section of the comment. The format will be
 * in XML as:
 *
 * @code
 * <method name="NAME">
 *   <info destructor="true" key=value/>
 *   <usr>USR</usr>
 *   <declaration>DECL</declaration>
 *   <parameters>
 *     <param>
 *     </param>
 *   </parameters>
 *   <!-- From the Decl output: -->
 *   <brief/>
 *   <detail/>
 * </method>
 * @endcode
 *
 * The @c brief and @c detail sections are output from the common declaration
 * part but will not contain the comments that has been withheld and be made
 * accessible in different parts of the output.
 *
 * @param ostr The stream to output the comments to.
 * @param obj The object to output.
 * @return The @p ostr stream.
 */
std::ostream& operator<<(std::ostream& ostr, const DestructorDescriptor& obj);

/**
 * @brief Descriptor for a method declaration.
 *
 * @details
 * The method declaration has separate accessors for method specific attributes,
 * like parameters and return values. These are not part of the @c detail block
 * but presented separately.
 */
class MethodDescriptor: public Descriptor
{
public:
    /**
     * @brief Create parsed comments for a method.
     *
     * @details
     * Create comments by parsing the comment block for a method declaration.
     * The method-specific attributes as part of the comments are extracted
     * in their own accessors and are not made part of the @detail block.
     *
     * @param cursor The cursor to the declaration.
     * @param decl The method declaration to extract the comments for.
     */
    MethodDescriptor(const CXCursor& cursor,
               const clang::CXXMethodDecl* decl);

    /**
     * @brief Destructor.
     */
    ~MethodDescriptor() = default;

    /**
     * @brief Generate the description for the method declaration.
     * @details
     * Generate the documentation for the method declaration. All information
     * is extracted from the declaration itself and its comments.
     */
    virtual void generate() override;

    /**
     * @brief Traversal routine overrides.
     *
     * @param comment The comment construct encountered.
     * @return The string representing the description in XML form.
     */
    virtual std::string traverse(
            const clang::comments::BlockCommandComment* comment) override;
    virtual std::string traverse(
            const clang::comments::ParamCommandComment* comment) override;

private:
    /* Friend class */
    friend std::ostream& operator<<(std::ostream&, const MethodDescriptor&);

    /* The declaration */
    const clang::CXXMethodDecl* _decl;

    /* The name of the method */
    std::string _name;

    /* The USR of the declaration */
    std::string _usr;

    /* The declaration in pretty-printed form */
    std::string _pretty;

    /* The parameters */
    std::vector<ParamDescriptor> _params;

    /* The return value */
    std::string _return;
};

/**
 * @brief Output the description of a method comment in XML.
 *
 * @details
 * Output the brief and detailed section of the comment. The format will be
 * in XML as:
 *
 * @code
 * <method name="NAME">
 *   <info key=value/>
 *   <usr>USR</usr>
 *   <declaration>DECL</declaration>
 *   <parameters>
 *     <param>
 *     </param>
 *   </parameters>
 *   <return>
 *   </return>
 *   <!-- From the Decl output: -->
 *   <brief/>
 *   <detail/>
 * </method>
 * @endcode
 *
 * The @c brief and @c detail sections are output from the common declaration
 * part but will not contain the comments that has been withheld and be made
 * accessible in different parts of the output.
 *
 * @param ostr The stream to output the comments to.
 * @param obj The object to output.
 * @return The @p ostr stream.
 */
std::ostream& operator<<(std::ostream& ostr, const MethodDescriptor& obj);

/**
 * @brief Descriptor for an enum declaration.
 *
 * @details
 * The method declaration for an enumeration.
 */
class EnumDescriptor: public Descriptor
{
public:
    /**
     * @brief Create parsed comments for an enumeration.
     *
     * @details
     * Create comments by parsing the comment block for an enumeration
     * declaration. 
     *
     * @param cursor The cursor to the declaration.
     * @param decl The enumration declaration to extract the comments for.
     * @param visitor The visitor object for its child constructs.
     */
    EnumDescriptor(const CXCursor& cursor,
                   const clang::EnumDecl* decl,
                   const Visitor& visitor);

    /**
     * @brief Destructor.
     */
    ~EnumDescriptor() = default;

    /**
     * @brief Generate the description for the enumeration declaration.
     * @details
     * Generate the documentation for the enumeration declaration. All
     * information is extracted from the declaration itself and its comments.
     */
    virtual void generate() override;

private:
    /* Friend class */
    friend std::ostream& operator<<(std::ostream&, const EnumDescriptor&);

    /* The declaration */
    const clang::EnumDecl* _decl;

    /* The visitor object */
    const Visitor& _visitor;

    /* The name of the enumeration */
    std::string _name;

    /* The XML representation of all its child member constructs */
    std::string _members;
};

/**
 * @brief Output the description of a method comment in XML.
 *
 * @details
 * Output the brief and detailed section of the comment. The format will be
 * in XML as:
 *
 * @code
 * <enum name="NAME">
 *   MEMBERS
 *   <!-- From the Decl output: -->
 *   <brief/>
 *   <detail/>
 * </method>
 * @endcode
 *
 * The @c brief and @c detail sections are output from the common declaration
 * part. The enumeration members are output inside the enumeration description.
 *
 * @param ostr The stream to output the comments to.
 * @param obj The object to output.
 * @return The @p ostr stream.
 */
std::ostream& operator<<(std::ostream& ostr, const EnumDescriptor& obj);

/**
 * @brief Descriptor for an enum constant value declaration.
 *
 * @details
 * The method declaration for an enumeration value.
 */
class EnumConstantDescriptor: public Descriptor
{
public:
    /**
     * @brief Create parsed comments for an enumeration constant.
     *
     * @details
     * Create comments by parsing the comment block for an enumeration
     * constant declaration. 
     *
     * @param cursor The cursor to the declaration.
     * @param decl The constant declaration to extract the comments for.
     */
    EnumConstantDescriptor(const CXCursor& cursor,
                           const clang::EnumConstantDecl* decl);

    /**
     * @brief Destructor.
     */
    ~EnumConstantDescriptor() = default;

    /**
     * @brief Generate the description for the enumeration constant declaration.
     * @details
     * Generate the documentation for the enumeration constant declaration. All
     * information is extracted from the declaration itself and its comments.
     */
    virtual void generate() override;

private:
    /* Friend class */
    friend std::ostream& operator<<(std::ostream&, const EnumConstantDescriptor&);

    /* The declaration */
    const clang::EnumConstantDecl* _decl;

    /* The name of the enumeration */
    std::string _name;
};

/**
 * @brief Output the description of a method comment in XML.
 *
 * @details
 * Output the brief and detailed section of the comment. The format will be
 * in XML as:
 *
 * @code
 * <value name="NAME">
 *   <!-- From the Decl output: -->
 *   <brief/>
 *   <detail/>
 * </value>
 * @endcode
 *
 * The @c brief and @c detail sections are output from the common declaration
 * part. The enumeration members are output inside the enumeration description.
 *
 * @param ostr The stream to output the comments to.
 * @param obj The object to output.
 * @return The @p ostr stream.
 */
std::ostream& operator<<(std::ostream& ostr, const EnumConstantDescriptor& obj);

} // namespace muddoc

#endif /* _MUDDOC_DESC_H_ */

