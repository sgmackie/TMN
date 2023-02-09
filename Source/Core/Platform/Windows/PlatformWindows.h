#if defined(CORE_PLATFORM_WINDOWS)

#pragma once

#include "CoreTypes.h"
#include "Platform.h"

// Includes
// TODO: Look at how to reduce windows header file include size
#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#define UNICODE
#include <windows.h>
#include <shellapi.h>

// Libs
#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Gdi32.lib")

namespace Core {
namespace Platform {
    namespace VirtualMemory {
        DWORD GetAllocFlags(const Flags Flags);
    }

	namespace Process {
		enum class WindowState
		{
			Active,
			Closed
		};

		struct Window
		{
			WindowState State = WindowState::Closed;
			HWND Handle = 0;
			RECT Rect;
			DWORD StyleFlags = 0;
			DWORD WindowFlags = 0;
		};

		enum class InputEventState
		{
			Handled,
			Prepared
		};

		enum class InputEventType
		{
			Mouse,
			Keyboard
		};


		struct InputEvent
		{
			InputEventState State = InputEventState::Handled;
			InputEventType Type = InputEventType::Mouse;
			u64 ID = 0;

			union Data
			{
				f32 XY[2];
			} Data;
		};

		#define WINDOWS_INPUT_BUFFER_SIZE 2048
		static LRESULT CALLBACK MainWindowCallback(HWND Handle, UINT Message, WPARAM WParam, LPARAM LParam);
		bool RegisterWindowClass(Core::Allocator *Allocator, WNDCLASS& OutClass);
		static bool GetNextInputEventIndex(usize &OutIndex);


		#define CORE_PLATFORM_MAX_WINDOWS 4
		static Window WindowList[CORE_PLATFORM_MAX_WINDOWS];
		
		#define CORE_PLATFORM_MAX_INPUT_EVENTS 64
		static InputEvent InputEventList[CORE_PLATFORM_MAX_INPUT_EVENTS];


		// https://learn.microsoft.com/en-us/windows-hardware/drivers/hid/hid-architecture#hid-clients-supported-in-windows
		static const i32 HID_CLIENT_PAGE = 0x01;
		static const i32 HID_CLIENT_MOUSE = 0x02;
		static const i32 HID_CLIENT_KEYBOARD = 0x06;
		static HWND RawInputDevice = nullptr; // Only one instance per application
	}
}
}

#endif
