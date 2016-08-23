//=================================================================================
// CellularAutomatonMapGenerator.cpp
// Author: Tyler George
// Date  : August 20, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/Generators/CellularAutomatonMapGenerator.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

 MapGeneratorRegistration CellularAutomatonMapGenerator::s_cellularAutomatonMapGeneratorRegistration( "Cellular Automaton", &CellularAutomatonMapGenerator::CreateMapGenerator );


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
CellularAutomatonMapGenerator::CellularAutomatonMapGenerator( const std::string& name )
    : BaseMapGenerator( name )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
CellularAutomatonMapGenerator::~CellularAutomatonMapGenerator()
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
BaseMapGenerator* CellularAutomatonMapGenerator::CreateMapGenerator( const std::string& name )
{
    return new CellularAutomatonMapGenerator( name );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Map* CellularAutomatonMapGenerator::CreateMap( const IntVector2& mapSize )
{
    const float percentOfAir = 60.0f;
    Map* map = new Map( mapSize );

    Cells* cells = map->GetCells();

    int seed = TripleRandomInt();
    map->SetSeed( seed );
    srand( seed );

    int numAirCells = 0;
    int i = 0;
    for (Cells::iterator cellIter = cells->begin(); cellIter != cells->end(); ++cellIter)
    {
        Cell& cell = *cellIter;

        float chance = GetRandomFloatZeroToOne();
        if (chance < percentOfAir / 100.0f) // (i == 1820 || i == 1821 || i == 1821 + ( 2*MAP_WIDTH_CELLS) || i == 1823 + MAP_WIDTH_CELLS || i == 1824 || i == 1825 || i == 1826 )
        {
            cell.SetType( CT_AIR );
            //cell.SetActive( true );
            ++numAirCells;
        }
        i++; // #KSH: I see this in the loops down below...for debugging?  Habit?
    }
    

    float percentAir = (float)numAirCells / (float)cells->size();
    ConsolePrintf( "%f percent air. \n", percentAir );
    return map;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool CellularAutomatonMapGenerator::ProcessOneStep( Map* map )
{
    static int step1Iterations = 20;
    static int step2Iterations = 2;

    Cells* cells = map->GetCells();
    Cells updatedCells = *cells;

    // Step 1
    if ( step1Iterations != 0 )
    {
        
        int i = 0; // #KSH Here it is again...
        for (Cells::const_iterator cellIter = cells->begin(); cellIter != cells->end(); ++cellIter )
        {
            Cell cell = *cellIter;

            if (map->GetNumCellsInRadiusOfType( cell.GetMapPosition(), 1, CT_WALL ) >= 5)
            {
                updatedCells[i].SetType( CT_WALL );
                ++i; // #KSH: If we're going to increment i regardless (here and down below), put it at the top of the loop? Inside the loop?
                continue;
            }
//             else
//                 updatedCells[i].SetType( CT_AIR );

            else if (map->GetNumCellsInRadiusOfType( cell.GetMapPosition(), 2, CT_WALL ) <= 2)
                updatedCells[i].SetType( CT_WALL );
            else
                updatedCells[i].SetType( CT_AIR );


            // game of life

//             int numActiveNeighbors = map->GetNumActiveNeighbors( cell.GetMapPosition() );
//             if (numActiveNeighbors < 2 && cell.IsActive())
//                 updatedCells[i].SetActive( false );
// //             else if ( (numActiveNeighbors == 2 || numActiveNeighbors == 3 ) && cell.IsActive())
// //                 updatedCells[i].SetActive( true );
//             else if (numActiveNeighbors == 3 && !cell.IsActive())
//             {
//                 updatedCells[i].SetActive( true );
//                 updatedCells[i].SetType( CT_AIR );
//             }
//             else if (numActiveNeighbors > 3 && cell.IsActive())
//                 updatedCells[i].SetActive( false );

                

            ++i; // #KSH: You just like counting iterations :)
        }
        *cells = updatedCells;
        step1Iterations--;
    }

    // Step 2
    if ( step1Iterations == 0 && step2Iterations != 0)
    {
        int i = 0; // #KSH: Aha, you just want a way to know what iteration you're on, when you're using iterators. I understand that, but as it
		// was, I spent a bit of time trying to figure out how you were using i--it has such a history of being the index variable that I was expecting
		// it to be used.  Maybe calling it iterationCounter and then tacking it onto the end of the for statement would help to clarify and hide it...
		for (Cells::const_iterator cellIter = cells->begin(); cellIter != cells->end(); ++cellIter ) // #KSH: ,++i
        {
            Cell cell = *cellIter;

            if (map->GetNumCellsInRadiusOfType( cell.GetMapPosition(), 1, CT_WALL ) >= 5)
                updatedCells[i].SetType( CT_WALL );
            else
                updatedCells[i].SetType( CT_AIR );

            ++i;
        }

        *cells = updatedCells;
        step2Iterations--;
    }

    if (step1Iterations == 0 && step2Iterations == 0)
    {
        step1Iterations = 20;
        step2Iterations = 2;
        return true;
    }
    return false;
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

