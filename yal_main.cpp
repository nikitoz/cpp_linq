#include <iostream>
#include <vector>
#include "yal.hpp"

template <typename T>
struct plus_one {
	void operator()(T& t) { t += 1; }
};

template <typename T>
struct greater5 {
	bool operator()(const T& t) { return (t > 5); }
};


int main() {
	std::vector<int> vec;
	vec.push_back(1); vec.push_back(2);  vec.push_back(5);  vec.push_back(4);
	std::vector<int> v = fromVector(vec).foreach(plus_one<int>()).where(greater5<int>()).foreach(plus_one<int>()).toVector();
	for (int i = 0; i != v.size(); ++i)
		std::cout << v[i] << " ";
	return 0;
}