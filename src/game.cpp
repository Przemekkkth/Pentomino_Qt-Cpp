#include "game.h"
#include <QDebug>

QString Game::tetronimo[] =
{
    //I
    QString().append("..X."
                     "..X."
                     "..X."
                     "..X."),
    //T
    QString().append("..X."
                     ".XX."
                     "..X."
                     "...."),
    //O
    QString().append("...."
                     ".XX."
                     ".XX."
                     "...."),
    //Z
    QString().append("..X."
                     ".XX."
                     ".X.."
                     "...."),
    //S
    QString().append(".X.."
                     ".XX."
                     "..X."
                     "...."),
    //L
    QString().append(".X.."
                     ".X.."
                     ".XX."
                     "...."),
    //J
    QString().append("..X."
                     "..X."
                     ".XX."
                     "....")
};


Game::Game()
{
    pField = new unsigned char[nFieldWidth*nFieldHeight];
    for (int x = 0; x < nFieldWidth; x++) // Board Boundary
    {
        for (int y = 0; y < nFieldHeight; y++)
        {
            pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
        }
    }

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
        //0  1   2  3
        //4  5   6  7
        //8  9  10 11
        //12 13 14 15
        pi = py * 4 + px;
    }
        break;
    case 1://90
    {
        //12  8  4  0
        //13  9  5  1
        //14 10  6  2
        //15 11  7  3
        pi = 12 + py - (px * 4);
    }
        break;
    case 2: //180
    {
        //15  14  13  12
        //11  10   9   8
        // 7   6   5   4
        // 3   2   1   0
        pi = 15 - (py * 4) - px;
    }
        break;
    case 3://270
    {
        // 3  7  11  15
        // 2  6  10  14
        // 1  5   9  13
        // 0  4   8  12
        pi = 3 - py + (px * 4);
    }
        break;
    }
    return pi;
}

bool Game::DoesPieceFit(int nTetronimo, int nRotation, int nPosX, int nPosY)
{
    // All Field cells >0 are occupied
    for (int px = 0; px < 4; px++)
        for (int py = 0; py < 4; py++)
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
                    if (Game::tetronimo[nTetronimo][pi] != '.' && pField[fi] != 0)
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
