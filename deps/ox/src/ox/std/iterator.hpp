
#pragma once

#ifndef OX_USE_STDLIB
namespace std {

struct input_iterator_tag {
};

struct output_iterator_tag {
};

struct forward_iterator_tag: public input_iterator_tag {
};

struct bidirectional_iterator_tag: public forward_iterator_tag {
};

struct random_access_iterator_tag: public bidirectional_iterator_tag {
};

struct contiguous_iterator_tag: public random_access_iterator_tag {
};

template<typename Category, typename T>
struct iterator {
	using iterator_category = Category;
	using value_type = T;
	using pointer = T*;
	using reference = T&;
};

}
#else
#include <iterator>
#endif
