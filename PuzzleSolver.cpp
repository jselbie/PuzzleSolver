// PuzzleSolver.cpp : Code that I wrote to solve the Lagoon "Internet Puzzle Adventure: Valley of the Kings"
// https://www.amazon.com/INTERACTIVE-INTERNET-PUZZLE-ADVENTURE-VALLEY/dp/B01L358FZC
// Basically, the code solves the puzzle game.
//     The idea is that you have 16 tiles, with a hieroglyphic on each side. 
//     Arrange the tiles in a 4x4 grid such that each glyph matches the glyph on the adjacent tile. 
//     After solving it, the center glyph on each tile maps to a password on a website that gives 
//     you additional puzzles.
// This is the code that solves it
// It basically builds of the set of "pairs" and then builds a set of possible "rows" and stuff each row into a hash table
// The hash table is used for the final solution build.

#include <stdint.h>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <assert.h>
using namespace std;

enum class Glyph : int
{
    HEAD,
    FALCON,
    PEACOCK,
    HAND,
    OWL,
    BOWL,
    FOOT,
    BUG
};

// Tile represents one of the physical game pieces and its current rotation
struct Tile
{
    Glyph glyphs[4];
    unsigned int rotation;   // 0 = no rotation, // 1:ninty degrees clockwise, 2:180 degrees, 3:270 degrees clockwise
    char name;
    string toString()
    {
        stringstream ss;
        ss << "{" << name << "," << rotation << "}   ";
        return ss.str();
    }
};

bool DoTilesMatchLeftRight(Tile* t1, Tile* t2)
{
    //  t1->glyphs[1] == t2->glyphs[3]      
    int t1_glyph_index = (5 - t1->rotation) % 4;
    int t2_glyph_index = (7 - t2->rotation) % 4;
    return (t1->glyphs[t1_glyph_index] == t2->glyphs[t2_glyph_index]);
}

bool DoTilesMatchTopBottom(Tile* t1, Tile* t2)
{
    //  t1->glyphs[2] == t2->glyphs[0]
    int t1_glyph_index = (6 - t1->rotation) % 4;
    int t2_glyph_index = (4 - t2->rotation) % 4;
    return (t1->glyphs[t1_glyph_index] == t2->glyphs[t2_glyph_index]);
}

vector<Tile> GetTiles()
{
    Tile t1 = { { Glyph::PEACOCK, Glyph::HAND, Glyph::HEAD, Glyph::FALCON }, 0, 'A' };
    Tile t2 = { { Glyph::PEACOCK, Glyph::HAND, Glyph::OWL, Glyph::FALCON }, 0, 'B' };
    Tile t3 = { { Glyph::PEACOCK, Glyph::FALCON, Glyph::OWL, Glyph::FALCON }, 0, 'C' };
    Tile t4 = { { Glyph::OWL, Glyph::FALCON, Glyph::BOWL, Glyph::FOOT }, 0, 'D' };
    Tile t5 = { { Glyph::OWL, Glyph::OWL, Glyph::FOOT, Glyph::BOWL }, 0, 'E' };
    Tile t6 = { { Glyph::HAND, Glyph::FALCON, Glyph::PEACOCK, Glyph::PEACOCK }, 0, 'F' };
    Tile t7 = { { Glyph::PEACOCK, Glyph::FALCON, Glyph::OWL, Glyph::FALCON }, 0, 'G' };
    Tile t8 = { { Glyph::HAND, Glyph::FOOT, Glyph::BUG, Glyph::HAND }, 0, 'H' };
    Tile t9 = { { Glyph::HEAD, Glyph::HEAD, Glyph::BUG, Glyph::BOWL }, 0, 'I' };
    Tile t10 = { { Glyph::PEACOCK, Glyph::FALCON, Glyph::BOWL, Glyph::OWL }, 0, 'J' };
    Tile t11 = { { Glyph::PEACOCK, Glyph::FALCON, Glyph::OWL, Glyph::OWL }, 0, 'K' };
    Tile t12 = { { Glyph::PEACOCK, Glyph::HAND, Glyph::FALCON, Glyph::BOWL }, 0, 'L' };
    Tile t13 = { { Glyph::PEACOCK, Glyph::PEACOCK, Glyph::FALCON, Glyph::OWL }, 0, 'M' };
    Tile t14 = { { Glyph::PEACOCK, Glyph::FALCON, Glyph::FOOT, Glyph::FALCON }, 0, 'N' };
    Tile t15 = { { Glyph::PEACOCK, Glyph::FALCON, Glyph::OWL, Glyph::OWL }, 0, 'O' };
    Tile t16 = { { Glyph::PEACOCK, Glyph::HAND, Glyph::HEAD, Glyph::FALCON }, 0, 'P' };

    vector<Tile> tiles = { t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16 };
    return tiles;
}

struct RowSignature
{
    unsigned int topsignature;       // bit encoding of what all the glyphs on this sequence's top row are
    unsigned int bottomsignature;    // bit encoding of what all the glpys on this squences's top row are
    unsigned int tilemask;           // bitmask representation of the tiles member below
    int tiles[4];                    // index into standard list of tiles (e.g. if this array is {2,4,6,9}, then it means {t3,t5,t7,t10} from the GetTiles function above
    int rotations[4];                // rotations of each tile
};


void CombinePairsIntoRow(vector<Tile>& tiles, RowSignature& rowsigLeft, RowSignature& rowsigRight, RowSignature* pFinal)
{
    RowSignature finalSig = {};

    finalSig.tiles[0] = rowsigLeft.tiles[0];
    finalSig.tiles[1] = rowsigLeft.tiles[1];
    finalSig.tiles[2] = rowsigRight.tiles[0];
    finalSig.tiles[3] = rowsigRight.tiles[1];

    finalSig.rotations[0] = rowsigLeft.rotations[0];
    finalSig.rotations[1] = rowsigLeft.rotations[1];
    finalSig.rotations[2] = rowsigRight.rotations[0];
    finalSig.rotations[3] = rowsigRight.rotations[1];

    finalSig.tilemask = rowsigLeft.tilemask | rowsigRight.tilemask;
    

    for (int i = 0; i < 4; i++)
    {
        int tile_index = finalSig.tiles[i];
        Tile& tile = tiles[tile_index];
        tile.rotation = finalSig.rotations[i];

        // top
        unsigned int glyph_index = (4 - tile.rotation) % 4;
        unsigned int glyph_value = (unsigned int)(tile.glyphs[glyph_index]);
        glyph_value = glyph_value & 0x0f;
        finalSig.topsignature = finalSig.topsignature << 4;
        finalSig.topsignature = finalSig.topsignature | glyph_value;

        // bottom
        glyph_index = (6 - tile.rotation) % 4;
        glyph_value = (unsigned int)(tile.glyphs[glyph_index]);
        glyph_value = glyph_value & 0x0f;
        finalSig.bottomsignature = finalSig.bottomsignature << 4;
        finalSig.bottomsignature = finalSig.bottomsignature | glyph_value;
    }

    *pFinal = finalSig;
}


// this function generats all the rows, but puts them in a hash table that maps "top signature" of the row to a vector of available rows
void GenerateAllRows(unordered_map<unsigned int, vector<RowSignature>>& finalRowMap)
{

    vector<RowSignature> startingpairs;
    vector<RowSignature> row;

    int count = 0;

    finalRowMap.clear();

    // generate all starting pairs
    auto tiles = GetTiles();
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            if (i == j)
            {
                continue;
            }

            for (int a = 0; a < 4; a++)
            {
                tiles[i].rotation = a;
                for (int b = 0; b < 4; b++)
                {
                    tiles[j].rotation = b;
                    if (DoTilesMatchLeftRight(&tiles[i], &tiles[j]))
                    {
                        count++;
                        cout << "Found starting pair: " << count << " " << tiles[i].toString() << tiles[j].toString() << endl;

                        RowSignature entry = {};
                        entry.rotations[0] = a;
                        entry.rotations[1] = b;
                        entry.tilemask = (1 << i) | (1 << j);
                        entry.tiles[0] = i;
                        entry.tiles[1] = j;

                        startingpairs.push_back(entry);
                    }
                }
            }
        }
    }

    cout << "Finished computing all the starting pairs.  Now computing all possible valid 4 piece rows" << endl;

    // at this point, we have all 622 combinations of piece that can be combined together as adjacent tiles (left to right)

    // now for each adjacent pair, build all the possible 4 tile rows and shove into a hash table
    // this is "n-squared", but I suspect we could have simply taken the result list above and made a hash table as well out of it
    // the resulting set of rows is about 50000 total.  That's why we're building a hash table for the final part - to avoid the 50000^4 set of operations we would need to do to evaluate all the possible boards
    count = 0;
    for (RowSignature &rowsigLeft : startingpairs)
    {
        for (RowSignature& rowsigRight : startingpairs)
        {
            if (rowsigLeft.tilemask & rowsigRight.tilemask)
            {
                // overlap - no longer a candidate
                continue;
            }

            Tile t1 = tiles[rowsigLeft.tiles[1]];
            t1.rotation = rowsigLeft.rotations[1];
            Tile t2 = tiles[rowsigRight.tiles[0]];
            t2.rotation = rowsigRight.rotations[0];

            if (DoTilesMatchLeftRight(&t1, &t2))
            {
                RowSignature rowsigFinal = {};
                CombinePairsIntoRow(tiles, rowsigLeft, rowsigRight, &rowsigFinal);
                count++;

                //cout << "row formed: count=" << count << std::hex << "    topsig=" << rowsigFinal.topsignature << std::dec << endl;
                //cout << tiles[rowsigFinal.tiles[0]].toString() << " " << tiles[rowsigFinal.tiles[1]].toString() << " " << tiles[rowsigFinal.tiles[2]].toString() << " " << tiles[rowsigFinal.tiles[3]].toString() << endl;

                finalRowMap[rowsigFinal.topsignature].push_back(rowsigFinal);
            }
        }
    }

    cout << "Done computing rows (total number of rows == " << count << ")" << endl;
}


void PrintSolution(vector<Tile>& tiles, RowSignature* pRow1, RowSignature* pRow2, RowSignature* pRow3, RowSignature* pRow4)
{
    tiles[pRow1->tiles[0]].rotation = pRow1->rotations[0];
    tiles[pRow1->tiles[1]].rotation = pRow1->rotations[1];
    tiles[pRow1->tiles[2]].rotation = pRow1->rotations[2];
    tiles[pRow1->tiles[3]].rotation = pRow1->rotations[3];

    tiles[pRow2->tiles[0]].rotation = pRow2->rotations[0];
    tiles[pRow2->tiles[1]].rotation = pRow2->rotations[1];
    tiles[pRow2->tiles[2]].rotation = pRow2->rotations[2];
    tiles[pRow2->tiles[3]].rotation = pRow2->rotations[3];

    tiles[pRow3->tiles[0]].rotation = pRow3->rotations[0];
    tiles[pRow3->tiles[1]].rotation = pRow3->rotations[1];
    tiles[pRow3->tiles[2]].rotation = pRow3->rotations[2];
    tiles[pRow3->tiles[3]].rotation = pRow3->rotations[3];

    tiles[pRow4->tiles[0]].rotation = pRow4->rotations[0];
    tiles[pRow4->tiles[1]].rotation = pRow4->rotations[1];
    tiles[pRow4->tiles[2]].rotation = pRow4->rotations[2];
    tiles[pRow4->tiles[3]].rotation = pRow4->rotations[3];

    cout << tiles[pRow1->tiles[0]].toString() << " " << tiles[pRow1->tiles[1]].toString() << " " << tiles[pRow1->tiles[2]].toString() << " " << tiles[pRow1->tiles[3]].toString() << endl;
    cout << tiles[pRow2->tiles[0]].toString() << " " << tiles[pRow2->tiles[1]].toString() << " " << tiles[pRow2->tiles[2]].toString() << " " << tiles[pRow2->tiles[3]].toString() << endl;
    cout << tiles[pRow3->tiles[0]].toString() << " " << tiles[pRow3->tiles[1]].toString() << " " << tiles[pRow3->tiles[2]].toString() << " " << tiles[pRow3->tiles[3]].toString() << endl;
    cout << tiles[pRow4->tiles[0]].toString() << " " << tiles[pRow4->tiles[1]].toString() << " " << tiles[pRow4->tiles[2]].toString() << " " << tiles[pRow4->tiles[3]].toString() << endl;
}


int g_total_solutions = 0;
void FindSolution(vector<Tile>& tiles, unordered_map<unsigned int, vector<RowSignature>>& finalRowMap, RowSignature* pRow1, RowSignature* pRow2, RowSignature* pRow3)
{
    RowSignature* pLastRow = (pRow3 ? pRow3 : (pRow2 ? pRow2 : pRow1));

    if (pRow1 == nullptr)
    {
        // for each row in the set of possible valid rows of the puzzle, make this the first row.  The recurse to build the next row
        for (auto itor = finalRowMap.begin(); itor != finalRowMap.end(); itor++)
        {
            vector<RowSignature>& vec = itor->second;
            for (auto itor2 = vec.begin(); itor2 != vec.end(); itor2++)
            {
                RowSignature& rowSig = *itor2;
                FindSolution(tiles, finalRowMap, &rowSig, nullptr, nullptr);
            }
        }
        return;
    }

    unsigned int bottomSig = pLastRow->bottomsignature;
    if (finalRowMap.find(bottomSig) == finalRowMap.end())
    {
        return;
    }

    assert(pRow1 != nullptr);

    // here's the key trick that makes this solution fast, 
    // use the "bottom signature" of the row previously added as the key into the hash table 
    // of available rows that have a matching "top signature"
    auto& vec = finalRowMap[bottomSig];
    for (RowSignature &rs : vec)
    {
        // skip any item that has overlap with the existing rows added to the solution
        if ((pRow1) && (pRow1->tilemask & rs.tilemask))
        {
            continue;
        }
        if ((pRow2) && (pRow2->tilemask & rs.tilemask))
        {
            continue;
        }
        if ((pRow3) && (pRow3->tilemask & rs.tilemask))
        {
            continue;
        }

        // this row, represented by "rs" can be added to the solution currently being built

        if (pRow2 == nullptr)
        {
            FindSolution(tiles, finalRowMap, pRow1, &rs, nullptr);
        }
        else if (pRow3 == nullptr)
        {
            FindSolution(tiles, finalRowMap, pRow1, pRow2, &rs);
        }
        else
        {
            // we have all 4 rows - we have a solution!
            cout << "A solution has been found! (solution index == " << g_total_solutions << ")" << endl;
            g_total_solutions++;
            PrintSolution(tiles, pRow1, pRow2, pRow3, &rs);
        }
    }

}


void main()
{
    unordered_map<unsigned int, vector<RowSignature>> finalRowMap;
    GenerateAllRows(finalRowMap);
    auto tiles = GetTiles();
    FindSolution(tiles, finalRowMap, nullptr, nullptr, nullptr);
}

