//=================================================================================
// MessageBar.cpp
// Author: Tyler George
// Date  : September 21, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/MessageBar.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================
MessageBar* MessageBar::s_theMessageBar = new MessageBar();


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MessageBar::~MessageBar()
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
void MessageBar::Startup( Player* player, Font* font )
{
    s_theMessageBar->m_player = player;
    s_theMessageBar->m_font = font;

    s_theMessageBar->m_clearCurrent = true;
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
void MessageBar::AddMessage( Message& msg )
{
    msg.m_playerTurn = s_theMessageBar->m_player->GetTurnsTaken();

    MapPosition playerPos = s_theMessageBar->m_player->GetMapPosition();
    MapPosition msgPos = msg.m_position;
    
    int distance = Map::CalculateManhattanDistance( msgPos, playerPos );

    std::vector< Cell* > visibleCells = s_theMessageBar->m_player->GetVisibleCells();

    for (std::vector< Cell* >::iterator visibleCellIter = visibleCells.begin(); visibleCellIter != visibleCells.end(); ++visibleCellIter)
    {
        Cell* cell = *visibleCellIter;
        if (cell->GetMapPosition() == msgPos)
        {
            s_theMessageBar->m_pendingMessages.push( msg );
            return;
        }
    }

    s_theMessageBar->m_archivedMessages.push_back( msg );

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MessageBar::TestMessages()
{
    Message msg;
    msg.m_text = "THIS IS A TEST. ";
    msg.m_position = s_theMessageBar->m_player->GetMapPosition();

    for (int i = 0; i < 30; i++)
        AddMessage( msg );
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MessageBar::ProcessInput( InputSystem* inputSystem, const double& deltaSeconds )
{
    UNUSED( deltaSeconds );

    if (inputSystem->WasKeyJustReleased( VK_SPACE ))
    {
        if (!s_theMessageBar->m_clearCurrent)
            s_theMessageBar->m_clearCurrent = true;
    }

    else
        s_theMessageBar->m_clearCurrent = false;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MessageBar::Update( const double& deltaSeconds, const bool& debugModeEnabled )
{
    UNUSED( deltaSeconds );
    UNUSED( debugModeEnabled );
    
    if (s_theMessageBar->m_clearCurrent )
    {
        for each (Message msg in s_theMessageBar->m_currentMessages )
            s_theMessageBar->m_archivedMessages.push_back( msg );
        
        s_theMessageBar->m_currentMessages.clear();

        float moreLength = FontRenderer::CalcTextWidth( 32, *s_theMessageBar->m_font, "--MORE--" );
        float maxLength = 1590.0f - moreLength;

        float currentLength = 0.0f;
        
        while (!s_theMessageBar->m_pendingMessages.empty() )
        {
            Message msg = s_theMessageBar->m_pendingMessages.front();

            float length = FontRenderer::CalcTextWidth( 32, *s_theMessageBar->m_font, msg.m_text );
            if (currentLength + length > maxLength)
            {
                break;
            }
            else if (currentLength + length <= maxLength)
            {
                s_theMessageBar->m_pendingMessages.pop();
                s_theMessageBar->m_currentMessages.push_back( msg );
                currentLength += length;
            }

        }
        if (s_theMessageBar->m_pendingMessages.empty())
            s_theMessageBar->m_moreMessages = false;
        else
            s_theMessageBar->m_moreMessages = true;
        s_theMessageBar->m_clearCurrent = false;
    }
}

////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MessageBar::Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer, const Font& font, const unsigned int& shaderID, bool debugModeEnabled )
{
    if (!renderer)
        return;

    UNUSED( debugModeEnabled );

    renderer->Disable( GL_CULL_FACE );
    renderer->Disable( GL_DEPTH_TEST );
    renderer->Enable( GL_BLEND );
    renderer->Enable( GL_TEXTURE_2D );
    renderer->BlendFunct( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // Messages
    int size = 32;
    std::string testText = "Test position";

    renderer->DrawColoredQuad( NULL, Vector2::ZERO, Vector2( 1600.0f, 40.0f ), 1.0f, Rgba::BLACK );

    std::string currentMessage = "";
    for each (Message msg in s_theMessageBar->m_currentMessages)
    {
        currentMessage += msg.m_text;
    }

    if (s_theMessageBar->m_moreMessages)
        currentMessage += "--MORE--";

    // make string from current messages
    Vector3 pos( 5.0f, 2.0f, 1.0f );
    fontRenderer->DrawFontText( size, font, currentMessage, pos, Rgba::WHITE );


}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MessageBar::MessageBar()
    : m_clearCurrent( true )
    , m_moreMessages( false )
{

}
