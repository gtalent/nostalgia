/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace ox {

/**
 * Prints a stack trace to stderr.
 *
 * @param shave number of call levels to shave off the top
 */
void printStackTrace(int shave = 1) noexcept;

}
