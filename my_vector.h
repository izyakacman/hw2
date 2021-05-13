#pragma once

/**
	Реализация контейнера типа "std::vector"
*/
template<typename T, typename Alloc = allocator<T> >
class MyVector
{
public:

	MyVector() = default;

	~MyVector()
	{
		for (size_t i = 0; i < m_size; ++i)
			m_allocator.destroy(&m_elem[i]);

		m_allocator.deallocate(m_elem, m_space);
	}

	void push_back(const T& value)
	{
		// Выделим память если она закончилась или не была выделена

		if (m_size == m_space)
		{
			m_space = (m_space) ? m_space * 2 : 10;
			T* tmp = m_allocator.allocate(m_space);

			if (m_elem) // если емлементы есть, необходимо перенести их в новую память
			{
				memcpy(tmp, m_elem, sizeof(T) * m_size);
				m_allocator.deallocate(m_elem, m_size);
			}

			m_elem = tmp;
		}

		m_allocator.construct(&m_elem[m_size++], value);
	}

	T* begin() { return m_elem; }

	T* end() { return m_elem + m_size; }

	T& operator[](size_t n) { return m_elem[n]; }

	size_t size() { return m_size; }

	bool empty() { return m_size == 0; }

private:

	Alloc m_allocator;
	T* m_elem = nullptr;
	size_t m_size = 0;
	size_t m_space = 0;
};