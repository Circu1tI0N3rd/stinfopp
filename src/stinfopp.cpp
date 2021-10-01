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

#include "stinfopp/stinfopp.h"
#include "stinfopp_internals.h"
#include "stinfopp/exceptions.h"
#include <cstdio>
#include "TidyXtras.h"
#include <tidybuffio.h>

using namespace stinfo_e;

// Internal function prototypes
static size_t postPush(char* dest, size_t size, size_t nmemb, void* src);
static size_t grabRespHeaders(char* buffer, size_t size, size_t nmemb, void* dest);
static size_t grabResponse(char* ptr, size_t size, size_t nmemb, void* strm);

// Class defines
stinfo::stinfo(bool curl_initglobal, bool curl_verbose)
{
    initialised = false;
    init(curl_initglobal, curl_verbose);
    biscuitBake();
    initialised = true;
}

stinfo::stinfo(const std::string& whatOven, bool curl_initglobal, bool curl_verbose)
{
    initialised = false;
    init(curl_initglobal, curl_verbose);
    biscuitBake(whatOven);
    initialised = true;
}

stinfo::stinfo(const char* whatOven, bool curl_initglobal, bool curl_verbose)
{
    initialised = false;
    init(curl_initglobal, curl_verbose);
    biscuitBake(whatOven);
    initialised = true;
}

stinfo::stinfo(const std::string& whatOven, const std::string& biscuitTray_orfn, bool curl_initglobal, bool curl_verbose)
{
    initialised = false;
    init(curl_initglobal, curl_verbose);
    biscuitBake(whatOven, biscuitTray_orfn);
    initialised = true;
}

stinfo::stinfo(const char* whatOven, const char* biscuitTray_orfn, bool curl_initglobal, bool curl_verbose)
{
    initialised = false;
    init(curl_initglobal, curl_verbose);
    biscuitBake(whatOven, biscuitTray_orfn);
    initialised = true;
}

void stinfo::init(bool curl_initglobal, bool curl_verbose)
{
    isLogin = false;
    globalClean = curl_initglobal;
    if (curl_initglobal)
        curl_assert("Global Init Failed!", curl_global_init(CURL_GLOBAL_ALL), true);
    if (!(sesh = curl_easy_init()))
        throw CURLerror(CURLE_FAILED_INIT, "cURL Easy Handle Init Failed!");
    curl_assert("Global Init Failed!", curl_easy_setopt(sesh, CURLOPT_ERRORBUFFER, this->errorBuffer), true);
    enableOpt(CURLOPT_FAILONERROR);
    disableOpt(CURLOPT_KEEP_SENDING_ON_ERROR);
    enableOpt(CURLOPT_FOLLOWLOCATION);
    if (curl_verbose)
        enableOpt(CURLOPT_VERBOSE);

    curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_WRITEDATA, &this->respStrm));
    curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_WRITEFUNCTION, grabResponse));

    curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_READDATA, &this->postData));
    curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_READFUNCTION, postPush));

    curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_HEADERDATA, &this->respHdrs));
    curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_HEADERFUNCTION, grabRespHeaders));

    hders = NULL;
    user = respHdrs = std::string();
    clearToken();
    clearPOST();
}

stinfo::~stinfo()
{
    curl_slist_free_all(hders);
    curl_easy_cleanup(sesh);
    if (globalClean)
        curl_global_cleanup();
    overbaked(true);
}

void stinfo::cleanGlobalOnDestroy(bool clean)
{
    globalClean = clean;
}

bool stinfo::login(const char *usr, const char *pwd)
{
    if (!assertLogin(true)) return false;
    {
        std::string _usr = usr, _pwd = pwd;
        return login(_usr, _pwd);
    }
}

bool stinfo::login(const std::string &usr, const char *pwd)
{
    if (!assertLogin(true)) return false;
    {
        std::string _pwd = pwd;
        return login(usr, _pwd);
    }
}

bool stinfo::login(const char *usr, const std::string &pwd)
{
    if (!assertLogin(true)) return false;
    {
        std::string _usr = usr;
        return login(_usr, pwd);
    }
}

bool stinfo::login(const std::string& usr, const std::string& pwd)
{
    if (!assertLogin(true)) return false;

    setURL(SSO_LOGIN_URL);

    performRequest();

    TidyDoc tdoc;
    if (!tidifyHtmlDoc(&tdoc)) return false;

    {
        std::string lt, execution;
        if (tidyOpsAssert(tdoc, getHiddenInput(tdoc, "lt", lt)))
            return false;
        if (tidyOpsAssert(tdoc, getHiddenInput(tdoc, "execution", execution)))
            return false;
        tidyRelease(tdoc);
        setPOST(lt, execution, usr, pwd);
    }

    {
        long respCode = 0;
        performRequest(&respCode, true);
        clearPOST();
        while (respCode >= 301 && respCode <= 303) {
            std::string url = getHeaderContent("Location");
            if (url.size() <= 0)
                url = getHeaderContent("LOCATION");
            else;
            if (url.size() > 0)
                setURL(url);
            else
                setURL(STINFO_PORTAL_URL);
            performRequest(&respCode);
        }
        std::string title;
        if (!tidifyHtmlDoc(&tdoc)) return false;
        if (tidyOpsAssert(tdoc, getTitle(tdoc, title)))
            return false;
        if (title.find(SSO_TITLE) != std::string::npos) {
            updateError(tdoc);
            tidyRelease(tdoc);
            return false;
        }
    }

    isLogin = setToken(tdoc);
    tidyRelease(tdoc);
    if (isLogin) user = usr;
    return isLogin;
}

void stinfo::logout()
{
    if (!assertLogin()) return;
    setURL(SSO_LOGOUT_URL);

    performRequest();

    isLogin = false;

    clearToken();
    user = std::string();
    overbaked();

    return;
}

// Table grabbing
bool stinfo::getRawTimeTable(std::string& jsonstr)
{
    if (!assertLogin()) return false;
    jsonstr = std::string();
    long respCode = 0;
    setURL(STINFO_TIMETABLE_URL);
    performRequest(&respCode);

    setURL(STINFO_TIMETABLE_P_URL);
    postData.data = tokenPOST;
    appendContentType(HEADER_CONTENT_TYPE_P);
    performRequest(&respCode, true);
    clearPOST();
    slistpop(hders);
    if (respCode != 200) {
        errStrm.str(std::string());
        errStrm << "HCMUT Student Info returned " << respCode;
    }

    jsonstr = getRawResponse();
    return true;
}

bool stinfo::getRawExamTable(std::string& jsonstr)
{
    if (!assertLogin()) return false;
    long respCode = 0;
    jsonstr = std::string();
    setURL(STINFO_EXAMTABLE_URL);
    performRequest(&respCode);

    setURL(STINFO_EXAMTABLE_P_URL);
    postData.data = tokenPOST;
    performRequest(&respCode, true);
    clearPOST();
    if (respCode != 200) {
        errStrm.str(std::string());
        errStrm << "HCMUT Student Info returned " << respCode;
    }

    jsonstr = getRawResponse();
    return true;
}

bool stinfo::getRawGrades(std::string &jsonstr)
{
    if (!assertLogin()) return false;
    long respCode = 0;
    jsonstr = std::string();
    setURL(STINFO_GRADES_URL);
    performRequest(&respCode);

    setURL(STINFO_GRADES_P_URL);
    postData.data = tokenPOST;
    performRequest(&respCode, true);
    clearPOST();
    if (respCode != 200) {
        errStrm.str(std::string());
        errStrm << "HCMUT Student Info returned " << respCode;
    }

    jsonstr = getRawResponse();
    return true;
}

// Miscelaneous
bool stinfo::isLoggedIn(void)
{
    return isLogin;
}

std::string stinfo::reason()
{
    std::string error = errStrm.str();
    errStrm.str(std::string());
    return error;
}

// Biscuit handling
void stinfo::biscuitBake(std::string whatOven, std::string biscuitTray_orfn)
{
    if (biscuitTray_orfn.size() > 0)
        biscuitTray_orfn += ".biscuits";
    else {
        std::time_t stmp = std::time(nullptr);
        std::tm* lstmp = std::localtime(&stmp);
        char clstmp[32];
        std::strftime(clstmp, 32, "batch_%Y%m%U%d%H%M%S.biscuits", lstmp);
        biscuitTray_orfn = clstmp;
    }
    if (whatOven.size() > 0) {
#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        biscuitTray_orfn.insert(0, "\\");
#else
        biscuitTray_orfn.insert(0, "/");
#endif
        biscuitTray_orfn.insert(0, whatOven);
    }
    std::FILE* f = fopen(biscuitTray_orfn.c_str(), "w");
    if (!f)
        throw DIRNOTEXIST(whatOven);
    std::fclose(f);

    biscuitfp = biscuitTray_orfn;

    curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_COOKIEFILE, biscuitfp.c_str()));
    curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_COOKIEJAR, biscuitfp.c_str()));
}

void stinfo::overbaked(bool ignore_clear)
{
    if (!assertInit()) return;
    if (!ignore_clear)
        curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_COOKIELIST, "ALL"));
    std::FILE* f = fopen(biscuitfp.c_str(), "w");
    if (f) {
        std::fflush(f);
        std::fclose(f);
    }
    std::remove(biscuitfp.c_str());
}

// Assertions of state
bool stinfo::assertInit()
{
    if (!initialised) {
        errStrm.str(std::string());
        errStrm << "Class initialisation encountered exception";
    }
    return initialised;
}

bool stinfo::assertLogin(bool revert)
{
    if (isLogin && revert) {
        errStrm.str(std::string());
        errStrm << "User [" << user << "] has already logged in";
    } else if (!isLogin && !revert) {
        errStrm.str(std::string());
        errStrm << "No user has logged in";
    }
    return (revert) ? !isLogin : isLogin;
}

// Basic requests
CURLcode stinfo::enableOpt(CURLoption option, bool doAssert)
{
    if (doAssert)
        return curl_assert(errorBuffer, curl_easy_setopt(sesh, option, 1L));
    else
        return curl_easy_setopt(sesh, option, 1L);
}

CURLcode stinfo::disableOpt(CURLoption option, bool doAssert)
{
    if (doAssert)
        return curl_assert(errorBuffer, curl_easy_setopt(sesh, option, 0L));
    else
        return curl_easy_setopt(sesh, option, 0L);
}

void stinfo::setURL(const char url[])
{
    curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_URL, url));
}

void stinfo::setURL(const std::string& url)
{
    setURL(url.c_str());
}

void stinfo::performRequest(long* respCode, bool doPOST)
{
    respStrm.str(std::string());
    respHdrs = std::string();

    if (doPOST) {
        enableOpt(CURLOPT_POST);
        disableOpt(CURLOPT_FOLLOWLOCATION);
    }

    curl_assert(errorBuffer, curl_easy_perform(sesh));
    if (respCode)
        curl_easy_getinfo(sesh, CURLINFO_RESPONSE_CODE, respCode);

    if (doPOST) {
        disableOpt(CURLOPT_POST);
        enableOpt(CURLOPT_FOLLOWLOCATION);
    }
}

std::string stinfo::getRawResponse(void)
{
    std::string raw = respStrm.str();
    respStrm.str(std::string());
    return raw;
}

// POST form send
void stinfo::setPOST(const std::string& lt, const std::string& exec, const std::string& usr, const std::string& pwd)
{
    char* enc = curl_easy_escape(sesh, usr.c_str(), usr.size());
    postData.data = "username=";
    postData.data += enc;
    curl_free(enc);

    enc = curl_easy_escape(sesh, pwd.c_str(), pwd.size());
    postData.data += "&password=";
    postData.data += enc;
    curl_free(enc);

    enc = curl_easy_escape(sesh, lt.c_str(), lt.size());
    postData.data += "&lt=";
    postData.data += enc;
    curl_free(enc);

    enc = curl_easy_escape(sesh, exec.c_str(), exec.size());
    postData.data += "&execution=";
    postData.data += enc;
    curl_free(enc);

    postData.data += "&_eventId=submit&submit=Login";
}

void stinfo::clearPOST(void)
{
    postData = { std::string(), 0 };
}

// On-HTML operations
std::string stinfo::extractAttrVal(TidyDoc tdoc, TidyTagId tag, TidyAttrId attr, const char val[], TidyAttrId attrx)
{
    TidyAttribPairList attrs_s;
    attrs_s.push_back({ attr, std::string(val) });
    TidyNode res = tidyx::tidyNodeFindTag(tidyGetRoot(tdoc), tag, attrs_s, true);
    if (res) {
        ctmbstr attrval = tidyx::tidyAttrValueById(res, attrx);
        if (attrval)
            return std::string((char*)attrval);
    }
    return std::string();
}

bool stinfo::getHiddenInput(TidyDoc tdoc, const char name[], std::string& out)
{
    out = std::string();
    out = extractAttrVal(tdoc, TidyTag_INPUT, TidyAttr_NAME, name, TidyAttr_VALUE);
    return (out.size() > 0);
}

bool stinfo::getToken(TidyDoc tdoc, std::string& out)
{
    out = std::string();
    out = extractAttrVal(tdoc, TidyTag_META, TidyAttr_NAME, "_token", TidyAttr_CONTENT);
    return (out.size() > 0);
}

bool stinfo::tidifyHtmlDoc(TidyDoc* tdoc)
{
    std::string html = respStrm.str(), diag;
    respStrm.str(std::string());
    *tdoc = tidyCreate();
    int t_rc = tidyx::CleanHTMLDoc(*tdoc, html, diag);
    if (t_rc < 0) {
        tidyRelease(*tdoc);
        errStrm.str(std::string());
        errStrm << "Error formating HTML";
        return false;
    }
    if (t_rc > 0) {
        std::time_t tsmp = std::time(nullptr);
        std::tm* ltsmp = std::localtime(&tsmp);
        char cltsmp[32];
        std::strftime(cltsmp, 32, "%n**** TIDY DIAG %F %T ***%n", ltsmp);
        tidyDiagStrm << cltsmp << diag << std::endl;
    }
    return true;
}

bool stinfo::getTitle(TidyDoc tdoc, std::string& title)
{
    title = std::string();
    TidyNode res = tidyx::tidyNodeFindTag(tidyGetRoot(tdoc), TidyTag_TITLE);
    if (res)
        title = tidyx::tidyNodeGetText(tdoc, res);
    return (title.size() > 0);
}

bool stinfo::updateError(TidyDoc tdoc)
{
    TidyAttribPairList attrs_s;
    attrs_s.push_back({ TidyAttr_CLASS, "errors" });
    TidyNode res = tidyx::tidyNodeFindTag(tidyGetRoot(tdoc), TidyTag_DIV, attrs_s, true);
    if (!res) return false;
    std::string errStr = tidyx::tidyNodeGetText(tdoc, res);
    errStrm.str(std::string());
    errStrm << errStr;
    return (errStr.size() > 0);
}

bool stinfo::tidyOpsAssert(TidyDoc tdoc, bool ops)
{
    if (!ops) {
        tidyRelease(tdoc);
        errStrm.str(std::string());
        errStrm << "Tidy Extract Operation Failure";
    }
    return !ops; // Return true on error
}

std::string stinfo::tidyDiag()
{
    return tidyDiagStrm.str();
}

// curl_slist reimplemented functions
void stinfo::slistfree(curl_slist **head)
{
    curl_slist_free_all(*head);
    *head = NULL;
}

void stinfo::slistpop(curl_slist *head)
{
    curl_slist* ptr = head;
    if (!ptr) return;
    if (!ptr->next) return;
    while (ptr->next->next)
        ptr = ptr->next;
    curl_slist_free_all(ptr->next);
    ptr->next = NULL;
}

// Header-related functions
void stinfo::resetHeaders(const char acceptStr[], bool update)
{
    curl_slist* temp = NULL;
    slistfree(&hders);
    hders = curl_slist_append(hders, acceptStr);
    if (!hders)
        throw CURLerror(CURLE_OUT_OF_MEMORY, "Allocate Headers Failed!");
    temp = curl_slist_append(hders, HEADER_USRAGENT);
    if (!temp)
        throw CURLerror(CURLE_OUT_OF_MEMORY, "Allocate Headers Failed!");
    hders = temp;

    if (update)
        updatecURLHeaders();
}

void stinfo::updatecURLHeaders(void)
{
    curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_HTTPHEADER, this->hders));
}

bool stinfo::setToken(TidyDoc tdoc)
{
    curl_slist* temp;
    std::string token;
    char* escch = NULL;

    if (!getToken(tdoc, token))
        return false;

    escch = curl_easy_escape(sesh, token.c_str(), token.size());
    if (!escch)
        return false;
    token = escch;
    curl_free(escch);
    tokenPOST += token;
    tokenPOST += "\"}";

    resetHeaders(HEADER_ACCEPT_JSON, false);
    std::string csrf = "X-CSRF-TOKEN: ";
    csrf += token;
    temp = curl_slist_append(hders, csrf.c_str());
    if (!temp)
        throw CURLerror(CURLE_OUT_OF_MEMORY, "Allocate Headers Failed!");
    hders = temp;

    temp = curl_slist_append(hders, HEADER_X_REQ_WITH);
    if (!temp)
        throw CURLerror(CURLE_OUT_OF_MEMORY, "Allocate Headers Failed!");
    hders = temp;

    updatecURLHeaders();
    return true;
}

void stinfo::clearToken(void)
{
    tokenPOST = "{\"_token\":\"";
    resetHeaders(HEADER_ACCEPT_HTML);
}

void stinfo::appendContentType(const char *content)
{
    curl_slist* temp = curl_slist_append(hders, content);
    if (!temp)
        throw CURLerror(CURLE_OUT_OF_MEMORY, "Allocate Headers Failed!");
    if (hders != temp) {
        hders = temp;
        updatecURLHeaders();
    }
}

std::string stinfo::getHeaderContent(const char hdrName[])
{
    std::string _hdr = hdrName;
    return getHeaderContent(_hdr);
}

std::string stinfo::getHeaderContent(const std::string& hdrName)
{
    size_t begin = respHdrs.find(hdrName);
    if (begin == std::string::npos)
        return std::string();
    begin += hdrName.size() + 1;
    if (begin > respHdrs.size())
        return std::string();
    while (respHdrs[begin] == ' ')
        begin++;

    size_t end = respHdrs.find("\r\n", begin);
    if (end == std::string::npos)
        end = respHdrs.find('\n', begin);

    if (end == std::string::npos)
        return respHdrs.substr(begin);
    else
        return respHdrs.substr(begin, end - begin);
}

// Internal functions

static size_t postPush(char* dest, size_t size, size_t nmemb, void* src)
{
    postDataPack* _src = (postDataPack*)src;
    size_t transfered = 0, buffsz = size * nmemb;

    if (_src->pos > _src->data.size()) {
        _src->pos = 0;
        return 0;
    }

    transfered = _src->data.copy(dest, buffsz, _src->pos);
    _src->pos += buffsz;

    if (transfered < buffsz)
        dest[transfered++] = '\0';

    return transfered;
}

static size_t grabRespHeaders(char* buffer, size_t size, size_t nmemb, void* dest)
{
    size_t len = size * nmemb;
    std::string *_dest = (std::string*)dest;
    _dest->append(buffer, len);
    return len;
}

static size_t grabResponse(char* ptr, size_t size, size_t nmemb, void* strm)
{
    std::ostringstream* _strm = (std::ostringstream*)strm;
    (*_strm) << ptr;
    return size * nmemb;
}
