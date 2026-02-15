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

#ifndef _MUDDOC_UTILITY_H_
#define _MUDDOC_UTILITY_H_

#include <string>
#include <clang-c/CXString.h>
#include <llvm/ADT/StringRef.h>

namespace muddoc {

/**
 * @brief Convert a @c CXString to a @c std::string.
 *
 * @details
 * Copy the value of a @c CXString object to a @c std::string object, while
 * ensuring that the underlying data is properly disposed of.
 *
 * @param obj The object to copy from.
 * @return The value of @p obj in a @c std::string object.
 */
std::string str(const CXString& obj);

/**
 * @brief Convert a @c llvm::StringRef to a @c std::string.
 *
 * @details
 * Copy the value of a @c llvm::StringRef object to a @c std::string object,
 * while ensuring that the underlying data is properly disposed of.
 *
 * @param obj The object to copy from.
 * @return The value of @p obj in a @c std::string object.
 */
std::string str(const llvm::StringRef& obj);

/**
 * @brief Apply XML character escaping.
 *
 * @details
 * Apply XML character escaping to an input string and return the escaped
 * result. Only 5 characters are escaped (single quote, double quote,
 * ampersand, less-than and greater-than characters).
 *
 * @param str The string to escape.
 * @return The XML escaped copy of @p str.
 */
std::string escape(const std::string& str);

} // namespace muddoc

#endif /* _MUDDOC_UTILITY_H_ */
