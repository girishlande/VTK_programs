// hashmaptest.cpp : Defines the entry point for the console application.
//

// #include "stdafx.h"
#include <iostream>
#include <vector>
#include <set>

using namespace std;

int main()
{
	set<int> list;
	list.insert(10);
	list.insert(20);
	list.insert(30);
	list.insert(40);
	list.insert(30);
	list.insert(10);
	list.insert(20);
	list.insert(30);
	list.insert(40);
	list.insert(30);

	set<int>::iterator it;
	for (it = list.begin(); it != list.end(); ++it) {
		cout << "\n :" << *it;
	}


	cout << "\n\n";
	return 0;
}

