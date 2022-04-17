#if defined(CORE_PLATFORM_MACOS)

#include "Platform.h"

namespace Core {
namespace Platform {
namespace File {
    bool DirectoryExists(const String directoryPath)
    {
        NSString* sourcePath = CoreStringToNSString(directoryPath);
        BOOL isDirectory;
        bool fileExists = [[NSFileManager defaultManager] fileExistsAtPath: sourcePath isDirectory: &isDirectory];
        if (!isDirectory)
        {
            return false;
        }

        return fileExists;
    }

	void OpenToWrite(const String path)
    {
        return;
    }
}
}
}

#endif
