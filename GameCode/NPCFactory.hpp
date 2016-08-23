//=================================================================================
// NPCFactory.hpp
// Author: Tyler George
// Date  : September 10, 2015
//=================================================================================

#pragma once

#ifndef __included_NPCFactory__
#define __included_NPCFactory__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include <map>

#include "Engine\Utilities\XMLParser.h"
#include "Engine\Math\IntRange.hpp"

#include "GameCode\Entities\NPC.hpp"

class NPCFactory;

///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------
typedef std::map< std::string, NPCFactory* > NPCFactories;

////===========================================================================================
///===========================================================================================
// NPCFactory Class
///===========================================================================================
////===========================================================================================
class NPCFactory
{
public:
	///---------------------------------------------------------------------------------
	/// Constructors/Destructors
	///---------------------------------------------------------------------------------
    NPCFactory( XMLNode& npcNode );
    ~NPCFactory();

	///---------------------------------------------------------------------------------
	/// Initialization
	///---------------------------------------------------------------------------------
    static bool LoadAllNPCFactories();

	///---------------------------------------------------------------------------------
	/// Accessors/Queries
	///---------------------------------------------------------------------------------
    static NPCFactory* FindFactoryByName( const std::string& name );
    static NPCFactories& GetFactories() { return s_NPCFactories; }

    NPC* SpawnNPC( XMLNode& possibleSaveData );

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Update
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Render
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Public Member Variables
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Static Variables
	///---------------------------------------------------------------------------------

private:
    ///---------------------------------------------------------------------------------
    /// Private Member Variables
    ///---------------------------------------------------------------------------------
    IntRange m_health;


protected:

    ///---------------------------------------------------------------------------------
    /// Protected Static Variables
    ///---------------------------------------------------------------------------------
    static NPCFactories s_NPCFactories;

    ///---------------------------------------------------------------------------------
    /// Protected Member Variables
    ///---------------------------------------------------------------------------------
    NPC* m_templateNPC;
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif