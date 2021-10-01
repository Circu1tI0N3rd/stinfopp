// libstinfopp.cpp : Defines the entry point for the application.
//

#include "stinfopp/stinfopp.h"
#include "stinfopp/exceptions.h"
#include <tidybuffio.h>
#include "TidyXtras.h"

using namespace stinfo_e;

// URL defines
#define SSO_BASE_URL			"https://sso.hcmut.edu.vn"
#define SSO_LOGIN_URL			SSO_BASE_URL"/cas/login?service=http%3A%2F%2Fmybk.hcmut.edu.vn%2Fstinfo%2F"
#define SSO_TITLE				"Central Authentication Service"
#define SSO_LOGOUT_URL			SSO_BASE_URL"/cas/logout?service=http%3A%2F%2Fmybk.hcmut.edu.vn%2Fstinfo%2F"
#define STINFO_PORTAL_URL		"https://mybk.hcmut.edu.vn/stinfo"
#define STINFO_TIMETABLE_URL	STINFO_PORTAL_URL"/lichhoc"
#define STINFO_EXAMTABLE_URL	STINFO_PORTAL_URL"/lichthi"
#define STINFO_TIMETABLE_P_URL	STINFO_EXAMTABLE_URL"/ajax_lichhoc"
#define STINFO_EXAMTABLE_P_URL	STINFO_EXAMTABLE_URL"/ajax_lichthi"
#define STINFO_GRADE_URL		STINFO_PORTAL_URL"/grade"
#define STINFO_GRADE_P_URL		STINFO_GRADE_URL"/ajax_grade"

// Std Headers defines
#define HEADER_USRAGENT			"User-Agent: Mozilla/5.0 (X11; Linux; x86_64; rv:89.0) Gecko/20100101 Firefox/89.0"
#define HEADER_X_REQ_WITH		"X-Requested-With: XMLHttpRequest"

// Internal variables
//static char errorBuffer[CURL_ERROR_SIZE];

// Internal function prototypes
static size_t postPush(char* dest, size_t size, size_t nmemb, void* src);
static size_t grabRespHeaders(char* buffer, size_t size, size_t nmemb, void* dest);
static size_t grabResponse(char* ptr, size_t size, size_t nmemb, void* strm);

// Class defines
stinfo::stinfo(const std::string& usr, bool global)
{
	user = usr;
	init(global);
}

stinfo::stinfo(const char usr[], bool global)
{
	user = usr;
	init(global);
}

stinfo::~stinfo()
{
	curl_slist_free_all(hders);
	curl_easy_cleanup(sesh);
	if (globalClean)
		curl_global_cleanup();
}

void stinfo::cleanGlobalOnDestroy(bool clean)
{
	globalClean = clean;
}

void stinfo::init(bool global)
{
	isLogin = false;
	globalClean = global;
	if (global)
		curl_assert("Global Init Failed!", curl_global_init(CURL_GLOBAL_ALL), true);
	if (!(sesh = curl_easy_init()))
		throw CURLerror(CURLE_FAILED_INIT, "cURL Easy Handle Init Failed!");
	curl_assert("Global Init Failed!", curl_easy_setopt(sesh, CURLOPT_ERRORBUFFER, this->errorBuffer), true);
	enableOpt(CURLOPT_FAILONERROR);
	disableOpt(CURLOPT_KEEP_SENDING_ON_ERROR);
	enableOpt(CURLOPT_FOLLOWLOCATION);
	//enableOpt(CURLOPT_VERBOSE);

	std::string thomasineFair = user;
	thomasineFair += ".biscuit";
	curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_COOKIEFILE, thomasineFair.c_str()));
	curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_COOKIEJAR, thomasineFair.c_str()));

	curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_WRITEDATA, &this->respStrm));
	curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_WRITEFUNCTION, grabResponse));

	curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_READDATA, &this->postData));
	curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_READFUNCTION, postPush));

	curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_HEADERDATA, &this->respHdrs));
	curl_assert(errorBuffer, curl_easy_setopt(sesh, CURLOPT_HEADERFUNCTION, grabRespHeaders));

	hders = NULL;
	errorStr = respHdrs = std::string();
	clearToken();
	clearPOST();
}

bool stinfo::login(const char pwd[])
{
	std::string pwds = pwd;
	return login(pwds);
}

bool stinfo::login(const std::string& pwd)
{
	if (isLogin) return isLogin;
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
		setPOST(lt, execution, pwd);
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
	return isLogin;
}

void stinfo::logout()
{
	if (!isLogin) return;
	setURL(SSO_LOGOUT_URL);

	performRequest();

	clearToken();

	return;
}

// Table grabbing
std::string stinfo::getRawTimeTable(void)
{
	long respCode = 0;
    setURL(STINFO_TIMETABLE_URL);
	performRequest(&respCode);

    setURL(STINFO_TIMETABLE_P_URL);
	postData.data = tokenPOST;
    appendContentType();
	performRequest(&respCode, true);
	clearPOST();
    slistpop(hders);
	if (respCode != 200) 
		return std::string();

	return getRawResponse();
}

std::string stinfo::getRawExamTable(void)
{
	long respCode = 0;
	setURL(STINFO_EXAMTABLE_URL);
	performRequest(&respCode);

	setURL(STINFO_EXAMTABLE_P_URL);
	postData.data = tokenPOST;
	performRequest(&respCode, true);
	clearPOST();
	if (respCode != 200)
		return std::string();

	return getRawResponse();
}

// Miscelaneous
bool stinfo::isLoggedIn(void)
{
	return isLogin;
}

std::string stinfo::reason()
{
	return errorStr;
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
void stinfo::setPOST(const std::string& lt, const std::string& exec, const std::string& pwd)
{
	char* enc = curl_easy_escape(sesh, user.c_str(), user.size());
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
		errorStr = "Error formating HTML";
		return false;
	}
	if (t_rc > 0)
		std::cout << "\n\tTidy Diag:\n" << diag << std::endl;
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
	errorStr = tidyx::tidyNodeGetText(tdoc, res);
	return (errorStr.size() > 0);
}

bool stinfo::tidyOpsAssert(TidyDoc tdoc, bool ops)
{
	if (!ops) {
		tidyRelease(tdoc);
		errorStr = "Tidy Extract Operation Failure";
	}
	return !ops; // Return true on error
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
	resetHeaders();
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
	//std::deque<char>* _src = (std::deque<char>*)src;
	postDataPack* _src = (postDataPack*)src;
	size_t transfered = 0, buffsz = size * nmemb;

	/*if (_src->size() > 0)
		while (transfered < size * nmemb) {
			if (_src->size() > 0)
				dest[transfered++] = _src->front();
			else
				break;
			_src->pop_front();
		}*/

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
