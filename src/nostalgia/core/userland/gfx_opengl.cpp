/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/fmt.hpp>
#define GL_SILENCE_DEPRECATION

#include <array>

#include <ox/std/defines.hpp>
#include <ox/std/defines.hpp>

#define GL_GLEXT_PROTOTYPES 1
//#include <GLES2/gl2.h>
#ifdef OX_OS_Darwin
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <nostalgia/core/gfx.hpp>

namespace nostalgia::core {

using TileMap = std::array<std::array<int, 128>, 128>;

struct GlImplData {
	std::array<TileMap, 4> bgTileMaps{};
	std::array<GLuint, 4> bgTextures{};
	int64_t prevFpsCheckTime = 0;
	uint64_t draws = 0;
};


namespace renderer {

ox::Error init(Context *ctx) {
	const auto id = new GlImplData;
	ctx->setRendererData(id);
	return OxError(0);
}

ox::Error shutdown(Context *ctx) {
	const auto id = ctx->rendererData<GlImplData>();
	ctx->setRendererData(nullptr);
	delete id;
	return OxError(0);
}

ox::Error loadTexture(Context *ctx, int section, void *pixels, int w, int h) {
	oxTracef("nostalgia::core::gfx::gl", "loadTexture: { section: {}, w: {}, h: {} }", section, w, h);
	const auto &id = ctx->rendererData<GlImplData>();
	auto &texId = id->bgTextures[static_cast<std::size_t>(section)];
	glGenTextures(1, &texId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,  GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return OxError(0);
}

}


void draw(Context *ctx) {
	const auto id = ctx->rendererData<GlImplData>();
	++id->draws;
	if (id->draws >= 5000) {
		using namespace std::chrono;
		const auto now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		const auto duration = static_cast<double>(now - id->prevFpsCheckTime) / 1000.0;
		const auto fps = static_cast<int>(static_cast<double>(id->draws) / duration);
#ifdef NOST_FPS_PRINT
		std::cout << "FPS: " << fps << '\n';
#endif
		oxTracef("nostalgia::core::gfx::gl::fps", "FPS: {}", fps);
		id->prevFpsCheckTime = now;
		id->draws = 0;
	}
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	auto &texId = id->bgTextures[0];

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, texId);

	// Draw a textured quad
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(-0.01,  1);
	glTexCoord2f(0, 1); glVertex2f(-0.01, -1);
	glTexCoord2f(1, 1); glVertex2f( 0.01, -1);
	glTexCoord2f(1, 0); glVertex2f( 0.01,  1);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	for (std::size_t i = 0; i < id->bgTileMaps.size(); i++) {
	}
}

void puts(Context *ctx, int column, int row, const char *str) {
	for (int i = 0; str[i]; ++i) {
		setTile(ctx, 0, column + i, row, static_cast<uint8_t>(charMap[static_cast<int>(str[i])]));
	}
}

void clearTileLayer(Context*, int) {
}

void hideSprite(Context*, unsigned) {
}

void setSprite(Context*,
               unsigned,
               unsigned,
               unsigned,
               unsigned,
               unsigned,
               unsigned,
               unsigned) {
}

void setTile(Context *ctx, int layer, int column, int row, uint8_t tile) {
	const auto id = ctx->rendererData<GlImplData>();
	const auto z = static_cast<unsigned>(layer);
	const auto y = static_cast<unsigned>(row);
	const auto x = static_cast<unsigned>(column);
	id->bgTileMaps[z][y][x] = tile;
}

}
