#pragma once

#include <mutex>
#include <deque>
#include "macros.hpp"
#include "hakupch.hpp"
#include "HakuLog.hpp"

// we will not be supporting iterators for the moment

namespace Haku
{
namespace Utils
{
template<typename T>
class Hk_Dequeue_mt
{
	using value_type = T;
	using ref		 = value_type&;
	using const_ref	 = const ref;
	using ptr		 = value_type*;
	using const_ptr	 = const ptr;

public:
	Hk_Dequeue_mt() = default;
	DISABLE_COPY(Hk_Dequeue_mt)
	DISABLE_MOVE(Hk_Dequeue_mt)

	ref at(size_t pos)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_Deque.at(pos);
	}
	const_ref at(size_t pos) const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_Deque.at(pos);
	}
	ref operator[](size_t pos)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_Deque[pos];
	}
	//const_ref operator[](size_t pos) const
	//{
	//	std::lock_guard<std::mutex> lock(m_mutex);
	//	return m_Deque[pos];
	//}
	ref front()
	{
	std::lock_guard<std::mutex> lock(m_mutex);
		return m_Deque.front();
	}
	ref back()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_Deque.back();
	}
	bool   empty() const { return m_Deque.empty(); }
	size_t size()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_Deque.size();
	}
	void clear()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_Deque.clear();
	}
	void push_back(const value_type& ref)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_Deque.push_back(ref);
	}
	void push_back(value_type&& r_ref)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_Deque.push_back(r_ref);
	}
	void pop_back()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_Deque.pop_back();
	}
	void push_front(const value_type& ref)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_Deque.push_front(ref);
	}
	void pop_front()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_Deque.pop_front();
	}

private:
	std::mutex			   m_mutex;
	std::deque<value_type> m_Deque;
};

} // namespace Utils
} // namespace Haku
