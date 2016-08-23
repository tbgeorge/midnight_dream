//=================================================================================
// StatusBar.cpp
// Author: Tyler George
// Date  : September 21, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/StatusBar.hpp"
#include "Engine/Utilities/StringTable.hpp"


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
StatusBar::StatusBar( Player* player )
    : m_player( player )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
StatusBar::~StatusBar()
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

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void StatusBar::Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer, const Font& font, const unsigned int& shaderID, bool debugModeEnabled )
{
    UNUSED( debugModeEnabled );

    if (!renderer)
        return;

    renderer->Disable( GL_CULL_FACE );
    renderer->Disable( GL_DEPTH_TEST );
    renderer->Enable( GL_BLEND );
    renderer->Enable( GL_TEXTURE_2D );
    renderer->BlendFunct( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // Title
    int size = 48;
    std::string playerHealth = "Health: " + std::to_string( m_player->GetHealth() ) + "/" + std::to_string( m_player->GetMaxHealth() );
    std::string numTurns = "Turns Taken: " + std::to_string( m_player->GetTurnsTaken() );

    std::string equipmentTitle = "Equipment";
    std::string equipmentUnderline =      "_________";

    std::string potionsTitle = "Potions";
    std::string potionsUnderline =    "_______";

    Equipment playerEquipment = m_player->GetInventory()->GetEquipment();
    std::string headStr = "HEAD:";
    std::string headItemStr = playerEquipment[ES_HEAD] ? playerEquipment[ES_HEAD]->GetName() : "";
    std::string chestStr = "CHEST:"; 
    std::string chestItemStr = playerEquipment[ES_CHEST] ? playerEquipment[ES_CHEST]->GetName() : "";
    std::string legsStr = "LEGS:"; 
    std::string legsItemStr = playerEquipment[ES_LEGS] ? playerEquipment[ES_LEGS]->GetName() : "";
    std::string feetStr = "FEET:"; 
    std::string feetItemStr = playerEquipment[ES_FEET] ? playerEquipment[ES_FEET]->GetName() : "";
    std::string primaryStr = "PRIMARY:"; 
    std::string primaryItemStr = playerEquipment[ES_PRIMARY] ? playerEquipment[ES_PRIMARY]->GetName() : "";
    std::string secondaryStr = "SECONDARY:"; 
    std::string secondaryItemStr = playerEquipment[ES_SECONDARY] ? playerEquipment[ES_SECONDARY]->GetName() : "";

    Vector3 initialLinePos = Vector3( 1300.0f, 750.0f, 1.0f );
    Vector3 nextLineOffset = Vector3(0.0f, -50.0f, 0.0f);

    Vector3 pos = initialLinePos;
    fontRenderer->DrawFontText(  size, font, playerHealth, pos, Rgba( 0x660066 ) );

    pos = initialLinePos + 1.0f * nextLineOffset;
    fontRenderer->DrawFontText(  size, font, numTurns, pos, Rgba( 0x660066 ) );

    pos = initialLinePos + 2.0f * nextLineOffset;
    fontRenderer->DrawFontText(  size, font, equipmentTitle, pos, Rgba( 0x660066 ) );
    pos = initialLinePos + 2.0f * nextLineOffset;
    fontRenderer->DrawFontText(  size, font, equipmentUnderline, pos, Rgba( 0x660066 ) );

    pos = initialLinePos + 6.0f * nextLineOffset;
    fontRenderer->DrawFontText(  size, font, potionsTitle, pos, Rgba( 0x660066 ) );
    pos = initialLinePos + 6.0f * nextLineOffset;
    fontRenderer->DrawFontText(  size, font, potionsUnderline, pos, Rgba( 0x660066 ) );

    size = 24;
    pos = initialLinePos + 2.5f * nextLineOffset;
    fontRenderer->DrawFontText(  size, font, headStr, pos, Rgba( 0x660066 ) );
    pos = initialLinePos + 3.0f * nextLineOffset;
    fontRenderer->DrawFontText(  size, font, chestStr, pos, Rgba( 0x660066 ) );
    pos = initialLinePos + 3.5f * nextLineOffset;
    fontRenderer->DrawFontText(  size, font, legsStr, pos, Rgba( 0x660066 ) );
    pos = initialLinePos + 4.0f * nextLineOffset;
    fontRenderer->DrawFontText(  size, font, feetStr, pos, Rgba( 0x660066 ) );
    pos = initialLinePos + 4.5f * nextLineOffset;
    fontRenderer->DrawFontText(  size, font, primaryStr, pos, Rgba( 0x660066 ) );
    pos = initialLinePos + 5.0f * nextLineOffset;
    fontRenderer->DrawFontText(  size, font, secondaryStr, pos, Rgba( 0x660066 ) );

  


    Items potions = m_player->GetInventory()->GetAllItemsOfType( IT_POTION );
    std::map< unsigned int, std::string > potionStrings;
    std::map< unsigned int, int > potionQuantities;

    for (Items::iterator potionIter = potions.begin(); potionIter != potions.end(); ++potionIter)
    {
        Item* potion = *potionIter;

        unsigned int potionTypeID = StringTable::GetStringID( potion->GetName() );

        std::map< unsigned int, std::string >::iterator potionStringIter = potionStrings.find( potionTypeID );

        if (potionStringIter == potionStrings.end())
        {
            potionStrings[potionTypeID] = potion->GetName();
            potionQuantities[potionTypeID] = 1;
        }
        else
            potionQuantities[potionTypeID]++;
    }

    float lineMultiplier = 6.5f;
    for (std::map< unsigned int, int >::iterator quantityIter = potionQuantities.begin(); quantityIter != potionQuantities.end(); ++quantityIter)
    {
        int quantity = quantityIter->second;

        pos = initialLinePos + lineMultiplier * nextLineOffset;
        fontRenderer->DrawFontText( size, font, std::to_string(quantity) + "x" , pos, Rgba( 0x660066 ) );
        lineMultiplier += 0.5f;
    }



    initialLinePos.x = initialLinePos.x + 125.0f;
    pos = initialLinePos + 2.5f * nextLineOffset;
    fontRenderer->DrawFontText( size, font, headItemStr, pos, Rgba( 0x660066 ) );
    pos = initialLinePos + 3.0f * nextLineOffset;
    fontRenderer->DrawFontText( size, font, chestItemStr, pos, Rgba( 0x660066 ) );
    pos = initialLinePos + 3.5f * nextLineOffset;
    fontRenderer->DrawFontText( size, font, legsItemStr, pos, Rgba( 0x660066 ) );
    pos = initialLinePos + 4.0f * nextLineOffset;
    fontRenderer->DrawFontText( size, font, feetItemStr, pos, Rgba( 0x660066 ) );
    pos = initialLinePos + 4.5f * nextLineOffset;
    fontRenderer->DrawFontText( size, font, primaryItemStr, pos, Rgba( 0x660066 ) );
    pos = initialLinePos + 5.0f * nextLineOffset;
    fontRenderer->DrawFontText( size, font, secondaryItemStr, pos, Rgba( 0x660066 ) );

    initialLinePos.x = initialLinePos.x - 75.0f;
    lineMultiplier = 6.5f;
    for (std::map< unsigned int, std::string >::iterator potionNameIter = potionStrings.begin(); potionNameIter != potionStrings.end(); ++potionNameIter)
    {
        std::string potionName = potionNameIter->second;

        pos = initialLinePos + lineMultiplier * nextLineOffset;
        fontRenderer->DrawFontText( size, font, potionName, pos, Rgba( 0x660066 ) );
        lineMultiplier += 0.5f;
    }



    

}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

