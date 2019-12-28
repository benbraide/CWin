#pragma once

#include "../thread/thread_object.h"

#define WINP_CLASS_UUID		 "{DABED3E8-D8A5-48FC-B80B-B17C167FA9B0}"
#define WINP_CLASS_WUUID	L"{DABED3E8-D8A5-48FC-B80B-B17C167FA9B0}"

namespace cwin::app{
	class object{
	public:
		static thread_local thread::object thread;
	private:
		friend class thread::object;

		static std::unordered_map<DWORD, thread::object *> threads_;
		static std::mutex lock_;
	};
}
