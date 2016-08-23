//=================================================================================
// GameOverMenu.cpp
// Author: Tyler George
// Date  : August 20, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "Engine/Utilities/DeveloperConsole.hpp"
#include "GameCode/GameOverMenu.hpp"
#include "Game.hpp"

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
GameOverMenu::GameOverMenu( Vector2 displaySize, unsigned int basicShaderID, unsigned int fontShaderID )
    : m_displaySize( displaySize )
    , m_basicShaderID( basicShaderID )
    , m_fontShaderID( fontShaderID )
    , m_state( GOMS_DO_NOTHING )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
GameOverMenu::~GameOverMenu()
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
void GameOverMenu::Startup( Font* titleFont, Font* font )
{
    m_font = font;
    m_titleFont = titleFont;
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
void GameOverMenu::Reset()
{
    SetState( GOMS_DO_NOTHING );
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void GameOverMenu::ProcessInput( InputSystem* inputSystem, double deltaseconds )
{
    m_mouseCoords = inputSystem->GetMousePosition();

    if (inputSystem->WasKeyJustReleased( 'Q' ))
        SetState( GOMS_IS_READY_TO_QUIT );

    if (inputSystem->WasKeyJustReleased( VK_ESCAPE ) || inputSystem->WasKeyJustReleased( 'M' ))
        SetState( GOMS_GO_TO_QUEST );

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
GameOverMenuState GameOverMenu::Update( double deltaseconds, bool debugModeEnabled )
{


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
void GameOverMenu::Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer, bool debugModeEnabled )
{
    renderer->Disable( GL_CULL_FACE );
    renderer->Disable( GL_DEPTH_TEST );
    renderer->Enable( GL_BLEND );
    renderer->Enable( GL_TEXTURE_2D );
    renderer->BlendFunct( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // Title
    int titleSize = 120;
    std::string title = "Game Over";
    float titleWidth = fontRenderer->CalcTextWidth( titleSize, *m_titleFont, title );
    float xOffsetTitle = (m_displaySize.x - titleWidth) / 2.0f;

    Vector3 titlePos( xOffsetTitle, 600.0f, 1.0f );
    fontRenderer->DrawFontText( titleSize, *m_titleFont, title, titlePos, Rgba( 0x660066 ) );

    // Game over reason
    int reasonSize = 64;
    std::string reason = m_player->HasDied() ? "You died" : "You defeated The Dreamer!";
    float reasonWidth = fontRenderer->CalcTextWidth( reasonSize, *m_titleFont, reason );
    float xOffsetReason = (m_displaySize.x - reasonWidth) / 2.0f;

    Vector3 reasonPos( xOffsetReason, 540.0f, 1.0f );
    fontRenderer->DrawFontText( reasonSize, *m_titleFont, reason, reasonPos, Rgba( 0x660066 ) );





    float buttonXOffset = 700.0f;
    float buttonYSpacing = 20.0f;
    int buttonSize = 48;
    Rgba buttonColor( 0x660033 );

    float nextButtonYPos = 450.0f;


    // Play Button
    std::string playText = "M) Quest Menu";
    Vector3 playButtonPos( buttonXOffset, nextButtonYPos, 1.0f );
    fontRenderer->DrawFontText( buttonSize, *m_titleFont, playText, playButtonPos, buttonColor );
    nextButtonYPos = nextButtonYPos - buttonSize - buttonYSpacing;

    // Quit Button
    std::string quitText = "Q) Quit";
    Vector3 quitButtonPos( buttonXOffset, nextButtonYPos, 1.0f );
    fontRenderer->DrawFontText( buttonSize, *m_titleFont, quitText, quitButtonPos, buttonColor );
    nextButtonYPos = nextButtonYPos - buttonSize - buttonYSpacing;

    // Stats
    std::string actorsKilled = "Enemies killed: " + std::to_string( m_player->GetActorsKilled() );
    std::string turnsTaken = "Turns taken: " + std::to_string( m_player->GetTurnsTaken() );
    Rgba statColor = Rgba::BLACK;
    Vector3 statPos( 20.0f, 20.0f, 1.0f );
    fontRenderer->DrawFontText( 36, *m_titleFont, actorsKilled, statPos, statColor );
    statPos.x += 20.0f;
    fontRenderer->DrawFontText( 36, *m_titleFont, turnsTaken, statPos, statColor );


    if (debugModeEnabled)
    {
        std::string mouseCoords = std::to_string( m_mouseCoords.x ) + ", " + std::to_string( m_mouseCoords.y );
        Vector3 mousePos( 10.0f, m_displaySize.y - 58.0f, 1.0f );
        fontRenderer->DrawFontText( 48, *m_font, mouseCoords, mousePos, Rgba::WHITE );

    }


}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void GameOverMenu::SetState( GameOverMenuState newState )
{
    if (m_state != newState)
    {
        m_state = newState;
    }
    else
    {
        DeveloperConsole::WriteLine( "GameOverMenu: Resetting State", WARNING_TEXT_COLOR );
    }
}