#pragma once
#ifndef _STINFO_EXCEPT_H
#define _STINFO_EXCEPT_H 1

#include "stinfopp_global.h"
#include <exception>
#include <string>
#include <curl/curl.h>

namespace stinfo_e {
	CURLcode STINFOPP_EXPORT curl_assert(const char errBuf[], CURLcode resp, bool justThrow = false);

	class STINFOPP_EXPORT CURLerror : public std::exception {
	private:
		CURLcode errCode;
		std::string msg;
	public:
		CURLerror(CURLcode code, const char errBuf[]) throw();

		virtual const char* what() const throw();
		virtual const CURLcode whatCode() const throw();
	};
}

#endif // !_STINFO_EXCEPT_H

