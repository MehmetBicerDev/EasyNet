#pragma once
#include "includes.h"











namespace easynet
{
	namespace util
	{
		class c_timer {
		public:
			c_timer() { start = (DWORD64)GetTickCount64(); }


			DWORD64 time_elapsed() { return GetTickCount64() - start; }

			void reset() { start = (DWORD64)GetTickCount64(); }

			DWORD64 start_time() { return start; }

		private:
			DWORD64 start = 0;
		};
	}
}