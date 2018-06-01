/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/memops.hpp>
#include <ox/std/strops.hpp>
#include "pathiterator.hpp"

namespace ox {

PathIterator::PathIterator(const char *path, std::size_t maxSize, std::size_t iterator) {
	m_path = path;
	m_maxSize = maxSize;
	m_iterator = iterator;
}

PathIterator::PathIterator(const char *path): PathIterator(path, ox_strlen(path)) {
}

/**
 * @return 0 if no error
 */
Error PathIterator::dirPath(char *out, std::size_t outSize) {
	int idx = ox_lastIndexOf(m_path, '/', m_maxSize);
	std::size_t size = idx + 1;
	if (idx >= 0 && size < outSize) {
		ox_memcpy(out, m_path, size);
		out[size] = 0;
		return OxError(0);
	} else {
		return OxError(1);
	}
}

/**
 * @return 0 if no error
 */
Error PathIterator::fileName(char *out, std::size_t outSize) {
	auto idx = ox_lastIndexOf(m_path, '/', m_maxSize);
	if (idx >= 0) {
		idx++; // pass up the preceding /
		std::size_t fileNameSize = ox_strlen(&m_path[idx]);
		if (fileNameSize < outSize) {
			ox_memcpy(out, &m_path[idx], fileNameSize);
			out[fileNameSize] = 0;
			return OxError(0);
		} else {
			return OxError(1);
		}
	} else {
		return OxError(2);
	}
}

// Gets the get item in the path
Error PathIterator::get(char *pathOut, std::size_t pathOutSize) {
	std::size_t size = 0;
	Error retval = 1;
	if (m_iterator < m_maxSize && ox_strlen(&m_path[m_iterator])) {
		retval = 0;
		auto start = m_iterator;
		if (m_path[start] == '/') {
			start++;
		}
		// end is at the next /
		const char *substr = ox_strchr(&m_path[start], '/', m_maxSize - start);
		// correct end if it is invalid, which happens if there is no next /
		if (!substr) {
			substr = ox_strchr(&m_path[start], 0, m_maxSize - start);
		}
		std::size_t end = substr - m_path;
		size = end - start;
		// cannot fit the output in the output parameter
		if (size >= pathOutSize) {
			return OxError(1);
		}
		ox_memcpy(pathOut, &m_path[start], size);
	}
	// truncate trailing /
	if (size && pathOut[size - 1] == '/') {
		size--;
	}
	pathOut[size] = 0; // end with null terminator
	return OxError(retval);
}

// Gets the get item in the path
Error PathIterator::next(char *pathOut, std::size_t pathOutSize) {
	std::size_t size = 0;
	Error retval = 1;
	if (m_iterator < m_maxSize && ox_strlen(&m_path[m_iterator])) {
		retval = 0;
		if (m_path[m_iterator] == '/') {
			m_iterator++;
		}
		std::size_t start = m_iterator;
		// end is at the next /
		const char *substr = ox_strchr(&m_path[start], '/', m_maxSize - start);
		// correct end if it is invalid, which happens if there is no next /
		if (!substr) {
			substr = ox_strchr(&m_path[start], 0, m_maxSize - start);
		}
		std::size_t end = substr - m_path;
		size = end - start;
		// cannot fit the output in the output parameter
		if (size >= pathOutSize) {
			return OxError(1);
		}
		ox_memcpy(pathOut, &m_path[start], size);
	}
	// truncate trailing /
	if (size && pathOut[size - 1] == '/') {
		size--;
	}
	pathOut[size] = 0; // end with null terminator
	m_iterator += size;
	return OxError(retval);
}

/**
 * @return 0 if no error
 */
Error PathIterator::get(BString<MaxFileNameLength> *fileName) {
	return get(fileName->data(), fileName->cap());
}

/**
 * @return 0 if no error
 */
Error PathIterator::next(BString<MaxFileNameLength> *fileName) {
	return next(fileName->data(), fileName->cap());
}

ValErr<std::size_t> PathIterator::nextSize() const {
	std::size_t size = 0;
	Error retval = 1;
	auto it = m_iterator;
	if (it < m_maxSize && ox_strlen(&m_path[it])) {
		retval = 0;
		if (m_path[it] == '/') {
			it++;
		}
		std::size_t start = it;
		// end is at the next /
		const char *substr = ox_strchr(&m_path[start], '/', m_maxSize - start);
		// correct end if it is invalid, which happens if there is no next /
		if (!substr) {
			substr = ox_strchr(&m_path[start], 0, m_maxSize - start);
		}
		std::size_t end = substr - m_path;
		size = end - start;
	}
	it += size;
	return {size, OxError(retval)};
}

bool PathIterator::hasNext() const {
	std::size_t size = 0;
	if (m_iterator < m_maxSize && ox_strlen(&m_path[m_iterator])) {
		std::size_t start = m_iterator;
		if (m_path[start] == '/') {
			start++;
		}
		// end is at the next /
		const char *substr = ox_strchr(&m_path[start], '/', m_maxSize - start);
		// correct end if it is invalid, which happens if there is no next /
		if (!substr) {
			substr = ox_strchr(&m_path[start], 0, m_maxSize - start);
		}
		std::size_t end = substr - m_path;
		size = end - start;
	}
	return size > 0;
}

bool PathIterator::valid() const {
	return m_path[m_iterator] != 0;
}

PathIterator PathIterator::operator+(int i) const {
	std::size_t size = 0;
	Error retval = 1;
	auto iterator = m_iterator;
	if (iterator < m_maxSize && ox_strlen(&m_path[iterator])) {
		retval = 0;
		if (m_path[iterator] == '/') {
			iterator++;
		}
		std::size_t start = iterator;
		// end is at the next /
		const char *substr = ox_strchr(&m_path[start], '/', m_maxSize - start);
		// correct end if it is invalid, which happens if there is no next /
		if (!substr) {
			substr = ox_strchr(&m_path[start], 0, m_maxSize - start);
		}
		std::size_t end = substr - m_path;
		size = end - start;
	}
	iterator += size;
	return PathIterator(m_path, m_maxSize, iterator + i);
}

}
