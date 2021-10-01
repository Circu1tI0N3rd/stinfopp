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

#include "stinfopp/exceptions.h"

using namespace stinfo_e;

CURLcode stinfo_e::curl_assert(const char errBuf[], CURLcode resp, bool justThrow)
{
#ifdef THROWALL_CURLE
	if (resp != CURLE_OK)
		throw CURLerror(resp, errBuf);
#else
	if (justThrow && (resp != CURLE_OK))
		throw CURLerror(resp, errBuf);
	switch (resp)
	{
	// HIGH SEVERITY LEVEL
	case CURLE_OUT_OF_MEMORY:
	case CURLE_NOT_BUILT_IN:
	case CURLE_FUNCTION_NOT_FOUND:
	// SSL core
	case CURLE_QUIC_CONNECT_ERROR: // Foreign error
	case CURLE_SSL_ENGINE_NOTFOUND:
	case CURLE_SSL_ENGINE_SETFAILED:
	case CURLE_SSL_ENGINE_INITFAILED:
	case CURLE_SSL_CERTPROBLEM:
	case CURLE_SSL_CIPHER:
	case CURLE_SSL_CACERT_BADFILE:
	case CURLE_SSL_CRL_BADFILE:
	// cURL easy function call
	case CURLE_BAD_FUNCTION_ARGUMENT:
	case CURLE_UNKNOWN_OPTION:
	case CURLE_SETOPT_OPTION_SYNTAX:
	case CURLE_INTERFACE_FAILED: // Invalid network interface
	case CURLE_RECURSIVE_API_CALL:
	// URL assignment
	case CURLE_UNSUPPORTED_PROTOCOL:
	case CURLE_URL_MALFORMAT:
	// HTTP request related
	case CURLE_COULDNT_CONNECT:
	case CURLE_COULDNT_RESOLVE_HOST:
	case CURLE_COULDNT_RESOLVE_PROXY:
	case CURLE_WEIRD_SERVER_REPLY:
	case CURLE_REMOTE_ACCESS_DENIED:
	case CURLE_HTTP2:
	case CURLE_PARTIAL_FILE:
#ifdef THROW_CURLFAILONERR
	case CURLE_HTTP_RETURNED_ERROR:
#endif // THROW_CURLFAILONERR
	case CURLE_WRITE_ERROR:
	case CURLE_READ_ERROR:
#ifdef THROW_TIMEOUTOPS
	case CURLE_OPERATION_TIMEDOUT:
#endif // THROW_TIMEOUTOPS
	case CURLE_HTTP_POST_ERROR: // Odd POST request error
	case CURLE_SSL_CONNECT_ERROR:
	case CURLE_BAD_DOWNLOAD_RESUME:
	case CURLE_FILE_COULDNT_READ_FILE:
	case CURLE_ABORTED_BY_CALLBACK:
	case CURLE_TOO_MANY_REDIRECTS:
#ifdef THROW_EMPTYRESP
	case CURLE_GOT_NOTHING:
#endif // THROW_EMPTYRESP
	case CURLE_SEND_ERROR:
	case CURLE_RECV_ERROR:
	case CURLE_BAD_CONTENT_ENCODING:
	case CURLE_FILESIZE_EXCEEDED:
	case CURLE_SEND_FAIL_REWIND: // POST-related
	case CURLE_CHUNK_FAILED:
	case CURLE_HTTP2_STREAM:
	case CURLE_HTTP3:
	// Authentication
	case CURLE_AUTH_ERROR:
	case CURLE_LOGIN_DENIED:
	// SSL negotiation
	case CURLE_SSL_CLIENTCERT:
	case CURLE_SSL_CACERT: // also CURLE_PEER_FAILED_VERIFICATION
	case CURLE_SSL_ISSUER_ERROR:
	case CURLE_SSL_PINNEDPUBKEYNOTMATCH:
	case CURLE_SSL_INVALIDCERTSTATUS:
	case CURLE_SSL_SHUTDOWN_FAILED:
		throw CURLerror(resp, errBuf);
		break;
	// All others
	default: // notable: CURLE_CONV_FAILED, CURLE_CONV_REQD
		// CURLE_AGAIN (curl_easy_recv & curl_easy_send)
		return resp;
		break;
	}
#endif
}

CURLerror::CURLerror(CURLcode code, const char errBuf[]) throw()
{
	msg = errBuf;
	errCode = code;
}

const char* CURLerror::what() const throw()
{
	return msg.c_str();
}

const CURLcode CURLerror::whatCode() const throw()
{
	return errCode;
}
