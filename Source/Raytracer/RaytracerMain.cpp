#include "Core.h"

using namespace Core;
using namespace Core::Container;
using namespace Core::Math;

// TODO: Fix clang-tidy auto save
// TODO: assert with message and call stack https://github.com/JochenKalmbach/StackWalker
// TODO: Add custom std::move https://www.foonathan.net/2020/09/move-forward/


void RenderImage(Memory::AllocatorLinear *frameAllocator, String* fileBuffer, const usize width, const usize height)
{
	PROFILER_EVENT_START("RenderImage", Colour::SRGBA(255, 0, 0))

	fileBuffer->Clear();
	fileBuffer->Append("P3\n");
	fileBuffer->AppendFormat("%d %d\n", width, height);
	fileBuffer->Append("255\n");

	DynamicArray<Colour::SRGBA> colours(frameAllocator, width * height);

	for (usize i = 0; i < width * height; ++i) {
		colours.Add(Colour::SRGBA(255, 0, 0));
	}

	for (const Colour::SRGBA& colour : colours) {
		fileBuffer->AppendFormat("%u %u %u\t", colour.Red, colour.Green, colour.Blue);
	}

	Vector3 cast = Math::RayCast(Vector3(), Vector3(50, 0, 0), 0.13f);

	fileBuffer->Append("\n");

	frameAllocator->Reset();

	PROFILER_EVENT_STOP()
}

void Run(Memory::Allocator *allocator, char **arguments)
{
	PROFILER_EVENT_START("Run", Colour::SRGBA(255, 0, 0))

	String testFilePath(allocator, arguments[0]);
	testFilePath.Trim(14);
	testFilePath.AppendAsPath(arguments[1]);
	printf("%s\n", testFilePath.ToUTF8());

	Platform::File testFile;
	if (Platform::FileIO::Exists(allocator, testFilePath.ToUTF8())) {
		Platform::FileIO::Remove(allocator, testFilePath.ToUTF8());
	}

	if (!Platform::FileIO::CreateToWrite(allocator, testFilePath.ToUTF8(), &testFile)) {
		return;
	}

	const usize imageWidth = String::ToU64(arguments[2]);
	const usize imageHeight = String::ToU64(arguments[3]);
	const usize bufferSize = ((4 * 3 + 2) * (imageWidth * imageHeight)) + 16;
	String fileBuffer(allocator, bufferSize);
	Memory::AllocatorLinear* frameAllocator = new(allocator) Memory::AllocatorLinear(allocator, Megabytes(64));
	RenderImage(frameAllocator, &fileBuffer, imageWidth, imageHeight);

	Platform::FileIO::Write(allocator, &testFile, (u8*) fileBuffer.ToUTF8(), fileBuffer.SizeInBytes());
	Platform::FileIO::Close(&testFile);

	operator::delete(frameAllocator, allocator);

	PROFILER_EVENT_STOP()
}

int main(int argc, char *argv[])
{
	assert(argc == 4);

	PROFILER_THREAD_ID("Main")

	Memory::Allocator *mainAllocator = Core::Startup(Core::GlobalAllocatorType::MiMalloc);

	Run(mainAllocator, argv);
		
	Core::Shutdown(mainAllocator);

	return 0;
}