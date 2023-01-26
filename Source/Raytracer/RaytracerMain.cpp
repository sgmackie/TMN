#include "../Core/Allocator.h"
#include "../Core/Colour.h"
#include "../Core/Container/String.h"
#include "../Core/CoreMath.h"
#include "../Core/Platform.h"

using namespace Core;
using namespace Core::Container;

typedef struct ProgramState {
    usize TotalMemoryInBytes;
    AllocatorTLSF *BackingAllocator = nullptr;
    AllocatorLinear *MainAllocator = nullptr;
    char **Arguments;
} ProgramState;

void RenderImage(ProgramState *RunState, String *FileBuffer, const usize Width, const usize Height)
{
    PROFILER_EVENT_START("RenderImage", ColourSRGBA::CreateRed());

    FileBuffer->Clear();
    FileBuffer->Append("P3\n");
    FileBuffer->AppendFormat("%d %d\n", Width, Height);
    FileBuffer->Append("255\n");

    const usize TotalImageSize = Width * Height;
    DynamicArray<ColourSRGBA> Colours(RunState->MainAllocator, TotalImageSize);

    for (usize i = 0; i < TotalImageSize; ++i) {
        Colours.Add(ColourSRGBA(255, 0, 0));
    }

    for (const ColourSRGBA &Colour : Colours) {
        FileBuffer->AppendFormat("%u %u %u\t", Colour.Red, Colour.Green, Colour.Blue);
    }

    Vector3 Cast = RayCast(Vector3(), Vector3(50, 0, 0), 0.13f);

    FileBuffer->Append("\n");

    PROFILER_EVENT_STOP();
}

void Run(ProgramState *RunState)
{
    PROFILER_EVENT_START("Run", ColourSRGBA::CreateRed());

    AllocatorLinear ProgramAllocator = AllocatorLinear(RunState->BackingAllocator, RunState->TotalMemoryInBytes);
    RunState->MainAllocator = &ProgramAllocator;

    String TestFilePath(RunState->MainAllocator, RunState->Arguments[1]);
    CORE_LOG(RunState->MainAllocator, "Loading file %s", TestFilePath.ToUTF8());

    Platform::FileIO::File TestFile;
    if (Platform::FileIO::Exists(RunState->MainAllocator, TestFilePath.ToUTF8())) {
        Platform::FileIO::Remove(RunState->MainAllocator, TestFilePath.ToUTF8());
    }

    if (!Platform::FileIO::CreateToWrite(&ProgramAllocator, TestFilePath.ToUTF8(), &TestFile)) {
        return;
    }

    const usize ImageWidth = String::ToU64(RunState->Arguments[2]);
    const usize ImageHeight = String::ToU64(RunState->Arguments[3]);
    const usize BufferSize = ((4 * 3 + 2) * (ImageWidth * ImageHeight)) + 16;
    String FileBuffer(RunState->MainAllocator, BufferSize);
    RenderImage(RunState, &FileBuffer, ImageWidth, ImageHeight);

    Platform::FileIO::Write(RunState->MainAllocator, &TestFile, reinterpret_cast<u8 *>(FileBuffer.ToUTF8()), FileBuffer.SizeInBytes());
    Platform::FileIO::Close(&TestFile);

    PROFILER_EVENT_STOP();
}

int main(int argc, char *argv[])
{
    PROFILER_THREAD_ID("Main");

    ProgramState RunState = {};
    RunState.TotalMemoryInBytes = Megabytes(64);
    RunState.Arguments = argv;

    AllocatorTLSF GlobalAllocator = AllocatorTLSF(RunState.TotalMemoryInBytes);
    RunState.BackingAllocator = &GlobalAllocator;

    Run(&RunState);

    return 0;
}
