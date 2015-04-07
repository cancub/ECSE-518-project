#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>


int main()
{
	CURL *curl;
    CURLcode curl_res;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://bit.ly/1zKaphR");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    	curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
        // curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)"); 

        /* Perform the request, curl_res will get the return code */ 
        curl_res = curl_easy_perform(curl);
        // printf("here\n");

        /* Check for errors */ 
        if(curl_res != CURLE_OK)
          fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(curl_res));

        if(CURLE_OK == curl_res) 
        {
             char *url;
             curl_res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
             long code;

             curl_res = curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&code);
             if((CURLE_OK == curl_res))
                 printf("CURLINFO_RESPONSE_CODE: .%d.\n", (int)code);

             if((CURLE_OK == curl_res) && url)
                 printf("CURLINFO_EFFECTIVE_URL: .%s.\n", url);


        }

        /* always cleanup */ 
        curl_easy_cleanup(curl);

        /* we're done with libcurl, so clean it up */ 
        curl_global_cleanup();

    }
    else
    {
        printf("cURL error.\n");
    }

    return 0;

}