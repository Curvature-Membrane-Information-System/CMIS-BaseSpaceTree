#pragma once
#include <iostream>
#include <vector>

#define �� nullptr

class ��γ
{
public:
	��γ()
	{
		std::cout << "��γ" << std::endl;
	}
	~��γ()
	{
		std::cout << "~��γ" << std::endl;
	}

public:
	double �� = 0;
	double γ = 0;
};
class ��
{
public:
	��γ ���о�γ;
	double ��Բ = 1.0;
};
class ��
{
public:
	double ���� = 0;
	double ���� = 0;
	double ���� = 0;
	double ���� = 0;
};
class Ԫ
{
	std::vector<��*> ����;
public:
	Ԫ() { std::cout << "Ԫ" << std::endl; }
	~Ԫ()
	{
		std::cout << "~Ԫ" << std::endl;
		for (auto ���� : ����)
		{
			delete ����;
		}
		delete ����;
		delete ����;
		delete ����;
		delete ����;
	}

	Ԫ* ���� = ��;
	Ԫ* ���� = ��;
	Ԫ* ���� = ��;
	Ԫ* ���� = ��;
	�� �ķ�;
	void ��(const ��& ������)
	{

	}
	void ��(�� ����)
	{

	}
};

int ����()
{
	Ԫ ����;

	����.�ķ�.���� = 1;
	����.�ķ�.���� = 2;
	����.�ķ�.���� = 3;
	����.�ķ�.���� = 4;

	����.���� = new Ԫ;
	����.���� = new Ԫ;
	����.����->���� = new Ԫ;

}