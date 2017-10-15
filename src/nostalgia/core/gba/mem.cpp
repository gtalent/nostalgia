/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "addresses.hpp"
#include "panic.hpp"

namespace nostalgia {
namespace core {

struct HeapSegment {
	size_t size;
	uint8_t inUse;
	HeapSegment *next;
};

static HeapSegment *_heapIdx = nullptr;

void initHeap() {
	_heapIdx = (HeapSegment*) MEM_WRAM_END;
	_heapIdx--;
	// set size to half of WRAM
	_heapIdx->size = (MEM_WRAM_END - MEM_WRAM_BEGIN) / 2;
	_heapIdx->next = nullptr;
	_heapIdx->inUse = false;
}

}
}

using namespace nostalgia::core;

void *operator new(size_t allocSize) {
	// add space for heap segment header data
	const auto fullSize = allocSize + sizeof(HeapSegment);
	auto seg = _heapIdx;
	HeapSegment *prev = nullptr;
	while (seg && seg->size < fullSize) {
		prev = seg;
		seg = seg->next;
	}

	// panic if the allocation failed
	if (seg == nullptr) {
		panic("Heap allocation failed");
	}

	seg = (HeapSegment*) (((uint8_t*) seg) - allocSize);
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
		_heapIdx = (HeapSegment*) (((uint8_t*) _heapIdx) - fullSize);
		*_heapIdx = hs;
	}

	return out;
}

void operator delete(void *ptrIn) {
	// get ptr back down to the meta data
	auto *ptr = ((HeapSegment*) ptrIn) - 1;
	HeapSegment *prev = nullptr;
	HeapSegment *current = _heapIdx;
	while (current && current != ptr) {
		prev = current;
		current = current->next;
	}

	// ptr was found as a valid memory allocation, deallocate it
	if (current) {
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

