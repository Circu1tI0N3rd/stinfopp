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

#ifndef _STINFOPP_INTERNALS_H
#define _STINFOPP_INTERNALS_H 1

// URL defines
#define SSO_BASE_URL              "https://sso.hcmut.edu.vn"
#define SSO_LOGIN_URL             SSO_BASE_URL"/cas/login?service=http%3A%2F%2Fmybk.hcmut.edu.vn%2Fstinfo%2F"
#define SSO_TITLE                 "Central Authentication Service"
#define SSO_LOGOUT_URL            SSO_BASE_URL"/cas/logout?service=http%3A%2F%2Fmybk.hcmut.edu.vn%2Fstinfo%2F"
#define STINFO_PORTAL_URL         "https://mybk.hcmut.edu.vn/stinfo"
#define STINFO_TIMETABLE_URL      STINFO_PORTAL_URL"/lichhoc"
#define STINFO_EXAMTABLE_URL      STINFO_PORTAL_URL"/lichthi"
#define STINFO_TIMETABLE_P_URL    STINFO_EXAMTABLE_URL"/ajax_lichhoc"
#define STINFO_EXAMTABLE_P_URL    STINFO_EXAMTABLE_URL"/ajax_lichthi"
#define STINFO_GRADES_URL         STINFO_PORTAL_URL"/grade"
#define STINFO_GRADES_P_URL       STINFO_GRADES_URL"/ajax_grade"

// Std Headers defines
#ifndef HEADER_USRAGENT
#   define HEADER_USRAGENT        "User-Agent: Mozilla/5.0 (X11; Linux; x86_64; rv:89.0) Gecko/20100101 Firefox/89.0"
#endif
#define HEADER_X_REQ_WITH         "X-Requested-With: XMLHttpRequest"
#define HEADER_CONTENT_TYPE_P     "Content-Type: application/json; charset=UTF-8"

#define HEADER_ACCEPT_HTML        "Accept: text/html,application/xhtml+xml,application/xml"
#define HEADER_ACCEPT_ANY         "Accept: */*"
#define HEADER_ACCEPT_JSON        "Accept: text/html,application/xhtml+xml,application/xml,application/json"

#endif
