#pragma once

#include "non_window_object.h"

namespace cwin::non_window{
	class lines_path : public object{
	public:
		using object::object;
		/*template <typename point_type>
		explicit lines_path(const std::vector<point_type> &points){
			insert_object<hook::non_window::lines_path_handle<hook::non_window::client_handle>>(nullptr, points);
		}

		template <typename point_type>
		lines_path(tree &parent, const std::vector<point_type> &points)
			: lines_path(parent, static_cast<std::size_t>(-1), points){}

		template <typename point_type>
		lines_path(tree &parent, std::size_t index, const std::vector<point_type> &points)
			: lines_path(points){
			index_ = parent.resolve_child_index<object>(index);
			if (&parent.get_thread() == &thread_)
				set_parent_(parent);
			else//Error
				throw thread::exception::context_mismatch();
		}*/

		virtual ~lines_path() = default;
	};
}
