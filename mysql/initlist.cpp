#include <iostream>
#include <vector>
#include <list>
#include <initializer_list>

class Base {
public:
	virtual void print() const = 0;
};

class Derived1: public Base {
	std::vector<int> vec;
public:
	Derived1(std::initializer_list<int> il): vec(il) {}
	virtual void print() const {
		for(size_t i = 0; i < vec.size(); ++i) std::cout << vec[i] << ' ';
		std::cout << std::endl;	
	}
};

class Derived2: public Base {
	std::list<int> lst;
public:
	Derived2(std::initializer_list<int> il): lst(il) {}
	virtual void print() const {
		for(std::list<int>::const_iterator it = lst.begin(); it != lst.end(); ++it) {
			std::cout << *it << ' ';		
		}
		std::cout << std::endl;
	}
};

void print(const Base& obj) {
	obj.print();
}

int main() {
	print({1, 2, 3, 4, 5});
	print({6, 7, 8, 9, 10});
	return 0;
}