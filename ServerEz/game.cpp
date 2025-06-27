#include "game.h"



Game::Game(int Cnt, QTcpSocket *Player) : round(1),PlayerCnt(Cnt),WaitingPlayers(0),IsServerFull(false) {
    Cards  =  CardManager::MakeDeck();
    AddPlayer(Player);


}

void Game::AddPlayer(QTcpSocket *p)
{
    Players.push_back(p);
    InCreaseWaitingPlayers();
}
