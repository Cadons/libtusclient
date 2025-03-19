# TusClient: C++ Implementation
![](https://tus.io/_astro/banner.6876e69d.png)

[![libtusclient CI Linux](https://github.com/Cadons/libtusclient/actions/workflows/linux-workflow.yml/badge.svg)](https://github.com/Cadons/libtusclient/actions/workflows/linux-workflow.yml)
[![libtusclient CI MacOS](https://github.com/Cadons/libtusclient/actions/workflows/mac-workflow.yml/badge.svg)](https://github.com/Cadons/libtusclient/actions/workflows/mac-workflow.yml)
[![libtusclient CI Windows](https://github.com/Cadons/libtusclient/actions/workflows/windows-workflow.yml/badge.svg)](https://github.com/Cadons/libtusclient/actions/workflows/windows-workflow.yml)

## Overview
Tus is an open-source protocol designed for resumable uploads, distributed under the MIT license. This C++ implementation of the Tus client allows you to perform resumable file uploads to a compatible server using HTTP requests.

The client supports various features to ensure a smooth upload process, including automatic file chunking, caching, asynchronous behavior, and real-time upload progress tracking.

## Features
- **Full HTTP Client**: Handles all HTTP requests via the `HttpClient` class, utilizing `curl` for network communication.
- **Caching**: Implements a caching mechanism to store and reuse data during the upload process.
- **Automatic File Chunking**: The `FileChunker` class automatically divides large files into smaller chunks for resumable uploads.
- **Progression Getter**: Tracks and retrieves upload progress.
- **Asynchronous Behavior**: Enables non-blocking, asynchronous uploads for efficient handling of large files.

## Installation

### Prerequisites

Before you begin, ensure that the following tools are installed on your system:
•	CMake: This project uses CMake as the build system. Install it if you don’t have it.
•	Vcpkg: We use Vcpkg to manage dependencies like curl. If you don’t have it, install it by following Vcpkg’s installation guide.
•	libcurl: The C++ client relies on libcurl for HTTP requests. Make sure it’s available on your system.

### Steps to Install
1.	Clone the repository:
```
git clone https://github.com/Cadons/libtusclient.git
cd libtusclient
```

2.	Initialize Vcpkg and install dependencies:
#### Run the appropriate script for your platform:
•	Linux/macOS:
```
./vpm.sh
```

•	Windows:
```
./vpm.bat
```

3.	Build the project with CMake:
#### Create a build directory, configure the project, and build it:
```
mkdir build
cd build
cmake .. --preset {YOUR-SELECTED-PRESET}
cmake --build .
```
After completing these steps, you should have the TusClient library built and ready to use.

Open-Source Collaboration

We welcome contributions to this open-source project! If you’d like to contribute, please follow these steps:
1.	Fork the repository on GitHub: https://github.com/Cadons/libtusclient
2.	Clone your fork locally:
```
git clone https://github.com/Cadons/libtusclient.git
```

3.	Create a new branch for your feature or bugfix:
```
git checkout -b feature/my-new-feature
```
4.	Make your changes and commit them:
```
git commit -m "Add my new feature"
```
5.	Push your changes to your fork:
```
git push origin feature/my-new-feature
```

6.	Create a pull request from your fork to the main repository.

---

## Library Components

### **TusClient**
The main class responsible for managing the upload process. You need to instantiate this class, passing in the required parameters to initiate the upload.

### **HttpClient**
The `HttpClient` class handles all HTTP requests. It is built using `curl` to manage network communication, ensuring efficient and reliable file uploads.

### **FileChunker**
This class is responsible for splitting the file into smaller chunks to facilitate resumable uploads. It ensures that large files are uploaded in manageable segments.

### **CacheRepository**
The `CacheRepository` class manages the caching of files, helping you avoid re-uploading parts of a file that have already been successfully uploaded.

### **Verifiers**
These classes are used to verify the integrity of uploaded files using hashing algorithms like MD5. They ensure that the data uploaded is valid and consistent.

### **Loggers**
The `ILogger` interface allows you to implement custom logging. The default logger is built using EasyLogger, but you can replace it with your own implementation to suit your needs.

---

## **Example: Simple Upload with TusClient (Without Google Test)**

This example demonstrates how to upload a file using the **TusClient** class, without relying on Google Test.

```cpp
#include <iostream>
#include <filesystem>
#include "TusClient.h" // Assuming the TusClient header is included

// Function to generate a test file (example)
std::filesystem::path generateTestFile(int sizeMB) {
    std::filesystem::path testFilePath = "test_file.txt";
    // Create the test file with 'sizeMB' MB content (example implementation)
    // In a real scenario, you'd populate the file with content
    std::ofstream file(testFilePath);
    for (int i = 0; i < sizeMB * 1024 * 1024; ++i) {
        file.put('A'); // Fill the file with 'A' character (just for testing)
    }
    file.close();
    return testFilePath;
}

int main() {
    // Generate a test file of size 1MB
    std::filesystem::path testFilePath = generateTestFile(1);
    std::cout << "Test file path: " << testFilePath << std::endl;
    
    // Create a TusClient instance with the necessary parameters: app name, server URL, file path, and log level
    std::string url = "http://your-tus-server-url"; // Replace with your server URL
    TUS::TusClient client("testapp", url, testFilePath, TUS::LogLevel::INFO);

    // Perform the upload
    std::cout << "Starting upload..." << std::endl;
    client.upload();

    // Check the status of the upload
    if (client.status() == TUS::TusStatus::FINISHED) {
        std::cout << "Upload finished successfully!" << std::endl;
    } else {
        std::cout << "Upload failed with status: " << static_cast<int>(client.status()) << std::endl;
    }

    return 0;
}
```

--- 

We encourage you to submit improvements, bug fixes, or new features via pull requests. If you have a bug report or feature request, feel free to open an issue on the repository page.

---
This library is distributed under [MIT License](LICENSE) 
