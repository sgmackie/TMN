#pragma once

#include "Memory.h"
#include "Container/String.h"
using namespace Core::Container;

namespace Core {
namespace Platform {
namespace File {
    // File system
    bool DirectoryExists(const String directoryPath);

    // File IO
    bool Exists(const String path);
	void OpenToWrite(const String path);
}
}
}

