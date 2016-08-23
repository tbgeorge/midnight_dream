//=================================================================================
// DungeonGenerator.cpp
// Author: Tyler George
// Date  : August 31, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/Generators/DungeonGenerator.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

MapGeneratorRegistration DungeonGenerator::s_dungeonGeneratorRegistration( "Dungeon Generator", &DungeonGenerator::CreateMapGenerator );


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
DungeonGenerator::DungeonGenerator( const std::string& name )
    : BaseMapGenerator( name )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
DungeonGenerator::~DungeonGenerator()
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

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BaseMapGenerator* DungeonGenerator::CreateMapGenerator( const std::string& name )
{
    return new DungeonGenerator( name );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Map* DungeonGenerator::CreateMap( const IntVector2& mapSize )
{
    Map* map = new Map( mapSize );

    Cells* cells = map->GetCells();

    int seed = TripleRandomInt();
    map->SetSeed( seed );
    srand( seed );

    int sizeX = GetRandomIntInRange( 5, 10 );
    int sizeY = GetRandomIntInRange( 4, 9 );

    IntVector2 startPos = ( mapSize / 2 ) - IntVector2( sizeX / 2, sizeY / 2 );
    AABB2D roomBounds( (float)startPos.x, (float)startPos.y, (float)startPos.x + sizeX, (float)startPos.y + sizeY );

    bool success = FillMapRect( map, roomBounds, CT_AIR );
    
    return map;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool DungeonGenerator::ProcessOneStep( Map* map )
{
    Cell* wallCell = nullptr;
    IntVector2 neighborAirPos;

    bool haveAddedFeature = false;

    int loopCount = 0;
    while (!haveAddedFeature)
    {
        // infinite loop check
        ++loopCount;
        if (loopCount > 5000)
            return true;

        while (true)
        {
            int x = GetRandomIntInRange( 0, map->GetMapSize().x - 1 );
            int y = GetRandomIntInRange( 0, map->GetMapSize().y - 1 );

            wallCell = map->GetCellAtPosition( IntVector2( x, y ) );

			// #KSH: I think this is a good use of the Map...ask a direct question of it for a factual answer.  No changes made, but it is a highly specific query.
            if (wallCell && wallCell->GetType() == CT_WALL && map->CellHasDirectNeighborsOfType( IntVector2( x, y ), CT_AIR, neighborAirPos ))
                break;
        }


        DungeonFeature featureToAdd = (DungeonFeature) GetRandomIntInRange( 0, (int) DF_NUM_FEATURES - 1 );
        // try again if hall to give rooms a larger chance of showing up
        if (featureToAdd == DF_HALL )
            featureToAdd = (DungeonFeature)GetRandomIntInRange( 0, (int)DF_NUM_FEATURES - 1 );

        IntVector2 dirToGo = wallCell->GetMapPosition() - neighborAirPos;

        if (featureToAdd == DF_ROOM )
        {
            int length = GetRandomIntInRange( 3, 10 );
            int width = GetRandomIntInRange( 3, 10 );

            if (dirToGo.x == 0)
            {
                int x = wallCell->GetMapPosition().x;
                int startX = x - (width / 2);
                int endX = startX + width;
                int startY = wallCell->GetMapPosition().y;
                int endY = startY + (length * dirToGo.y);

                AABB2D verifyBounds;
                AABB2D featureBounds;

                if (startY < endY)
                {
                    verifyBounds = AABB2D( (float)startX - 1, (float)startY, (float)endX + 1, (float)endY + 2 );
                    featureBounds = AABB2D( (float)startX, (float)startY + 1, (float)endX, (float)endY + 1 );

                }
                else
                {
                    verifyBounds = AABB2D( (float)startX - 1, (float)endY - 2, (float)endX + 1, (float)startY );
                    featureBounds = AABB2D( (float)startX, (float)endY - 1, (float)endX, (float)startY - 1 );
                }

                if (map->AreAllInRangeWall( verifyBounds ))
                {
                    FillMapRect( map, featureBounds, CT_AIR );
                    haveAddedFeature = true;
                    wallCell->SetType( CT_AIR );
                    map->AddPossibleDoorLocation( wallCell->GetMapPosition() );

                    break;
                }

            }

            if (dirToGo.y == 0)
            {
                int y = wallCell->GetMapPosition().y;
                int startY = y - (width / 2);
                int endY = startY + width;
                int startX = wallCell->GetMapPosition().x;
                int endX = startX + (length * dirToGo.x);

                AABB2D verifyBounds;
                AABB2D featureBounds;

                if (startX < endX)
                {
                    verifyBounds = AABB2D( (float)startX, (float)startY - 1, (float)endX + 2, (float)endY + 1 );
                    featureBounds = AABB2D( (float)startX + 1, (float)startY, (float)endX + 1, (float)endY );

                }
                else
                {
                    verifyBounds = AABB2D( (float)endX - 2, (float)startY - 1, (float)startX, (float)endY + 1 );
                    featureBounds = AABB2D( (float)endX - 1, (float)startY, (float)startX - 1, (float)endY );

                }

                if (map->AreAllInRangeWall( verifyBounds ))
                {
                    FillMapRect( map, featureBounds, CT_AIR );
                    haveAddedFeature = true;
                    wallCell->SetType( CT_AIR );
                    map->AddPossibleDoorLocation( wallCell->GetMapPosition() );

                    break;
                }
            }

        }

        if (featureToAdd == DF_HALL)
        {
            int length = GetRandomIntInRange( 6, 14 );

            if (dirToGo.x == 0)
            {
                int x = wallCell->GetMapPosition().x;                
                int startY = wallCell->GetMapPosition().y;
                int endY = startY + (length * dirToGo.y);

                AABB2D verifyBounds;
                AABB2D featureBounds;

                if (startY < endY)
                {
                    verifyBounds = AABB2D( (float)x - 1, (float)startY, (float)x + 1, (float)endY + 1 );
                    featureBounds = AABB2D( (float)x, (float)startY, (float)x, (float)endY );

                }
                else
                {
                    verifyBounds = AABB2D( (float)x - 1, (float)endY - 1, (float)x + 1, (float)startY );
                    featureBounds = AABB2D( (float)x, (float)endY, (float)x, (float)startY );
                }

                if (map->AreAllInRangeWall( verifyBounds ))
                {
                    FillMapRect( map, featureBounds, CT_AIR );
                    haveAddedFeature = true;
                    break;
                }

            }

            if (dirToGo.y == 0)
            {
                int y = wallCell->GetMapPosition().y;
                int startX = wallCell->GetMapPosition().x;
                int endX = startX + (length * dirToGo.x);

                AABB2D verifyBounds;
                AABB2D featureBounds;

                if (startX < endX)
                {
                    verifyBounds = AABB2D( (float)startX, (float)y - 1, (float)endX + 1, (float)y + 1 );
                    featureBounds = AABB2D( (float)startX, (float)y, (float)endX, (float)y );

                }
                else
                {
                    verifyBounds = AABB2D( (float)endX - 1, (float)y - 1, (float)startX, (float)y + 1 );
                    featureBounds = AABB2D( (float)endX, (float)y, (float)startX, (float)y );

                }

                if (map->AreAllInRangeWall( verifyBounds ))
                {
                    FillMapRect( map, featureBounds, CT_AIR );
                    haveAddedFeature = true;
                    break;
                }
            }
            
        }
    }

    return false;
   

}

////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool DungeonGenerator::FillMapRect( Map* map, const AABB2D& rectBounds, CellType fillType )
{
    bool valid = map->AreAllInRangeWall( rectBounds );
    if (!valid)
        return false;

    for (int x = rectBounds.m_mins.x; x <= rectBounds.m_maxs.x; ++x)
    {
        for (int y = rectBounds.m_mins.y; y <= rectBounds.m_maxs.y; ++y)
        {
            Cell* cell = map->GetCellAtPosition( IntVector2( x, y ) );
            cell->SetType( fillType );
        }
    }
    
    return true;
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

