#include "window.h"

#include "debug.h"

namespace dxlib {
namespace win32 {

WNDCLASSEX register_window_class_ex(
    const wchar_t* window_class_name,
    WNDPROC        window_procedure)
{
	const WNDCLASSEX wcex = {
		.cbSize        = sizeof(WNDCLASSEX),
		.style         = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc   = window_procedure,
		.cbClsExtra    = 0,
		.cbWndExtra    = 0,
		.hInstance     = GetModuleHandle(nullptr),
		.hIcon         = LoadIcon(nullptr, IDI_APPLICATION),
		.hCursor       = LoadCursor(nullptr, IDC_ARROW),
		.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)),
		.lpszMenuName  = nullptr,
		.lpszClassName = window_class_name,
		.hIconSm       = nullptr
	};
	ASSERT_RETURN(RegisterClassEx(&wcex), WNDCLASSEX());

	return wcex;
}

HWND create_window(
    const wchar_t* window_class_name,
    const wchar_t* window_title,
    const DWORD    window_style,
    const LONG     window_width,
    const LONG     window_height)
{
	RECT rect = { 0, 0, window_width, window_height };
	::AdjustWindowRect(&rect, window_style, false);

	auto hwnd = CreateWindowEx(
	    WS_EX_ACCEPTFILES,
	    window_class_name,
	    window_title,
	    window_style,
	    CW_USEDEFAULT,
	    CW_USEDEFAULT,
	    rect.right - rect.left,
	    rect.bottom - rect.top,
	    nullptr,
	    nullptr,
	    GetModuleHandle(nullptr),
	    nullptr);
	ASSERT_RETURN(hwnd, nullptr);

	return hwnd;
}

void destroy_window(
    const WNDCLASSEX& wcex,
    const HWND        hwnd)
{
	DestroyWindow(hwnd);
	UnregisterClassW(wcex.lpszClassName, wcex.hInstance);
}

void get_window_size(
    HWND    hwnd,
    UINT32& width,
    UINT32& height)
{
	ASSERT(hwnd);

	RECT rect = {};
	GetClientRect(hwnd, &rect);
	width  = static_cast<UINT32>(rect.right - rect.left);
	height = static_cast<UINT32>(rect.bottom - rect.top);
}

} // namespace win32
} // namespace dxlib
