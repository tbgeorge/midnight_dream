//=================================================================================
// NPCFactory.cpp
// Author: Tyler George
// Date  : September 10, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================
#include "Engine/Utilities/FileUtilities.hpp"
#include "Engine/Utilities/Utilities.hpp"
#include "Engine/Utilities/XMLHelper.hpp"

#include "GameCode/NPCFactory.hpp"
#include "Engine/Utilities/DeveloperConsole.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
/// Protected Variables
///---------------------------------------------------------------------------------
NPCFactories NPCFactory::s_NPCFactories;

////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
NPCFactory::NPCFactory(  XMLNode& npcNode )
{
    m_templateNPC = new NPC( npcNode );

    m_health = GetIntIntervalProperty( npcNode, "health" );

    // Generate AI Behaviors list
    AIBehaviorRegistryMap* behaviorMap = AIBehaviorRegistration::GetAIBehaviorRegistry();

    int behaviorCounter = 0;
    bool hasAnotherBehavior;

    do
    {
        XMLNode& behaviorNode = npcNode.getChildNode( "AIBehavior", behaviorCounter++ );
        if (!behaviorNode.isEmpty())
        {
            hasAnotherBehavior = true;
            std::string name = GetStringProperty( behaviorNode, "name", "unknown behavior", true );
            if (name == "unknown behavior")
            {
                // Soft fail
                DeveloperConsole::WriteLine( "Attempted to load AI Behavior " + std::to_string( behaviorCounter ) + " on NPC: " + m_templateNPC->GetName() + ". XML doesn't contain \"name\".", WARNING_TEXT_COLOR );
                continue;
            }
            
            AIBehaviorRegistryMap::const_iterator behaviorIter = behaviorMap->find( name );
            if (behaviorIter == behaviorMap->end())
            {
                // Soft fail
                DeveloperConsole::WriteLine( "Attempted to load AI Behavior " + std::to_string( behaviorCounter ) + " on NPC: " + m_templateNPC->GetName() + ". Behavior: " + name + " does not exist.", WARNING_TEXT_COLOR );
                continue;
            }

            AIBehaviorRegistration* behaviorRegistration = behaviorIter->second;
            BaseAIBehavior* behavior = behaviorRegistration->CreateAIBehavior( behaviorNode );

            behavior->SetActor( m_templateNPC );
            m_templateNPC->AddBehavior( behavior );
        }
        else
            hasAnotherBehavior = false;

    } while (hasAnotherBehavior);
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
NPCFactory::~NPCFactory()
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
bool NPCFactory::LoadAllNPCFactories()
{
    std::string directoryToSearch = "Data/NPCs/";
    Strings npcFiles;
    bool success = FindAllFilesOfType( directoryToSearch, "*.npc.xml", npcFiles );

    if (success)
    {
        for (Strings::const_iterator npcFileIter = npcFiles.begin(); npcFileIter != npcFiles.end(); ++npcFileIter)
        {
            std::string filePath = *npcFileIter;
            // need to handle error checking here
            XMLNode npcRoot = XMLNode::parseFile( filePath.c_str(), "NPC" );

            std::string fileName = ExtractFileNameFromPath( filePath );
            std::string nameOfNPC = RemoveFileExtension( fileName );

            NPCFactory* npcFactory = new NPCFactory( npcRoot );

            s_NPCFactories.insert( std::pair< std::string, NPCFactory* >( nameOfNPC, npcFactory ) );

            npcRoot.deleteNodeContent();
        }
        return true;
    }
    else
        return false;
}

////===========================================================================================
///===========================================================================================
// Accessors/Queries
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
NPCFactory* NPCFactory::FindFactoryByName( const std::string& name )
{
    NPCFactories::iterator factoryIter = s_NPCFactories.find( name );
    if (factoryIter == s_NPCFactories.end())
    {
        //__debugbreak();
        return nullptr;
    }
    else
        return s_NPCFactories.at( name );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
NPC* NPCFactory::SpawnNPC( XMLNode& possibleSaveData )
{
    NPC* newNPC = new NPC( *m_templateNPC, possibleSaveData );

    if (possibleSaveData.isEmpty() || newNPC->GetMaxHealth() == 0)
    {
        int health = GetRandomValueInIntRangeInclusive( m_health );
        newNPC->SetHealth( health );
        newNPC->SetMaxHealth( health );
    }
    newNPC->SetPrototype( m_templateNPC );

    return newNPC;
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

