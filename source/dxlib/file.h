#pragma once

#include <memory>

namespace dxlib {

bool load_file(const wchar_t* filename, std::unique_ptr<uint8_t[]>& filedata, uint32_t& filesize);

}