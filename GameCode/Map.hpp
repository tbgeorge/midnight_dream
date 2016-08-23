//=================================================================================
// Map.hpp
// Author: Tyler George
// Date  : August 19, 2015
//=================================================================================

#pragma once

#ifndef __included_Map__
#define __included_Map__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/FontRenderer.hpp"

#include "GameCode/Cell.hpp"
#include "Engine/Utilities/XMLParser.h"

class Actor;

///---------------------------------------------------------------------------------
/// Constants
///---------------------------------------------------------------------------------
const float MAP_WIDTH_PIXELS = 1280.0f;
const float MAP_HEIGHT_PIXELS = 720.0f;

const float MAP_OFFSET_X = 0.0f;
const float MAP_OFFSET_Y = 75.0f;

///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------

enum MapDirection
{
    MD_EAST,
    MD_NORTHEAST,
    MD_NORTH,
    MD_NORTHWEST,
    MD_WEST,
    MD_SOUTHWEST,
    MD_SOUTH,
    MD_SOUTHEAST,
    MD_INVALID
};

////===========================================================================================
///===========================================================================================
// Map Class
///===========================================================================================
////===========================================================================================
class Map
{
public:
	///---------------------------------------------------------------------------------
	/// Constructors/Destructors
	///---------------------------------------------------------------------------------
    Map( const IntVector2& mapSize );
    ~Map();

	///---------------------------------------------------------------------------------
	/// Initialization
	///---------------------------------------------------------------------------------
    void Startup();

	///---------------------------------------------------------------------------------
	/// Accessors/Queries
	///---------------------------------------------------------------------------------
    Cells* GetCells() { return &m_cells; }
    Vector2 GetCellSizePixels() { return m_cellSize; }
    int GetSeed() { return m_seed; }
    Cell* GetCellAtPosition( const MapPosition& mapPos );
    Cell* GetCellInDirection( const MapPosition& mapPos, const MapDirection& dir );
    unsigned int GetNumCellsInRadiusOfType( const MapPosition& centerPos, const int& radius, CellType type );
    MapPosition GetPlayerStart() { return m_playerStart; }
    MapPosition ConvertScreenCoordsToMapCoords( const Vector2& screenCoords );
    //unsigned int GetNumActiveNeighbors( const MapPosition& centerPos );
    
    IntVector2 GetMapSize() { return m_mapSize; }
    int GetTotalIterationCount() { return m_totalIterationsCount; }
    int GetCurrentGeneratorIterationCount() { return m_currentGeneratorIterationCount; }
    bool AreAllInRangeWall( const AABB2D& rectBounds );
    bool CellHasDirectNeighborsOfType( const MapPosition& mapPos, CellType type, MapPosition& neighborPos );
    MapPosition GetRandomOpenPosition();
    MapPosition GetRandomOpenPositionInBounds( const AABB2D& bounds, bool ignoreFeatures );

    void CalculateLocalCost( Actor* actor, const MapPosition& startPosition, const MapPosition& endPosition, float& out_avoidanceCost, float& out_distanceCost );
    static int CalculateManhattanDistance( const MapPosition& start, const MapPosition& end );
    MapPositions GetAdjacentValidNeighbors( Actor* actor, const MapPosition& position, const bool& ignoreActors );
    std::vector< Feature* > GetAdjacentFeatures( const MapPosition& position );
    std::vector< Entity* >& GetGeneratedEntities() { return m_generatedEntities; }
    std::vector< MapPosition >& GetPossibleDoorPositions() { return m_possibleDoorPositions; }

    static bool IsAdjacent( const MapPosition& a, const MapPosition& b );
    static MapDirection GetDirection( const MapPosition& from, const MapPosition& to );

    void Save( XMLNode& parent );

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------
    void SetSeed( const int& seed ) { m_seed = seed; }
    void IncrementIterationCount();
    void ResetCurrentGeneratorIterationCount() { m_currentGeneratorIterationCount = 0; }
    void SetEntities( const std::vector< Entity* >& npcs ) { m_generatedEntities = npcs; }
    void ClearEntities();
    void SetPlayerStart( const MapPosition& playerStart ) { m_playerStart = playerStart; }
    void SetEntityAtPosition( Entity* entity, const MapPosition& position );
    void SetFeatureAtPosition( Feature* feature, const MapPosition& position );
    void AddItemAtPosition( Item* newItem, const MapPosition& newItemPos );
    void AddPossibleDoorLocation( const MapPosition& doorLocation );

    static Map* LoadFromFile( XMLNode& node );

	///---------------------------------------------------------------------------------
	/// Update
	///---------------------------------------------------------------------------------
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
    Cells m_cells;
    int m_seed;

    IntVector2 m_mapSize;
    Vector2 m_cellSize;

    MapPosition m_playerStart;

    int m_totalIterationsCount;
    int m_currentGeneratorIterationCount;

    std::vector< Entity* > m_generatedEntities;
    std::vector< MapPosition > m_possibleDoorPositions;
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif