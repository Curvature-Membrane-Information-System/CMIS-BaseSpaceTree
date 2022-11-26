#include "test_3rd.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <map>
#include <set>

using namespace std;
class Point
{
public:
	double x;
	double y;
public:
	constexpr Point(double x = 0, double y = 0) noexcept : x(x), y(y) {}
	constexpr Point& operator+=(const Point& other) noexcept
	{
		x += other.x;
		y += other.y;
		return *this;
	}
	constexpr Point& operator/=(int t) noexcept
	{
		x /= t;
		y /= t;
		return *this;
	}
};
class Rect
{
public:
	double x;
	double y;
	double w;
	double h;
public:
	constexpr Rect(double x = 0, double y = 0, double w = 0, double h = 0) : x(x), y(y), w(w), h(h) {}
};
class GeoRect
{
public:
	Rect rect;
	std::string name;
public:
	GeoRect(Rect rect, std::string name = "") : rect(rect), name(name) {}
};
class Node
{
	struct Quad
	{
		Node* top    = nullptr;
		Node* bottom = nullptr;
		Node* left   = nullptr;
		Node* right  = nullptr;
	};
public:
	Rect rect;
	Quad quad;
};
class Tree
{
public:
	Node* root = nullptr;
public:
	void add_rect(Rect rect);
	void add_point(Point point);
	// void search(Rect rect);
	Node* search(Rect rect);
	void create_georect(GeoRect geo);
};


void test_3rd()
{
	Tree tree;
	tree.add_rect(Rect(0, 0, 2018, 2018));
	tree.add_point(Point(0, 0));
	tree.add_point(Point(2018, 2018));
	
	 tree.search(Rect(100, 100, 200, 200));
}

void Tree::add_rect(Rect rect)
{
}
void Tree::add_point(Point point)
{
}
Node* Tree::search(Rect rect)
{
	return nullptr;
}

void Tree::create_georect(GeoRect geo)
{
	if (root == nullptr)
	{
		root = new Node();
		root->rect = geo.rect;
	}
	else
	{
		// Node* node = search(geo.rect);
		
	}
}
