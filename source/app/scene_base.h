#pragma once

namespace app {

class scene_base
{
public:
	virtual ~scene_base() = default;

	virtual bool initialize() = 0;

	virtual void update() = 0;
};

} // namespace app
