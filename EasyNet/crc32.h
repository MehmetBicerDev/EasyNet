#pragma once
#include <Windows.h>
#include <iostream>
#include "c_buffer.h"








namespace easynet {
	namespace util
	{
		std::uint32_t ComputeHash(data::c_buffer Message);
		std::uint32_t ComputeHash(std::string Message);
	}
}