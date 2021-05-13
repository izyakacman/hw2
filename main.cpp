#include <iostream>
#include <cstring>

using namespace std;

#include "my_allocator.h"
#include "my_vector.h"

/**
	Менеджер памяти
*/
map<char*, MemoryBlock> g_memory_map;

/**
	Вычисление факториала числа n
*/
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
}


