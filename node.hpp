#ifndef NODE_HPP_INCLUDED__
#define NODE_HPP_INCLUDED__
#include <vector>
#include "file_manager.h"
#include <stdint.h>
class Node{
public:
	int id;
	std::vector<int> mbr;
	int parentId;
	std::vector<int>childId;
	std::vector<std::vector<int>> childMbr;
	//No need of constructor
	Node();
	Node(int maxcap,int d);
};
#endif