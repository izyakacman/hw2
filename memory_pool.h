#pragma once

/**
*	��� ������
*/
struct MemoryPool
{
	char* ptr; // ��������� �� ���� ������ � ����
	MemoryPool* next_block; // ��������� �� ��������� ���� MemoryPool
	size_t space; // ������ ����� � ������
	size_t size; // ������ �������� ������������ ����� � ������
	size_t count; // ���������� �������� � �����

} memory_pool;
