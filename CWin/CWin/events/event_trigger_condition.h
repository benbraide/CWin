#pragma once

#include <functional>

namespace cwin::events{
	class trigger_condition{
	public:
		using m_callback_type = std::function<bool(std::size_t)>;

		virtual ~trigger_condition();

		virtual operator m_callback_type()const;

		virtual m_callback_type get() const;
	};

	class external_trigger_condition : public trigger_condition{
	public:
		explicit external_trigger_condition(const m_callback_type &value);

		virtual ~external_trigger_condition();

		virtual m_callback_type get() const override;

	protected:
		m_callback_type value_;
	};

	class odd_count_trigger_condition : public trigger_condition{
	public:
		virtual ~odd_count_trigger_condition();

		virtual m_callback_type get() const override;
	};

	class even_count_trigger_condition : public trigger_condition{
	public:
		virtual ~even_count_trigger_condition();

		virtual m_callback_type get() const override;
	};

	class max_count_trigger_condition : public trigger_condition{
	public:
		explicit max_count_trigger_condition(std::size_t value);

		virtual ~max_count_trigger_condition();

		virtual m_callback_type get() const override;

	protected:
		std::size_t value_;
	};
}
