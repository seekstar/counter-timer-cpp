#ifndef COUNTER_TIMER_H_
#define COUNTER_TIMER_H_

#include <rusty/time.h>

#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>

namespace counter_timer {

struct CountTime {
	uint64_t count;
	rusty::time::Duration time;
	CountTime operator+(const CountTime &rhs) const {
		return CountTime{
			.count = count + rhs.count,
			.time = time + rhs.time,
		};
	}
};

class TimerGuard;
class Timer {
public:
	Timer() : count_(0), nsec_(0) {}
	uint64_t count() const { return count_.load(); }
	rusty::time::Duration time() const {
		return rusty::time::Duration::from_nanos(nsec_.load());
	}
	void add(rusty::time::Duration time) const {
		count_.fetch_add(1, std::memory_order_relaxed);
		nsec_.fetch_add(time.as_nanos(), std::memory_order_relaxed);
	}
	TimerGuard start() const;
	CountTime status_nonatomic() const {
		return CountTime{
			.count = count(),
			.time = time(),
		};
	}
private:
	mutable std::atomic<uint64_t> count_;
	mutable std::atomic<uint64_t> nsec_;
};

class TimerGuard {
public:
	~TimerGuard() {
		if (timer_ == nullptr) return;
		timer_->add(rusty::time::Instant::now() - start_time_);
	}
private:
	friend class Timer;
	TimerGuard(
		const Timer &timer
	) : timer_(&timer), start_time_(rusty::time::Instant::now()) {}
	const Timer *timer_;
	rusty::time::Instant start_time_;
};

inline TimerGuard Timer::start() const { return TimerGuard(*this); }

class Timers {
public:
	Timers(size_t num) : timers_(num) {}
	size_t len() const { return timers_.size(); }
	const Timer &timer(size_t type) const { return timers_[type]; }
private:
	std::vector<Timer> timers_;
};

template <typename Type>
class TypedTimers {
public:
	TypedTimers(size_t num) : timers_(num) {}
	const Timers &timers() const { return timers_; }
	const Timer &timer(Type type) const {
		return timers_.timer(static_cast<size_t>(type));
	}
private:
	Timers timers_;
};

} // namespace counter_timer

#endif // COUNTER_TIMER_H_
