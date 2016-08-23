//=================================================================================
// Map.cpp
// Author: Tyler George
// Date  : August 20, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/Map.hpp"
#include "GameCode/Entities/Actor.hpp"
#include "GameCode/Entities/Feature.hpp"
#include "Engine/Utilities/Time.hpp"
#include "Engine/Utilities/XMLHelper.hpp"
#include "Engine/Utilities/Utilities.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Map::Map( const IntVector2& mapSize )
    : m_mapSize( mapSize )
    , m_cellSize(MAP_WIDTH_PIXELS / (float)m_mapSize.x, MAP_HEIGHT_PIXELS / (float) m_mapSize.y )
    , m_totalIterationsCount( 0 )
    , m_currentGeneratorIterationCount( 0 )
    , m_playerStart(MapPosition( -1, -1 ) )
{
    for (int y = 0; y < m_mapSize.y; ++y)
    {
        for (int x = 0; x < m_mapSize.x; ++x)
        {
            IntVector2 cellMapPosition( x, y );
            Vector2 cellMinRenderBounds( MAP_OFFSET_X + (x * m_cellSize.x), MAP_OFFSET_Y + (y * m_cellSize.y) );
            Vector2 cellMaxRenderBounds( cellMinRenderBounds.x + m_cellSize.x, cellMinRenderBounds.y + m_cellSize.y );
            AABB2D cellRenderBounds( cellMinRenderBounds, cellMaxRenderBounds );
            CellType cellType = CT_WALL;
            
            m_cells.push_back( Cell( cellType, cellMapPosition, cellRenderBounds ) );
        }
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Map::~Map()
{

}

////===========================================================================================
///===========================================================================================
// Initialization
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::Startup()
{

}

////===========================================================================================
///===========================================================================================
// Accessors/Queries
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Cell* Map::GetCellAtPosition( const IntVector2& mapPos )
{
    if (mapPos.x < 0 || mapPos.x >= m_mapSize.x || mapPos.y < 0 || mapPos.y >= m_mapSize.y)
        return nullptr;
    int i = (m_mapSize.x * mapPos.y) + mapPos.x;

    return &m_cells[i];
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Cell* Map::GetCellInDirection( const MapPosition& mapPos, const MapDirection& dir )
{
    MapPosition newPos = mapPos;

    switch (dir)
    {
    case MD_EAST:
        newPos.x++;
        break;
    case MD_NORTHEAST:
        newPos.x++;
        newPos.y++;
        break;
    case MD_NORTH:
        newPos.y++;
        break;
    case MD_NORTHWEST:
        newPos.x--;
        newPos.y++;
        break;
    case MD_WEST:
        newPos.x--;
        break;
    case MD_SOUTHWEST:
        newPos.x--;
        newPos.y--;
        break;
    case MD_SOUTH:
        newPos.y--;
        break;
    case MD_SOUTHEAST:
        newPos.x++;
        newPos.y--;
        break;
    default:
        break;
    }

    return GetCellAtPosition( newPos );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
unsigned int Map::GetNumCellsInRadiusOfType( const IntVector2& centerPos, const int& radius, CellType type )
{
    unsigned int numCells = 0;

    for (int y = centerPos.y - radius; y <= centerPos.y + radius; y++)
    {
        for (int x = centerPos.x - radius; x <= centerPos.x + radius; x++)
        {
//             if (y == centerPos.y && x == centerPos.x)
//                 continue;

            Cell* cell = GetCellAtPosition( IntVector2( x, y ) );
            if ( cell && cell->GetType() == type)
                numCells++;
        }
    }

    return numCells;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
// unsigned int Map::GetNumActiveNeighbors( const IntVector2& centerPos )
// {
//     unsigned int numCells = 0;
// 
//     for (int y = centerPos.y - 1; y <= centerPos.y + 1; y++)
//     {
//         for (int x = centerPos.x - 1; x <= centerPos.x + 1; x++)
//         {
//             if ( IntVector2( x, y ) == centerPos )
//                 continue;
// 
//             Cell* cell = GetCellAtPosition( IntVector2( x, y ) );
//             if (cell && cell->IsActive())
//                 numCells++;
//         }
//     }
// 
//     return numCells;
// }

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Map::AreAllInRangeWall( const AABB2D& rectBounds )
{
    for (int x = (int) rectBounds.m_mins.x; x <= (int) rectBounds.m_maxs.x; ++x)
    {
        for (int y = (int) rectBounds.m_mins.y; y <= (int) rectBounds.m_maxs.y; ++y)
        {
            Cell* cell = GetCellAtPosition( IntVector2( x, y ) );
            if ( cell && cell->GetType() == CT_WALL )
                continue;

            else
                return false;
        }
    }

    return true;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Map::CellHasDirectNeighborsOfType( const IntVector2& mapPos, CellType type, IntVector2& neighborPos )
{
    Cell* centerCell = GetCellAtPosition( mapPos );
    
    int x = centerCell->GetMapPosition().x;
    int y = centerCell->GetMapPosition().y;

    Cell* check = GetCellAtPosition( IntVector2( x - 1, y ) );
    if ( check && check->GetType() == type)
    {
        neighborPos = IntVector2( x - 1, y );
        return true;
    }
    check = GetCellAtPosition( IntVector2( x + 1, y ) );
    if (check && check->GetType() == type)
    {
        neighborPos = IntVector2( x + 1, y );
        return true;
    }
    check = GetCellAtPosition( IntVector2( x, y - 1 ) );
    if (check && check->GetType() == type)
    {
        neighborPos = IntVector2( x, y - 1 );
        return true;
    }
    check = GetCellAtPosition( IntVector2( x, y + 1 ) );
    if (check && check->GetType() == type)
    {
        neighborPos = IntVector2( x, y + 1 );
        return true;
    }

    neighborPos = IntVector2::ZERO;
    return false;

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MapPosition Map::ConvertScreenCoordsToMapCoords( const Vector2& screenCoords )
{
    Vector2 localScreenCoords = screenCoords - Vector2( MAP_OFFSET_X, MAP_OFFSET_Y );
    MapPosition mapCoords = MapPosition( floorf( localScreenCoords.x / m_cellSize.x ), floorf( localScreenCoords.y / m_cellSize.y ) );
    return mapCoords;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MapPosition Map::GetRandomOpenPosition()
{
    while (true)
    {
        int x = GetRandomIntInRange( 0, m_mapSize.x - 1 );
        int y = GetRandomIntInRange( 0, m_mapSize.y - 1 );

        MapPosition pos( x, y );

        Cell* cell = GetCellAtPosition( pos );
        if (cell && cell->GetType() == CT_AIR && !cell->IsOccupied() && !cell->HasFeature() )
            return pos;
    }   
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MapPosition Map::GetRandomOpenPositionInBounds( const AABB2D& bounds, bool ignoreFeatures )
{
    int count = 0;
    while (true)
    {
        // infinite loop
        if (count > 500000)
            return MapPosition( -1, -1 );

        count++;
        int x = GetRandomIntInRange( (int)bounds.m_mins.x, (int)bounds.m_maxs.x );
        int y = GetRandomIntInRange( (int)bounds.m_mins.y, (int)bounds.m_maxs.y );

        MapPosition pos( x, y );

        Cell* cell = GetCellAtPosition( pos );
        if (cell && cell->GetType() == CT_AIR && !cell->IsOccupied())
        {
            if (ignoreFeatures)
                return pos;
            else if (!cell->HasFeature())
                return pos;

        }
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::CalculateLocalCost( Actor* actor, const MapPosition& startPosition, const MapPosition& endPosition, float& out_avoidanceCost, float& out_distanceCost )
{
    out_avoidanceCost = 1.0f;

    out_distanceCost = CalcDistance( startPosition, endPosition );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
int Map::CalculateManhattanDistance( const MapPosition& start, const MapPosition& end )
{
    int xDiff = abs( end.x - start.x );
    int yDiff = abs( end.y - start.y );

    return xDiff + yDiff;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MapPositions Map::GetAdjacentValidNeighbors( Actor* actor, const MapPosition& position, const bool& ignoreActors )
{
    MapPositions neighbors;

    for (int x = position.x - 1; x <= position.x + 1; ++x)
    {
        for (int y = position.y - 1; y <= position.y + 1; ++y)
        {
            if (x == position.x && y == position.y)
                continue;

            Cell* cell = GetCellAtPosition( MapPosition( x, y ) );
            if ( cell && cell->GetType() == CT_AIR  )
            {
                if ( ignoreActors )
                    neighbors.push_back( MapPosition( x, y ) );
                else if ( !cell->IsOccupied() )
                    neighbors.push_back( MapPosition( x, y ) );
            }
        }
    }
    return neighbors;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
std::vector< Feature* > Map::GetAdjacentFeatures( const MapPosition& position )
{
    std::vector< Feature* > adjacentFeatures;

    for (int x = position.x - 1; x <= position.x + 1; ++x)
    {
        for (int y = position.y - 1; y <= position.y + 1; ++y)
        {
            if (x == position.x && y == position.y)
                continue;

            Cell* cell = GetCellAtPosition( MapPosition( x, y ) );
            if (cell && cell->HasFeature() && !cell->IsOccupied() )
                    adjacentFeatures.push_back( cell->GetFeature() );
        }
    }
    return adjacentFeatures;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Map::IsAdjacent( const MapPosition& a, const MapPosition& b )
{
    int xDiff = abs( a.x - b.x );
    int yDiff = abs( a.y - b.y );

    if (xDiff <= 1)
    {
        if (yDiff <= 1)
            return true;
        return false;
    }

    else if (yDiff <= 1)
    {
        if (xDiff <= 1)
            return true;
        return false;
    }

    else
        return false;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MapDirection Map::GetDirection( const MapPosition& from, const MapPosition& to )
{
    MapPosition difference = to - from;

    if (difference.x == 0)
    {
        if (difference.y > 0)
            return MD_NORTH;
        if (difference.y < 0)
            return MD_SOUTH;
        else
            return MD_INVALID;
    }

    else if (difference.y == 0)
    {
        if (difference.x > 0)
            return MD_EAST;
        if (difference.x < 0)
            return MD_WEST;
        else
            return MD_INVALID;
    }

    else if (difference.x > 0)
    {
        if (difference.y > 0)
            return MD_NORTHEAST;
        if (difference.y < 0)
            return MD_SOUTHEAST;
        else
            return MD_INVALID;
    }

    else if (difference.x < 0)
    {
        if (difference.y > 0)
            return MD_NORTHWEST;
        if (difference.y < 0)
            return MD_SOUTHWEST;
        else
            return MD_INVALID;
    }
    else
        return MD_INVALID;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::Save( XMLNode& parent )
{
    XMLNode mapData = CreateNode( "MapData" );
    XMLNode tileData = CreateNode( "TileData" );
    XMLNode knownData = CreateNode( "KnownData" );

    std::string tileDataStr = "\n";
    std::string knownDataStr = "\n";

    for (int y = m_mapSize.y - 1; y >= 0; --y)
    {
        for (int x = 0; x < m_mapSize.x; ++x)
        {
            Cell* cell = GetCellAtPosition( MapPosition( x, y ) );
            if (cell)
            {
                if (cell->GetType() == CT_AIR)
                {
                    tileDataStr += ".";
                    if (cell->IsKnown())
                        knownDataStr += ".";
                    else
                        knownDataStr += "?";
                }
                if (cell->GetType() == CT_WALL)
                {
                    tileDataStr += "#";
                    if (cell->IsKnown())
                        knownDataStr += "#";
                    else
                        knownDataStr += "?";
                }
            }
        }
        tileDataStr += "\n";
        knownDataStr += "\n";
    }

    AddText( tileData, tileDataStr );
    AddText( knownData, knownDataStr );

    AddChild( parent, mapData );
    AddChild( mapData, tileData );
    AddChild( mapData, knownData );


}

////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::IncrementIterationCount()
{
    m_totalIterationsCount++;
    m_currentGeneratorIterationCount++;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::ClearEntities()
{
    m_generatedEntities.clear();
    m_generatedEntities.resize( 0 );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::SetEntityAtPosition( Entity* entity, const MapPosition& position )
{
    // Remove entity from previous cell if valid
    if (entity)
    {
        Cell* previousCell = GetCellAtPosition( entity->GetMapPosition() );
        if (previousCell)
            previousCell->RemoveEntity();
    }

    // Set entity to new position
    if ( entity )
        entity->SetMapPosition( position );
    Cell* newCell = GetCellAtPosition( position );
    newCell->SetEntity( entity );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::SetFeatureAtPosition( Feature* feature, const MapPosition& position )
{
    // Remove entity from previous cell if valid
    if (feature)
    {
        Cell* previousCell = GetCellAtPosition( feature->GetMapPosition() );
        if (previousCell)
            previousCell->RemoveFeature();
    }

    // Set entity to new position
    if (feature)
        feature->SetMapPosition( position );
    Cell* newCell = GetCellAtPosition( position );
    newCell->SetFeature( feature );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::AddItemAtPosition( Item* item, const MapPosition& position )
{
    // Remove entity from previous cell if valid
    if (item)
    {
        Cell* previousCell = GetCellAtPosition( item->GetMapPosition() );
        if (previousCell)
            previousCell->RemoveEntity();
    }

    // Set entity to new position
    if (item)
        item->SetMapPosition( position );
    Cell* newCell = GetCellAtPosition( position );
    newCell->AddItem( item, this );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Map* Map::LoadFromFile( XMLNode& node )
{
    XMLNode tileDataNode = node.getChildNode( "TileData" );
    XMLNode knownDataNode = node.getChildNode( "KnownData" );
    std::string tileDataText( tileDataNode.getText() );
    std::string knownDataText( knownDataNode.getText() );

    Strings mapRows;
    Tokenize( tileDataText, mapRows, "\r\n" );

    Strings knownDataRows;
    Tokenize( knownDataText, knownDataRows, "\r\n" );

    IntVector2 dataMapSize;
    dataMapSize.y = mapRows.size();
    dataMapSize.x = mapRows[0].size();

    Map* map = new Map( dataMapSize );
    Cells* cells = map->GetCells();

    int seed = TripleRandomInt();
    map->SetSeed( seed );
    srand( seed );

    int cellIndex = 0;
    for (Strings::reverse_iterator mapRowIter = mapRows.rbegin(); mapRowIter != mapRows.rend(); ++mapRowIter)
    {
        std::string mapRow = *mapRowIter;
        for (std::string::const_iterator cellTypeIter = mapRow.begin(); cellTypeIter != mapRow.end(); ++cellTypeIter)
        {
            char cellTypeSymbol = *cellTypeIter;
            if (cellTypeSymbol == '#')
                cells->at( cellIndex ).SetType( CT_WALL );
            else
                cells->at( cellIndex ).SetType( CT_AIR );

            ++cellIndex;
        }
    }

    cellIndex = 0;
    for (Strings::reverse_iterator knownRowIter = knownDataRows.rbegin(); knownRowIter != knownDataRows.rend(); ++knownRowIter)
    {
        std::string knownRow = *knownRowIter;
        for (std::string::const_iterator cellTypeIter = knownRow.begin(); cellTypeIter != knownRow.end(); ++cellTypeIter)
        {
            char cellTypeSymbol = *cellTypeIter;
            if (cellTypeSymbol != '?')
                cells->at( cellIndex ).SetKnown( true );
            else
                cells->at( cellIndex ).SetKnown( false );

            ++cellIndex;
        }
    }

    for (Cells::iterator cellIter = cells->begin(); cellIter != cells->end(); ++cellIter)
    {
        Cell& cell = *cellIter;

        MapPosition mapPos = cell.GetMapPosition();

        if (cell.GetType() == CT_WALL && map->GetNumCellsInRadiusOfType( mapPos, 1, CT_AIR ) == 0)
            cell.SetTrulyHidden();
    }

    return map;

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::AddPossibleDoorLocation( const MapPosition& doorLocation )
{
    m_possibleDoorPositions.push_back( doorLocation );
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::Update( double deltaSeconds, bool debugModeEnabled )
{

}


////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer, const Font& font, const unsigned int& shaderID, bool debugModeEnabled )
{
    if (!renderer)
        return;

    if (m_cells.size() == 0)
        return;

    

    renderer->DrawColoredQuad( NULL, Vector2( MAP_OFFSET_X, MAP_OFFSET_Y ), Vector2( MAP_OFFSET_X + MAP_WIDTH_PIXELS, MAP_OFFSET_Y + MAP_HEIGHT_PIXELS ), 2.0f, Rgba::BLACK );
    //renderer->DrawColoredQuad( NULL, Vector2( 0.0f, 75.0f ), Vector2( 0.0f + 1280.0f, 75.0f + 720.0f ), 1.0f, Rgba::BLACK );

//     std::string line = "";
    //Vector3s positions;
//    int lineCount = 0;

    //double startTime = GetCurrentSeconds();

    //double averageCellDrawTime = 0.0;

    for (Cells::const_iterator cellIter = m_cells.begin(); cellIter != m_cells.end(); ++cellIter)
    //for (unsigned int i = 0; i < m_cells.size(); ++i )
    {
        //double cellStartTime = GetCurrentSeconds();

        Cell currentCell = *cellIter;
        //Cell currentCell = m_cells[i];

       // Vector3 position;

		// #KSH: See note in Cell::Render, but my main concern here is that I saw the duplicated string, inside a for loop that's the size of the map
		// that's processed every frame.  So that's a lot of string allocations.  Since I know we're dealing with pre-ordained glyphs here (the size of 
		// that string is going to be 1, and deterministic, it made me dive into it.  In a game like this you can get away with it, but it has the look
		// of a lot of unneeded inefficiency for something that should be easier to handle.
        currentCell.Render( renderer, fontRenderer, font, shaderID, debugModeEnabled );
        //positions.push_back( position );

/*        line += symbol;*/

		// #KSH: This logic is a little dangerous--Cell::Render can return an empty string as written (not that you would expect it to), so knowing when
		// you've completed the line could be off.  Consider always returning a ' ', even for an invalid tile?  Anyway, if things go wrong this logic would
		// complicate things.
//         if (line.size() == m_mapSize.x)
//         {
//             Vector3 pos = positions[lineCount * m_mapSize.x];
//             fontRenderer->DrawFontTextWH( shaderID, m_cellSize.x, m_cellSize.y, font, line, pos );
//             line.clear();
//             line.resize( 0 );
//             lineCount++;
//         }

        //double cellEndTime = GetCurrentSeconds();
        //double cellTime = cellEndTime - cellStartTime;

        //averageCellDrawTime += cellTime;
    }

    //averageCellDrawTime /= (double)m_cells.size();

    //double endTime = GetCurrentSeconds();

    //double totalTime = endTime - startTime;

}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

