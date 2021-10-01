#pragma once
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
