# StInfoPP
## Dependencies
- libcurl _(with SSL support)_



## Building
```
git clone https://github.com/Circu1tI0N3rd/stinfopp.git
git submodule update --init
mkdir build && cd build
cmake ../ [-DSTINFOPP_BUILD_SHARED=0] [-DCMAKE_BUILD_TYPE=Release]
```
**_Installing of the library currently not supported!_**



## APIs
### Constructors:
```
stinfo(bool curl_initglobal = true, bool curl_verbose = false)
```
Basic construction of class.
If curl_global_init has already been called, set `curl_initglobal` to `false` to avoid re-initialisation of cURL.
Set `curl_verbose` to `true` for cURL Easy API debug output to `stdout`.

```
stinfo(const std::string& path_to_store_cookiefile, bool curl_initglobal = true, bool curl_verbose = false)
stinfo(const char* path_to_store_cookiefile, bool curl_initglobal = true, bool curl_verbose = false)
```
Set `path_to_store_cookiefile` to a desired path for temporarily storing cookies _(default: executable folder)_.

```
stinfo(const std::string& path_to_store_cookiefile, const std::string& cookie_filename, bool curl_initglobal = true, bool curl_verbose = false)
stinfo(const char* path_to_store_cookiefile, const char* cookie_filename, bool curl_initglobal = true, bool curl_verbose = false)
```
Set `cookie_filename` to a desired name for the cookiefile on creation _(default: batch_YYYYMMWWDDhhmmss)_.


### Exported functions:
```
bool login(const std::string& username, const std::string& password)
bool login(const char* username, const char* password)
bool login(const std::string& username, const char* password)
bool login(const char* username, const std::string& password)
```
Login using plain-text `username` and `password`. Return `true` if successful, `false` otherwise _(refer to `reason()` for details)_.

```
void logout()
```
Logout of exisiting account. Return imediately if no user is logged in.

```
bool getRawTimeTable(std::string& jsonstr)
```
Fetch and store raw JSON-formatted list of schedule(s) under `jsonstr`. Return `true` if successful, `false` otherwise _(refer to `reason()` for details)_.

```
bool getRawExamTable(std::string& jsonstr)
```
Fetch and store raw JSON-formatted list of examination timetable(s) under `jsonstr`. Return `true` if successful, `false` otherwise _(refer to `reason()` for details)_.

```
bool getRawGrades(std::string& jsonstr)
```
Fetch and store raw JSON-formatted list of score(s) under `jsonstr`. Return `true` if successful, `false` otherwise _(refer to `reason()` for details)_.

```
bool isLoggedIn(void)
```
Return `true` if the user is logged in, `false` otherwise.

```
std::string reason()
```
Return the reason after failure of either `login`, `getRawTimeTable`, `getRawExamTable` or `getRawGrades`.

```
std::string tidyDiag()
```
Return diagnostic informations regarding HTML document parsing.


### Exceptions
```
CURLerror
```
Thrown if any cURL Easy API execution fails.
- `what()`: return user-friendly string for detail of the exception;
- `whatCode()`: return the CURLcode of the previously executed cURL Easy function.

```
DIRNOTEXIST
```
Thrown if the specified `path_to_store_cookiefile` or current executable folder cannot store cookie file.



## License
_See [LICENSE](LICENSE)_
