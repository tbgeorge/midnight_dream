//=================================================================================
// DungeonGenerator.hpp
// Author: Tyler George
// Date  : August 31, 2015
//=================================================================================

#pragma once

#ifndef __included_DungeonGenerator__
#define __included_DungeonGenerator__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/FontRenderer.hpp"

#include "GameCode/Generators/BaseMapGenerator.hpp"


///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------

enum DungeonFeature
{
    DF_ROOM,
    DF_HALL, 
    DF_NUM_FEATURES
};


////===========================================================================================
///===========================================================================================
// DungeonGenerator Class
///===========================================================================================
////===========================================================================================
class DungeonGenerator : public BaseMapGenerator
{
public:
    ///---------------------------------------------------------------------------------
    /// Constructors/Destructors
    ///---------------------------------------------------------------------------------
    DungeonGenerator( const std::string& name );
    ~DungeonGenerator();

    ///---------------------------------------------------------------------------------
    /// Initialization
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Accessors/Queries
    ///---------------------------------------------------------------------------------
    static BaseMapGenerator* CreateMapGenerator( const std::string& name );
    Map* CreateMap( const IntVector2& mapSize = IntVector2::ZERO );
    bool ProcessOneStep( Map* map );

    ///---------------------------------------------------------------------------------
    /// Mutators
    ///---------------------------------------------------------------------------------
    bool FillMapRect( Map* map, const AABB2D& rectBounds, CellType fillType );

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
    static MapGeneratorRegistration s_dungeonGeneratorRegistration;

private:
    ///---------------------------------------------------------------------------------
    /// Private Functions
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Private Member Variables
    ///---------------------------------------------------------------------------------
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif