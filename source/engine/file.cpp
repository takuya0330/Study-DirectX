#include "file.h"

#include <fstream>

#include "debug.h"

namespace dxlib {

bool load_file(const wchar_t* filename, std::unique_ptr<uint8_t[]>& file_data, uint32_t& file_size)
{
	std::ifstream ifs;
	ifs.open(filename, std::ifstream::in | std::ifstream::binary);
	if (ifs.fail()) 
	{
		ifs.close();
		_LOG_ERROR_MSG(L"%s open failed.\n", filename);
		return false;
	}

	ifs.seekg(0, std::ios_base::end);
	auto cso_size = static_cast<uint32_t>(ifs.tellg());
	ifs.seekg(0, std::ios_base::beg);

	auto cso_data = std::make_unique<uint8_t[]>(cso_size);
	ifs.read(reinterpret_cast<char*>(cso_data.get()), cso_size);

	file_data = std::move(cso_data);
	file_size = cso_size;

	return true;
}

} // namespace dxlib
