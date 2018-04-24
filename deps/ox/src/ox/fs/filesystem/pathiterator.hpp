/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/strops.hpp>
#include <ox/std/types.hpp>

namespace ox {

class PathIterator {
	private:
		const char *m_path = nullptr;
		std::size_t m_iterator = 0;
		std::size_t m_maxSize = 0;

	public:
		PathIterator(const char *path, std::size_t maxSize, std::size_t iterator = 0);

		PathIterator(const char *path);

		/**
		 * @return 0 if no error
		 */
		int dirPath(char *pathOut, std::size_t pathOutSize);

		/**
		 * @return 0 if no error
		 */
		int fileName(char *out, std::size_t outSize);

		/**
		 * @return 0 if no error
		 */
		int next(char *pathOut, std::size_t pathOutSize);

		/**
		 * @return 0 if no error
		 */
		ValErr<std::size_t> nextSize() const;

		bool hasNext() const;

		PathIterator operator+(int i);

		PathIterator operator-(int i);

};

}
