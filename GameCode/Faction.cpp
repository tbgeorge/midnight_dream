//=================================================================================
// Faction.cpp
// Author: Tyler George
// Date  : September 18, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/Faction.hpp"
#include "Engine/Utilities/FileUtilities.hpp"
#include "Engine/Utilities/Utilities.hpp"
#include "Engine/Utilities/XMLHelper.hpp"
#include "Engine/Utilities/DeveloperConsole.hpp"
#include "Engine/Utilities/StringTable.hpp"
#include "Entities/Entity.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

FactionMap Faction::s_factionMap;

////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Faction::Faction( const XMLNode& factionRoot )
{
    m_factionName = GetStringProperty( factionRoot, "name", "unknown faction", true );
    m_factionID = StringTable::GetStringID( m_factionName );

    bool hasAnotherRelation = false;
    int relationCounter = 0;

    FactionRelationship selfRelation;
    selfRelation.factionID = m_factionID;
    selfRelation.status = (int)FR_LOVED;
    m_factionRelationships.insert( std::pair< unsigned int, FactionRelationship>( m_factionID, selfRelation ) );

    do
    {
        XMLNode factionNode = factionRoot.getChildNode( "Relation", relationCounter++ );
        if (!factionNode.isEmpty())
        {
            hasAnotherRelation = true;
            std::string name = GetStringProperty( factionNode, "name", "unknown faction", true );
            if (name == "unknown faction")
            {
                // Soft fail
                DeveloperConsole::WriteLine( "Attempted to load Faction Relation " + std::to_string( relationCounter ) + " on the " + name + " faction. XML doesn't contain \"name\".", WARNING_TEXT_COLOR );
                __debugbreak();
                continue;
            }
            unsigned int factionID = StringTable::GetStringID( name );
            
            int factionStatus = GetIntProperty( factionNode, "status", 10000 );
            if (factionStatus == 10000)
            {
                std::string statusStr = GetStringProperty( factionNode, "status", "neutral", true );
                if (statusStr == "hated")
                    factionStatus = (int)FactionRelationDefinition::FR_HATED;
                else if (statusStr == "disliked")
                    factionStatus = (int)FactionRelationDefinition::FR_DISLIKED;
                else if (statusStr == "neutral")
                    factionStatus = (int)FactionRelationDefinition::FR_NEUTRAL;
                else if (statusStr == "liked")
                    factionStatus = (int)FactionRelationDefinition::FR_LIKED;
                else if (statusStr == "loved")
                    factionStatus = (int)FactionRelationDefinition::FR_LOVED;
                else
                    factionStatus = 0;
            }

            FactionRelationship relation;
            relation.factionID = factionID;
            relation.status = factionStatus;

            m_factionRelationships.insert( std::pair< unsigned int, FactionRelationship >( factionID, relation ) );
        }
        else
            hasAnotherRelation = false;

    } while (hasAnotherRelation);
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Faction::Faction( const Faction& copy, const XMLNode& factionNode )
{
    m_factionName = copy.m_factionName;
    m_factionID = copy.m_factionID;

    for (FactionRelationshipMap::const_iterator relationIter = copy.m_factionRelationships.begin(); relationIter != copy.m_factionRelationships.end(); ++relationIter)
    {
        FactionRelationship relation = relationIter->second;
        FactionRelationship newRelation;
        newRelation.factionID = relation.factionID;
        newRelation.status = relation.status;
        m_factionRelationships.insert( std::pair< unsigned int, FactionRelationship >( newRelation.factionID, newRelation ) );
    }

    if (!factionNode.isEmpty())
    {
        XMLNode entityRelationsNode = factionNode.getChildNode( "EntityRelationships" );

        int entityRelationCount = 0;
        bool hasAnotherRelation = false;
        do
        {
            // load entityRelations
            XMLNode entityRelationNode = entityRelationsNode.getChildNode( "EntityRelation", entityRelationCount++ );
            if (!entityRelationNode.isEmpty())
            {
                hasAnotherRelation = true;
                std::string name = GetStringProperty( entityRelationNode, "name", "unknown faction", true );
                if (name == "unknown faction")
                {
                    // Soft fail
                    DeveloperConsole::WriteLine( "Attempted to load Entity Relation " + std::to_string( entityRelationCount ) + ". XML doesn't contain \"name\".", WARNING_TEXT_COLOR );
                    __debugbreak();
                    continue;
                }
                unsigned int factionID = StringTable::GetStringID( name );
                int status = GetIntProperty( entityRelationNode, "status", 0 );
                int entityHookupID = GetIntProperty( entityRelationNode, "hookupID", 0 );

                FactionRelationship newRelation;
                newRelation.factionID = factionID;
                newRelation.status = status;

                // assuming that entity relations don't exist by default
                m_entityRelationships.insert( std::pair< unsigned int, FactionRelationship >( entityHookupID, newRelation ) );

            }
            else
                hasAnotherRelation = false;

        } while (hasAnotherRelation);


        // load factionRelations
        XMLNode factionRelationsNode = factionNode.getChildNode( "FactionRelationships" );

        int factionRelationCount = 0;
        hasAnotherRelation = false;
        do
        {
            XMLNode factionRelationNode = factionRelationsNode.getChildNode( "EntityRelation", factionRelationCount++ );
            if (!factionRelationNode.isEmpty())
            {
                hasAnotherRelation = true;
                std::string name = GetStringProperty( factionRelationNode, "name", "unknown faction", true );
                if (name == "unknown faction")
                {
                    // Soft fail
                    DeveloperConsole::WriteLine( "Attempted to load Faction Relation " + std::to_string( factionRelationCount ) + ". XML doesn't contain \"name\".", WARNING_TEXT_COLOR );
                    __debugbreak();
                    continue;
                }
                unsigned int factionID = StringTable::GetStringID( name );
                int status = GetIntProperty( factionRelationNode, "status", 0 );

                FactionRelationship newRelation;
                newRelation.factionID = factionID;
                newRelation.status = status;

                // assuming that entity relations don't exist by default
                FactionRelationshipMap::iterator foundRelation = m_factionRelationships.find( factionID );
                if (foundRelation != m_factionRelationships.end())
                    m_factionRelationships.at( factionID ) = newRelation;

                else
                    m_factionRelationships.insert( std::pair< unsigned int, FactionRelationship >( factionID, newRelation ) );

            }
            else
                hasAnotherRelation = false;

        } while (hasAnotherRelation);

    }

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Faction::~Faction()
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
bool Faction::LoadAllFactions()
{
    std::string filePath = "Data/NPCs/Factions.xml";

    // need to check for errors here
    XMLNode factionsRoot = XMLNode::parseFile( filePath.c_str(), "Factions" );

    int factionCounter = 0;
    bool hasAnotherFaction = false;

    do
    {
        XMLNode factionNode = factionsRoot.getChildNode( "Faction", factionCounter++ );
        if (!factionNode.isEmpty())
        {
            hasAnotherFaction = true;
            std::string name = GetStringProperty( factionNode, "name", "unknown faction", true );
            if (name == "unknown faction")
            {
                // Soft fail
                DeveloperConsole::WriteLine( "Attempted to load Faction " + std::to_string( factionCounter ) + ". XML doesn't contain \"name\".", WARNING_TEXT_COLOR );
                __debugbreak();
                continue;
            }
            unsigned int factionID = StringTable::GetStringID( name );
            Faction* faction = new Faction( factionNode );
            s_factionMap.insert( std::pair< unsigned int, Faction* >( factionID, faction ) );

        }
        else
            hasAnotherFaction = false;

    } while (hasAnotherFaction);

    factionsRoot.deleteNodeContent();

    return true;
}

////===========================================================================================
///===========================================================================================
// Accessors/Queries
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Faction* Faction::GetMasterFactionFromName( const std::string& factionName )
{
    unsigned int factionID = StringTable::GetStringID( factionName );

    FactionMap::iterator factionIter = s_factionMap.find( factionID );
    if (factionIter != s_factionMap.end())
        return s_factionMap.at( factionID );

    else
        return nullptr;
}

///---------------------------------------------------------------------------------
/// 
///---------------------------------------------------------------------------------
int Faction::GetRelationshipStatus( unsigned int factionID, unsigned int entityID )
{
//     if (entityID != 0)
//     {
        FactionRelationshipMap::iterator entityRelationIter = m_entityRelationships.find( entityID );
        if (entityRelationIter != m_entityRelationships.end())
        {
            FactionRelationship relation = entityRelationIter->second;
            // TODO: should check if factionID's match as well
            return relation.status;
        }
//         else
//         {
//             FactionRelationship newRelation;
//             newRelation.factionID = factionID;
//             newRelation.status = (int) FR_NEUTRAL;
// 
//             m_entityRelationships.insert( std::pair< unsigned int, FactionRelationship >( entityID, newRelation ) );
//             return newRelation.status;
//         }
/*    }*/

//     else
//     {
        FactionRelationshipMap::iterator factionRelationIter = m_factionRelationships.find( factionID );
        if (factionRelationIter != m_factionRelationships.end())
        {
            FactionRelationship relation = factionRelationIter->second;
            return relation.status;
        }
        //         else
        //         {
        //             FactionRelationship newRelation;
        //             newRelation.factionID = factionID;
        //             newRelation.status = (int)FR_NEUTRAL;
        // 
        //             m_factionRelationships.insert( std::pair< unsigned int, FactionRelationship >( factionID, newRelation ) );
        //             return newRelation.status;
        //         }
        else
            return 0;
/*    }*/

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Faction::Save( XMLNode& parent )
{
    XMLNode factionNode = CreateNode( "Faction" );

    XMLNode factionRelationsNode = CreateNode( "FactionRelationships" );
    XMLNode entityRelationsNode = CreateNode( "EntityRelationships" );

    Faction* defaultFaction = Faction::GetMasterFactionFromName( m_factionName );

    bool hasFactionRelations = false;
    for (FactionRelationshipMap::const_iterator factionRelationIter = m_factionRelationships.begin(); factionRelationIter != m_factionRelationships.end(); ++factionRelationIter)
    {
        FactionRelationship factionRelation = factionRelationIter->second;
        std::string factionName = StringTable::GetStringFromID( factionRelation.factionID );

        int defaultStatus = defaultFaction->GetRelationshipStatus( factionRelation.factionID, 0 );
        if (defaultStatus != factionRelation.status)
        {
            hasFactionRelations = true;
            XMLNode factionRelationNode = CreateNode( "FactionRelation" );
            SetStringProperty( factionRelationNode, "name", factionName, "" );
            SetIntProperty( factionRelationNode, "status", factionRelation.status, defaultStatus );
            AddChild( factionRelationsNode, factionRelationNode );
        }
    }
    if (hasFactionRelations)
    {
        AddChild( parent, factionNode );
        AddChild( factionNode, factionRelationsNode );
    }

    bool hasEntityRelations = false;
    for (FactionRelationshipMap::const_iterator entityRelationIter = m_entityRelationships.begin(); entityRelationIter != m_entityRelationships.end(); ++entityRelationIter)
    {
        hasEntityRelations = true;
        FactionRelationship entityRelation = entityRelationIter->second;
        std::string factionName = StringTable::GetStringFromID( entityRelation.factionID );

        XMLNode entityRelationNode = CreateNode( "EntityRelation" );
        SetStringProperty( entityRelationNode, "name", factionName, "" );
        SetIntProperty( entityRelationNode, "hookupID", entityRelationIter->first, 0 );
        SetIntProperty( entityRelationNode, "status", entityRelation.status, 0 );
        AddChild( entityRelationsNode, entityRelationNode );
    }
    if (hasEntityRelations)
    {
        AddChild( parent, factionNode );
        AddChild( factionNode, entityRelationsNode );
    }

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Faction::CleanUpPointers( std::map< int, Entity* > hookupMap )
{
    if (m_entityRelationships.size() == 0)
        return;

    FactionRelationshipMap newRelationshipMap;

    for (FactionRelationshipMap::const_iterator entityRelationIter = m_entityRelationships.begin(); entityRelationIter != m_entityRelationships.end(); ++entityRelationIter)
    {
        int hookupID = entityRelationIter->first;

        Entity* entity = hookupMap[hookupID];

        if ( entity )
            newRelationshipMap.insert( std::pair< unsigned int, FactionRelationship >( (unsigned int)entity->GetEntityID(), entityRelationIter->second ) );
    }

    m_entityRelationships.clear();
    m_entityRelationships.swap( newRelationshipMap );

}

////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Faction::AdjustRelationshipStatus( unsigned int factionID, unsigned int entityID, int changeAmount )
{
    int adjustedChange = changeAmount;

    if (entityID != 0)
    {
        adjustedChange = changeAmount / 2;

        FactionRelationshipMap::iterator entityRelationIter = m_entityRelationships.find( entityID );
        if (entityRelationIter != m_entityRelationships.end())
        {
            FactionRelationship& relation = entityRelationIter->second;
            // TODO: should check if factionID's match as well
            relation.status += changeAmount;
//             return;
        }
        else
        {
            FactionRelationship newRelation;
            newRelation.factionID = factionID;
            newRelation.status = (int)FR_NEUTRAL + changeAmount;

            m_entityRelationships.insert( std::pair< unsigned int, FactionRelationship >( entityID, newRelation ) );
//             return;
        }
    }

    FactionRelationshipMap::iterator factionRelationIter = m_factionRelationships.find( factionID );
    if (factionRelationIter != m_factionRelationships.end())
    {
        FactionRelationship& relation = factionRelationIter->second;
        relation.status += adjustedChange;
    }
    else
    {
        FactionRelationship newRelation;
        newRelation.factionID = factionID;
        newRelation.status = (int)FR_NEUTRAL + adjustedChange;

        m_factionRelationships.insert( std::pair< unsigned int, FactionRelationship >( factionID, newRelation ) );
//         return;
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

