#include "game.h"
#include <QDebug>

QString Game::pentomino[] =
{
    //F
    QString().append("....."
                     "..XX."
                     ".XX.."
                     "..X.."
                     "....."),
    //RF
    QString().append("....."
                     ".XX.."
                     "..XX."
                     "..X.."
                     "....."),
    //I
    QString().append("..X.."
                     "..X.."
                     "..X.."
                     "..X.."
                     "..X.."),
    //J
    QString().append("..X.."
                     "..X.."
                     "..X.."
                     ".XX.."
                     "....."),
    //N
    QString().append("....."
                     ".XX.."
                     "..XXX"
                     "....."
                     "....."),
    //RN
    QString().append("....."
                     "..XX."
                     "XXX.."
                     "....."
                     "....."),
    //P
    QString().append("....."
                     "..XX."
                     "..XX."
                     "..X.."
                     "....."),
    //RP
    QString().append("....."
                     ".XX.."
                     ".XX.."
                     "..X.."
                     "....."),
    //T
    QString().append("....."
                     ".XXX."
                     "..X.."
                     "..X.."
                     "....."),
    //V
    QString().append("..X.."
                     "..X.."
                     "..XXX"
                     "....."
                     "....."),
    //W
    QString().append("....."
                     ".X..."
                     ".XX.."
                     "..XX."
                     "....."),
    //X
    QString().append("....."
                     "..X.."
                     ".XXX."
                     "..X.."
                     "....."),
    //Y
    QString().append("....."
                     "..X.."
                     "XXXX."
                     "....."
                     "....."),
    //RY
    QString().append("....."
                     "....."
                     "XXXX."
                     "..X.."
                     "....."),
    //Z
    QString().append("....."
                     ".XX.."
                     "..X.."
                     "..XX."
                     "....."),
    //RZ
    QString().append("....."
                     "..XX."
                     "..X.."
                     ".XX.."
                     ".....")
};


void Game::initBoard()
{
    for (int x = 0; x < nFieldWidth; x++) // Board Boundary
    {
        for (int y = 0; y < nFieldHeight; y++)
        {
            pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? Game::BOUNDARY_BLOCK : 0;
        }
    }
}

Game::Game()
{
    pField = new unsigned char[nFieldWidth*nFieldHeight];
    initBoard();

    if(m_mainPixmap.load(":/res/blocks.png"))
    {
        qDebug() << "Open :/res/blocks.png";
    }
    else
    {
        qDebug() << "Not open :/res/blocks.png";
        exit(EXIT_FAILURE);
    }
}

unsigned char *Game::field() const
{
    return pField;
}

int Game::Rotate(int px, int py, int r)
{
    int pi = 0;
    switch(r%4)
    {
    case 0://0
    {
        //0  1   2  3  4
        //5  6   7  8  9
        //10 11  12 13 14
        //15 16  17 18 19
        //20 21  22 23 24
        pi = py * COUNT_OF_BLOCKS + px;
    }
        break;
    case 1://90
    {
        //20  15 10  5  0
        //21  16 11  6  1
        //22  17 12  7  2
        //23  18 13  8  3
        //24  19 14  9  4
        pi = 20 + py - (px * COUNT_OF_BLOCKS);
    }
        break;
    case 2: //180
    {
        //24   23  22  21  20
        //19   18  17  16  15
        //14   13  12  11  10
        //9     8   7   6   5
        //4     3   2   1   0
        pi = 24 - (py * COUNT_OF_BLOCKS) - px;
    }
        break;
    case 3://270
    {
        // 4  9  14  19  24
        // 3  8  13  18  23
        // 2  7  12  17  22
        // 1  6  11  16  21
        // 0  5  10  15  20
        pi = 4 - py + (px * COUNT_OF_BLOCKS);
    }
        break;
    }
    return pi;
}

bool Game::DoesPieceFit(int nTetronimo, int nRotation, int nPosX, int nPosY)
{
    // All Field cells >0 are occupied
    for (int px = 0; px < COUNT_OF_BLOCKS; px++)
        for (int py = 0; py < COUNT_OF_BLOCKS; py++)
        {
            // Get index into piece
            int pi = Rotate(px, py, nRotation);

            // Get index into field
            int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

            // Check that test is in bounds. Note out of bounds does
            // not necessarily mean a fail, as the long vertical piece
            // can have cells that lie outside the boundary, so we'll
            // just ignore them
            if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
            {
                if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
                {
                    // In Bounds so do collision check
                    if (Game::pentomino[nTetronimo][pi] != '.' && pField[fi] != 0)
                        return false; // fail on first hit
                }
            }
        }

    return true;
}

QPixmap Game::getPixmap() const
{
    return m_mainPixmap;
}
