#include "SearchMod.hpp"

#include <iostream>


class Person {
	public:
		unsigned int age;

		DORM::SearchMod<unsigned int> younger_than;
};


int main() {
	Person p;

	std::cout << "younger_than set: " << (p.younger_than ? "YES" : "NO") << std::endl;

	p.younger_than(5);

	std::cout << "younger_than set: " << (p.younger_than ? "YES" : "NO") << std::endl;

	std::cout << "younger_than value: " << p.younger_than() << std::endl;
}
