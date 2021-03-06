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

#ifndef _STINFO_EXCEPT_H
#define _STINFO_EXCEPT_H 1

#include <exception>
#include <string>
#include <curl/curl.h>

namespace stinfo_e {
    CURLcode curl_assert(const char errBuf[], CURLcode resp, bool justThrow = false);

    class CURLerror : public std::exception {
    private:
        CURLcode errCode;
        std::string msg;
    public:
        CURLerror(CURLcode code, const char errBuf[]) throw();

        virtual const char* what() const throw();
        virtual const CURLcode whatCode() const throw();
    };

    class DIRNOTEXIST : public std::exception {
    private:
       std::string msg;
    public:
       DIRNOTEXIST(const std::string& path) throw();

       virtual const char* what() const throw();
    };
}

#endif // !_STINFO_EXCEPT_H

