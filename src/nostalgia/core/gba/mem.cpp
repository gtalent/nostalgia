/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "addresses.hpp"
#include "panic.hpp"

#include <ox/std/std.hpp>

#pragma GCC diagnostic ignored "-Wcast-align"

namespace nostalgia::core {

struct HeapSegment {
	std::size_t size;
	uint8_t inUse;
	HeapSegment *next;

	uint8_t *end() {
		return ((uint8_t*) this) + this->size;
	}
};

static HeapSegment *volatile _heapIdx = nullptr;

void initHeap() {
	_heapIdx = reinterpret_cast<HeapSegment*>(MEM_WRAM_END) - 1;
	// set size to half of WRAM
	_heapIdx->size = (MEM_WRAM_END - MEM_WRAM_BEGIN) / 2;
	_heapIdx->next = nullptr;
	_heapIdx->inUse = false;
}

}

using namespace nostalgia::core;

void *malloc(std::size_t allocSize) {
	// add space for heap segment header data
	const auto fullSize = allocSize + sizeof(HeapSegment);
	auto seg = _heapIdx;
	HeapSegment *prev = nullptr;
	while (seg && (seg->inUse || seg->size < fullSize)) {
		prev = seg;
		seg = seg->next;
	}

	// panic if the allocation failed
	if (seg == nullptr) {
		panic("Heap allocation failed");
	}

	seg = reinterpret_cast<HeapSegment*>(reinterpret_cast<uint8_t*>(seg) - allocSize);
	if (prev) {
		prev->next = seg;
	}
	// update size for the heap segment now that it is to be considered
	// allocated
	seg->size = fullSize;
	seg->next = (HeapSegment*) (((uint8_t*) seg) + fullSize);
	seg->inUse = true;
	auto out = seg + 1;

	auto hs = *_heapIdx;
	hs.size -= fullSize;
	if (hs.size == 0) {
		_heapIdx = hs.next;
	} else {
		_heapIdx = reinterpret_cast<HeapSegment*>((reinterpret_cast<uint8_t*>(_heapIdx)) - fullSize);
		*_heapIdx = hs;
	}

	return out;
}

void free(void *ptrIn) {
	// get ptr back down to the meta data
	auto *ptr = ((HeapSegment*) ptrIn) - 1;
	HeapSegment *prev = nullptr;
	auto current = _heapIdx;
	while (current && current != ptr) {
		prev = current;
		current = current->next;
	}

	// ptr was found as a valid memory allocation, deallocate it
	if (current) {
		// move header back to end of segment
		auto newCurrent = reinterpret_cast<HeapSegment*>(current->end() - sizeof(HeapSegment));
		*newCurrent = *current;
		current = newCurrent;
		prev->next = current;

		// mark as not in use
		current->inUse = false;

		// join with next if next is also unused
		if (current->next && !current->next->inUse) {
			current->size += current->next->size;
			current->next = current->next->next;
		}

		// join with prev if prev is also unused
		if (prev && !prev->inUse) {
			prev->size += current->size;
			prev->next = current->next;
			current = prev;
		}

		// if current is closer heap start than _heapIdx, _heapIdx becomes
		// current
		if (current > _heapIdx) {
			_heapIdx = current;
		}
	}
}

void *operator new(std::size_t allocSize) {
	return malloc(allocSize);
}

void *operator new[](std::size_t allocSize) {
	return malloc(allocSize);
}

void operator delete(void *ptr) {
	free(ptr);
}

void operator delete[](void *ptr) {
	free(ptr);
}

void operator delete(void *ptr, unsigned) {
	free(ptr);
}

void operator delete[](void *ptr, unsigned) {
	free(ptr);
}
