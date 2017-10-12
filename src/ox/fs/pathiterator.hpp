/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/types.hpp>

namespace ox {

class PathIterator {
	private:
		const char *m_path = nullptr;
		size_t m_iterator = 0;
		size_t m_maxSize = 0;

	public:
		PathIterator(const char *path, size_t maxSize);

		/**
		 * @return 0 if no error
		 */
		int dirPath(char *pathOut, size_t pathOutSize);

		/**
		 * @return 0 if no error
		 */
		int fileName(char *out, size_t outSize);

		/**
		 * @return 0 if no error
		 */
		int next(char *pathOut, size_t pathOutSize);

		bool hasNext();
};

}
