
#include <map>
#include <string>
#include <vector>

#include <ox/std/std.hpp>

#include <nostalgia/core/mem.hpp>

namespace nostalgia::core {

[[nodiscard]] void *malloc(std::size_t allocSize);

void free(void *ptr);

void initHeap(char *heapBegin, char *heapEnd);

}

using namespace nostalgia;

int testMalloc(std::string) {
    std::vector<char> buff(ox::units::MB);
    core::initHeap(&buff.front(), &buff.back());
    oxAssert(core::malloc(5) != nullptr, "malloc is broken");
    oxAssert(core::malloc(5) != nullptr, "malloc is broken");
    return 0;
}

std::map<std::string, int(*)(std::string)> tests = {
	{
		{ "malloc", testMalloc },
    }
};

int main(int argc, const char **args) {
	int retval = -1;
	if (argc > 1) {
		auto testName = args[1];
		std::string testArg = "";
		if (args[2]) {
			testArg = args[2];
		}
		if (tests.find(testName) != tests.end()) {
			retval = tests[testName](testArg);
		}
	}
	return retval;
}

