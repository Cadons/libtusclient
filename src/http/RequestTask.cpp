#include "http/RequestTask.h"

using TUS::Http::RequestTask;


RequestTask::RequestTask(Request request, CURL *curl) : Request(request), curl(curl)
{
}

RequestTask::~RequestTask()
{
    curl_easy_cleanup(curl);
}
