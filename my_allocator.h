#pragma once

#include "memory_map.h"

/**
	���������� ���������� �� ������ ��������� ������ g_memory_map
*/
template<typename T, size_t size>
struct MyAllocator {

	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;

	template<typename U>
	struct rebind {
		using other = MyAllocator<U, size>;
	};

	MyAllocator() = default;
	~MyAllocator() = default;

	template<typename U, size_t other_size>
	MyAllocator(const MyAllocator<U, other_size>&) {}

	/**
		��������� ������ ��� n �������� ���� T
		����������� ��������� ������ ���������� ����� ���������� ������ � ��������� ������������
	*/
	T* allocate(std::size_t n)
	{
		T* return_p = nullptr;

		for (const auto& pair : g_memory_map) // ���� �� ����� � ������������ ������ ������
		{
			auto block_info = pair.second;

			if (n * sizeof(T) > block_info.m_space - block_info.m_size) // ������������ �����
				continue;

			return_p = reinterpret_cast<T*>(pair.first + block_info.m_size);

			block_info.m_size += n * sizeof(T);
			++block_info.m_count;

			g_memory_map[pair.first] = block_info;

			break;
		}

		if (return_p == nullptr) // ����� ��� - ������� ����� ����
		{
			return_p = add_alloc_block(n);
		}

		return return_p;
	}

	/**
		������������ ������ �� ������ p
		����������� ������������ ������ ���������� ����� ������ ��������� ������ � �����
	*/
	void deallocate(T* p, std::size_t)
	{
		for (const auto& pair : g_memory_map)
		{
			auto block_info = pair.second;

			// ������ ��������� � ����� �� ������ ������?

			if (reinterpret_cast<char*>(p) >= pair.first &&
				reinterpret_cast<char*>(p) < (pair.first + block_info.m_space))
			{
				--block_info.m_count;

				if (block_info.m_count == 0) // ��������� ������ - ����������� ������
				{
					free(pair.first);
					g_memory_map.erase(pair.first);
				}
				else
				{
					g_memory_map[pair.first] = block_info;
				}

				break;
			}
		}
	}

	/**
		��������������� ������ ���� U �� ������ p � ������� ������������ � ����������� Args
	*/
	template<typename U, typename ...Args>
	void construct(U* p, Args &&...args) {

		new(p) U(std::forward<Args>(args)...);
	}

	/**
		����������� ������ ���� T �� ������ p
	*/
	void destroy(T* p) {

		p->~T();
	}

private:

	/**
		���������� ������ ����� ������ � �������� ������
		��������� ������ � ������� malloc
		n - ���������� �������� T
	*/
	T* add_alloc_block(size_t n)
	{
		size_t space_bytes = size * sizeof(T);
		size_t size_bytes = n * sizeof(T);

		if (size_bytes > space_bytes) space_bytes = size_bytes;

		void* p = malloc(space_bytes);
		if (!p)	throw std::bad_alloc();

		g_memory_map[static_cast<char*>(p)] = { space_bytes, size_bytes, 1 };

		return static_cast<T*>(p);
	}
};

