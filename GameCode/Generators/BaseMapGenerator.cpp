//=================================================================================
// BaseMapGenerator.cpp
// Author: Tyler George
// Date  : August 20, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================
#include <iterator>
#include "GameCode/Generators/BaseMapGenerator.hpp"
#include "GameCode/NPCFactory.hpp"
#include "../ItemFactory.hpp"
#include "../FeatureFactory.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================
MapGeneratorRegistryMap* MapGeneratorRegistration::s_registry = NULL;


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BaseMapGenerator::BaseMapGenerator( const std::string& name )
    : m_name( name )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BaseMapGenerator::~BaseMapGenerator()
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
void BaseMapGenerator::FinalizeMap( Map* map )
{
    Cells* cells = map->GetCells();

    for (Cells::iterator cellIter = cells->begin(); cellIter != cells->end(); ++cellIter)
    {
        Cell& cell = *cellIter;

        MapPosition mapPos = cell.GetMapPosition();
        MapPosition mapSize = map->GetMapSize();
        if (mapPos.x == 0 || mapPos.x == mapSize.x - 1 || mapPos.y == 0 || mapPos.y == mapSize.y - 1)
            cell.SetType( CT_WALL );
    }

    for (Cells::iterator cellIter = cells->begin(); cellIter != cells->end(); ++cellIter)
    {
        Cell& cell = *cellIter;

        MapPosition mapPos = cell.GetMapPosition();

        if (cell.GetType() == CT_WALL && map->GetNumCellsInRadiusOfType( mapPos, 1, CT_AIR ) == 0)
            cell.SetTrulyHidden();
    }


    NPCFactories npcFactories = NPCFactory::GetFactories();
    std::vector< Entity* > generatedEntities;
    std::vector< MapPosition > possibleDoorPositions = map->GetPossibleDoorPositions();

    IntVector2 mapSize = map->GetMapSize();
    unsigned int numberOfNPCsToSpawn = (unsigned int) ( (mapSize.x * mapSize.y) / (mapSize.x + mapSize.y) );
    unsigned int numberOfItemsToSpawn = numberOfNPCsToSpawn * 2;


    // Doors
    std::vector< FeatureFactory* > doorFactories = FeatureFactory::FindAllFactoriesOfType( FT_DOOR );

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

    for (unsigned int npcNum = 0; npcNum < numberOfNPCsToSpawn; ++npcNum)
    {
        int factoryIndex = GetRandomIntInRange( 0, npcFactories.size() - 1 );
        NPCFactories::iterator factoryIter = npcFactories.begin();
        std::advance( factoryIter, factoryIndex );

        if (factoryIter->first == "The Dreamer")
        {
            npcNum--;
            continue;
        }

        NPCFactory* factory = factoryIter->second;
        
        NPC* newNPC = factory->SpawnNPC( XMLNode::emptyNode() );
        MapPosition newNPCPos = map->GetRandomOpenPosition();

        newNPC->SetMap( map );
        map->SetEntityAtPosition( newNPC, newNPCPos );

        generatedEntities.push_back( newNPC );
    }

    // items
    ItemFactories itemFactories = ItemFactory::GetFactories();
    for (unsigned int itemNum = 0; itemNum < numberOfItemsToSpawn; ++itemNum)
    {
        int factoryIndex = GetRandomIntInRange( 0, itemFactories.size() - 1 );
        ItemFactories::iterator factoryIter = itemFactories.begin();
        std::advance( factoryIter, factoryIndex );

        if (factoryIter->first == "Blood Sword")
        {
            itemNum--;
            continue;
        }

        ItemFactory* factory = factoryIter->second;
        Item* newItem = factory->SpawnItem( XMLNode::emptyNode() );
        MapPosition newItemPos = map->GetRandomOpenPosition();

        newItem->SetMap( map );
        map->AddItemAtPosition( newItem, newItemPos );

        generatedEntities.push_back( newItem );
    }

    // find player starting location
    MapPosition playerStart = map->GetPlayerStart();
    if (map->GetPlayerStart() == MapPosition( -1, -1 ))
    {
        playerStart = map->GetRandomOpenPosition();
        map->SetPlayerStart( playerStart );
    }

    // spawn the dreamer
    NPCFactory* dreamerFactory = NPCFactory::FindFactoryByName( "The Dreamer" );
    NPC* dreamer = dreamerFactory->SpawnNPC( XMLNode::emptyNode() );
    dreamer->SetMap( map );
    generatedEntities.push_back( dreamer );

    // place dreamer in the world
    MapPosition furthestFromPlayer;
    int furthestDistanceFromPlayer = 0;

    for (int i = 0; i < 20; ++i)
    {
        MapPosition pos = map->GetRandomOpenPosition();
        int dist = Map::CalculateManhattanDistance( playerStart, pos );

        if (dist > furthestDistanceFromPlayer)
        {
            furthestDistanceFromPlayer = dist;
            furthestFromPlayer = pos;
        }
    }
    map->SetEntityAtPosition( dreamer, furthestFromPlayer );

    // spawn the blood sword
    ItemFactory* bloodSwordFactory = ItemFactory::FindFactoryByName( "Blood Sword" );
    Item* bloodSword = bloodSwordFactory->SpawnItem( XMLNode::emptyNode() );
    bloodSword->SetMap( map );
    generatedEntities.push_back( bloodSword );

    // place dreamer in the world
    MapPosition furthestFromPlayerAndDreamer;
    int furthestDistanceFromPlayerAndDreamer = 0;

    for (int i = 0; i < 40; ++i)
    {
        MapPosition pos = map->GetRandomOpenPosition();
        int dist = Map::CalculateManhattanDistance( playerStart, pos );
        dist += Map::CalculateManhattanDistance( furthestFromPlayer, pos );

        if (dist > furthestDistanceFromPlayerAndDreamer)
        {
            furthestDistanceFromPlayerAndDreamer = dist;
            furthestFromPlayerAndDreamer = pos;
        }
    }
    map->AddItemAtPosition( bloodSword, furthestFromPlayerAndDreamer );


    map->SetEntities( generatedEntities );

    for (Cells::iterator cellIter = cells->begin(); cellIter != cells->end(); ++cellIter)
    {
        Cell& cell = *cellIter;
        cell.SetVisible( false );
    }

//     // Spawn 20 orcs
//     NPCFactory* orcFactory = NPCFactory::FindFactoryByName( "Orc" );
//     std::vector< Entity* > generatedNPCs;
// 
//     
//     for (int i = 0; i < 20; i++)
//     {
//         NPC* newOrc = orcFactory->SpawnNPC();
// 
//         MapPosition newOrcPos = map->GetRandomOpenPosition();
//         newOrc->SetMap( map );
//         newOrc->SetMapPosition( newOrcPos );
//         map->GetCellAtPosition( newOrcPos )->SetEntity( newOrc );
// 
//         generatedNPCs.push_back( newOrc );
//     }
//     map->SetNPCs( generatedNPCs );

}

////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================


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

