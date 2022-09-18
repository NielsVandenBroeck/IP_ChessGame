//  Student: Niels Van den Broeck
//  Rolnummer: S0203844
//  Opmerkingen: (bvb aanpassingen van de opgave)
//

#ifndef SCHAKEN_GAME_H
#define SCHAKEN_GAME_H
#include "SchaakStuk.h"
#include <QMessageBox>

class Game {
// variabelen om de status van het spel/bord te bewaren

public:
    Game();
    ~Game();
    Game* getGame();
    bool canMove(SchaakStuk* s,int r, int k);
    bool testMove(SchaakStuk* s, int r, int k, zw kleur);
    void move(SchaakStuk* s,int r, int k);
    bool schaak(zw kleur);
    bool schaakmatOrPat(zw kleur);
    vector<SchaakStuk*> getPieces(zw kleur) const;
    void setStartBord();
    void deleteBord();
    bool possiblemoves(SchaakStuk* s);
    SchaakStuk* getPiece(int r, int k) const {return bord[r][k];}
    void setPiece(int r,int k, SchaakStuk* s);
    bool kingMoved(zw kleur) const;
    bool rookMoved(zw kleur, int x) const;
    SchaakStuk* getDoubleStep() const;
    bool Undo();
    bool Redo();
    void end();
    void setEndOfGame(bool b);
    bool getEndOfGame() const;

private:
    SchaakStuk* bord[8][8];
    vector<pair<int,int>> kingPos = {};
    bool kingMove = false;
    vector<bool> kingmoved = {false, false}; //houdt bij of een koning al bewogen is of niet
    vector<bool> kingmovedjustnow = {false, false};
    vector<vector<pair<SchaakStuk*,bool>>> rookmoved = {{{nullptr, false},{nullptr, false}},{{nullptr, false},{nullptr, false}}}; //Geeft alle torens mee en of ze al verplaatst zijn voor rokade
    vector<vector<SchaakStuk*>> Pieces = {{},{}}; //houdt alle stukken bij
    SchaakStuk* doubleStep; //houdt bij of er een dubbele sprong is gemaakt door een pion
    vector<pair<SchaakStuk*, pair<int,int>>> undo = {{nullptr,{}},{nullptr,{}},{nullptr,{}}}; //item 1: item dat teruggezet moet worden(van de kleur zelf) met orginele positie. item 2: item dat geslagen is met orginele positie
    pair<SchaakStuk*, pair<int,int>> redo = {nullptr,{}}; //item dat opnieuw bewogen moet worden naar juiste plaats.
    bool endOfGame = false;
};


#endif //SCHAKEN_GAME_H
