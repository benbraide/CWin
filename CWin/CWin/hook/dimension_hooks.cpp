#include "../thread/thread_object.h"

#include "dimension_hooks.h"

void cwin::hook::animated_dimension_helper::animate(object &target, const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float, bool)> &callback){
	target.get_thread().animate(timing, duration, callback, target.get_talk_id());
}
