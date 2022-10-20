#pragma once
#include <iostream>
#include <vector>

#define 无 nullptr

class 经纬
{
public:
	经纬()
	{
		std::cout << "经纬" << std::endl;
	}
	~经纬()
	{
		std::cout << "~经纬" << std::endl;
	}

public:
	double 经 = 0;
	double 纬 = 0;
};
class 气
{
public:
	经纬 域中经纬;
	double 方圆 = 1.0;
};
class 域
{
public:
	double 东域 = 0;
	double 西域 = 0;
	double 南域 = 0;
	double 北域 = 0;
};
class 元
{
	std::vector<气*> 有气;
public:
	元() { std::cout << "元" << std::endl; }
	~元()
	{
		std::cout << "~元" << std::endl;
		for (auto 气者 : 有气)
		{
			delete 气者;
		}
		delete 东者;
		delete 西者;
		delete 南者;
		delete 北者;
	}

	元* 东者 = 无;
	元* 西者 = 无;
	元* 南者 = 无;
	元* 北者 = 无;
	域 四方;
	void 有(const 域& 有限域)
	{

	}
	void 存(气 有者)
	{

	}
};

int 测试()
{
	元 世界;

	世界.四方.东域 = 1;
	世界.四方.西域 = 2;
	世界.四方.南域 = 3;
	世界.四方.北域 = 4;

	世界.东者 = new 元;
	世界.西者 = new 元;
	世界.西者->北者 = new 元;

}