#include "PreGame.h"



PreGame::PreGame(int Cnt, QTcpSocket *Player) : round(1),PlayerCnt(Cnt),WaitingPlayers(0),IsServerFull(false) {
    Cards  =  CardManager::MakeDeck();
    AddPlayer(Player);


}

void PreGame::AddPlayer(QTcpSocket *p)
{
    Players.push_back(p);
    InCreaseWaitingPlayers();
}
