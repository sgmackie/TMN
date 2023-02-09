#if defined(CORE_PLATFORM_WINDOWS)

#include "PlatformWindows.h"

#include "../../Allocator.h"
#include "../../Logging.h"
#include "../../Unicode.h"

namespace Core {
namespace Platform {
    const char *GetLastSystemError(Core::Allocator *Allocator)
    {
        wchar_t *UTF16 = NULL;
        const DWORD ErrorCode = GetLastError();
        const DWORD StringLength = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &UTF16, 0, NULL);

		// TODO: Print Error code, remove carriage return with StringLength
        char *UFT8 = UTF16ToUTF8(Allocator, (const u16 *) UTF16);
        LocalFree(UTF16);

        return UFT8;
    }

    VirtualMemory::MemoryStats VirtualMemory::GetMemoryStats()
    {
        MemoryStats Result = {};

        MEMORYSTATUSEX MemoryStatus = {};
        MemoryStatus.dwLength = sizeof(MemoryStatus);
        GlobalMemoryStatusEx(&MemoryStatus);
        Result.TotalPhysical = MemoryStatus.ullTotalPhys;
        Result.TotalVirtual = MemoryStatus.ullTotalPageFile;

        SYSTEM_INFO SystemInfo = {};
        GetSystemInfo(&SystemInfo);
        Result.PageSize = SystemInfo.dwPageSize;
		Result.MinimumAllocationSize = SystemInfo.dwAllocationGranularity;

        return Result;
    }

    DWORD VirtualMemory::GetAllocFlags(const Flags Flags)
    {
        DWORD Result = 0;
        if (Flags == Flags::ReadWrite) {
            Result = PAGE_READWRITE;
        }
        if (Flags == Flags::Protect) {
            Result = PAGE_NOACCESS;
        }
        return Result;
    }

    void *VirtualMemory::Reserve(const usize Size, const Flags Flags)
    {
        return VirtualAlloc(0, Size, MEM_RESERVE, GetAllocFlags(Flags));
    }

    void VirtualMemory::Commit(void *Pointer, const usize Size, const Flags Flags)
    {
        VirtualAlloc(Pointer, Size, MEM_COMMIT, GetAllocFlags(Flags));
    }

    void VirtualMemory::Decommit(void *Pointer, const usize Size)
    {
        VirtualFree(Pointer, Size, MEM_DECOMMIT);
    }

    void VirtualMemory::Protect(void *Pointer, const usize Size)
    {
        BOOL Result = VirtualProtect(Pointer, Size, PAGE_NOACCESS, 0);
        CORE_ASSERT(Result);
    }

	u64 Time::GetCPUTime()
	{
		LARGE_INTEGER Counter;
		QueryPerformanceCounter(&Counter);
		return Counter.QuadPart;
	}

	Time::SystemTime Time::GetSystemTime()
	{
		SYSTEMTIME ClockTime;
		GetSystemTime(&ClockTime);

		Time::SystemTime Result;
		Result.Year = ClockTime.wYear;
		Result.Month = ClockTime.wMonth;
		Result.Day = ClockTime.wDay;
		Result.Hour = ClockTime.wHour;
		Result.Minute = ClockTime.wMinute;
		Result.Second = ClockTime.wSecond;
		Result.Milliseconds = ClockTime.wMilliseconds;
		return Result;
	}

    void Process::PrintToConsole(Core::Allocator *Allocator, const char *Buffer)
    {
        wchar_t *WideBuffer = reinterpret_cast<wchar_t *>(UTF8ToUTF16(Allocator, Buffer));
        OutputDebugStringW(WideBuffer);
    }

	Process::CommandLineParameters Process::GetCommandLineParameters()
	{
		Process::CommandLineParameters Result {};

		wchar_t *CommandLine = GetCommandLineW();
		wchar_t **WideArgv = CommandLineToArgvW(CommandLine, &Result.Count);

		Result.Size = 0;
		for (usize i = 0; i < Result.Count; ++i) {
			Result.Size += GetUTF8LengthFromUTF16((const u16 *) WideArgv[i]);
		}

		return Result;
	}

	void Process::GetCommandLineArguments(Core::Allocator *Allocator, Process::CommandLineParameters Parameters, char** ArgBuffer)
	{
		wchar_t *CommandLine = GetCommandLineW();
		wchar_t **WideArgv = CommandLineToArgvW(CommandLine, &Parameters.Count);

		for (usize i = 0; i < Parameters.Count; ++i) {
			ArgBuffer[i] = UTF16ToUTF8(Allocator, (const u16 *) WideArgv[i]);
		}
	}

	static bool Process::GetNextInputEventIndex(usize &OutIndex) 
	{
		for (usize i = 0; i < CORE_PLATFORM_MAX_INPUT_EVENTS; ++i)
		{
			InputEvent &Event = InputEventList[i];
			if (Event.State != InputEventState::Prepared && Event.ID == 0)
			{
				OutIndex = i;
				return true;
			}
		}

		return false;
	}

	static LRESULT CALLBACK Process::MainWindowCallback(HWND Handle, UINT Message, WPARAM WParam, LPARAM LParam)
	{
		Process::Window *Window = (Process::Window *)(GetWindowLongPtr(Handle, GWLP_USERDATA));
		if (Window == nullptr)
		{
			return DefWindowProc(Handle, Message, WParam, LParam);
		}

		switch (Message)
		{
			case WM_ACTIVATE:
			{
				Window->State = Process::WindowState::Active;
				return DefWindowProc(Handle, Message, WParam, LParam);
			}
			case WM_CLOSE:
			case WM_DESTROY:
			{
				Window->State = Process::WindowState::Closed;
				break;
			}
			case WM_SYSCOMMAND:
			{
				if (WParam == SC_CLOSE) 
				{
					Window->State = Process::WindowState::Closed;
					return 0;
				}
				break;
			}
			case WM_SYSKEYUP:
			{
				return 0;
			}
			case WM_SYSCHAR:
			{
				u32 ScanCode = (LParam >> 16) & 0x7f;
				u32 VKey = MapVirtualKey(ScanCode, MAPVK_VSC_TO_VK);
				if (VKey >= '0' && VKey <= '9')
				{
					return 0;
				}
				if (VKey >= 'A' && VKey <= 'Z')
				{
					return 0;
				}
			}
			case WM_INPUT:
			{
				HRAWINPUT RawInput = (HRAWINPUT) LParam;
				UINT Size;
				GetRawInputData(RawInput, RID_INPUT, NULL, &Size, sizeof(RAWINPUTHEADER));

				u8 Buffer[WINDOWS_INPUT_BUFFER_SIZE];
				UINT ActualSize = GetRawInputData(RawInput, RID_INPUT, Buffer, &Size, sizeof(RAWINPUTHEADER));
				CORE_ASSERT(ActualSize == Size);

				RAWINPUT *InputData = (RAWINPUT *) Buffer;

				InputEvent Event = {};
				if (InputData->header.dwType == RIM_TYPEMOUSE)
				{

				}
				else if (InputData->header.dwType == RIM_TYPEKEYBOARD)
				{
					RAWKEYBOARD *Keyboard = &InputData->data.keyboard;

					usize FreeEventSlot;
					if (GetNextInputEventIndex(FreeEventSlot))
					{
						InputEvent Event = {};
						Event.ID = Keyboard->VKey;
						Event.State = InputEventState::Prepared;
						InputEventList[FreeEventSlot] = Event;
					}
				}

				break;
			}
			case WM_WINDOWPOSCHANGED: 
			{
		
				break;
			}
			case WM_PAINT: 
			{
				HDC DeviceContext;
				PAINTSTRUCT PaintData;
				DeviceContext = BeginPaint(Window->Handle, &PaintData);

				HBRUSH Brush = (HBRUSH)(GetStockObject(BLACK_BRUSH));
				RECT PaintArea;
				GetClientRect(Window->Handle, &PaintArea);
				FillRect(DeviceContext, &PaintArea, Brush);
				EndPaint(Window->Handle, &PaintData);
				break;
			}
		}

		return DefWindowProc(Handle, Message, WParam, LParam);
	}

	bool Process::RegisterWindowClass(Core::Allocator *Allocator, WNDCLASS& OutClass)
	{
		OutClass = {};
		OutClass.hInstance = GetModuleHandle(NULL);
		OutClass.lpszClassName = TEXT("TMNE");
		OutClass.lpfnWndProc = MainWindowCallback;
		OutClass.style = CS_DBLCLKS;
		OutClass.hCursor = LoadCursor(OutClass.hInstance, IDC_ARROW);
		OutClass.hIcon = LoadIcon(OutClass.hInstance, IDI_APPLICATION);
		OutClass.hbrBackground = 0;

		if (!RegisterClass(&OutClass))
		{
			CORE_LOG(Allocator, "%s\n", Platform::GetLastSystemError(Allocator));
			return false;
		}

		return true;
	}

	Process::Window* Process::WindowCreate(Core::Allocator *Allocator, Rect* Rect, const char* Title)
	{
		// Get first free window in the list
		Process::Window *Result;
		for (usize i = 0; i < CORE_PLATFORM_MAX_WINDOWS; ++i)
		{
			if (WindowList[i].Handle == 0)
			{
				Result = &WindowList[i];
				break;
			}
		}

		WNDCLASS WindowClass = {};
		if (!RegisterWindowClass(Allocator, WindowClass))
		{
			return nullptr;
		}

		const f32 WindowWidth = Rect->X + Rect->W;
		const f32 WindowHeight = Rect->Y + Rect->H;
		SetRect(&Result->Rect, (i32) Rect->X, (i32) Rect->Y, (i32) WindowWidth, (i32) WindowHeight);

		Result->StyleFlags = WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
		Result->WindowFlags = WS_EX_ACCEPTFILES;

		wchar_t *WideTitle = reinterpret_cast<wchar_t *>(UTF8ToUTF16(Allocator, Title));
		const i32 X = Result->Rect.left;
		const i32 Y = Result->Rect.top;
		const i32 Width = Result->Rect.right - Result->Rect.left;
		const i32 Height = Result->Rect.top - Result->Rect.bottom;
		Result->Handle = CreateWindowExW(Result->WindowFlags, WindowClass.lpszClassName, WideTitle, Result->StyleFlags, X, Y, Width, Height, NULL, NULL, WindowClass.hInstance, NULL);

		if (Result->Handle == 0)
		{
			CORE_LOG(Allocator, "%s\n", Platform::GetLastSystemError(Allocator));
			return nullptr;
		}

		SetWindowLongPtr(Result->Handle, GWLP_USERDATA, (LONG_PTR) Result);

		RECT ActualRect;
		GetWindowRect(Result->Handle, &ActualRect);

		SetFocus(Result->Handle);
		ShowWindow(Result->Handle, SW_SHOW);
		SetActiveWindow(Result->Handle);
		UpdateWindow(Result->Handle);

		if (RawInputDevice == nullptr) 
		{
			RawInputDevice = Result->Handle;
			RAWINPUTDEVICE InputDevices[2];
			
			// Mouse
			InputDevices[0].usUsagePage = (USHORT) HID_CLIENT_PAGE;
			InputDevices[0].usUsage = (USHORT) HID_CLIENT_MOUSE;
			InputDevices[0].dwFlags = RIDEV_INPUTSINK;
			InputDevices[0].hwndTarget = Result->Handle;

			// Keyboard
			InputDevices[1].usUsagePage = (USHORT) HID_CLIENT_PAGE;
			InputDevices[1].usUsage = (USHORT) HID_CLIENT_KEYBOARD;
			InputDevices[1].hwndTarget = Result->Handle;

			if (!RegisterRawInputDevices(InputDevices, 2, sizeof(InputDevices[0])))
			{
				CORE_LOG(Allocator, "%s\n", Platform::GetLastSystemError(Allocator));
				return nullptr;
			}
		}
		Result->State = WindowState::Active;
		return Result;
	}

	void Process::WindowDestroy(Process::Window* Window)
	{
		SetWindowLongPtr(Window->Handle, GWLP_USERDATA, 0);
		DestroyWindow(Window->Handle);
		
		for (usize i = 0; i < CORE_PLATFORM_MAX_WINDOWS; ++i)
		{
			if (WindowList[i].Handle == Window->Handle)
			{
				if (RawInputDevice == Window->Handle)
				{
					RAWINPUTDEVICE InputDevices[2];
			
					// Mouse
					InputDevices[0].usUsagePage = (USHORT) HID_CLIENT_PAGE;
					InputDevices[0].usUsage = (USHORT) HID_CLIENT_MOUSE;
					InputDevices[0].dwFlags = RIDEV_REMOVE;

					// Keyboard
					InputDevices[1].usUsagePage = (USHORT) HID_CLIENT_PAGE;
					InputDevices[1].usUsage = (USHORT) HID_CLIENT_KEYBOARD;
					InputDevices[1].dwFlags = RIDEV_REMOVE;

					RegisterRawInputDevices(InputDevices, 2, sizeof(InputDevices[0]));
				}

				WindowList[i].Handle = 0;
				return;
			}
		}

		Window->Handle = 0;
		Window->State = WindowState::Closed;
	}

	void Process::WindowUpdate(Process::Window* Window)
	{
		MSG Message;
		while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE) != 0) 
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}

	Process::InputEvent* Process::GetNextInputEvent()
	{
		for (usize i = 0; i < CORE_PLATFORM_MAX_INPUT_EVENTS; ++i)
		{
			if (InputEventList[i].State == InputEventState::Prepared)
			{
				return &InputEventList[i];
			}
		}
		
		return nullptr;
	}

	void Process::ClearInputEvent(Process::InputEvent* Event)
	{
		Event->State = InputEventState::Handled;
		Event->ID = 0;
	}
}
}

#endif
