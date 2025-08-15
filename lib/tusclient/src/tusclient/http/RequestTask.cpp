/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#include "http/RequestTask.h"

using TUS::Http::RequestTask;


RequestTask::RequestTask(const Request &request, CURL *curl) : Request(request), curl(curl) {
}

RequestTask::~RequestTask()
= default;
