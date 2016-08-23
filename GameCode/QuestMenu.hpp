//=================================================================================
// QuestMenu.hpp
// Author: Tyler George
// Date  : August 26, 2015
//=================================================================================

#pragma once

#ifndef __included_QuestMenu__
#define __included_QuestMenu__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/FontRenderer.hpp"
#include "GameCode/Generators/BaseMapGenerator.hpp"

///---------------------------------------------------------------------------------
/// Enums
///---------------------------------------------------------------------------------
enum QuestMenuState
{
    QMS_DO_NOTHING,
    QMS_ONE_STEP,
    QMS_AUTOMATIC,
    QMS_BACK
};

///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------
typedef std::map< std::string, BaseMapGenerator* > GeneratorMap;

////===========================================================================================
///===========================================================================================
// QuestMenu Class
///===========================================================================================
////===========================================================================================
class QuestMenu
{
public:
	///---------------------------------------------------------------------------------
	/// Constructors/Destructors
	///---------------------------------------------------------------------------------
    QuestMenu( const Vector2& displaySize, const unsigned int& basicShaderID, const unsigned int& fontShaderID );
    ~QuestMenu();

	///---------------------------------------------------------------------------------
	/// Initialization
	///---------------------------------------------------------------------------------
    void Startup( Font* titleFont, Font* font );

	///---------------------------------------------------------------------------------
	/// Accessors/Queries
	///---------------------------------------------------------------------------------
    BaseMapGenerator* GetGeneratorToUse() { return m_generatorToUse; }

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------
    void Reset();

	///---------------------------------------------------------------------------------
	/// Update
	///---------------------------------------------------------------------------------
    void ProcessInput( InputSystem* inputSystem, double deltaSeconds );
    QuestMenuState Update( double deltaSeconds, bool debugModeEnabled );

	///---------------------------------------------------------------------------------
	/// Render
	///---------------------------------------------------------------------------------
    void Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer, bool debugModeEnabled );

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
    void SetState( QuestMenuState state );

	///---------------------------------------------------------------------------------
	/// Private Member Variables
	///---------------------------------------------------------------------------------
    Vector2 m_displaySize;
    unsigned int m_basicShaderID;
    unsigned int m_fontShaderID;
    Font* m_font;
    Font* m_titleFont;

    QuestMenuState m_state;
    bool m_oneStep;

    GeneratorMap m_generators;
    std::map< std::string, char > m_generatorKeys;

    BaseMapGenerator* m_generatorToUse;
};


///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif