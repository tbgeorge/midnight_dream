//=================================================================================
// FieldOfViewCalculator.cpp
// Author: Tyler George
// Date  : September 8, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/FieldOfViewCalculator.hpp"

#include "GameCode/Entities/Actor.hpp"
#include "Entities/Feature.hpp"


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
FieldOfViewCalculator::FieldOfViewCalculator()
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
FieldOfViewCalculator::~FieldOfViewCalculator()
{

}

////===========================================================================================
///===========================================================================================
// Initialization
///===========================================================================================
////===========================================================================================

////===========================================================================================
///===========================================================================================
// Accessors/Queries
///===========================================================================================
////===========================================================================================


////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
/// Brute Force Raytrace
///---------------------------------------------------------------------------------
void FieldOfViewCalculator::CalculateFOV_BFR( Actor* actor, const int& radius, Map* map, bool markVisible )
{
    MapPosition center = actor->GetMapPosition();

    static Vector2 offset = map->GetCellSizePixels();
    static Vector2 halfOffset = offset / 2.0f;

    Cell* centerCell = map->GetCellAtPosition( center );
    Vector2 centerFlt = centerCell->GetRenderBounds().m_mins + halfOffset;

    if (markVisible)
    {
        Cells* cells = map->GetCells();
        for (Cells::iterator cellIter = cells->begin(); cellIter != cells->end(); ++cellIter)
        {
            Cell& cell = *cellIter;
            cell.SetVisible( false );
        }
    }

    std::set< Cell* > visibleCells;

    for (int x = center.x - radius; x <= center.x + radius; ++x)
    {
        for (int y = center.y - radius; y <= center.y + radius; ++y)
        {
            int xDiff = abs( center.x - x );
            int yDiff = abs( center.y - y );

            int dist = xDiff + yDiff;

            if (dist <= radius)
            {
                Cell* cell = map->GetCellAtPosition( MapPosition( x, y ) );
                if (cell)
                {
                    Vector2 endFlt = cell->GetRenderBounds().m_mins + halfOffset;

                    Vector2 centerLowerLeft = centerFlt - halfOffset;
                    Vector2 centerUpperRight = centerFlt + halfOffset;
                    Vector2 centerUpperLeft = centerLowerLeft + Vector2( 0.0f, offset.y );
                    Vector2 centerLowerRight = centerLowerLeft + Vector2( offset.x, 0.0f );

                    Vector2 endLowerLeft = endFlt - halfOffset;
                    Vector2 endUpperRight = endFlt + halfOffset;
                    Vector2 endUpperLeft = endLowerLeft + Vector2( 0.0f, offset.y );
                    Vector2 endLowerRight = endLowerLeft + Vector2( offset.x, 0.0f );

//                     BruteForceRaytrace( centerLowerLeft, endLowerLeft, map, visibleCells );
//                     BruteForceRaytrace( centerLowerLeft, endLowerRight, map, visibleCells );
//                     BruteForceRaytrace( centerLowerLeft, endUpperLeft, map, visibleCells );
//                     BruteForceRaytrace( centerLowerLeft, endUpperRight, map, visibleCells );
//                     BruteForceRaytrace( centerLowerLeft, endFlt, map, visibleCells );
// 
//                     BruteForceRaytrace( centerLowerRight, endLowerLeft, map, visibleCells );
//                     BruteForceRaytrace( centerLowerRight, endLowerRight, map, visibleCells );
//                     BruteForceRaytrace( centerLowerRight, endUpperLeft, map, visibleCells );
//                     BruteForceRaytrace( centerLowerRight, endUpperRight, map, visibleCells );
//                     BruteForceRaytrace( centerLowerRight, endFlt, map, visibleCells );
// 
//                     BruteForceRaytrace( centerUpperLeft, endLowerLeft, map, visibleCells );
//                     BruteForceRaytrace( centerUpperLeft, endLowerRight, map, visibleCells );
//                     BruteForceRaytrace( centerUpperLeft, endUpperLeft, map, visibleCells );
//                     BruteForceRaytrace( centerUpperLeft, endUpperRight, map, visibleCells );
//                     BruteForceRaytrace( centerUpperLeft, endFlt, map, visibleCells );
// 
//                     BruteForceRaytrace( centerUpperRight, endLowerLeft, map, visibleCells );
//                     BruteForceRaytrace( centerUpperRight, endLowerRight, map, visibleCells );
//                     BruteForceRaytrace( centerUpperRight, endUpperLeft, map, visibleCells );
//                     BruteForceRaytrace( centerUpperRight, endUpperRight, map, visibleCells );
//                     BruteForceRaytrace( centerUpperRight, endFlt, map, visibleCells );
// 
//                     BruteForceRaytrace( centerFlt, endLowerLeft, map, visibleCells );
//                     BruteForceRaytrace( centerFlt, endLowerRight, map, visibleCells );
//                     BruteForceRaytrace( centerFlt, endUpperLeft, map, visibleCells );
//                     BruteForceRaytrace( centerFlt, endUpperRight, map, visibleCells );
                    BruteForceRaytrace( centerFlt, endFlt, map, visibleCells );

                }
            }
        }
    }

    Actors visibleActors;
    std::vector< Cell* > visibleCellVector;

    for (std::set< Cell* >::iterator cellPtrIter = visibleCells.begin(); cellPtrIter != visibleCells.end(); ++cellPtrIter)
    {
        Cell* cell = *cellPtrIter;

        visibleCellVector.push_back( cell );

        if (cell->IsOccupied() && cell->GetEntity()->IsActor())
            visibleActors.push_back( (Actor*)( cell->GetEntity()) );

        if (cell && markVisible)
            cell->SetVisible( true );
    }

    actor->SetVisibleActors( visibleActors );
    actor->SetVisibleCells( visibleCellVector );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void FieldOfViewCalculator::BruteForceRaytrace( const Vector2& start, const Vector2& end, Map* map, std::set< Cell* >& visibleCells )
{
    Vector2 currentPos;

    for (float t = 0.0f; t < 1.0f; t += 0.01f)
    {
        currentPos = Interpolate( start, end, t );

        MapPosition cellPos = map->ConvertScreenCoordsToMapCoords( currentPos );
        Cell* cell = map->GetCellAtPosition( cellPos );

        if (cell->GetType() != CT_WALL)
        {
            Feature* feature = cell->GetFeature();
            if (feature)
            {
                bool featureActive = feature->IsActive();
                if (featureActive && feature->ActiveBlocksLOS())
                {
                    visibleCells.insert( cell );
                    break;
                }
                else if (!featureActive && feature->InactiveBlocksLOS())
                {
                    visibleCells.insert( cell );
                    break;
                }
                else
                    visibleCells.insert( cell );
            }
            else
                visibleCells.insert( cell );
        }
        else
        {
            visibleCells.insert( cell );
            break;
        }
    }
}


////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

