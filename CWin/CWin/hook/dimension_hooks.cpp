#include "../thread/thread_object.h"

#include "dimension_hooks.h"

cwin::hook::object *cwin::hook::dimension_pair_helper::get_existing_hook(hook::target &target, const std::type_info &type){
	if (&type == &typeid(SIZE)){
		if (auto value = target.find_first_similar_hook<size>(); value != nullptr)
			return value;
	}

	if (&type == &typeid(POINT)){
		if (auto value = target.find_first_similar_hook<position>(); value != nullptr)
			return value;
	}

	return nullptr;
}

void cwin::hook::animated_dimension_helper::animate(thread::object &thread, const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float, bool)> &callback){
	thread.animate(timing, duration, callback);
}
