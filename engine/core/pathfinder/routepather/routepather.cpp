/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include <cassert>

#include "pathfinder/searchspace.h"

#include "routepather.h"
#include "routepathersearch.h"

namespace FIFE {
	void RoutePather::setMap(Map* map) {
		if(!map) {
			return;
		}
		m_map = map;
	}

	int RoutePather::getNextLocation(const Instance* instance, const Location& target, 
				double distance_to_travel, Location& nextLocation,
				Location& facingLocation, int session_id) {
		assert(instance);
		assert(instance->getLocation().getLayer() == target.getLayer());
		if(session_id != -1) {
			//This means we're updating a session.
			SessionMap::iterator i = m_sessions.find(session_id);
			if(i != m_sessions.end()) {
				i->second->updateSearch();
				if(i->second->getSearchStatus() == Search::search_status_complete) {
					Path newPath = i->second->calcPath();
					m_paths.insert(PathMap::value_type(session_id, newPath));
					m_sessions.erase(i);
					return session_id;
				} else if(i->second->getSearchStatus() == Search::search_status_failed) {
					m_sessions.erase(i);
					return -1;
				}
			} else {
				//Check to see if this session is in the movement phase.
				PathMap::iterator j = m_paths.find(session_id);
				if(j != m_paths.end()) {
					if(j->second.empty()) {
						m_paths.erase(j);
						return -1;
					} else {
						followPath(instance, j->second, distance_to_travel, nextLocation, facingLocation);
						return session_id;
					}
				}
			}
		}
		//TODO: Create a new session.
		if((instance->getLocation().getLayer() != target.getLayer()) || (instance->getLocation().getLayerCoordinates() ==
			target.getLayerCoordinates())) {
			return -1;
		}
		SearchSpaceMap::iterator i = m_searchspaces.find(target.getLayer());
		if(i == m_searchspaces.end()) {
			SearchSpace* newSearchSpace = new SearchSpace(target.getLayer());
			i = m_searchspaces.insert(SearchSpaceMap::value_type(target.getLayer(), newSearchSpace)).first;
		}
		session_id = m_nextFreeSessionId++;
		RoutePatherSearch* newSearch = new RoutePatherSearch(session_id, instance->getLocation(), target, i->second);
		m_sessions.insert(SessionMap::value_type(session_id, newSearch));
		return session_id;
	}

	void RoutePather::followPath(const Instance* instance, Path& path, double speed, Location& nextLocation, Location& facingLocation) {
		//First check to see if we're at the location at the front of the path.
		if(path.front().getExactLayerCoordinates() == instance->getLocation().getExactLayerCoordinates()) {
			//If we are remove it.
			path.pop_front();
			if(path.empty()) {
				return;
			}
		} 

		nextLocation = instance->getLocation();
		facingLocation = path.front();
		DoublePoint3D desiredVelocity = path.front().getExactLayerCoordinates() - nextLocation.getExactLayerCoordinates();
		double length = desiredVelocity.length();
		if(length != 0) {
			//Make sure we don't go over the target location.
			if(speed < length) {
				desiredVelocity = (desiredVelocity / length) * speed;
				nextLocation.setExactLayerCoordinates(nextLocation.getExactLayerCoordinates() + desiredVelocity);
				nextLocation.setLayerCoordinates(FIFE::doublePt2intPt(nextLocation.getExactLayerCoordinates()));
			} 
		}
	}
	
	bool RoutePather::cancelSession(const int session_id) {
		if(session_id >= 0) {
			SessionMap::iterator i = m_sessions.find(session_id);
			if(i != m_sessions.end()) {
				delete i->second;
				m_sessions.erase(i);
				return true;
			}
		}
		return false;
	}
}