/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2011-2012 Nojan
Copyright © 2012 Stefan Beller
Copyright © 2012 Justin Jacobs

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

#include "AStarNode.h"

AStarNode::AStarNode()
	: x(0)
	, y(0)
	, g(0)
	, h(0)
	, parent({0,0}) {
}

AStarNode::AStarNode(const pdcpp::Point<int> &p)
	: x(p.x)
	, y(p.y)
	, g(0)
	, h(0)
	, parent({0,0}) {
}

int AStarNode::getX() const {
	return x;
}

int AStarNode::getY() const {
	return y;
}

float AStarNode::getH() const {
	return h;
}

pdcpp::Point<int> AStarNode::getParent() const {
	return parent;
}

void AStarNode::setParent(const pdcpp::Point<int>& p) {
	this->parent = p;
}

std::list<pdcpp::Point<int>> AStarNode::getNeighbours(int limitX, int limitY) const {
    pdcpp::Point<int> toAdd =  {0,0};
	std::list<pdcpp::Point<int>> res;
	if (x>node_stride && y>node_stride) {
		toAdd.x = x-node_stride;
		toAdd.y = y-node_stride;
		res.push_back(toAdd);
	}
	if (x>node_stride && (limitY==0 || y<limitY-node_stride)) {
		toAdd.x = x-node_stride;
		toAdd.y = y+node_stride;
		res.push_back(toAdd);
	}
	if (y>node_stride && (limitX==0 || x<limitX-node_stride)) {
		toAdd.x = x+node_stride;
		toAdd.y = y-node_stride;
		res.push_back(toAdd);
	}
	if ((limitX==0 || x<limitX-node_stride) && (limitY==0 || y<limitY-node_stride)) {
		toAdd.x = x+node_stride;
		toAdd.y = y+node_stride;
		res.push_back(toAdd);
	}
	if (x>node_stride) {
		toAdd.x = x-node_stride;
		toAdd.y = y;
		res.push_back(toAdd);
	}
	if (y>node_stride) {
		toAdd.x = x;
		toAdd.y = y-node_stride;
		res.push_back(toAdd);
	}
	if (limitX==0 || x<limitX-node_stride) {
		toAdd.x = x+node_stride;
		toAdd.y = y;
		res.push_back(toAdd);
	}
	if (limitY==0 || y<limitY-node_stride) {
		toAdd.x = x;
		toAdd.y = y+node_stride;
		res.push_back(toAdd);
	}

	return res;
}


float AStarNode::getActualCost() const {
	return g;
}

void AStarNode::setActualCost(const float G) {
	g = G;
}

void AStarNode::setEstimatedCost(const float H) {
	h = H;
}

float AStarNode::getFinalCost() const {
	return g+h*2.f;
}

bool AStarNode::operator<(const AStarNode& n) const {
	return getFinalCost() < n.getFinalCost();
}

bool AStarNode::operator==(const AStarNode& n) const {
	return x == n.x && y == n.y;
}

bool AStarNode::operator==(const pdcpp::Point<int>& p) const {
	return x == p.x && y == p.y;
}

bool AStarNode::operator!=(const pdcpp::Point<int>& p) const {
	return x != p.x || y != p.y;
}
