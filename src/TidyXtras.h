/*
 * MIT License
 *
 * Copyright (c) 2021 Circu1tI0N3rd (thanhhaidoan@tutanota.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _TIDYXTRAS_H
#define _TIDYXTRAS_H 1

#include <tidy.h>
#include <string>
#include <list>

struct TidyAttribPair {
    TidyAttrId attr;
    std::string val;
};

typedef std::list<TidyAttribPair> TidyAttribPairList;

namespace tidyx {
    int CleanHTMLDoc(TidyDoc& tdoc, const std::string& html);
    int CleanHTMLDoc(TidyDoc& tdoc, const std::string& html, std::string& diag);
    bool tidyTestAttrib(TidyNode tnod, TidyAttribPair& attrib, bool exact = false);
    TidyNode tidyNodeFindTag(TidyNode tnod, TidyTagId tag, TidyAttribPairList& attrs, bool exact = false);
    TidyNode tidyNodeFindTag(TidyNode tnod, TidyTagId tag);
    ctmbstr tidyAttrValueById(TidyNode tnod, TidyAttrId attrId);
    TidyAttribPairList NewTidyAttrList(TidyAttrId attr, const std::string& val);
    TidyAttribPairList NewTidyAttrList(TidyAttrId attr, const char* val);
    void AddAttribToList(TidyAttribPairList& lst, TidyAttrId attr, const std::string& val);
    void AddAttribToList(TidyAttribPairList& lst, TidyAttrId attr, const char* val);
    void dumpNode(TidyNode tnod, int indent);
    void dumpDoc(TidyDoc tdoc);
    std::string tidyNodeGetText(TidyDoc tdoc, TidyNode tnod);
}
#endif // !_TIDYXTRAS_H
