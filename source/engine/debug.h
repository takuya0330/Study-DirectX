#pragma once

#include <stdarg.h>
#include <stdio.h>

#include <cassert>

namespace dxlib {
namespace develop {
namespace detail {

inline constexpr int default_console_buffer_size = 256;

#if defined(_DEBUG)
#if defined(_CONSOLE)
inline void console(const char* msg)
{
	printf("%s", msg);
}

inline void console(const wchar_t* msg)
{
	wprintf(L"%ws", msg);
}
#else
inline void console(const char* msg)
{
	OutputDebugStringA(msg);
}

inline void console(const wchar_t* msg)
{
	OutputDebugString(msg);
}
#endif
#endif

} // namespace detail

//! \brief コンソールに出力します
//! 
//! \param[in] format
//! \param[in] ...
inline void log(const char* format, ...)
{
#if defined(_DEBUG)
	char    buffer[detail::default_console_buffer_size];
	va_list ap;
	va_start(ap, format);
	vsprintf_s(buffer, detail::default_console_buffer_size, format, ap);
	va_end(ap);
	detail::console(buffer);
#endif
}

//! \brief コンソールに出力します
//!
//! \param[in] format
//! \param[in] ...
inline void log(const wchar_t* format, ...)
{
#if defined(_DEBUG)
	wchar_t buffer[detail::default_console_buffer_size];
	va_list ap;
	va_start(ap, format);
	vswprintf(buffer, detail::default_console_buffer_size, format, ap);
	va_end(ap);
	detail::console(buffer);
#endif
}

} // namespace develop
} // namespace dxlib

#define _LOG_NEW_LINE dxlib::develop::log("\n")
#define _LOG_SUCCESS  dxlib::develop::log("[Success] ")
#define _LOG_WARNING  dxlib::develop::log("[Warning] ")
#define _LOG_ERROR    dxlib::develop::log("[Error] ")
#define _LOG_FILE     dxlib::develop::log("- file: %s", __FILE__);
#define _LOG_FUNC     dxlib::develop::log("- func: %s", __FUNCTION__);
#define _LOG_LINE     dxlib::develop::log("- line: %d", __LINE__);

#define _LOG_SUCCESS_MSG(...)             \
	{                                     \
		_LOG_SUCCESS;                     \
		dxlib::develop::log(__VA_ARGS__); \
	}

#define _LOG_WARNING_MSG(...)             \
	{                                     \
		_LOG_WARNING;                     \
		dxlib::develop::log(__VA_ARGS__); \
		_LOG_FILE;                        \
		_LOG_NEW_LINE;                    \
		_LOG_FUNC;                        \
		_LOG_NEW_LINE;                    \
		_LOG_LINE;                        \
		_LOG_NEW_LINE;                    \
	}

#define _LOG_ERROR_MSG(...)               \
	{                                     \
		_LOG_ERROR;                       \
		dxlib::develop::log(__VA_ARGS__); \
		_LOG_FILE;                        \
		_LOG_NEW_LINE;                    \
		_LOG_FUNC;                        \
		_LOG_NEW_LINE;                    \
		_LOG_LINE;                        \
		_LOG_NEW_LINE;                    \
	}

#define _LOG_ERROR_HRESULT(hr) _LOG_ERROR_MSG("hr = 0x%08X\n", hr);

#define RETURN_IF_FAILED(hr, ...) \
	if (FAILED(hr))               \
	{                             \
		_LOG_ERROR_HRESULT(hr);   \
		return __VA_ARGS__;       \
	}

#define ASSERT(expr) assert(expr)

#define ASSERT_RETURN(expr, ...) \
	if (!(expr))                 \
	{                            \
		assert(expr);            \
		return __VA_ARGS__;      \
	}

#if defined(_DEBUG)

#if defined(_CONSOLE)
#define _CRTDBG_MAP_ALLOC
#endif

#include <crtdbg.h>

#define ENABLE_CRT_DEBUG_FLAG _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)

//#define new new (_NORMAL_BLOCK, __FILE__, __LINE__)

#else

#define ENABLE_CRT_DEBUG_FLAG

#endif
