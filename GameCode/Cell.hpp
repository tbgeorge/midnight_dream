//=================================================================================
// Cell.hpp
// Author: Tyler George
// Date  : August 19, 2015
//=================================================================================

#pragma once

#ifndef __included_Cell__
#define __included_Cell__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include <vector>

#include "Engine\Math\AABB2D.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Renderer\OpenGLRenderer.hpp"
#include "Engine\Renderer\FontRenderer.hpp"

class Entity;
class Feature;
class Inventory;
class Item;
class Map;

///---------------------------------------------------------------------------------
/// Enums
///---------------------------------------------------------------------------------

enum CellType
{
    CT_INVALID = -1,
    CT_WALL,
    CT_AIR,
};


///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------
typedef IntVector2 MapPosition;
typedef std::vector< IntVector2 > MapPositions;

////===========================================================================================
///===========================================================================================
// Cell Class
///===========================================================================================
////===========================================================================================
class Cell
{
public:
	///---------------------------------------------------------------------------------
	/// Constructors/Destructors
	///---------------------------------------------------------------------------------
    Cell( CellType type, MapPosition mapPos, AABB2D renderBounds );
    ~Cell();

	///---------------------------------------------------------------------------------
	/// Initialization
	///---------------------------------------------------------------------------------
	
	///---------------------------------------------------------------------------------
	/// Accessors/Queries
	///---------------------------------------------------------------------------------
    CellType GetType() { return m_type; }
    MapPosition GetMapPosition() { return m_mapPosition; }
    AABB2D GetRenderBounds() { return m_renderBoundaries; }
    bool IsActive() { return m_isActive; }
    bool IsTrulyHidden() { return m_isTrulyHidden; }
    bool IsVisible() { return m_isVisible; }
    bool IsKnown() { return m_isKnown; }
    bool IsOccupied() { return m_entity != nullptr; }
    bool HasFeature() { return m_feature != nullptr; }
    Entity* GetEntity() { return m_entity; }
    Feature* GetFeature() { return m_feature; }
    Inventory* GetInventory() { return m_inventory; }

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------
    void SetType( CellType type ) { m_type = type; }
    void SetActive( bool isActive ) { m_isActive = isActive; }
    void SetVisible( bool isVisible );
    void SetKnown( bool isKnown ) { m_isKnown = isKnown; }
    void SetTrulyHidden() { m_isTrulyHidden = true; }
    void SetEntity( Entity* entity ) { m_entity = entity; }
    void SetFeature( Feature* feature ) { m_feature = feature; }
    void RemoveEntity() { m_entity = nullptr; }
    void RemoveFeature() { m_feature = nullptr; }

    void AddItem( Item* item, Map* map );

	///---------------------------------------------------------------------------------
	/// Update
	///---------------------------------------------------------------------------------
    void ProcessInput( InputSystem* inputSystem, double deltaSeconds );
    void Update( double deltaSeconds, bool debugModeEnabled );

	///---------------------------------------------------------------------------------
	/// Render
	///---------------------------------------------------------------------------------
    void Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer, const Font& font, const unsigned int& shaderID, bool debugModeEnabled );

	///---------------------------------------------------------------------------------
	/// Public Member Variables
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Static Variables
	///---------------------------------------------------------------------------------


private:
	///---------------------------------------------------------------------------------
	/// Private Functions
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Private Member Variables
	///---------------------------------------------------------------------------------
    CellType m_type;
    MapPosition m_mapPosition;
    AABB2D m_renderBoundaries;
    Entity* m_entity;
    Feature* m_feature;
    Inventory* m_inventory;

    bool m_isActive;

    bool m_isTrulyHidden;
    bool m_isVisible;
    bool m_isKnown;

};

typedef std::vector< Cell > Cells;

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif