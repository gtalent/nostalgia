/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <array>

#include <ox/std/bit.hpp>
#include <ox/std/fmt.hpp>

#include <nostalgia/core/config.hpp>
#include <nostalgia/core/gfx.hpp>

#include "glutils.hpp"

namespace nostalgia::core {


namespace renderer {

constexpr auto TileRows = 128;
constexpr auto TileColumns = 128;
constexpr auto TileCount = TileRows * TileColumns;
constexpr auto BgVertexVboRows = 4;
constexpr auto BgVertexVboRowLength = 4;
constexpr auto BgVertexVboLength = BgVertexVboRows * BgVertexVboRowLength;
constexpr auto BgVertexEboLength = 6;

struct Background {
	VertexArray vao;
	Buffer vbo;
	Buffer ebo;
	Texture tex;
	bool enabled = false;
	bool updated = false;
	std::array<float, TileCount * BgVertexVboLength> bgVertices = {};
	std::array<GLuint, TileCount * BgVertexEboLength> bgElements = {};
};

struct GlImplData {
	Program bgShader;
	int64_t prevFpsCheckTime = 0;
	uint64_t draws = 0;
	std::array<Background, 4> backgrounds;
};

constexpr const GLchar *bgvshad = R"(
	#version 150
	in vec2 vTexCoord;
	in vec2 position;
	out vec2 fTexCoord;
	uniform float vTileHeight;
	void main() {
	    gl_Position = vec4(position, 0.0, 1.0);
	    fTexCoord = vTexCoord * vec2(1, vTileHeight);
	})";

constexpr const GLchar *bgfshad = R"(
	#version 150
	out vec4 outColor;
	in vec2 fTexCoord;
	uniform sampler2D image;
	void main() {
	    //outColor = vec4(0.0, 0.7, 1.0, 1.0);
	    outColor = texture(image, fTexCoord);
	})";

[[nodiscard]]
static constexpr auto bgVertexRow(unsigned x, unsigned y) noexcept {
	return y * TileRows + x;
}

static void setTileBufferObject(Context *ctx, unsigned vi, float x, float y, int textureRow, float *vbo, GLuint *ebo) {
	// don't worry, this memcpy gets optimized to something much more ideal
	const auto [sw, sh] = getScreenSize(ctx);
	constexpr float ymod = 2.0f / 20.0f;
	const float xmod = ymod * static_cast<float>(sh) / static_cast<float>(sw);
	x *= xmod;
	y *= -ymod;
	x -= 1.0f;
	y += 1.0f - ymod;
	const float vertices[BgVertexVboLength] = {
		       x,        y, 0, static_cast<float>(textureRow + 1), // bottom left
		x + xmod,        y, 1, static_cast<float>(textureRow + 1), // bottom right
		x + xmod, y + ymod, 1, static_cast<float>(textureRow + 0), // top right
		       x, y + ymod, 0, static_cast<float>(textureRow + 0), // top left
	};
	memcpy(vbo, vertices, sizeof(vertices));
	const GLuint elms[BgVertexEboLength] = {
		vi + 0, vi + 1, vi + 2,
		vi + 2, vi + 3, vi + 0,
	};
	memcpy(ebo, elms, sizeof(elms));
}

static void sendVbo(const Background &bg) noexcept {
	// vbo
	glBindBuffer(GL_ARRAY_BUFFER, bg.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bg.bgVertices), &bg.bgVertices, GL_DYNAMIC_DRAW);
}

static void sendEbo(const Background &bg) noexcept {
	// ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bg.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bg.bgElements), &bg.bgElements, GL_STATIC_DRAW);
}

static void initBackgroundBufferObjects(Context *ctx, Background *bg) noexcept {
	for (auto x = 0u; x < TileColumns; ++x) {
		for (auto y = 0u; y < TileRows; ++y) {
			const auto i = bgVertexRow(x, y);
			auto vbo = &bg->bgVertices[i * BgVertexVboLength];
			auto ebo = &bg->bgElements[i * BgVertexEboLength];
			setTileBufferObject(ctx, i * BgVertexVboRows, x, y, 0, vbo, ebo);
		}
	}
}

static VertexArray genVertexArrayObject() noexcept {
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	return VertexArray(vao);
}

static Buffer genBuffer() noexcept {
	GLuint buff = 0;
	glGenBuffers(1, &buff);
	return Buffer(buff);
}

static void initBackgroundBufferset(Context *ctx, GLuint shader, Background *bg) noexcept {
	// vao
	bg->vao = genVertexArrayObject();
	glBindVertexArray(bg->vao);
	// vbo & ebo
	bg->vbo = genBuffer();
	bg->ebo = genBuffer();
	initBackgroundBufferObjects(ctx, bg);
	sendVbo(*bg);
	sendEbo(*bg);
	// vbo layout
	auto posAttr = static_cast<GLuint>(glGetAttribLocation(shader, "position"));
	glEnableVertexAttribArray(posAttr);
	glVertexAttribPointer(posAttr, 2, GL_FLOAT, GL_FALSE, BgVertexVboRowLength * sizeof(float), nullptr);
	auto texCoordAttr = static_cast<GLuint>(glGetAttribLocation(shader, "vTexCoord"));
	glEnableVertexAttribArray(texCoordAttr);
	glVertexAttribPointer(texCoordAttr, 2, GL_FLOAT, GL_FALSE, BgVertexVboRowLength * sizeof(float),
	                      ox::bit_cast<void*>(2 * sizeof(float)));
}

static Texture loadTexture(GLsizei w, GLsizei h, void *pixels) noexcept {
	GLuint texId = 0;
	glGenTextures(1, &texId);
	Texture tex(texId);
	tex.width = w;
	tex.height = h;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex.id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0,  GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return ox::move(tex);
}

static void tickFps(GlImplData *id) noexcept {
	++id->draws;
	if (id->draws >= 500) {
		using namespace std::chrono;
		const auto now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		const auto duration = static_cast<double>(now - id->prevFpsCheckTime) / 1000.0;
		const auto fps = static_cast<int>(static_cast<double>(id->draws) / duration);
		if constexpr(config::UserlandFpsPrint) {
			oxInfof("FPS: {}", fps);
		}
		oxTracef("nostalgia::core::gfx::gl::fps", "FPS: {}", fps);
		id->prevFpsCheckTime = now;
		id->draws = 0;
	}
}

static void drawBackground(Background *bg) noexcept {
	if (bg->enabled) {
		glBindVertexArray(bg->vao);
		if (bg->updated) {
			bg->updated = false;
			renderer::sendVbo(*bg);
		}
		glBindTexture(GL_TEXTURE_2D, bg->tex);
		glDrawElements(GL_TRIANGLES, bg->bgElements.size(), GL_UNSIGNED_INT, 0);
	}
}

static void drawBackgrounds(GlImplData *id) noexcept {
	// load background shader and its uniforms
	glUseProgram(id->bgShader);
	const auto uniformTileHeight = static_cast<GLint>(glGetUniformLocation(id->bgShader, "vTileHeight"));
	for (auto &bg : id->backgrounds) {
		glUniform1f(uniformTileHeight, 1.0f / static_cast<float>(bg.tex.height / 8));
		drawBackground(&bg);
	}
}

ox::Error init(Context *ctx) noexcept {
	const auto id = new GlImplData;
	ctx->setRendererData(id);
	oxReturnError(buildShaderProgram(bgvshad, bgfshad).moveTo(&id->bgShader));
	for (auto &bg : id->backgrounds) {
		initBackgroundBufferset(ctx, id->bgShader, &bg);
	}
	return OxError(0);
}

ox::Error shutdown(Context *ctx) noexcept {
	const auto id = ctx->rendererData<GlImplData>();
	ctx->setRendererData(nullptr);
	delete id;
	return OxError(0);
}

ox::Error loadBgTexture(Context *ctx, int section, void *pixels, int w, int h) noexcept {
	oxTracef("nostalgia::core::gfx::gl", "loadBgTexture: { section: {}, w: {}, h: {} }", section, w, h);
	const auto &id = ctx->rendererData<GlImplData>();
	auto &tex = id->backgrounds[static_cast<std::size_t>(section)].tex;
	tex = loadTexture(w, h, pixels);
	return OxError(0);
}

}

uint8_t bgStatus(Context *ctx) noexcept {
	const auto &id = ctx->rendererData<renderer::GlImplData>();
	uint8_t out = 0;
	for (unsigned i = 0; i < id->backgrounds.size(); ++i) {
		out |= id->backgrounds[i].enabled << i;
	}
	return out;
}

void setBgStatus(Context *ctx, uint32_t status) noexcept {
	const auto &id = ctx->rendererData<renderer::GlImplData>();
	for (unsigned i = 0; i < id->backgrounds.size(); ++i) {
		id->backgrounds[i].enabled = (status >> i) & 1;
	}
}

bool bgStatus(Context *ctx, unsigned bg) noexcept {
	const auto &id = ctx->rendererData<renderer::GlImplData>();
	return id->backgrounds[bg].enabled;
}

void setBgStatus(Context *ctx, unsigned bg, bool status) noexcept {
	const auto &id = ctx->rendererData<renderer::GlImplData>();
	id->backgrounds[bg].enabled = status;
}


void draw(Context *ctx) noexcept {
	const auto id = ctx->rendererData<renderer::GlImplData>();
	renderer::tickFps(id);
	// clear screen
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	// render
	renderer::drawBackgrounds(id);
}

void clearTileLayer(Context *ctx, int layer) noexcept {
	const auto id = ctx->rendererData<renderer::GlImplData>();
	auto &bg = id->backgrounds[static_cast<std::size_t>(layer)];
	initBackgroundBufferObjects(ctx, &bg);
	bg.updated = true;
}

void hideSprite(Context*, unsigned) noexcept {
}

void setSprite(Context*,
               unsigned,
               unsigned,
               unsigned,
               unsigned,
               unsigned,
               unsigned,
               unsigned) noexcept {
}

void setTile(Context *ctx, int layer, int column, int row, uint8_t tile) noexcept {
	const auto id = ctx->rendererData<renderer::GlImplData>();
	const auto z = static_cast<unsigned>(layer);
	const auto y = static_cast<unsigned>(row);
	const auto x = static_cast<unsigned>(column);
	const auto i = renderer::bgVertexRow(x, y);
	auto &bg = id->backgrounds[z];
	auto vbo = &bg.bgVertices[i * renderer::BgVertexVboLength];
	auto ebo = &bg.bgElements[i * renderer::BgVertexEboLength];
	renderer::setTileBufferObject(ctx, i * renderer::BgVertexVboRows, x, y, tile, vbo, ebo);
	bg.updated = true;
}

}
