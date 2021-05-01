#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <map>

using namespace std;

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
		if (m_size == m_space) // закончилась память
		{
			m_space = (m_space) ? m_space * 2 : 10;
			T* tmp = m_allocator.allocate(m_space);

			if (m_elem)
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

/**
	Блок памяти
*/
struct MemoryBlock
{
	size_t m_space; // размер блока в байтах
	size_t m_size; // размер занятого пространства блока в байтах
	size_t m_count; // количество объектов в блоке
};

/**
	Менеджер памяти
*/
map<char*, MemoryBlock> g_memory_map;

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

	T* allocate(std::size_t n) {

		T* return_p = nullptr;

		for (const auto& pair : g_memory_map) // есть ли место в существующих блоках памяти
		{
			auto block_info = pair.second;

			if (n * sizeof(T) > block_info.m_space - block_info.m_size) // недостаточно места
				continue;

			return_p = (T*)(pair.first + block_info.m_size);

			block_info.m_size += n * sizeof(T);
			++block_info.m_count;

			g_memory_map[pair.first] = block_info;

			break;
		}

		if (return_p == nullptr) // места нет - создаем новый блок
		{
			return_p = add_alloc_block(n);
		}

		return return_p;
	}

	void deallocate(T* p, std::size_t n) {

		for (const auto& pair : g_memory_map)
		{
			auto block_info = pair.second;

			// Объект находится в одном из блоков памяти

			if ((char*)p >= pair.first && (char*)p < (pair.first + block_info.m_space))
			{
				--block_info.m_count;

				if (block_info.m_count == 0) // последний объект
				{
					//cout << "free " << (void*)pair.first << endl;
					free(pair.first);
					g_memory_map.erase(pair.first);
				}
				else
				{
					g_memory_map[pair.first] = block_info;
					//cout << block_info.m_count << endl;
				}

				break;
			}
		}
	}

	template<typename U, typename ...Args>
	void construct(U* p, Args &&...args) {

		new(p) U(std::forward<Args>(args)...);
	};

	void destroy(T* p) {

		p->~T();
	}

private:

	/**
		Добавление нового блока памяти в менеджер памяти
		Выделение памяти с помощью malloc
		n - количество объектов T
	*/
	T* add_alloc_block(size_t n)
	{
		size_t space_bytes = size * sizeof(T);
		size_t size_bytes = n * sizeof(T);

		if (size_bytes > space_bytes) space_bytes = size_bytes;


		void* p = malloc(space_bytes);
		if (!p)	throw std::bad_alloc();
		//cout << "malloc " << p << " " << space_bytes <<  endl;

		g_memory_map[(char*)p] = { space_bytes, size_bytes, 1 };

		return (T*)p;
	}
};

int factorial(int n)
{
	if (n == 0 || n == 1) return 1;
	return factorial(n - 1) * n;
}

int main()
{
	try {

		// std::map

		map<int, int> std_map;

		for (int i = 0; i < 10; ++i) std_map[i] = factorial(i);

		cout << "std::map\n";

		for (const auto& p : std_map) cout << p.first << " " << p.second << endl;

		// std::map with my_allocator

		std::map<int, int, less<int>, MyAllocator<pair<const int, int>, 10>> m;

		for (int i = 0; i < 10; ++i)
		{
			m[i] = factorial(i);
		}

		cout << "std::map with my_allocator\n";

		for (const auto& p : m) cout << p.first << " " << p.second << endl;

		// my_vector

		MyVector<int> my_v;

		for (int i = 0; i < 10; ++i) my_v.push_back(i);

		cout << "my_vector\n";
		for (const auto& n : my_v) cout << n << " ";
		cout << endl;

		// my_vector with my allocator

		MyVector<int, MyAllocator<int, 10>> v;

		for (int i = 0; i < 10; ++i) v.push_back(i);

		cout << "my_vector with my_allocator\n";
		for (const auto& n : v) cout << n << " ";
		cout << endl;

	} // try

	catch (...) {

		cout << "unexpected exception\n";
	}

	if (g_memory_map.empty()) cout << "g_memory_map is empty...OK\n";

}


