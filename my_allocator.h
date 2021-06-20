#pragma once

#include "memory_pool.h"

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
		MemoryPool* ptr = &memory_pool;

		while( ptr ) // ���� �� ����� � ������������ ������ ������
		{
			if (n * sizeof(T) > ptr->space - ptr->size) // ������������ �����
			{
				ptr = ptr->next_block;
				continue;
			}

			return_p = reinterpret_cast<T*>(ptr->ptr + ptr->size);

			ptr->size += n * sizeof(T);
			++ptr->count;

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
		MemoryPool* ptr = &memory_pool;

		while( ptr )
		{
			// ������ ��������� � ����� �� ������ ������?

			if (reinterpret_cast<char*>(p) >= ptr->ptr &&
				reinterpret_cast<char*>(p) < (ptr->ptr + ptr->space))
			{
				--ptr->count;

				if (ptr->count == 0) // ��������� ������ - ����������� ������
				{
					free(ptr->ptr);
					if (ptr != &memory_pool)
						free(ptr);
				}

				break;
			}

			ptr = ptr->next_block;

		} // while(ptr)
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

		MemoryPool* ptr = &memory_pool;

		while (ptr->next_block)
		{
			ptr = ptr->next_block;
		}

		void* p = malloc(space_bytes);
		if (!p) throw std::bad_alloc();

		ptr->next_block = static_cast<MemoryPool*>(malloc(sizeof(MemoryPool)));
		if (!ptr->next_block) throw std::bad_alloc();

		ptr->next_block->ptr = static_cast<char*>(p);
		ptr->next_block->space = space_bytes;
		ptr->next_block->size = size_bytes;
		ptr->next_block->count = 1;
		ptr->next_block->next_block = nullptr;

		return static_cast<T*>(p);
	}
};

