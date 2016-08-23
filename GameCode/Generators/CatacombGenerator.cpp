//=================================================================================
// CatacombGenerator.cpp
// Author: Tyler George
// Date  : October 8, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/Generators/CatacombGenerator.hpp"
#include "../Pathfinder.hpp"
#include "../NPCFactory.hpp"
#include "../FeatureFactory.hpp"
#include "../ItemFactory.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

MapGeneratorRegistration CatacombGenerator::s_catacombGeneratorRegistration( "Catacomb Generator", &CatacombGenerator::CreateMapGenerator );


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
CatacombGenerator::CatacombGenerator( const std::string& name )
    : BaseMapGenerator( name )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
CatacombGenerator::~CatacombGenerator()
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
BaseMapGenerator* CatacombGenerator::CreateMapGenerator( const std::string& name )
{
    return new CatacombGenerator( name );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Map* CatacombGenerator::CreateMap( const IntVector2& mapSize )
{
    // hard-coded map size
    Map* map = new Map( IntVector2( 64, 32 ) );

    Cells* cells = map->GetCells();

    int seed = TripleRandomInt();
    map->SetSeed( seed );
    srand( seed );

    return map;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool CatacombGenerator::ProcessOneStep( Map* map )
{

    static bool hasFinishedCatacombs = false;
    static bool hasFinishedVampireRoom = false;
    static bool hasFinishedEntrance = false;
    static bool isValid = false;

    static AABB2D vampireRoomBounds;
    static AABB2D entranceRoomBounds;
    static AABB2D catacombBounds( 5, 2, 59, 30 );

    static MapPosition entranceDoorway;
    static MapPosition vampireDoorway;


    while (!isValid)
    {
        if (!hasFinishedCatacombs)
        {
            for (int x = 5; x <= 59; ++x)
            {
                for (int y = 2; y <= 30; ++y)
                {
                    bool changeToAir = GetRandomFloatZeroToOne() <= 0.66f;

                    if (changeToAir)
                        map->GetCellAtPosition( MapPosition( x, y ) )->SetType( CT_AIR );
                }
            }
            hasFinishedCatacombs = true;
            return false;
        }

        if (!hasFinishedVampireRoom)
        {
            int height = GetRandomIntInRange( 4, 6 );
            int width = GetRandomIntInRange( 3, 4 );

            int xMin = map->GetMapSize().x - ( width + 2 );
            int yMin = GetRandomIntInRange( 0, map->GetMapSize().y - ( height + 1 ) );

            AABB2D roomBounds( xMin, yMin, xMin + width, yMin + height );
            vampireRoomBounds = roomBounds;

            FillMapRect( map, roomBounds, CT_WALL );
            FillMapRect( map, roomBounds, CT_AIR );

            MapPosition centerOfLeftWall( xMin - 1, yMin + ( height / 2 ) );
            map->AddPossibleDoorLocation( centerOfLeftWall );
            vampireDoorway = centerOfLeftWall;

            // surround room with walls
            for (int x = xMin - 1; x <= xMin + width + 1; ++x)
            {
                for (int y = yMin - 1; y <= yMin + height + 1; ++y)
                {
                    if (x == (xMin - 1) || x == (xMin + width + 1) || y == (yMin - 1) || y == (yMin + height + 1) )
                    {
                        MapPosition pos( x, y );
                        map->GetCellAtPosition( pos )->SetType( CT_WALL );
                    }
                }
            }

            map->GetCellAtPosition( centerOfLeftWall )->SetType( CT_AIR );

            bool foundAirCell = false;
            int xPos = centerOfLeftWall.x - 1;

            while (!foundAirCell)
            {
                MapPosition pos( xPos, centerOfLeftWall.y );

                foundAirCell = map->GetCellAtPosition( pos )->GetType() == CT_AIR;
                map->GetCellAtPosition( pos )->SetType( CT_AIR );
                xPos--;
            }

            hasFinishedVampireRoom = true;
            return false;
        }

        if (!hasFinishedEntrance)
        {
            int height = GetRandomIntInRange( 4, 6 );
            int width = GetRandomIntInRange( 3, 4 );

            int xMin = 1;
            int yMin = GetRandomIntInRange( 0, map->GetMapSize().y - ( height + 1 ) );

            AABB2D roomBounds( xMin, yMin, xMin + width, yMin + height );
            entranceRoomBounds = roomBounds;

            FillMapRect( map, roomBounds, CT_WALL );
            FillMapRect( map, roomBounds, CT_AIR );


            // surround room with walls
            for (int x = xMin - 1; x <= xMin + width + 1; ++x)
            {
                for (int y = yMin - 1; y <= yMin + height + 1; ++y)
                {
                    if (x == (xMin - 1) || x == (xMin + width + 1) || y == (yMin - 1) || y == (yMin + height + 1))
                    {
                        MapPosition pos( x, y );
                        map->GetCellAtPosition( pos )->SetType( CT_WALL );
                    }
                }
            }

            MapPosition centerOfRightWall( xMin + width + 1, yMin + (height / 2) );
            map->AddPossibleDoorLocation( centerOfRightWall );
            entranceDoorway = centerOfRightWall;

            map->GetCellAtPosition( centerOfRightWall )->SetType( CT_AIR );

            bool foundAirCell = false;
            int xPos = centerOfRightWall.x + 1;

            while (!foundAirCell)
            {
                MapPosition pos( xPos, centerOfRightWall.y );

                foundAirCell = map->GetCellAtPosition( pos )->GetType() == CT_AIR;
                map->GetCellAtPosition( pos )->SetType( CT_AIR );
                xPos++;
            }

            hasFinishedEntrance = true;
            return false;
        }

        else
        {
            // validation
            Path* pathThroughCatacombs = Pathfinder::CalculatePath( map, nullptr, entranceDoorway, vampireDoorway, true, true );
            if (pathThroughCatacombs->m_answer->m_position == vampireDoorway)
                isValid = true;

            else
            {
                Cells* cells = map->GetCells();

                for (Cells::iterator cellIter = cells->begin(); cellIter != cells->end(); ++cellIter)
                {
                    Cell& cell = *cellIter;
                    cell.SetVisible( false );
                }

                hasFinishedCatacombs = false;
                hasFinishedEntrance = false;
                hasFinishedVampireRoom = false;
                isValid = false;

                return false;

            }

        }
    }

    if (isValid)
    {
        // spawn inhabitants

        std::vector< Entity* > generatedEntities;

        // vampire lord
        NPCFactory* vampireLordFactory = NPCFactory::FindFactoryByName( "Vampire Lord" );
        NPC* vampireLord = vampireLordFactory->SpawnNPC( XMLNode::emptyNode() );

        vampireLord->SetMap( map );
        MapPosition vampireLordPosition = map->GetRandomOpenPositionInBounds( vampireRoomBounds, false );

        map->SetEntityAtPosition( vampireLord, vampireLordPosition );
        generatedEntities.push_back( vampireLord );

        // entrance skeletons
        NPCFactory* skeletonFactory = NPCFactory::FindFactoryByName( "Skeleton" );
        NPC* skeleton1 = skeletonFactory->SpawnNPC( XMLNode::emptyNode() );
        NPC* skeleton2 = skeletonFactory->SpawnNPC( XMLNode::emptyNode() );

        skeleton1->SetMap( map );
        skeleton2->SetMap( map );

        MapPosition skeleton1Position = map->GetRandomOpenPositionInBounds( entranceRoomBounds, false );
        MapPosition skeleton2Position = map->GetRandomOpenPositionInBounds( entranceRoomBounds, false );

        map->SetEntityAtPosition( skeleton1, skeleton1Position );
        map->SetEntityAtPosition( skeleton2, skeleton2Position );

        generatedEntities.push_back( skeleton1 );
        generatedEntities.push_back( skeleton2 );


        // entrance paladin
        NPCFactory* paladinFactory = NPCFactory::FindFactoryByName( "Paladin" );
        NPC* paladin = paladinFactory->SpawnNPC( XMLNode::emptyNode() );

        paladin->SetMap( map );

        MapPosition paladinPosition = map->GetRandomOpenPositionInBounds( entranceRoomBounds, false );

        map->SetEntityAtPosition( paladin, paladinPosition );
        generatedEntities.push_back( paladin );


        // spawn skeletons and bats in the catacombs
        NPCFactory* batFactory = NPCFactory::FindFactoryByName( "Vampire Bat" );

        for (int x = catacombBounds.m_mins.x; x <= catacombBounds.m_maxs.x; ++x)
        {
            for (int y = catacombBounds.m_mins.y; y <= catacombBounds.m_maxs.y; ++y)
            {
                MapPosition pos( x, y );

                bool isAir = map->GetCellAtPosition( pos )->GetType() == CT_AIR;
                bool isOccupied = map->GetCellAtPosition( pos )->IsOccupied();

                if (!isAir || isOccupied)
                    continue;

                bool spawnNPC = GetRandomFloatZeroToOne() <= 0.02f;

                if (spawnNPC)
                {
                    int batOrSkeleton = GetRandomIntInRange( 0, 1 );
                    
                    NPC* npc = nullptr;

                    switch (batOrSkeleton)
                    {
                    case 0:
                        // spawn bat
                        npc = batFactory->SpawnNPC( XMLNode::emptyNode() );
                        break;
                    case 1:
                        // spawn skeleton 
                        npc = skeletonFactory->SpawnNPC( XMLNode::emptyNode() );
                        break;
                    default:
                        break;
                    }

                    npc->SetMap( map );
                    map->SetEntityAtPosition( npc, pos );

                    generatedEntities.push_back( npc );
                }
            }
        }

        // doors
        std::vector< FeatureFactory* > doorFactories = FeatureFactory::FindAllFactoriesOfType( FT_DOOR );
        std::vector< MapPosition > possibleDoorPositions = map->GetPossibleDoorPositions();

        if (doorFactories.size() > 0)
        {
            for (std::vector< MapPosition >::iterator doorPosIter = possibleDoorPositions.begin(); doorPosIter != possibleDoorPositions.end(); ++doorPosIter)
            {
                //             float generateDoor = GetRandomFloatZeroToOne();
                //             if (generateDoor < 0.4f)
                //                 continue;

                int factoryIndex = GetRandomIntInRange( 0, doorFactories.size() - 1 );
                FeatureFactory* factory = doorFactories[factoryIndex];

                Feature* newFeature = factory->SpawnFeature( XMLNode::emptyNode() );
                MapPosition newFeaturePos = *doorPosIter;

                newFeature->SetMap( map );
                map->SetFeatureAtPosition( newFeature, newFeaturePos );

                generatedEntities.push_back( newFeature );
            }
        }

        // spawn spawners
        FeatureFactory* spawnerFactory = FeatureFactory::FindFactoryByName( "Bone Pile" );

        for (int i = 0; i < 10; ++i)
        {
            Feature* newSpawner = spawnerFactory->SpawnFeature( XMLNode::emptyNode() );
            MapPosition newSpawnerPosition = map->GetRandomOpenPositionInBounds( catacombBounds, false );

            newSpawner->SetMap( map );
            map->SetFeatureAtPosition( newSpawner, newSpawnerPosition );

            generatedEntities.push_back( newSpawner );
        }

        int numberOfItemsToSpawn = 30;
        // items
        ItemFactories itemFactories = ItemFactory::GetFactories();
        for (unsigned int itemNum = 0; itemNum < numberOfItemsToSpawn; ++itemNum)
        {
            int factoryIndex = GetRandomIntInRange( 0, itemFactories.size() - 1 );
            ItemFactories::iterator factoryIter = itemFactories.begin();
            std::advance( factoryIter, factoryIndex );
        
            if (factoryIter->first == "Blood Sword" || factoryIter->first == "Wooden Stake" )
            {
                itemNum--;
                continue;
            }
        
            ItemFactory* factory = factoryIter->second;
            Item* newItem = factory->SpawnItem( XMLNode::emptyNode() );
            MapPosition newItemPos = map->GetRandomOpenPositionInBounds( catacombBounds, false );
        
            newItem->SetMap( map );
            map->AddItemAtPosition( newItem, newItemPos );
        
            generatedEntities.push_back( newItem );
        }


        // spawn wooden stake
        ItemFactory* stakeFactory = ItemFactory::FindFactoryByName( "Wooden Stake" );

        Item* newStake = stakeFactory->SpawnItem( XMLNode::emptyNode() );
        MapPosition newStakePosition = map->GetRandomOpenPositionInBounds( catacombBounds, false );
        newStake->SetMap( map );
        map->AddItemAtPosition( newStake, newStakePosition );
        generatedEntities.push_back( newStake );

        map->SetEntities( generatedEntities );

        // set player start

        MapPosition playerStart = map->GetRandomOpenPositionInBounds( entranceRoomBounds, false );
        map->SetPlayerStart( playerStart );

        // reset
        hasFinishedCatacombs = false;
        hasFinishedEntrance = false;
        hasFinishedVampireRoom = false;
        isValid = false;

        return true;

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
bool CatacombGenerator::FillMapRect( Map* map, const AABB2D& rectBounds, CellType fillType )
{
//     bool valid = map->AreAllInRangeWall( rectBounds );
//     if (!valid)
//         return false;

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

