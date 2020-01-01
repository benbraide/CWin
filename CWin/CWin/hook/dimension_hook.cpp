#include "../thread/thread_object.h"

#include "dimension_hook.h"

void cwin::hook::animated_dimension_helper::animate(thread::object &thread, const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float, bool)> &callback){
	thread.animate(timing, duration, callback);
}
