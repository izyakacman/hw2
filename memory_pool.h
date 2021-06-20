#pragma once

/**
*	Пул памяти
*/
struct MemoryPool
{
	char* ptr; // указатель на блок памяти в куче
	MemoryPool* next_block; // указатель на следующий блок MemoryPool
	size_t space; // размер блока в байтах
	size_t size; // размер занятого пространства блока в байтах
	size_t count; // количество объектов в блоке

} memory_pool;
