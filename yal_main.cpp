#include <iostream>
#include <vector>
#include <set>
#include "yal.hpp"

template <typename T>
void print_collection(T& collection) {
	for (typename T::iterator it = collection.begin(); it != collection.end(); ++it)
		std::cout << *it << " ";
	std::cout << std::endl;
}

int main() {
	std::vector<char> vec = {'H', 'E', 'L', 'L', 'O'};
	std::set<char>    el  = {'H', 'E', 'L', 'L'};
	std::vector<char> v = yal::from(vec).where([&](const char& ch) { return el.find(ch) != el.end(); }).toVector();
	print_collection(v);

	std::vector<std::string> vs = {"hello", "cruel", "world"};
	vs = yal::from(vs).where([](const std::string& str) { return str != "cruel"; }).toVector();
	print_collection(vs);


	std::vector<int> vint = {1,2,3,4,5,6,7};
	vint = yal::from(vint).foreach([](int& i){ ++i; }).where([](int i){ return i < 5;}).foreach([](int& i) { i += 10; }).toVector();
	print_collection(vint);

	return 0;
}
