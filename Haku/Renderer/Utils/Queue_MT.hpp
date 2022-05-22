#pragma once
#include <mutex>
#include "macros.hpp"
// this is questionable include as if this is used outside the context of haku dll this will break dependencies
#include "HakuLog.hpp"
#include "hakupch.hpp"
/*Queue_MT*/
// This is intended as multi threading synchronizing queue that is at its infancy

// TODO : finish this

namespace Haku
{
namespace Utils
{
template<typename T>
class HK_Queue_mt
{
	using value_type = T;

public:
	HK_Queue_mt() = default;
	HK_Queue_mt(HK_Queue_mt&& r_val) noexcept { r_val.m_Queue = std::move(m_Queue); }
	HK_Queue_mt(const HK_Queue_mt& ref) noexcept { ref.m_Queue = m_Queue; }

	void push(const value_type& val)
	{
		std::lock_guard<std::mutex> guard(m_mutex);
		m_Queue.push(val);
	}

	void push(const value_type&& val)
	{
		std::lock_guard<std::mutex> guard(m_mutex);
		m_Queue.push(std::forward<value_type>(val));
	}

	value_type& front()
	{
		std::lock_guard<std::mutex> guard(m_mutex);
		return m_Queue.front();
	}

	void pop()
	{
		std::lock_guard<std::mutex> guard(m_mutex);
		m_Queue.pop();
	}

	size_t size()
	{
		std::lock_guard<std::mutex> guard(m_mutex);
		return m_Queue.size();
	}

	bool empty()
	{
		std::lock_guard<std::mutex> guard(m_mutex);
		return m_Queue.empty();
	}

private:
	std::queue<value_type> m_Queue;
	std::mutex			   m_mutex;
};

} // namespace Utils
} // namespace Haku
