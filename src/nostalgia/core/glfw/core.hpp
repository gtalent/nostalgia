/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <nostalgia/core/core.hpp>

namespace nostalgia::core {

struct GlfwImplData {
	class GLFWwindow *window = nullptr;
	int64_t startTime = 0;
	bool running = false;
	event_handler eventHandler = nullptr;
	uint64_t wakeupTime = 0;
};

}
