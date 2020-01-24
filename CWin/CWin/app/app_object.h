#pragma once

#include "../thread/thread_object.h"

#include "app_exception.h"

#define WINP_CLASS_UUID		 "{DABED3E8-D8A5-48FC-B80B-B17C167FA9B0}"
#define WINP_CLASS_WUUID	L"{DABED3E8-D8A5-48FC-B80B-B17C167FA9B0}"

namespace cwin::app{
	class object{
	public:
		static void auto_init();

		static void init();

		static void uninit();

		static thread::object &get_thread();

	private:
		friend class thread::object;

		class auto_initializer{
		public:
			auto_initializer();

			~auto_initializer();
		};

		static DWORD thread_id_;
		static ATOM class_id_;
		static std::atomic_bool auto_init_;

		static thread_local auto_initializer auto_initializer_;
		static thread_local std::shared_ptr<thread::object> thread_;

		static std::unordered_map<DWORD, thread::object *> threads_;
		static std::mutex lock_;
	};
}
