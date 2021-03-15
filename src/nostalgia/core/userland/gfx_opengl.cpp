/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <array>

#include <ox/std/bit.hpp>
#include <ox/std/defines.hpp>
#include <ox/std/fmt.hpp>

#include <nostalgia/core/gfx.hpp>

#include "glutils.hpp"

namespace nostalgia::core {

using TileMap = std::array<std::array<int, 128>, 128>;

namespace renderer {

constexpr auto TileRows = 16;
constexpr auto TileColumns = 16;
constexpr auto TileCount = TileRows * TileColumns;
constexpr auto BgVertexVboRowLength = 6;
constexpr auto BgVertexVboLength = 16;
constexpr auto BgVertexEboLength = 6;

struct BackgroundBufferset: public Bufferset {
	std::array<float, TileCount * BgVertexVboLength * 4> bgVertices;
	std::array<GLuint, TileCount * BgVertexEboLength * 4> bgEbos;
};

struct GlImplData {
	std::array<TileMap, 4> bgTileMaps{};
	int64_t prevFpsCheckTime = 0;
	uint64_t draws = 0;
	std::array<BackgroundBufferset, 1> backgrounds;
	GLuint bgShader = 0;
};

constexpr const GLchar *bgvshad = R"(
	#version 150
	in vec2 vTexCoord;
	in vec2 position;
	out vec2 fTexCoord;
	void main() {
	    gl_Position = vec4(position, 0.0, 1.0);
	    fTexCoord = vTexCoord;
	})";

constexpr const GLchar *bgfshad = R"(
	#version 150
	out vec4 outColor;
	in vec2 fTexCoord;
	uniform sampler2D image;
	void main() {
	    //outColor = vec4(0.0, 0.7, 1.0, 1.0);
	    outColor = texture(image, fTexCoord) * vec4(1.0, 1.0, 1.0, 1.0);
	})";

void initTileBufferObjects(unsigned vi, float x, float y, float *vbo, GLuint *ebo) {
	// don't worry, this gets optimized to something much more ideal
	constexpr float xmod = 0.06f;
	x *= xmod;
	y *= 0.1f;
	const float vertices[BgVertexVboLength] = {
		       x,        y, 0, 0.04, // bottom left
		x + xmod,        y, 1, 0.04, // bottom right
		x + xmod, y + 0.1f, 1,    0, // top right
		       x, y + 0.1f, 0,    0, // top left
	};
	memcpy(vbo, vertices, sizeof(vertices));
	const GLuint elms[BgVertexEboLength] = {
		vi + 0, vi + 1, vi + 2,
		vi + 2, vi + 3, vi + 0,
	};
	memcpy(ebo, elms, sizeof(elms));
}

void initBackgroundBufferObjects(GLuint shader, BackgroundBufferset *bs) {
	auto i = 0u;
	for (auto x = 0u; x < TileColumns; ++x) {
		for (auto y = 0u; y < TileRows; ++y) {
			const auto vi = i * BgVertexVboLength;
			auto vbo = &bs->bgVertices[vi];
			auto ebo = &bs->bgEbos[i * BgVertexEboLength];
			initTileBufferObjects(vi, x, y, vbo, ebo);
			++i;
		}
	}
	// vbo
	glBindBuffer(GL_ARRAY_BUFFER, bs->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bs->bgVertices), &bs->bgVertices, GL_DYNAMIC_DRAW);
	// ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bs->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bs->bgEbos), &bs->bgEbos, GL_STATIC_DRAW);
	// vbo layout
	auto posAttr = static_cast<GLuint>(glGetAttribLocation(shader, "position"));
	glEnableVertexAttribArray(posAttr);
	glVertexAttribPointer(posAttr, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	auto texCoordAttr = static_cast<GLuint>(glGetAttribLocation(shader, "vTexCoord"));
	glEnableVertexAttribArray(texCoordAttr);
	glVertexAttribPointer(texCoordAttr, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
	                      ox::bit_cast<void*>((2 * sizeof(float))));
}

void initBackgroundBufferset(GLuint shader, BackgroundBufferset *bufferset) {
	// vao
	glGenVertexArrays(1, &bufferset->vao);
	glBindVertexArray(bufferset->vao);
	// vbo & ebo
	glGenBuffers(1, &bufferset->vbo);
	glGenBuffers(1, &bufferset->ebo);
	initBackgroundBufferObjects(shader, bufferset);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texture);
	//auto texAttr = static_cast<GLuint>(glGetUniformLocation(shaderPrgm, "image"));
	//glUniform1i(static_cast<int>(texAttr), static_cast<int>(texture));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

ox::Error init(Context *ctx) {
	const auto id = new GlImplData;
	ctx->setRendererData(id);
	id->bgShader = buildShaderProgram(bgvshad, bgfshad);
	for (auto &bg : id->backgrounds) {
		initBackgroundBufferset(id->bgShader, &bg);
	}
	return OxError(0);
}

ox::Error shutdown(Context *ctx) {
	const auto id = ctx->rendererData<GlImplData>();
	for (auto &bg : id->backgrounds) {
		destroy(bg);
	}
	ctx->setRendererData(nullptr);
	delete id;
	return OxError(0);
}

ox::Error loadTexture(GLuint *texId, void *pixels, int w, int h) {
	if (*texId == 0) {
		glGenTextures(1, texId);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,  GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return OxError(0);
}

ox::Error loadBgTexture(Context *ctx, int section, void *pixels, int w, int h) {
	oxTracef("nostalgia::core::gfx::gl", "loadBgTexture: { section: {}, w: {}, h: {} }", section, w, h);
	const auto &id = ctx->rendererData<GlImplData>();
	const auto texId = &id->backgrounds[static_cast<std::size_t>(section)].tex;
	return loadTexture(texId, pixels, w, h);
}

}


void draw(Context *ctx) {
	const auto id = ctx->rendererData<renderer::GlImplData>();
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

	//glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	for (std::size_t i = 0; i < id->backgrounds.size(); i++) {
		const auto &bg = id->backgrounds[i];
		glUseProgram(id->bgShader);
		glBindTexture(GL_TEXTURE_2D, bg.tex);
		glBindVertexArray(bg.vao);
		glDrawElements(GL_TRIANGLES, bg.bgEbos.size(), GL_UNSIGNED_INT, 0);
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
	const auto id = ctx->rendererData<renderer::GlImplData>();
	const auto z = static_cast<unsigned>(layer);
	const auto y = static_cast<unsigned>(row);
	const auto x = static_cast<unsigned>(column);
	id->bgTileMaps[z][y][x] = tile;
}

}
