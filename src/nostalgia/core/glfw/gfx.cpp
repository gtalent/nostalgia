/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <array>

#include <GLFW/glfw3.h>

#include <ox/claw/read.hpp>

#include <nostalgia/core/gfx.hpp>
#include <nostalgia/core/userland/gfx.hpp>

#include "core.hpp"

namespace nostalgia::core {

constexpr auto Scale = 5;

static void handleGlfwError(int err, const char *desc) {
    oxErrf("GLFW error ({}): {}", err, desc);
}

ox::Error initGfx(Context *ctx) noexcept {
	auto id = ctx->windowerData<GlfwImplData>();
	glfwSetErrorCallback(handleGlfwError);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	id->window = glfwCreateWindow(240 * Scale, 160 * Scale, "nostalgia", nullptr, nullptr);
	if (id->window == nullptr) {
		return OxError(1, "Could not open GLFW window");
	}
	glfwSetWindowUserPointer(id->window, ctx);
	glfwMakeContextCurrent(id->window);
	oxReturnError(renderer::init(ctx));
	return OxError(0);
}

ox::Error shutdownGfx(Context *ctx) noexcept {
	oxReturnError(renderer::shutdown(ctx));
	auto id = ctx->windowerData<GlfwImplData>();
	glfwDestroyWindow(id->window);
	ctx->setWindowerData(nullptr);
	delete id;
	return OxError(0);
}

int getScreenWidth(Context *ctx) noexcept {
	auto id = ctx->windowerData<GlfwImplData>();
	int w = 0, h = 0;
	glfwGetFramebufferSize(id->window, &w, &h);
	return w;
}

int getScreenHeight(Context *ctx) noexcept {
	auto id = ctx->windowerData<GlfwImplData>();
	int w = 0, h = 0;
	glfwGetFramebufferSize(id->window, &w, &h);
	return h;
}

common::Size getScreenSize(Context *ctx) noexcept {
	auto id = ctx->windowerData<GlfwImplData>();
	int w = 0, h = 0;
	glfwGetFramebufferSize(id->window, &w, &h);
	return {w, h};
}

}
