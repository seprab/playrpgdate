/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2013-2016 Justin Jacobs

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/

/*
 * MapCollision.h
 * RPGEngine
 *
 * Handle collisions between objects and the map
 */

#ifndef MAP_COLLISION_H
#define MAP_COLLISION_H

#include "pdcpp/graphics/ImageTable.h"

typedef std::vector< std::vector<unsigned short>> Map_Layer;

class MapCollision {
private:
	static const float MIN_TILE_GAP;

	// collision check types
	enum {
		CHECK_MOVEMENT = 1,
		CHECK_SIGHT = 2
	};

	bool isTileOutsideMap(const int& tile_x, const int& tile_y) const;

	bool lineCheck(const float& x1, const float& y1, const float& x2, const float& y2, int check_type, int movement_type);

	bool smallStepForcedSlideAlongGrid(
		float &x, float &y, float step_x, float step_y, int movement_type, int collide_type);
	bool smallStepForcedSlide(
		float &x, float &y, float step_x, float step_y, int movement_type, int collide_type);
	bool smallStep(
		float &x, float &y, float step_x, float step_y, int movement_type, int collide_type);

	bool isValidTile(const int& x, const int& y, int movement_type, int collide_type) const;

    pdcpp::Point<int> collisionToMap(const pdcpp::Point<int>& p);

	bool has_empty_tile;

public:
	// const flags
	static const bool IS_ALLY = true;
	static const int DEFAULT_PATH_LIMIT = 0;

	// entity collision type
	enum {
		ENTITY_COLLIDE_ALL = 0,
		ENTITY_COLLIDE_HERO = 1,
		ENTITY_COLLIDE_NONE = 2
	};

	// movement options
	enum {
		MOVE_NORMAL = 0,
		MOVE_FLYING = 1, // can move through BLOCKS_MOVEMENT (e.g. water)
		MOVE_INTANGIBLE = 2 // can move through BLOCKS_ALL (e.g. walls)
	};

	// collision tile types
	// The numbers 0..6 are the collision tiles as produced by tiled,
	// only 7 and 8 deal with entities on the map
	enum {
		BLOCKS_NONE = 0,
		BLOCKS_ALL = 1,
		BLOCKS_MOVEMENT = 2,
		BLOCKS_ALL_HIDDEN = 3,
		BLOCKS_MOVEMENT_HIDDEN = 4,
		MAP_ONLY = 5,
		MAP_ONLY_ALT = 6,
		BLOCKS_ENTITIES = 7, // hero or enemies are blocking this tile, so any other entity is blocked
		BLOCKS_ENEMIES = 8  // an ally is standing on that tile, so the hero could pass if ENABLE_ALLY_COLLISION is false
	};

	MapCollision();
	~MapCollision();

	void SetMap(const Map_Layer& _colmap, unsigned short w, unsigned short h);
	bool Move(float &x, float &y, float step_x, float step_y, int movement_type, int collide_type);

	bool isOutsideMap(const float& tile_x, const float& tile_y) const;
	bool isWall(const float& x, const float& y) const;

	bool isValidPosition(const float& x, const float& y, int movement_type, int collide_type) const;

	bool lineOfSight(const float& x1, const float& y1, const float& x2, const float& y2);
	bool lineOfMovement(const float& x1, const float& y1, const float& x2, const float& y2, int movement_type);

	bool isFacing(const float& x1, const float& y1, char direction, const float& x2, const float& y2);

	bool ComputePath(const pdcpp::Point<int>& start, const pdcpp::Point<int>& end, std::vector<pdcpp::Point<int>> &path, int movement_type, unsigned int limit);

	void block(const float& map_x, const float& map_y, bool is_ally);
	void unblock(const float& map_x, const float& map_y);

    pdcpp::Point<int> getRandomNeighbor(const pdcpp::Point<int>& target, int range, int movement_type, int collide_type);

	int getCollideType(bool hero) {
		return hero ? ENTITY_COLLIDE_HERO : ENTITY_COLLIDE_ALL;
	}

	bool hasEmptyTile() { return has_empty_tile; }

	Map_Layer colmap;
    pdcpp::Point<int> map_size;
};

#endif
