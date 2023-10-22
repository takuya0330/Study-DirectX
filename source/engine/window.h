#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>

namespace dxlib {
namespace win32 {

//! \brief デフォルトのウィンドウ横幅
inline constexpr UINT32 default_window_width  = 1280;

//! \brief デフォルトのウィンドウ立幅
inline constexpr UINT32 default_window_height = 720;

//! \brief ウィンドウクラスの登録
//! 
//! \param[in] window_class_name
//! \param[in] window_procedure
//! 
//! \ret WNDCLASSEX
WNDCLASSEX register_window_class_ex(
    const wchar_t* window_class_name,
    WNDPROC        window_procedure);

//! \brief ウィンドウの作成
//!
//! \param[in] window_class_name
//! \param[in] window_title
//! \param[in] window_style
//! \param[in] window_width
//! \param[in] window_height
//! 
//! \ret HWND
HWND create_window(
    const wchar_t* window_class_name,
    const wchar_t* window_title,
    const DWORD    window_style,
    const LONG     window_width,
    const LONG     window_height);

//! \brief ウィンドウの破棄
//!
//! \param[in] wcex
//! \param[in] hwnd
void destroy_window(
    const WNDCLASSEX& wcex,
    const HWND        hwnd);

//! \brief ウィンドウサイズの取得
//!
//! \param[in] hwnd
//! \param[out] width
//! \param[out] height
void get_window_size(
    HWND    hwnd,
    UINT32& width,
    UINT32& height);

} // namespace win32
} // namespace dxlib
