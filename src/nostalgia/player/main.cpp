/*
 * Copyright 2016 - 2020 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/fs/fs.hpp>
#include <ox/std/units.hpp>
#include <nostalgia/core/core.hpp>
#include <nostalgia/core/input.hpp>
#include <nostalgia/world/world.hpp>

using namespace nostalgia;

constexpr auto GroundY = 128;

struct Character {
	unsigned m_tile = 0;
	unsigned m_id = 0;
	int m_height = 32;
	int m_x = 72;
	int m_y = GroundY - m_height;
	int m_yVel = 0;
	int m_xSpeed = 2;

	Character(unsigned m_id, int x, unsigned tile) {
		this->m_id = m_id;
		m_x = x;
		m_tile = tile;
	}

	void init() {
		core::setSprite(m_id, static_cast<unsigned>(m_x), static_cast<unsigned>(m_y), m_tile, 2, 1);
	}

	void readUserInput() {
		if (core::buttonDown(core::GamePad_Right)) {
			if (m_x + 16 < 240) {
				m_x += m_xSpeed;
			}
		} else if (core::buttonDown(core::GamePad_Left)) {
			if (m_x > 0) {
				m_x -= m_xSpeed;
			}
		}
		if (core::buttonDown(core::GamePad_A) && m_y + m_height == GroundY) {
			m_yVel = -10;
		}
	}

	void update() {
		if (m_yVel || m_y + m_height != GroundY) {
			m_y += m_yVel;
			m_yVel++;
			if (m_y >= GroundY - m_height) {
				m_y = GroundY - m_height;
				m_yVel = 0;
			}
		}
		// update sprites
		core::setSprite(m_id, static_cast<unsigned>(m_x), static_cast<unsigned>(m_y), m_tile, 2, 2);
	}
};

Character player(0, 72, 16);
Character opponent(1, 132, 24);

static void createCloud(core::Context *ctx, int x, int y) {
	core::setTile(ctx, 0, x + 0, y + 0, 12);
	core::setTile(ctx, 0, x + 1, y + 0, 13);
	core::setTile(ctx, 0, x + 0, y + 1, 14);
	core::setTile(ctx, 0, x + 1, y + 1, 15);
}

ox::Error initArena(core::Context *ctx) {
	constexpr auto TileSheetAddr = "/TileSheets/Background.ng";
	constexpr auto PaletteAddr = "/Palettes/Background.npal";
	oxReturnError(core::loadBgTileSheet(ctx, 0, TileSheetAddr, PaletteAddr));
	oxReturnError(core::loadSpriteTileSheet(ctx, 0, TileSheetAddr, PaletteAddr));
	// setup sky
	for (int ii = 0; ii < 16; ii += 2) {
		for (int i = 0; i < 32; i += 2) {
			core::setTile(ctx, 0, i + 0, ii + 0, 8);
			core::setTile(ctx, 0, i + 1, ii + 0, 9);
			core::setTile(ctx, 0, i + 0, ii + 1, 10);
			core::setTile(ctx, 0, i + 1, ii + 1, 11);
		}
	}
	createCloud(ctx, 8, 10);
	createCloud(ctx, 22, 7);
	createCloud(ctx, 3, 4);
	createCloud(ctx, 16, 2);
	// setup ground
	for (int ii = 16; ii < 32; ii += 2) {
		for (int i = 0; i < 32; i += 2) {
			core::setTile(ctx, 0, i + 0, ii + 0, 4);
			core::setTile(ctx, 0, i + 1, ii + 0, 5);
			core::setTile(ctx, 0, i + 0, ii + 1, 6);
			core::setTile(ctx, 0, i + 1, ii + 1, 7);
		}
	}
	player.init();
	opponent.init();
	return OxError(0);
}

int arenaLoop() {
	player.readUserInput();
	player.update();
	opponent.update();
	return 32;
}

ox::Error initMainScreen(core::Context *ctx) {
	oxReturnError(core::initConsole(ctx));
	core::puts(ctx, 8, 4, "Remote Fighter!");
	core::puts(ctx, 10, 10, "Press Start");
	return OxError(0);
}

ox::Error run(ox::FileSystem *fs) {
	core::Context ctx;
	ctx.rom = fs;
	oxReturnError(core::init(&ctx));
	oxReturnError(initArena(&ctx));
	core::setEventHandler(arenaLoop);
	oxReturnError(core::run(&ctx));
	oxReturnError(core::shutdownGfx(&ctx));
	return OxError(0);
}

int main(int argc, const char **argv) {
	if (argc > 1) {
		auto path = argv[1];
		auto fs = core::loadRomFs(path);
		auto err = run(fs);
		oxAssert(err, "Something went wrong...");
		delete fs;
		return err;
	}
	return 1;
}
