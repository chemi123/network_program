/*
 * Sample for single curl request
 */

#include <iostream>
#include <curl/curl.h>

using namespace std;

size_t callbackWrite(char* ptr, size_t size, size_t nmemb, std::string* stream) {
  int realsize = 0;

  if (stream != NULL) {
    stream->append(ptr, size * nmemb);
    realsize = size * nmemb;
  }

  return realsize;
}

void setOptions(CURL *http_handle, const char *uri, string &chunk) {
  curl_easy_setopt(http_handle, CURLOPT_URL, uri);
  curl_easy_setopt(http_handle, CURLOPT_WRITEFUNCTION, callbackWrite);
  curl_easy_setopt(http_handle, CURLOPT_WRITEDATA, &chunk);
}

int main() {
  CURL *http_handle;
  CURLcode res;

  char uri[256] = "https://www.google.com";
  string chunk;

  http_handle = curl_easy_init();
  if (!http_handle) {
    cerr << "failed to init" << endl;
    return 1;
  }

  setOptions(http_handle, uri, chunk);
  res = curl_easy_perform(http_handle);

  if (res != CURLE_OK) {
    cout << "Fail" << endl;
  }

  cout << chunk << endl;

  curl_easy_cleanup(http_handle);

  return 0;
}
