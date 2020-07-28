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

struct Bolt {
	unsigned m_id = 0;
	unsigned m_tile = 0;
	unsigned m_ticksToLive = 0;
	bool m_active = false;
	int m_x = 0;
	int m_y = 0;
	int m_xVel = 0;

	Bolt(unsigned id, unsigned tile);

	void init();

	void update();

	void fire(int x, int y, bool left);

	int tipX() {
		// add 4 for whitespace around remote sprite
		if (m_xVel < 0) {
			return m_x + 4;
		}
		return m_x + 8 + 4;
	}

	int tipY() {
		return m_y + 3;
	}

};

struct Character {
	static constexpr auto Width = 16;
	static constexpr auto Height = 32;
	unsigned m_tile = 0;
	Bolt *m_bolt = nullptr;
	unsigned m_id = 0;
	int m_height = 32;
	int m_defaultX = 0;
	int m_x = 72;
	int m_y = GroundY - m_height;
	int m_yVel = 0;
	int m_xSpeed = 2;

	Character(unsigned id, Bolt *bolt, int x, unsigned tile);

	void init();

	void readUserInput(Character *opponent);

	void runAI(Character *opponent);

	void update(Character *opponent, Bolt *opponentBolt);

};

Bolt playerBolt(1, 32);
Bolt opponentBolt(3, 33);
Character player(0, &playerBolt, 0 + Character::Width, 16);
Character opponent(2, &opponentBolt, 240 - (10 + Character::Width), 24);
core::Context ctx;

void initGameOverScreen(Character *winner);


Bolt::Bolt(unsigned id, unsigned tile) {
	m_id = id;
	m_tile = tile;
}

void Bolt::init() {
	m_ticksToLive = 0;
	m_active = false;
	core::hideSprite(m_id);
}

void Bolt::update() {
	if (m_ticksToLive) {
		--m_ticksToLive;
		m_x += m_xVel;
		if (m_x > 240 || m_x <= 0) {
			core::hideSprite(m_id);
			m_active = false;
		} else {
			core::setSprite(m_id, static_cast<unsigned>(m_x), static_cast<unsigned>(m_y), m_tile);
		}
	} else {
		core::hideSprite(m_id);
	}
}

void Bolt::fire(int x, int y, bool left) {
	if (m_ticksToLive == 0) {
		constexpr auto Vel = 4;
		m_x = x;
		m_y = y;
		if (left) {
			m_xVel = -Vel;
		} else {
			m_xVel = Vel;
		}
		m_active = true;
		m_ticksToLive = 30 * 2;
	}
}


Character::Character(unsigned id, Bolt *bolt, int x, unsigned tile) {
	m_id = id;
	m_bolt = bolt;
	m_x = x;
	m_defaultX = x;
	m_tile = tile;
}

void Character::init() {
	m_x = m_defaultX;
	m_y = GroundY - m_height;
	core::setSprite(m_id, static_cast<unsigned>(m_x), static_cast<unsigned>(m_y), m_tile, 2, 2, 1);
}

void Character::readUserInput(Character *opponent) {
	if (core::buttonDown(core::GamePad_Right)) {
		if (m_x + Width < 240) {
			m_x += m_xSpeed;
		}
	} else if (core::buttonDown(core::GamePad_Left)) {
		if (m_x > 0) {
			m_x -= m_xSpeed;
		}
	}
	if (core::buttonDown(core::GamePad_A) && m_y + m_height == GroundY) {
		m_yVel = -9;
	}
	if (core::buttonDown(core::GamePad_B)) {
		m_bolt->fire(m_x, m_y + 16, m_x > opponent->m_x);
	}
}

void Character::runAI(Character *opponent) {
	// start jumping if opponent->m_bolt is active
	if (opponent->m_bolt->m_active) {
		if (m_y + m_height == GroundY) {
			m_yVel = -9;
		}
		if (opponent->m_bolt->m_xVel < 0 && m_x > 0) {
			m_x -= m_xSpeed;
		} else if (m_x + Width < 240) {
			m_x += m_xSpeed;
		}
	}
	if ((core::ticksMs() & 2) == 2) {
		if ((player.m_x >> 2) & 1 || (core::ticksMs() & 8) == 8) {
			m_bolt->fire(m_x, m_y + 16, m_x > opponent->m_x);
		}
		if (!opponent->m_bolt->m_active) {
			if (m_x > 0 && m_x > opponent->m_x) {
				m_x -= m_xSpeed;
			} else if (m_x + Width < 240) {
				m_x += m_xSpeed;
			}
		}
	}
}

void Character::update(Character *opponent, Bolt *opponentBolt) {
	if (m_yVel || m_y + m_height != GroundY) {
		m_y += m_yVel;
		m_yVel++;
		if (m_y >= GroundY - m_height) {
			m_y = GroundY - m_height;
			m_yVel = 0;
		}
	}
	// check if enemy bolt is in this sprite
	if (opponentBolt->m_active &&
		 m_x +  4 <= opponentBolt->tipX() && m_x + (Width-4) >= opponentBolt->tipX() &&
	    m_y + 12 <= opponentBolt->tipY() && m_y + Height >= opponentBolt->tipY()) {
		initGameOverScreen(opponent);
	}
	// update sprites
	auto flip = m_x > opponent->m_x;
	core::setSprite(m_id, static_cast<unsigned>(m_x), static_cast<unsigned>(m_y), m_tile, 2, 2, flip);
}

static void createCloud(int x, int y) {
	core::setTile(&ctx, 0, x + 0, y + 0, 12);
	core::setTile(&ctx, 0, x + 1, y + 0, 13);
	core::setTile(&ctx, 0, x + 0, y + 1, 14);
	core::setTile(&ctx, 0, x + 1, y + 1, 15);
}

int arenaLoop() {
	player.readUserInput(&opponent);
	player.update(&opponent, &opponentBolt);
	opponent.runAI(&player);
	opponent.update(&player, &playerBolt);
	playerBolt.update();
	opponentBolt.update();
	return 32;
}

ox::Error initArena() {
	// rom FS needs to be reloaded for some reason
	ctx.rom = core::loadRomFs("rom.oxfs");
	// setup scene
	constexpr auto TileSheetAddr = "/TileSheets/Background.ng";
	constexpr auto PaletteAddr = "/Palettes/Background.npal";
	oxReturnError(core::loadBgTileSheet(&ctx, 0, TileSheetAddr, PaletteAddr));
	oxReturnError(core::loadSpriteTileSheet(&ctx, 0, TileSheetAddr, PaletteAddr));
	// setup sky
	for (int ii = 0; ii < 16; ii += 2) {
		for (int i = 0; i < 32; i += 2) {
			core::setTile(&ctx, 0, i + 0, ii + 0, 8);
			core::setTile(&ctx, 0, i + 1, ii + 0, 9);
			core::setTile(&ctx, 0, i + 0, ii + 1, 10);
			core::setTile(&ctx, 0, i + 1, ii + 1, 11);
		}
	}
	createCloud(8, 10);
	createCloud(22, 7);
	createCloud(3, 4);
	createCloud(16, 2);
	// setup ground
	for (int ii = 16; ii < 32; ii += 2) {
		for (int i = 0; i < 32; i += 2) {
			core::setTile(&ctx, 0, i + 0, ii + 0, 4);
			core::setTile(&ctx, 0, i + 1, ii + 0, 5);
			core::setTile(&ctx, 0, i + 0, ii + 1, 6);
			core::setTile(&ctx, 0, i + 1, ii + 1, 7);
		}
	}
	player.init();
	opponent.init();
	playerBolt.init();
	opponentBolt.init();
	core::setEventHandler(arenaLoop);
	return OxError(0);
}

int gameOverLoop() {
	if (core::buttonDown(core::GamePad_Start)) {
		initArena();
	}
	return 16;
}

void initGameOverScreen(Character *winner) {
	// rom FS needs to be reloaded for some reason
	ctx.rom = core::loadRomFs("rom.oxfs");
	oxIgnoreError(core::initConsole(&ctx));
	core::clearTileLayer(nullptr, 0);
	if (winner == &player) {
		core::puts(&ctx, 6, 4, "You're a murderer!");
		core::puts(&ctx, 3, 10, "Press Start To Play Again");
	} else {
		core::puts(&ctx, 8, 4, "You're a loser!");
		core::puts(&ctx, 3, 10, "Press Start To Play Again");
	}
	core::setEventHandler(gameOverLoop);
}

int mainScreenLoop() {
	if (core::buttonDown(core::GamePad_Start)) {
		initArena();
	}
	return 16;
}

ox::Error initMainScreen() {
	oxReturnError(core::initConsole(&ctx));
	core::clearTileLayer(nullptr, 0);
	core::puts(&ctx, 8, 4, "Remote Fighter!");
	core::puts(&ctx, 10, 10, "Press Start");
	core::puts(&ctx, 0, 18, "Drinking Tea @");
	core::setEventHandler(mainScreenLoop);
	return OxError(0);
}

ox::Error run(ox::FileSystem *fs) {
	ctx.rom = fs;
	oxReturnError(core::init(&ctx));
	for (unsigned i = 0; i < 128; ++i) {
		core::hideSprite(i);
	}
	oxReturnError(initMainScreen());
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
