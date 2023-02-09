#include "../Core/Allocator.h"
#include "../Core/Colour.h"
#include "../Core/Container/String.h"
#include "../Core/CoreMath.h"
#include "../Core/Platform.h"

using namespace Core;
using namespace Core::Container;

typedef struct ProgramState {
    Core::Allocator *MainAllocator = nullptr;
    char **Arguments;
} ProgramState;

typedef struct RenderCamera
{
	Vector3 Position;
	f32 ViewPortWidth;
	f32 ViewPortHeight;
	f32 FocalLength;
} RenderCamera;

typedef struct Ray
{
	Ray(const Vector3 &InPosition, const Vector3 &InDirection)
	{
		Position = InPosition;
		Direction = InDirection;
	}
	
	Vector3 Position;
	Vector3 Direction;
} Ray;

void RenderImage(ProgramState *RunState, String *FileBuffer, RenderCamera *Camera, const u32 ImageHeight, const u32 ImageWidth)
{
    PROFILER_EVENT_START("RenderImage", ColourSRGBA::CreateRed());

	// Render constants
	const Vector3 LastHorizontalPixel = Vector3(Camera->ViewPortWidth, 0.0f, 0.0f);
	const Vector3 LastVerticalPixel = Vector3(0.0f, Camera->ViewPortHeight, 0.0f);
	const Vector3 HalfWidth = LastHorizontalPixel / 2.0f;
	const Vector3 HalfHeight = LastVerticalPixel / 2.0f;
	const Vector3 FocalLengthDirection = Vector3(0.0f, 0.0f, Camera->FocalLength);
	const Vector3 LowerLeftCorner = Camera->Position - HalfWidth - HalfHeight - FocalLengthDirection;

	// Write colour values
	const usize PixelsToWrite = ImageHeight * ImageWidth;
	DynamicArray<ColourSRGBA> Colours(RunState->MainAllocator, PixelsToWrite);
	
	const ColourLinear White = ColourLinear(1.0f, 1.0f, 1.0f);
	const ColourLinear Blue = ColourLinear(0.5f, 0.7f, 1.0f);


	for (int YIdx = (ImageHeight - 1); YIdx >= 0; --YIdx)
	{
		for (int XIdx = 0; XIdx < ImageWidth; ++XIdx)
		{
			const f32 U = f32(XIdx) / (ImageWidth - 1);
			const f32 V = f32(YIdx) / (ImageHeight - 1);
			const Vector3 RayHorizontal = LastHorizontalPixel * U;
			const Vector3 RayVertical = LastVerticalPixel * V;

			Ray CurrentRay = Ray(Camera->Position, (LowerLeftCorner + RayHorizontal + RayVertical - Camera->Position));
			const Vector3 RayDirection = Vector3::UnitVector(CurrentRay.Direction);

			const f32 LerpDelta = 0.5f * (RayDirection.Y + 1.0f);
			const ColourLinear Gradient = (White * (1.0f - LerpDelta)) + (Blue * LerpDelta);

			ColourSRGBA Convert;
			Convert.Red = 255.999 * Gradient.Red;
			Convert.Green = 255.999 * Gradient.Green;
			Convert.Blue = 255.999 * Gradient.Blue;

			Colours.Add(Convert);
		}
	}

	// Write out to file
	for (const ColourSRGBA &Colour : Colours) {
		FileBuffer->AppendFormat("%u %u %u\t", Colour.Red, Colour.Green, Colour.Blue);
	}

    FileBuffer->Append("\n");

    PROFILER_EVENT_STOP();
}

void Run(ProgramState *RunState)
{
    PROFILER_EVENT_START("Run", ColourSRGBA::CreateRed());

	String RenderOutputPath(RunState->MainAllocator, RunState->Arguments[1], strlen(RunState->Arguments[1]));
	if (!Platform::FileIO::DirectoryExists(RunState->MainAllocator, RenderOutputPath.ToUTF8()))
	{
		Platform::FileIO::DirectoryCreate(RunState->MainAllocator, RenderOutputPath.ToUTF8());
	}

	// Create timestamped file
	String TimeStamp = String::FromSystemTime(RunState->MainAllocator, Platform::Time::GetSystemTime(), true);

	RenderOutputPath.AppendAsPath(&TimeStamp);
	RenderOutputPath.Append(".ppm");
	CORE_LOG(RunState->MainAllocator, "Rendering to %s", RenderOutputPath.ToUTF8());

    Platform::FileIO::File TestFile;
	if (Platform::FileIO::Exists(RunState->MainAllocator, RenderOutputPath.ToUTF8())) {
		Platform::FileIO::Remove(RunState->MainAllocator, RenderOutputPath.ToUTF8());
    }

	if (!Platform::FileIO::CreateToWrite(RunState->MainAllocator, RenderOutputPath.ToUTF8(), &TestFile)) {
        return;
    }

	// Image dimensions
    const f32 ImageAspectRatioXUnits = String::ToF32(RunState->Arguments[2]);
    const f32 ImageAspectRatioYUnits = String::ToF32(RunState->Arguments[3]);
	const f32 ImageAspectRatio = ImageAspectRatioXUnits / ImageAspectRatioYUnits;
	const u32 ImageWidth = String::ToU64(RunState->Arguments[4]);
	const u32 ImageHeight = ImageWidth / ImageAspectRatio;
    const usize BufferSize = ((4 * 3 + 2) * (ImageWidth * ImageHeight)) + 16; // Include PPM header
  
	// Create file header
	String FileBuffer(RunState->MainAllocator, BufferSize);
	FileBuffer.Clear();
	FileBuffer.Append("P3\n");
	FileBuffer.AppendFormat("%d %d\n", ImageWidth, ImageHeight);
	FileBuffer.Append("255\n");

	// Create camera
	RenderCamera PrimaryCamera;
	PrimaryCamera.Position = Vector3(0.0f, 0.0f, 0.0f);
	PrimaryCamera.ViewPortHeight = 2.0f;
	PrimaryCamera.ViewPortWidth = ImageAspectRatio * PrimaryCamera.ViewPortHeight;
	PrimaryCamera.FocalLength = 1.0f;

	// Render loop
	RenderImage(RunState, &FileBuffer, &PrimaryCamera, ImageWidth, ImageHeight);

    Platform::FileIO::Write(RunState->MainAllocator, &TestFile, reinterpret_cast<u8 *>(FileBuffer.ToUTF8()), FileBuffer.SizeInBytes());
    Platform::FileIO::Close(&TestFile);

    PROFILER_EVENT_STOP();
}

bool Update(ProgramState *RunState, Platform::Process::Window *MainWindow)
{
	while (MainWindow->State == Platform::Process::WindowState::Active)
	{
		Platform::Process::WindowUpdate(MainWindow);

		bool bOutOfInputs = false;
		do
		{
			Platform::Process::InputEvent* Event = Platform::Process::GetNextInputEvent();
			if (Event != nullptr) 
			{
				CORE_LOG(RunState->MainAllocator, "%llu", Event->ID);
			}
			
			bOutOfInputs = true;
		} while (!bOutOfInputs);

		if (MainWindow->State == Platform::Process::WindowState::Closed)
		{
			return false;
		}
	}

	return true;
}


#if defined(CORE_PLATFORM_WINDOWS)
int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCommand)
#else
int main(int argc, char *argv[])
#endif
{
    PROFILER_THREAD_ID("Main");

	ProgramState RunState = {};
	AllocatorMiMalloc GlobalAllocator;
	RunState.MainAllocator = &GlobalAllocator;


	const Platform::Process::CommandLineParameters ArgParameters = Platform::Process::GetCommandLineParameters();
	RunState.Arguments = (char**) RunState.MainAllocator->Allocate(sizeof(char) * ArgParameters.Size);
	Platform::Process::GetCommandLineArguments(RunState.MainAllocator, ArgParameters, RunState.Arguments);

	Rect DefaultRect = { 100, 100, 1920, 1080 };
	Platform::Process::Window *MainWindow = Platform::Process::WindowCreate(RunState.MainAllocator, &DefaultRect, "Raytracer");
	if (MainWindow == nullptr)
	{
		return -1;
	}

	if (!Update(&RunState, MainWindow))
	{
		Platform::Process::WindowDestroy(MainWindow);
	}

	Run(&RunState);

	RunState.MainAllocator->Free(RunState.Arguments, ArgParameters.Size);



    return 0;
}
