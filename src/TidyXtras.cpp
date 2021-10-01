#include "TidyXtras.h"
#include <tidybuffio.h>

int do_CleanHTMLDoc(TidyDoc& tdoc, const std::string& html, TidyBuffer& errbuf);

int tidyx::CleanHTMLDoc(TidyDoc& tdoc, const std::string& html)
{
    TidyBuffer errbuf = { 0 };
    int rc = do_CleanHTMLDoc(tdoc, html, errbuf);
    tidyBufFree(&errbuf);
    return rc;
}

int tidyx::CleanHTMLDoc(TidyDoc& tdoc, const std::string& html, std::string& diag)
{
    TidyBuffer errbuf = { 0 };
    int rc = do_CleanHTMLDoc(tdoc, html, errbuf);
    diag = (char*)errbuf.bp;
    tidyBufFree(&errbuf);
    return rc;
}

bool tidyx::tidyTestAttrib(TidyNode tnod, TidyAttribPair& attrib, bool exact)
{
    TidyAttr attr = tidyAttrGetById(tnod, attrib.attr);
    if (!attr) return false;
    ctmbstr val = tidyAttrValue(attr);
    if (exact && attrib.val == (char*)val)
        return true;
    else if (!exact && attrib.val.find((char*)val) == 0)
        return true;
    else
        return false;
}

TidyNode tidyx::tidyNodeFindTag(TidyNode tnod, TidyTagId tag, TidyAttribPairList& attrs, bool exact)
{
    if (!tnod) return NULL;

    // Find in child
    TidyNode child, ans = NULL;
    for (child = tidyGetChild(tnod); child; child = tidyGetNext(child)) {
        ans = tidyx::tidyNodeFindTag(child, tag, attrs, exact);
        if (ans) return ans;
    }

    // Find in current node
    if (tag != tidyNodeGetId(tnod)) return NULL;
    if (attrs.size() > 0)
        for (TidyAttribPairList::iterator iter = attrs.begin(); iter != attrs.end(); iter++)
            if (!tidyx::tidyTestAttrib(tnod, *iter, exact))
                return NULL;
    return tnod;
}

TidyNode tidyx::tidyNodeFindTag(TidyNode tnod, TidyTagId tag)
{
    TidyAttribPairList attrs;
    return tidyx::tidyNodeFindTag(tnod, tag, attrs);
}

ctmbstr tidyx::tidyAttrValueById(TidyNode tnod, TidyAttrId attrId)
{
    TidyAttr attr = tidyAttrGetById(tnod, attrId);
    if (!attr) return NULL;
    return tidyAttrValue(attr);
}

TidyAttribPairList tidyx::NewTidyAttrList(TidyAttrId attr, const std::string& val)
{
    TidyAttribPairList blnk;
    blnk.push_back({ attr, val });
    return blnk;
}

TidyAttribPairList tidyx::NewTidyAttrList(TidyAttrId attr, const char* val)
{
    TidyAttribPairList blnk;
    blnk.push_back({ attr, val });
    return blnk;
}

void tidyx::AddAttribToList(TidyAttribPairList& lst, TidyAttrId attr, const std::string& val)
{
    lst.push_back({ attr, val });
}

void tidyx::AddAttribToList(TidyAttribPairList& lst, TidyAttrId attr, const char* val)
{
    lst.push_back({ attr, val });
}

void tidyx::dumpNode(TidyNode tnod, int indent) {
    TidyNode child;

    for (child = tidyGetChild(tnod); child; child = tidyGetNext(child)) {
        tidyx::dumpNode(child, indent + 4);
        ctmbstr name;
        switch (tidyNodeGetType(child)) {
        case TidyNode_Root:       name = "Root";                    break;
        case TidyNode_DocType:    name = "DOCTYPE";                 break;
        case TidyNode_Comment:    name = "Comment";                 break;
        case TidyNode_ProcIns:    name = "Processing Instruction";  break;
        case TidyNode_Text:       name = "Text";                    break;
        case TidyNode_CDATA:      name = "CDATA";                   break;
        case TidyNode_Section:    name = "XML Section";             break;
        case TidyNode_Asp:        name = "ASP";                     break;
        case TidyNode_Jste:       name = "JSTE";                    break;
        case TidyNode_Php:        name = "PHP";                     break;
        case TidyNode_XmlDecl:    name = "XML Declaration";         break;

        case TidyNode_Start:
        case TidyNode_End:
        case TidyNode_StartEnd:
        default:
            name = tidyNodeGetName(child);
            break;
        }
        assert(name != NULL);
        printf("\%*.*sNode: \%s\n", indent, indent, " ", name);
    }
}

void tidyx::dumpDoc(TidyDoc tdoc) {
    tidyx::dumpNode(tidyGetRoot(tdoc), 0);
}

std::string tidyx::tidyNodeGetText(TidyDoc tdoc, TidyNode tnod)
{
    TidyNode child;
    std::string val = "";
    for (child = tidyGetChild(tnod); child; child = tidyGetNext(child))
        if (tidyNodeIsText(child) == yes && tidyNodeHasText(tdoc, child) == yes) {
            TidyBuffer valbuf = { 0 };
            if (tidyNodeGetValue(tdoc, child, &valbuf) == yes)
                if (valbuf.size > 0)
                    val += (char*)valbuf.bp;
        }
        else
            val += tidyx::tidyNodeGetText(tdoc, child);
    return val;
}

// Privates
int do_CleanHTMLDoc(TidyDoc& tdoc, const std::string& html, TidyBuffer& errbuf)
{
    if (!(tdoc)) tdoc = tidyCreate();
    int t_rc = -1;
    if (tidyOptSetBool(tdoc, TidyXhtmlOut, yes) == yes)
        t_rc = tidySetErrorBuffer(tdoc, &errbuf);
    if (t_rc >= 0)
        t_rc = tidyParseString(tdoc, html.c_str());
    if (t_rc >= 0)
        t_rc = tidyCleanAndRepair(tdoc);
    if (t_rc >= 0)
        t_rc = tidyRunDiagnostics(tdoc);
    if (t_rc > 1)
        t_rc = (tidyOptSetBool(tdoc, TidyForceOutput, yes) == yes) ? t_rc : -2;
    return t_rc;
}
