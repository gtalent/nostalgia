/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/memops.hpp>
#include <ox/std/strops.hpp>
#include "pathiterator.hpp"

namespace ox {

PathIterator::PathIterator(const char *path, size_t maxSize) {
	m_path = path;
	m_maxSize = maxSize;
}

/**
 * @return 0 if no error
 */
int PathIterator::dirPath(char *out, size_t outSize) {
	int idx = ox_lastIndexOf(m_path, '/', m_maxSize);
	size_t size = idx + 1;
	if (idx >= 0 && size < outSize) {
		ox_memcpy(out, m_path, size);
		out[size] = 0;
		return 0;
	} else {
		return 1;
	}
}

/**
 * @return 0 if no error
 */
int PathIterator::fileName(char *out, size_t outSize) {
	auto idx = ox_lastIndexOf(m_path, '/', m_maxSize);
	if (idx >= 0) {
		idx++; // pass up the preceding /
		size_t fileNameSize = ox_strlen(&m_path[idx]);
		if (fileNameSize < outSize) {
			ox_memcpy(out, &m_path[idx], fileNameSize);
			out[fileNameSize] = 0;
			return 0;
		} else {
			return 1;
		}
	} else {
		return 2;
	}
}

// Gets the next item in the path
int PathIterator::next(char *pathOut, size_t pathOutSize) {
	size_t size = 0;
	int retval = 1;
	if (m_iterator < m_maxSize && ox_strlen(&m_path[m_iterator])) {
		retval = 0;
		if (m_path[m_iterator] == '/') {
			m_iterator++;
		}
		size_t start = m_iterator;
		// end is at the next /
		const char *substr = ox_strchr(&m_path[start], '/', m_maxSize - start);
		// correct end if it is invalid, which happens if there is no next /
		if (!substr) {
			substr = ox_strchr(&m_path[start], 0, m_maxSize - start);
		}
		size_t end = substr - m_path;
		size = end - start;
		// cannot fit the output in the output parameter
		if (size >= pathOutSize) {
			return -1;
		}
		ox_memcpy(pathOut, &m_path[start], size);
	}
	// truncate trailing /
	if (size && pathOut[size - 1] == '/') {
		size--;
	}
	pathOut[size] = 0; // end with null terminator
	m_iterator += size;
	return retval;
}

bool PathIterator::hasNext() {
	size_t size = 0;
	if (m_iterator < m_maxSize && ox_strlen(&m_path[m_iterator])) {
		size_t start = m_iterator;
		if (m_path[start] == '/') {
			start++;
		}
		// end is at the next /
		const char *substr = ox_strchr(&m_path[start], '/', m_maxSize - start);
		// correct end if it is invalid, which happens if there is no next /
		if (!substr) {
			substr = ox_strchr(&m_path[start], 0, m_maxSize - start);
		}
		size_t end = substr - m_path;
		size = end - start;
	}
	return size > 0;
}

}
