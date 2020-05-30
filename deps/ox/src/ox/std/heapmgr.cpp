/*
 * Copyright 2016 - 2020 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "assert.hpp"
#include "bit.hpp"
#include "heapmgr.hpp"

namespace ox::heapmgr {

static struct HeapSegment *volatile g_heapBegin = nullptr;
static struct HeapSegment *volatile g_heapEnd = nullptr;
static struct HeapSegment *volatile heapIdx = nullptr;

static constexpr std::size_t alignedSize(std::size_t sz) {
	return sz + (sz & 7);
}

template<typename T>
static constexpr std::size_t alignedSize(T = {}) {
	return alignedSize(sizeof(T));
}

void HeapSegment::init(std::size_t maxSize = ox::bit_cast<std::size_t>(g_heapEnd)) {
	this->size = maxSize - ox::bit_cast<std::size_t>(this);
	this->inUse = false;
}

template<typename T>
T *HeapSegment::data() {
	return ox::bit_cast<T*>(ox::bit_cast<uint8_t*>(this) + alignedSize(this));
}

template<typename T = uint8_t>
T *HeapSegment::end() {
	const auto size = alignedSize(this) + alignedSize(this->size);
	auto e = ox::bit_cast<uintptr_t>(ox::bit_cast<uint8_t*>(this) + size);
	return ox::bit_cast<T*>(e);
}


void initHeap(char *heapBegin, char *heapEnd) {
	g_heapBegin = ox::bit_cast<HeapSegment*>(heapBegin);
	g_heapEnd = ox::bit_cast<HeapSegment*>(heapEnd);
	heapIdx = g_heapBegin;
	heapIdx->size = ox::bit_cast<std::size_t>(heapEnd) - ox::bit_cast<std::size_t>(heapIdx);
	heapIdx->inUse = false;
}

struct SegmentPair {
	HeapSegment *anteSegment = nullptr;
	HeapSegment *segment = nullptr;
};

static SegmentPair findSegmentOf(void *ptr) {
	HeapSegment *prev = nullptr;
	for (auto seg = g_heapBegin; seg < g_heapEnd;) {
		if (seg->data<void>() == ptr) {
			return {prev, seg};
		}
		prev = seg;
		seg = seg->end<HeapSegment>();
	}
	return {};
}

static HeapSegment *findSegmentFor(std::size_t sz) {
	for (auto s = g_heapBegin; s <= g_heapEnd; s = s->end<HeapSegment>()) {
		if (s->size >= sz && !s->inUse) {
			return s;
		}
	}
	oxPanic(OxError(1), "malloc: could not find segment");
	return nullptr;
}

[[nodiscard]] void *malloc(std::size_t allocSize) {
	const auto targetSize = alignedSize(sizeof(HeapSegment)) + alignedSize(allocSize);
	auto seg = findSegmentFor(targetSize);
	if (seg == nullptr) {
		return nullptr;
	}
	const auto bytesRemaining = seg->size - targetSize;
	seg->size = targetSize;
	seg->inUse = true;
	seg->end<HeapSegment>()->init(bytesRemaining);
	return seg->data<void>();
}

void free(void *ptr) {
	auto p = findSegmentOf(ptr);
	if (p.anteSegment) {
		p.anteSegment->size += p.segment->size;
	} else if (p.segment) {
		p.segment->inUse = false;
	} else {
		oxPanic(OxError(1), "Bad heap free");
	}
}

}

#ifndef OX_USE_STDLIB

using namespace ox;

void *operator new(std::size_t allocSize) {
	return heapmgr::malloc(allocSize);
}

void *operator new[](std::size_t allocSize) {
	return heapmgr::malloc(allocSize);
}

void operator delete(void *ptr) {
	heapmgr::free(ptr);
}

void operator delete[](void *ptr) {
	heapmgr::free(ptr);
}

void operator delete(void *ptr, unsigned) {
	heapmgr::free(ptr);
}

void operator delete[](void *ptr, unsigned) {
	heapmgr::free(ptr);
}

void operator delete(void *ptr, unsigned long int) {
	heapmgr::free(ptr);
}

void operator delete[](void *ptr, unsigned long int) {
	heapmgr::free(ptr);
}

#endif
