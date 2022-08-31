#include "gamescene.h"
#include <QKeyEvent>
#include <QGraphicsPixmapItem>
#include <QThread>
#include <QGraphicsSimpleTextItem>

GameScene::GameScene()
    : m_loopSpeed(50),//50ms
      bKey{false, false, false, false},
      bGameOver(false)
{
    connect(&m_timer, &QTimer::timeout, this, &GameScene::loop);
    m_timer.start(m_loopSpeed);
    m_elapsedTimer.start();
    setSceneRect(0,0,Game::RESOLUTION.width(), Game::RESOLUTION.height());
    setBackgroundBrush(QBrush(Qt::black));

    nCurrentX = Game::nFieldWidth / 2;
    nCurrentY = 0;
    nCurrentRotation = 0;
    //nCurrentPiece = rand() % 7;
    nCurrentPiece = 0;
}

void GameScene::loop()
{
    m_deltaTime = m_elapsedTimer.elapsed();
    m_elapsedTimer.restart();

    m_loopTime += m_deltaTime;
    if( m_loopTime > m_loopSpeed && !bGameOver)
    {
        m_loopTime -= m_loopSpeed;
        nSpeedCount++;
        bForceDown = (nSpeedCount == nSpeed);
        // Handle player movement
        handePlayerInput();
        bool isGenerateNewPiece = false;
        if (bForceDown)
        {
            // Update difficulty every 50 pieces
            nSpeedCount = 0;
            nPieceCount++;
            if (nPieceCount % 50 == 0)
            {
                if (nSpeed >= 10)
                {
                    nSpeed--;
                }
            }


            // Test if piece can be moved down
            if (m_game.DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
            {
                nCurrentY++; // It can, so do it!
            }
            else
            {
                // It can't! Lock the piece in place
                for (int px = 0; px < Game::COUNT_OF_BLOCKS; px++)
                {
                    for (int py = 0; py < Game::COUNT_OF_BLOCKS; py++)
                    {
                        if (m_game.tetronimo[nCurrentPiece][m_game.Rotate(px, py, nCurrentRotation)] != '.')
                        {
                            m_game.field()[(nCurrentY + py) * Game::nFieldWidth + (nCurrentX + px)] = nCurrentPiece+1;
                        }

                    }

                }

                // Check for lines
                for (int py = 0; py < Game::COUNT_OF_BLOCKS; py++)
                {
                    if(nCurrentY + py < Game::nFieldHeight - 1)
                    {
                        bool bLine = true;
                        for (int px = 1; px < Game::nFieldWidth - 1; px++)
                        {
                            bLine &= (m_game.field()[(nCurrentY + py) * Game::nFieldWidth + px]) != 0;
                        }


                        if (bLine)
                        {
                            // Remove Line, set to =
                            for (int px = 1; px < Game::nFieldWidth - 1; px++)
                            {
                                m_game.field()[(nCurrentY + py) * Game::nFieldWidth + px] = 8;
                            }

                            vLines.push_back(nCurrentY + py);
                        }
                    }
                }


                nScore += 25;
                if(!vLines.empty())	nScore += (1 << vLines.size()) * 100;

                isGenerateNewPiece = true;



            }

        }
        //draw
        clear();

        drawCurrentPiece();
        drawField();
        drawScore();
        if (!vLines.empty())
        {
            //this_thread::sleep_for(400ms); // Delay a bit
            m_timer.stop();
            QTimer::singleShot(400, [this]()
            {
                m_timer.start(m_loopSpeed);
            });
            for (auto &v : vLines)
                for (int px = 1; px < Game::nFieldWidth - 1; px++)
                {
                    for (int py = v; py > 0; py--)
                        m_game.field()[py * Game::nFieldWidth + px] = m_game.field()[(py - 1) * Game::nFieldWidth + px];
                    m_game.field()[px] = 0;
                }

            vLines.clear();
        }


        if(isGenerateNewPiece)
        {
            // Pick New Piece
            nCurrentX = Game::nFieldWidth / 2;
            nCurrentY = 0;
            nCurrentRotation = 0;
            //nCurrentPiece = rand() % 7;
            nCurrentPiece = 0;
            // If piece does not fit straight away, game over!
            bGameOver = !m_game.DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
            if(bGameOver)
            {
                drawGameOverText();
            }
        }
    }
}

void GameScene::drawField()
{
    for (int x = 0; x < Game::nFieldWidth; x++)
    {
        for (int y = 0; y < Game::nFieldHeight; y++)
        {
            QGraphicsPixmapItem *pItem = new QGraphicsPixmapItem(
                        m_game.m_mainPixmap.copy(int(m_game.pField[y*Game::nFieldWidth + x])*Game::GRID_SIZE, 0, Game::GRID_SIZE, Game::GRID_SIZE));
            if(m_game.pField[y*Game::nFieldWidth + x] == 8)
            {
                int i = 0;
                i++;
            }
            pItem->setPos(x*Game::GRID_SIZE + Game::OFFSET_X/2, y*Game::GRID_SIZE + Game::OFFSET_Y);
            addItem(pItem);
        }
    }
}

void GameScene::drawCurrentPiece()
{
    for (int px = 0; px < 4; px++)
    {
        for (int py = 0; py < 4; py++)
        {
            if (m_game.tetronimo[nCurrentPiece][m_game.Rotate(px, py, nCurrentRotation)] != '.')
            {
                QGraphicsPixmapItem *pItem = new QGraphicsPixmapItem(m_game.getPixmap().copy( (nCurrentPiece+1)*Game::GRID_SIZE, 0, Game::GRID_SIZE, Game::GRID_SIZE));
                pItem->setPos((nCurrentX + px)*Game::GRID_SIZE + Game::OFFSET_X/2 , (nCurrentY + py)*Game::GRID_SIZE + Game::OFFSET_Y);
                addItem(pItem);
                //screen[(nCurrentY + py + 2)*nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;
            }

        }
    }

}

void GameScene::drawScore()
{
    QGraphicsSimpleTextItem* tItem = new QGraphicsSimpleTextItem();
    tItem->setText("Score: " + QString::number(nScore).left(4));
    tItem->setBrush(QBrush(Qt::white));
    tItem->setPos(Game::RESOLUTION.width()-tItem->boundingRect().width(), 0);
    addItem(tItem);
}

void GameScene::drawGameOverText()
{
    QGraphicsSimpleTextItem* tItem = new QGraphicsSimpleTextItem();
    tItem->setText("Game Over");
    tItem->setBrush(QBrush(Qt::white));
    tItem->setFont(QFont("Arial", 20, 50, true));
    tItem->setPos(Game::RESOLUTION.width()/2-tItem->boundingRect().width()/2, 10);
    addItem(tItem);

}

void GameScene::handePlayerInput()
{
    if(bKey[0])
    {
        if(m_game.DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))
        {
            nCurrentX--;
        }
        //bKey[0] = false;
    }
    else if(bKey[1])
    {
        if(m_game.DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY))
        {
            nCurrentX++;
        }
        //bKey[1] = false;
    }
    else if(bKey[2])
    {
        if(m_game.DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
        {
            nCurrentY++;
        }
        //bKey[2] = false;
    }
    else if(bKey[3])
    {
        if(m_game.DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY))
        {
            nCurrentRotation++;
        }
        bKey[3] = false;
    }
}

void GameScene::keyPressEvent(QKeyEvent *event)
{
    if(!event->isAutoRepeat())
    {
        switch (event->key())
        {
        case Qt::Key_Left:
        {
            bKey[0] = true;
        }
            break;
        case Qt::Key_Right:
        {
            bKey[1] = true;
        }
            break;
        case Qt::Key_Down:
        {
            bKey[2] = true;
        }
            break;
        case Qt::Key_Z:
        {
            bKey[3] = true;
        }
            break;
        }
    }
}

void GameScene::keyReleaseEvent(QKeyEvent *event)
{
    if(!event->isAutoRepeat())
    {
        switch (event->key())
        {
        case Qt::Key_Left:
        {
            bKey[0] = false;
        }
            break;
        case Qt::Key_Right:
        {
            bKey[1] = false;
        }
            break;
        case Qt::Key_Down:
        {
            bKey[2] = false;
        }
            break;
        case Qt::Key_Z:
        {
            bKey[3] = false;
        }
            break;
        }
    }
}
