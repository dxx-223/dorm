#include "Test.hpp"

int main() {
	Test t;
	t.name("Fudge");
	t.age(5);

	auto cloned_t = t.clone();

	std::cout << "Clone's name: " << t.name() << std::endl;
}
