/***************************************************************************
*   Copyright (C) 2005-2011 by the FIFE team                              *
*   http://www.fifengine.net                                              *
*   This file is part of FIFE.                                            *
*                                                                         *
*   FIFE is free software; you can redistribute it and/or                 *
*   modify it under the terms of the GNU Lesser General Public            *
*   License as published by the Free Software Foundation; either          *
*   version 2.1 of the License, or (at your option) any later version.    *
*                                                                         *
*   This library is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
*   Lesser General Public License for more details.                       *
*                                                                         *
*   You should have received a copy of the GNU Lesser General Public      *
*   License along with this library; if not, write to the                 *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
***************************************************************************/

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "ext/tinyxml/fife_tinyxml.h"
#include "model/structures/map.h"
#include "model/structures/layer.h"
#include "model/structures/instance.h"
#include "model/structures/cell.h"
#include "model/structures/cellcache.h"
#include "model/metamodel/object.h"
#include "model/metamodel/grids/cellgrid.h"
#include "util/structures/point.h"
#include "util/structures/rect.h"
#include "view/visual.h"
#include "view/camera.h"

#include "mapsaver.h"

namespace FIFE {
    static Logger _log(LM_NATIVE_SAVERS);

    MapSaver::MapSaver() {
        //m_objectSaver = new ObjectSaver();
        //m_animationSaver = new AnimationSaver();
        //m_atlasSaver = new AtlasSaver();
    }

    MapSaver::~MapSaver()
    {

    }

    void MapSaver::setObjectSaver(const FIFE::ObjectSaverPtr& objectSaver) {
        m_objectSaver = objectSaver;
    }


    void MapSaver::setAnimationSaver(const FIFE::AnimationSaverPtr& animationSaver) {
        m_animationSaver = animationSaver;
    }


    void MapSaver::setAtlasSaver(const FIFE::AtlasSaverPtr& atlasSaver) {
        m_atlasSaver = atlasSaver;
    }

    void MapSaver::save(const Map& map, const std::string& filename, const std::vector<std::string>& importFiles) {
        TiXmlDocument doc;

        // add xml declaration
        TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "ascii", "");
        doc.LinkEndChild(decl);

        // add map element
        TiXmlElement* mapElement = new TiXmlElement("map");
        mapElement->SetAttribute("id", map.getId());
        mapElement->SetAttribute("format", "1.0");
        doc.LinkEndChild(mapElement);

        for (std::vector<std::string>::const_iterator iter = importFiles.begin(); iter != importFiles.end(); ++iter)
        {
            TiXmlElement* importElement = new TiXmlElement("import");
            importElement->SetAttribute("file", *iter);

            // link to map element
            mapElement->LinkEndChild(importElement);
        }

        typedef std::list<Layer*> LayerList;
        LayerList layers = map.getLayers();
        for (LayerList::iterator iter = layers.begin(); iter != layers.end(); ++iter)
        {
            TiXmlElement* layerElement = new TiXmlElement("layer");
            CellGrid* grid = (*iter)->getCellGrid();
            layerElement->SetAttribute("id", (*iter)->getId());
            layerElement->SetDoubleAttribute("x_offset", grid->getXShift());
            layerElement->SetDoubleAttribute("y_offset", grid->getYShift());
            layerElement->SetDoubleAttribute("z_offset", grid->getZShift());
            layerElement->SetDoubleAttribute("x_scale", grid->getXScale());
            layerElement->SetDoubleAttribute("y_scale", grid->getYScale());
            layerElement->SetDoubleAttribute("rotation", grid->getRotation());
            layerElement->SetAttribute("grid_type", grid->getType());
            layerElement->SetAttribute("transparency", (*iter)->getLayerTransparency());

            std::string pathingStrategy;
            switch ((*iter)->getPathingStrategy())
            {
                case CELL_EDGES_ONLY:
                {
                    pathingStrategy = "cell_edges_only";
                }
                break;
                case CELL_EDGES_AND_DIAGONALS:
                {
                    pathingStrategy = "cell_edges_and_diagonals";
                }
                break;
                default:
                {
                    pathingStrategy = "cell_edges_only";
                }
                break;
            }
            layerElement->SetAttribute("pathing", pathingStrategy);
            
			if ((*iter)->isWalkable()) {
				layerElement->SetAttribute("layer_type", "walkable");
			} else if ((*iter)->isInteract()) {
				layerElement->SetAttribute("layer_type", "interact");
				layerElement->SetAttribute("layer_type_id", (*iter)->getWalkableId());
			}

            // add layer to document
            mapElement->LinkEndChild(layerElement);
            
            // add instances tag to document
            TiXmlElement* instancesElement = new TiXmlElement("instances");
            layerElement->LinkEndChild(instancesElement);

            std::string currentNamespace = "";
            typedef std::vector<Instance*> InstancesContainer;
            InstancesContainer instances = (*iter)->getInstances();
            for (InstancesContainer::iterator iter = instances.begin(); iter != instances.end(); ++iter)
            {
				Object* obj = (*iter)->getObject();
                // don't save part instances
				if (obj->isMultiPart()) {
					continue;
				}

				// create instance element
                TiXmlElement* instanceElement = new TiXmlElement("i");

                if (!obj->getNamespace().empty() && currentNamespace != obj->getNamespace())
                {
                    instanceElement->SetAttribute("ns", obj->getNamespace());
                    
                    // update current namespace
                    currentNamespace = obj->getNamespace();
                }

                if (!(*iter)->getId().empty())
                {
                    instanceElement->SetAttribute("id", (*iter)->getId());
                }

                instanceElement->SetAttribute("o", obj->getId());

                ExactModelCoordinate position = (*iter)->getLocationRef().getExactLayerCoordinates();
                instanceElement->SetDoubleAttribute("x", position.x);
                instanceElement->SetDoubleAttribute("y", position.y);
                instanceElement->SetDoubleAttribute("z", position.z);
                instanceElement->SetAttribute("r", (*iter)->getRotation());

                if ((*iter)->isBlocking())
                {
                    instanceElement->SetAttribute("blocking", (*iter)->isBlocking());    
                }

				if ((*iter)->getCellStackPosition() != obj->getCellStackPosition()) {
					instanceElement->SetAttribute("cellstack", (*iter)->getCellStackPosition());
				}

				if ((*iter)->isVisitor()) {
					instanceElement->SetAttribute("visitor_radius", (*iter)->getVisitorRadius());
					std::string shape("none");
					if ((*iter)->getVisitorShape() == ITYPE_QUAD_SHAPE) {
						shape = "quad";
					} else if ((*iter)->getVisitorShape() == ITYPE_CIRCLE_SHAPE) {
						shape = "circle";
					}
					instanceElement->SetAttribute("visitor_shape", shape);
				}

				if ((*iter)->isSpecialCost()) {
					if (!obj->isSpecialCost()) {
						instanceElement->SetAttribute("cost_id", (*iter)->getCostId());
						instanceElement->SetDoubleAttribute("cost", (*iter)->getCost());
					} else if ((*iter)->getCostId() != obj->getCostId() ||
						!Mathd::Equal((*iter)->getCost(), obj->getCost())) {
						instanceElement->SetAttribute("cost_id", (*iter)->getCostId());
						instanceElement->SetDoubleAttribute("cost", (*iter)->getCost());
					}
				}

                InstanceVisual* instanceVisual = (*iter)->getVisual<InstanceVisual>();
                instanceElement->SetAttribute("stackpos", instanceVisual->getStackPosition());

                instancesElement->LinkEndChild(instanceElement);
            }
        }
		// add cellcaches tag to document
		TiXmlElement* cellcachesElement = new TiXmlElement("cellcaches");
		mapElement->LinkEndChild(cellcachesElement);
		for (LayerList::iterator iter = layers.begin(); iter != layers.end(); ++iter) {
			CellCache* cache = (*iter)->getCellCache();
			if (!cache) {
				continue;
			}
			// add cellcache tag to document
			TiXmlElement* cellcacheElement = new TiXmlElement("cellcache");
			cellcacheElement->SetAttribute("id", (*iter)->getId());
			cellcacheElement->SetDoubleAttribute("default_cost", cache->getDefaultCostMultiplier());
			cellcacheElement->SetDoubleAttribute("default_speed", cache->getDefaultSpeedMultiplier());
			cellcacheElement->SetAttribute("search_narrow", cache->isSearchNarrowCells());

			const std::set<Cell*>& narrowCells = cache->getNarrowCells();
			bool saveNarrows = !cache->isSearchNarrowCells() && !narrowCells.empty();

			const std::vector<std::vector<Cell*> >& cells = cache->getCells();
			std::vector<std::vector<Cell*> >::const_iterator it = cells.begin();
			for (; it != cells.end(); ++it) {
				std::vector<Cell*>::const_iterator cit = (*it).begin();
				for (; cit != (*it).end(); ++cit) {
					Cell* cell = *cit;
					std::list<std::string> costIds = cache->getCosts();
					bool costsEmpty = costIds.empty();
					bool defaultCost = cell->defaultCost();
					bool defaultSpeed = cell->defaultSpeed();
					CellVisualEffect cve = cell->getFoWType();
					bool cellVisual = cve == CELLV_CONCEALED;
					CellTypeInfo cti = cell->getCellType();
					bool cellBlocker = (cti != CTYPE_CELL_NO_BLOCKER && cti != CTYPE_CELL_BLOCKER);
					TransitionInfo* transition = cell->getTransition();
					bool isNarrow = false;
					if (saveNarrows) {
						std::set<Cell*>::const_iterator narrow_it = narrowCells.find(cell);
						if (narrow_it != narrowCells.end()) {
							isNarrow = true;
						}
					}
					if (costsEmpty && defaultCost && defaultSpeed &&
						cellVisual && cellBlocker && !transition && !isNarrow) {
						continue;
					}
					// add cell tag to document
					ModelCoordinate cellCoord = cell->getLayerCoordinates();
					TiXmlElement* cellElement = new TiXmlElement("cell");
					cellElement->SetAttribute("x", cellCoord.x);
					cellElement->SetAttribute("y", cellCoord.y);
					if (!defaultCost) {
						cellElement->SetDoubleAttribute("default_cost", cell->getCostMultiplier());
					}
					if (!defaultSpeed) {
						cellElement->SetDoubleAttribute("default_speed", cell->getSpeedMultiplier());
					}
					if (!cellVisual) {
						if (cve == CELLV_REVEALED) {
							cellElement->SetAttribute("state", "revealed");
						} else {
							cellElement->SetAttribute("state", "masked");
						}
					}
					if (!cellBlocker) {
						if (cti == CTYPE_CELL_NO_BLOCKER) {
							cellElement->SetAttribute("blocker_type", "no_blocker");
						} else {
							cellElement->SetAttribute("blocker_type", "blocker");
						}
					}
					if (isNarrow) {
						cellElement->SetAttribute("narrow", true);
					}
					// add cost tag
					if (!costsEmpty) {
						std::list<std::string>::iterator cost_it = costIds.begin();
						for (; cost_it != costIds.end(); ++cost_it) {
							if (cache->existsCostForCell(*cost_it, cell)) {
								TiXmlElement* costElement = new TiXmlElement("cost");
								costElement->SetAttribute("id", *cost_it);
								costElement->SetDoubleAttribute("value", cache->getCost(*cost_it));
								cellElement->LinkEndChild(costElement);
							}
						}
					}
					// add transition tag
					if (transition) {
						TiXmlElement* transitionElement = new TiXmlElement("transition");
						transitionElement->SetAttribute("id", transition->m_layer->getId());
						transitionElement->SetAttribute("x", transition->m_mc.x);
						transitionElement->SetAttribute("y", transition->m_mc.y);
						if (transition->m_immediate) {
							transitionElement->SetAttribute("immediate", true);
						} else {
							transitionElement->SetAttribute("immediate", false);
						}
						cellElement->LinkEndChild(transitionElement);
					}
					cellcacheElement->LinkEndChild(cellElement);
				}
			}
			cellcachesElement->LinkEndChild(cellcacheElement);
        }
        typedef std::vector<Camera*> CameraContainer;
        CameraContainer cameras = map.getCameras();
        for (CameraContainer::iterator iter = cameras.begin(); iter != cameras.end(); ++iter)
        {
            if ((*iter)->getLocationRef().getMap()->getId() == map.getId())
            {
                TiXmlElement* cameraElement = new TiXmlElement("camera");

                cameraElement->SetAttribute("id", (*iter)->getId());
                cameraElement->SetAttribute("ref_layer_id", (*iter)->getLocation().getLayer()->getId());
                cameraElement->SetDoubleAttribute("zoom", (*iter)->getZoom());
                cameraElement->SetDoubleAttribute("tilt", (*iter)->getTilt());
                cameraElement->SetDoubleAttribute("rotation", (*iter)->getRotation());
                
                Rect viewport = (*iter)->getViewPort();
                std::ostringstream viewportString;
                viewportString << viewport.x << "," 
                               << viewport.y << ","
                               << viewport.w << ","
                               << viewport.h;

                cameraElement->SetAttribute("viewport", viewportString.str());

                Point p = (*iter)->getCellImageDimensions();
                cameraElement->SetAttribute("ref_cell_width", p.x);
                cameraElement->SetAttribute("ref_cell_height", p.y);

                std::vector<float> lightingColor = (*iter)->getLightingColor();
                bool writeLightingColor = false;
                for (uint32_t i=0; i < lightingColor.size(); ++i)
                {
                    if (lightingColor[i] < 1.0)
                    {
                        writeLightingColor = true;
                        break;
                    }
                }

                if (writeLightingColor)
                {
                    std::ostringstream lightingColorString;
                    for (uint32_t i=0; i < lightingColor.size(); ++i)
                    {
                        if (i > 0)
                        {
                            lightingColorString << ",";
                        }
                        
                        lightingColorString << lightingColor[i];

                        cameraElement->SetAttribute("light_color", lightingColorString.str());
                    }
                }
                
                mapElement->LinkEndChild(cameraElement);
            }
        }

        // save the map xml file
        doc.SaveFile(filename);
    }
}