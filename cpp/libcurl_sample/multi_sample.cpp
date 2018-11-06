/*
 * Sample for double parallel curl requests with asynchrony
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

void doCurl(CURLM *multi_handle, int *still_running) {
  CURLMcode res;
  struct timeval timeout;
  int rc;

  fd_set fdread;
  fd_set fdwrite;
  fd_set fdexcep;
  int maxfd = -1;

  long curl_timeo = -1;
  FD_ZERO(&fdread);
  FD_ZERO(&fdwrite);
  FD_ZERO(&fdexcep);

  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  curl_multi_timeout(multi_handle, &curl_timeo);
  if (curl_timeo >= 0) {
    timeout.tv_sec = curl_timeo / 1000;
    if (timeout.tv_sec > 1) {
      timeout.tv_sec = 1;
    } else {
      timeout.tv_sec = (curl_timeo % 1000) * 1000;
    }
  }

  res = curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);
  cout << "maxfd: " << maxfd;
  if (res != CURLM_OK) {
    cerr << "curl_multi_fdset failed" << endl;
    return;
  }

  if (maxfd == -1) {
    struct timeval wait = { 0, 100 * 1000 };
    rc = select(0, NULL, NULL, NULL, &wait);
  } else {
    rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
  }

  switch (rc) {
  case -1:
    cout << endl;
    break;
  case 0:
  default:
    while ((res = curl_multi_perform(multi_handle, still_running))
            == CURLM_CALL_MULTI_PERFORM) {
      cout << "CURLM_CALL_MULTI_PERFORM" << endl;
    }
    cout << ", res: " << res << ", still_running: " << *still_running << endl;
    break;
  }
}

int main() {
  CURL *http_handle1;
  CURL *http_handle2;
  CURLM *multi_handle;
  string chunk1;
  string chunk2;
  CURLMcode res;
  int still_running = 0;

  http_handle1 = curl_easy_init();
  http_handle2 = curl_easy_init();
  if (!http_handle1 || !http_handle2) {
    cerr << "failed to init" << endl;
    return 1;
  }

  setOptions(http_handle1, "https://www.google.com", chunk1);
  setOptions(http_handle2, "https://www.yahoo.co.jp", chunk2);

  multi_handle = curl_multi_init();
  curl_multi_add_handle(multi_handle, http_handle1);
  curl_multi_add_handle(multi_handle, http_handle2);

  res = curl_multi_perform(multi_handle, &still_running);

  // Debug, this should return 2
  cout << "first perform. res: " << res << ", still_running: " << still_running << endl;

  while (still_running) {
    doCurl(multi_handle, &still_running);
  }

  cout << chunk1 << endl;
  cout << chunk2 << endl;

  curl_multi_cleanup(multi_handle);
  curl_easy_cleanup(http_handle1);
  curl_easy_cleanup(http_handle2);

  return 0;
}
