#include <platform.h>

// Code generator
// Forget c++ and templates! Just use a code generator:
// There's a source file for each generic object that just writes a giant string literal to a c and h file
// Read a .meta file that is just a text file, new type for every line, commas to denote paired types
// Each module has it's own .meta file - when you run the generator, provide a base path and scan all subfolders for meta files. When found, output into a "generated" folder

void *CLibRealloc(Allocator *allocator, void *oldBlock, usize newSize) {
    return realloc(oldBlock, newSize);
}

void CLibFree(void *block, usize size) {
    free(block);
}

Allocator CreateCLibAllocator() {
    Allocator result = { 0 };
    result.Reallocate = CLibRealloc;
    result.Free = CLibFree;
    return result;
}

int main(int argc, char **argv) {
    if (argc != 2)
        return;

    const char *codePath = argv[1];
    if (!platform->fileSystem->GetFileInfo(codePath).flags || FILE_IS_VALID & FILE_IS_DIRECTORY)
        return;

    // Don't really care about strict memory usage in the code generator so just use the C library realloc function
    Allocator allocator = CreateCLibAllocator();
    FileInfo *filePaths = platform->fileSystem->FindFiles(codePath, ".cgen", &allocator);
    allocator.Free(filePaths, 0);

    // Can this search subfolders?
    // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstfileexa

    // File metaFile = platform->fileIO->OpenToRead("C:\\BuildScratch\\TMNE\\tools\\code_generation\\meta_files\\dynamic_array.cmeta");

    // File test;
    // test.isValid = true;

    return 0;
}
