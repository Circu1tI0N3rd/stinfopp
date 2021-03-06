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

#ifndef STINFOPP_H
#define STINFOPP_H

#include <iostream>
#include <sstream>
#include <string>
#include <tidy.h>
#include <curl/curl.h>
#include <curl/options.h>

struct postDataPack
{
    std::string data;
    size_t pos;
};

class stinfo
{
private:
    char errorBuffer[CURL_ERROR_SIZE];
    CURL* sesh;
    curl_slist* hders;
    std::ostringstream respStrm, tidyDiagStrm, errStrm;
    std::string user, tokenPOST, respHdrs, biscuitfp;
    postDataPack postData;
    bool isLogin, globalClean, initialised;

public:
    stinfo(bool curl_initglobal = true, bool curl_verbose = false);
    stinfo(const std::string& whatOven, bool curl_initglobal = true, bool curl_verbose = false);
    stinfo(const char* whatOven, bool curl_initglobal = true, bool curl_verbose = false);
    stinfo(const std::string& whatOven, const std::string& biscuitTray_orfn, bool curl_initglobal = true, bool curl_verbose = false);
    stinfo(const char* whatOven, const char* biscuitTray_orfn, bool curl_initglobal = true, bool curl_verbose = false);

    ~stinfo();
    void cleanGlobalOnDestroy(bool clean = true);

    bool login(const std::string& usr, const std::string& pwd);
    bool login(const char* usr, const char* pwd);
    bool login(const std::string& usr, const char* pwd);
    bool login(const char* usr, const std::string& pwd);
    void logout();

    bool getRawExamTable(std::string& jsonstr);
    bool getRawTimeTable(std::string& jsonstr);
    bool getRawGrades(std::string& jsonstr);

    bool isLoggedIn(void);
    std::string reason();
    std::string tidyDiag();

private:
    void init(bool curl_initglobal = true, bool curl_verbose = false);
    bool assertInit();
    bool assertLogin(bool revert = false);
    void biscuitBake(std::string whatOven = std::string(), std::string biscuitTray_orfn = std::string());
    void overbaked(bool ignore_clear = false);

    void setPOST(const std::string& lt, const std::string& exec, const std::string& usr, const std::string& pwd);
    void clearPOST(void);

    std::string extractAttrVal(TidyDoc tdoc, TidyTagId tag, TidyAttrId attr, const char val[], TidyAttrId attrx);
    bool getHiddenInput(TidyDoc tdoc, const char name[], std::string& out);
    bool getToken(TidyDoc tdoc, std::string& out);

    CURLcode enableOpt(CURLoption option, bool doAssert = true);
    CURLcode disableOpt(CURLoption option, bool doAssert = true);
    void setURL(const char url[]);
    void setURL(const std::string& url);
    void performRequest(long* respCode = NULL, bool doPOST = false);
    std::string getRawResponse(void);
    bool tidifyHtmlDoc(TidyDoc* tdoc);
    bool tidyOpsAssert(TidyDoc tdoc, bool ops);

    bool getTitle(TidyDoc tdoc, std::string& title);
    bool updateError(TidyDoc tdoc);

    void slistpop(curl_slist* head);
    void slistfree(curl_slist** head);

    void resetHeaders(const char acceptStr[], bool update = true);
    bool setToken(TidyDoc tdoc);
    void clearToken(void);
    void updatecURLHeaders(void);
    void appendContentType(const char content[]);

    std::string getHeaderContent(const std::string& hdrName);
    std::string getHeaderContent(const char hdrName[]);
};

#endif // STINFOPP_H
