//=================================================================================
// QuestMenu.cpp
// Author: Tyler George
// Date  : August 26, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/QuestMenu.hpp"
#include "Generators/BaseMapGenerator.hpp"
#include "Engine/Utilities/DeveloperConsole.hpp"


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
QuestMenu::QuestMenu( const Vector2& displaySize, const unsigned int& basicShaderID, const unsigned int& fontShaderID )
    : m_displaySize( displaySize )
    , m_basicShaderID( basicShaderID )
    , m_fontShaderID( fontShaderID )
    , m_font( nullptr )
    , m_generatorToUse( nullptr )
    , m_oneStep( false )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
QuestMenu::~QuestMenu()
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
void QuestMenu::Startup( Font* titleFont, Font* font)
{
    m_font = font;
    m_titleFont = titleFont;
    MapGeneratorRegistryMap* generatorMap = MapGeneratorRegistration::GetMapGeneratorRegistry();

    for (MapGeneratorRegistryMap::const_iterator generatorIter = generatorMap->begin(); generatorIter != generatorMap->end(); ++generatorIter)
    {

        std::string generatorName = generatorIter->first;
        if (generatorName == "catacomb generator")
            continue;
        MapGeneratorRegistration* generatorRegistration = generatorIter->second;
        m_generators[generatorName] = generatorRegistration->CreateMapGenerator();
    }

    SetState( QMS_DO_NOTHING );
}

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
///
///---------------------------------------------------------------------------------
void QuestMenu::Reset()
{
    SetState( QMS_DO_NOTHING );
    m_generatorToUse = nullptr;
}


////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void QuestMenu::ProcessInput( InputSystem* inputSystem, double deltaSeconds )
{
//     int generatorKey = 65;
    UNUSED( deltaSeconds );

    if (inputSystem->WasKeyJustReleased( VK_TAB ))
        m_oneStep = !m_oneStep;

    if (inputSystem->WasKeyJustReleased( VK_ESCAPE ))
    {
        SetState( QMS_BACK );
        return;
    }

    for (GeneratorMap::const_iterator generatorIter = m_generators.begin(); generatorIter != m_generators.end(); ++generatorIter)
    {
        if (inputSystem->WasKeyJustReleased( m_generatorKeys[generatorIter->first] ))
        {
            m_generatorToUse = m_generators[generatorIter->first];
            break;
        }
    }

    if (m_generatorToUse)
    {
        if (m_oneStep)
            SetState( QMS_ONE_STEP );
        else
            SetState( QMS_AUTOMATIC );
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
QuestMenuState QuestMenu::Update( double deltaSeconds, bool debugModeEnabled )
{
    UNUSED( deltaSeconds );
    UNUSED( debugModeEnabled );

    return m_state;
}


////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void QuestMenu::Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer,  bool debugModeEnabled )
{
    if (!renderer)
        return;

    UNUSED( debugModeEnabled );

    renderer->Disable( GL_CULL_FACE );
    renderer->Disable( GL_DEPTH_TEST );
    renderer->Enable( GL_BLEND );
    renderer->Enable( GL_TEXTURE_2D );
    renderer->BlendFunct( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // Title
    int titleSize = 120;
    std::string title = "Quest Menu";
    float titleWidth = fontRenderer->CalcTextWidth( titleSize, *m_titleFont, title );
    float xOffsetTitle = (m_displaySize.x - titleWidth) / 2.0f;

    Vector3 titlePos( xOffsetTitle, 600.0f, 1.0f );
    fontRenderer->DrawFontText( titleSize, *m_titleFont, title, titlePos, Rgba( 0x660066 ) );

    // Buttons
    float buttonXOffset = 600.0f;
    float buttonYSpacing = 20.0f;
    int buttonSize = 48;
    Rgba buttonColor( 0x660033 );

    float nextButtonYPos = 450.0f;

    char generatorCount = 65;
    for (GeneratorMap::const_iterator generatorIter = m_generators.begin(); generatorIter != m_generators.end(); ++generatorIter)
    {
        BaseMapGenerator* mapGenerator = generatorIter->second;
        std::string buttonText;
        buttonText.push_back( generatorCount );
        buttonText +=  ") " + mapGenerator->GetName();

        Vector3 buttonPos( buttonXOffset, nextButtonYPos, 1.0f );
        fontRenderer->DrawFontText( buttonSize, *m_titleFont, buttonText, buttonPos, buttonColor );
        nextButtonYPos = nextButtonYPos - buttonSize - buttonYSpacing;

        m_generatorKeys[generatorIter->first] = generatorCount++;
    }

    // One Step on/off
    int size = 32;
    std::string oneStepStr = m_oneStep ? "Tab) One Step Generation : On" : "Tab) One Step Generation : Off";
    Rgba oneStepColor = m_oneStep ? Rgba::GREEN : Rgba::RED;
//    float width = fontRenderer->CalcTextWidth( size, *m_titleFont, oneStepStr );

    Vector3 oneStepStrPos( 20.0f, 20.0f, 1.0f );
    fontRenderer->DrawFontText( size, *m_titleFont, oneStepStr, oneStepStrPos, oneStepColor );
    
}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void QuestMenu::SetState( QuestMenuState state )
{
    if (m_state != state)
    {
        m_state = state;
    }
    else
    {
        DeveloperConsole::WriteLine( "Quest Menu: Resetting State", WARNING_TEXT_COLOR );
    }

}
