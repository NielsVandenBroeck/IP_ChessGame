//  Student: Niels Van den Broeck
//  Rolnummer: S0203844
//  Opmerkingen: (bvb aanpassingen van de opgave)
//
#include "game.h"
#include<iostream>
#include <algorithm>
#include <vector>
using namespace std;
//constructor
Game::Game() {
    setStartBord();
}

//destructor
Game::~Game() {
    deleteBord();
}

//Zet het bord klaar; voeg de stukken op de jusite plaats toe
void Game::setStartBord() {
    //Alle zwarte stukken plaatsen
    setPiece(1,0,new Pion(zwart,1,0));
    setPiece(1,1,new Pion(zwart,1,1));
    setPiece(1,2,new Pion(zwart,1,2));
    setPiece(1,3,new Pion(zwart,1,3));
    setPiece(1,4,new Pion(zwart,1,4));
    setPiece(1,5,new Pion(zwart,1,5));
    setPiece(1,6,new Pion(zwart,1,6));
    setPiece(1,7,new Pion(zwart,1,7));
    setPiece(0,3,new Koningin(zwart,0,3));
    setPiece(0,4,new Koning(zwart,0,4));
    setPiece(0,1,new Paard(zwart,0,1));
    setPiece(0,6,new Paard(zwart,0,6));
    setPiece(0,2,new Loper(zwart,0,2));
    setPiece(0,5,new Loper(zwart,0,5));
    setPiece(0,0,new Toren(zwart,0,0));
    setPiece(0,7,new Toren(zwart,0,7));

    //Alle witte stukken plaatsen
    setPiece(6,0,new Pion(wit,6,0));
    setPiece(6,1,new Pion(wit,6,1));
    setPiece(6,2,new Pion(wit,6,2));
    setPiece(6,3,new Pion(wit,6,3));
    setPiece(6,4,new Pion(wit,6,4));
    setPiece(6,5,new Pion(wit,6,5));
    setPiece(6,6,new Pion(wit,6,6));
    setPiece(6,7,new Pion(wit,6,7));
    setPiece(7,3,new Koningin(wit,7,3));
    setPiece(7,4,new Koning(wit,7,4));
    setPiece(7,1,new Paard(wit,7,1));
    setPiece(7,6,new Paard(wit,7,6));
    setPiece(7,2,new Loper(wit,7,2));
    setPiece(7,5,new Loper(wit,7,5));
    setPiece(7,0,new Toren(wit,7,0));
    setPiece(7,7,new Toren(wit,7,7));

    //posities van koning bijhouden
    kingPos.push_back(make_pair(0,4));
    kingPos.push_back(make_pair(7,4));

    //lege posities worden nullptr
    for(int i = 2 ; i < 6 ; i++){
        for(int j = 0; j < 8 ; j++){
            bord[i][j] = nullptr;
        }
    }

    kingMove = false;
    kingmoved = {false,false};
    doubleStep = nullptr;
}

//verwijdert alle stukken op het bord
void Game::deleteBord() {
    //alle stukken uit vector van stukken halen
    Pieces[wit].clear();
    Pieces[zwart].clear();
    //over bord loopen
    for(int r = 0; r < 8; r++) {
        for (int k = 0; k < 8; k++) {
            //verwijder alle niet nullptrs
            if (bord[r][k] != nullptr) {
                delete bord[r][k];
                bord[r][k] = nullptr;
            }
        }
    }
    if(undo[1].first != nullptr){
        delete undo[1].first;
    }
    if(undo[2].first != nullptr){
        if(undo[2].first->getType() == "Koningin"){
            delete undo[2].first;
        }
    }
    undo[0].first = nullptr;
    undo[1].first = nullptr;
    undo[2].first = nullptr;
    redo.first = nullptr;

}

//geeft game terug by reference
Game* Game::getGame(){
    return this;
}

//Plaatst een stuk op het bord
void Game::setPiece(int r, int k, SchaakStuk* s) {
    if(s != nullptr) {
        //op bord plaatsen
        bord[r][k] = s;
        //in vector plaatsen
        Pieces[s->getKleur()].push_back(s);
        //als toren -> in rookmoved zetten
        if (s->getType() == "Toren") {
            if (r == 0 && k == 0) {
                rookmoved[0][0].first = s;
            } else if (r == 0 && k == 7) {
                rookmoved[0][1].first = s;
            } else if (r == 7 && k == 0) {
                rookmoved[1][0].first = s;
            } else if (r == 7 && k == 7) {
                rookmoved[1][1].first = s;
            }
        }
    }
    else {
        bord[r][k] = nullptr;
    }
}

//Kijkt na of de koning al bewogen is of niet
bool Game::kingMoved(zw kleur) const{
    return kingmoved[kleur];
}

//Kijkt na of een toren al bewogen is of niet
bool Game::rookMoved(zw kleur, int x) const {
        return rookmoved[kleur][x].second;
}

//Kijkt na of er een dubbele sprong is gemaakt in de vorige zet van de tegenstander (door pion)
SchaakStuk* Game::getDoubleStep() const {
    return doubleStep;
}

//Kijkt na of het geselecteerde schaakstuk naar de geslecteerde positie kan verplaatsen
bool Game::canMove(SchaakStuk *s, int r, int k) {
    vector<pair<int,int>> zetten = s->geldige_zetten(this);
    //loopt over de mogelijke zetten
    for(int i = 0; i < zetten.size(); i++) {
        //als zetten overeen komen, true teruggeven
        if (zetten[i].first == r && zetten[i].second == k) {
            return true;
        }
    }
    return false;
}

//Kijkt na of de gegeven zet een legale zet is (zelf niet schaak zetten)
bool Game::testMove(SchaakStuk *s, int r, int k, zw kleur) {
    bool legalMove;
    pair<int,int> originalRookPos;
    bool take = false;
    int originalR = s->getRij();
    int originalK = s->getKolom();
    SchaakStuk* t = nullptr;
    SchaakStuk* p = nullptr;
    bool longCastle = false;
    bool shortCastle = false;
    bool enPassant = false;
    bool promotion = false;
    //verwijder het geslagen stuk
    if(bord[r][k] != nullptr){
        take = true;
        //zoek stuk in vector van stukken
        for(int i = 0; i < Pieces[s->getAndereKleur()].size(); i++){
            if(Pieces[s->getAndereKleur()][i] == bord[r][k]){
                //verwijder het uit vector
                Pieces[s->getAndereKleur()].erase(Pieces[s->getAndereKleur()].begin()+i);
                break;
            }
        }
        //houdt het geslagen stuk bij om later terug te plaatsen
        t = bord[r][k];
    }
    //Kijkt na of en passant kan gebeuren
    if(s->getType() == "Pion" && doubleStep != nullptr){
        //Pionnen moeten op dezelfde rij staan
        if(doubleStep->getRij() == r + ((s->getKleur()*2)-1) && doubleStep->getKolom() == k){
            enPassant = true;
            take = true;
            //zoek stuk in vector van stukken
            for(int i = 0; i < Pieces[s->getAndereKleur()].size(); i++){
                if(Pieces[s->getAndereKleur()][i] == doubleStep){
                    //verwijder het uit vector
                    Pieces[s->getAndereKleur()].erase(Pieces[s->getAndereKleur()].begin()+i);
                    break;
                }
            }
            //houdt het geslagen stuk bij
            t = bord[r + ((s->getKleur()*2)-1)][k];
            bord[r + ((s->getKleur()*2)-1)][k] = nullptr;
        }
    }
    //Verplaatsen van koning
    if(s->getRij() == kingPos[s->getKleur()].first && s->getKolom() == kingPos[s->getKleur()].second){
        //Long Castle
        if(kingPos[s->getKleur()].second - 2 == k){
            longCastle = true;
            originalRookPos = make_pair(rookmoved[s->getKleur()][0].first->getRij(),rookmoved[s->getKleur()][0].first->getKolom());
            bord[kingPos[s->getKleur()].first][kingPos[s->getKleur()].second - 1] = rookmoved[s->getKleur()][0].first;
            bord[rookmoved[s->getKleur()][0].first->getRij()][rookmoved[s->getKleur()][0].first->getKolom()] = nullptr;
            rookmoved[s->getKleur()][0].first->setKolom(kingPos[s->getKleur()].second - 1);
        }
        //Short Castle
        if(kingPos[s->getKleur()].second + 2 == k ){
            shortCastle = true;
            originalRookPos = make_pair(rookmoved[s->getKleur()][1].first->getRij(),rookmoved[s->getKleur()][1].first->getKolom());
            bord[kingPos[s->getKleur()].first][kingPos[s->getKleur()].second + 1] = rookmoved[s->getKleur()][1].first;
            bord[rookmoved[s->getKleur()][1].first->getRij()][rookmoved[s->getKleur()][1].first->getKolom()] = nullptr;
            rookmoved[s->getKleur()][1].first->setKolom(kingPos[s->getKleur()].second + 1);
        }
        //zet kingpos/kingMove of de juiste waarde
        kingMove = true;
        kingPos[s->getKleur()] = make_pair(r,k);
    }
    //promotie (autopromotie naar koningin)
    if(s->getType() == "Pion" && r == (s->getKleur()-1)*(-7)){
        promotion = true;
        //zoek stuk in vector van stukken en verplaatst het met een koningin
        for(int i = 0; i < Pieces[s->getKleur()].size(); i++){
            if(Pieces[s->getKleur()][i] == s){
                bord[s->getRij()][s->getKolom()] = nullptr;
                p = new Koningin(s->getKleur(),r,k);
                Pieces[s->getKleur()][i] = p;
                bord[r][k] = p;
                break;
            }
        }
    }
    //verplaatsen van stuk
    else {
        bord[s->getRij()][s->getKolom()] = nullptr;
        bord[r][k] = s;
        s->setRij(r);
        s->setKolom(k);
    }
    //Kijkt na of de speler schaak staat na deze zet te spelen
    if(schaak(kleur)){
        legalMove = false;
    }
    else{
        legalMove = true;
    }
    //zet het bord terug naar de orginele positie
    if(longCastle){
        bord[originalR][originalK - 1] = nullptr;
        bord[originalRookPos.first][originalRookPos.second] = rookmoved[s->getKleur()][0].first;
        rookmoved[s->getKleur()][0].first->setKolom(originalRookPos.second);
    }
    if(shortCastle){
        bord[originalR][originalK + 1] = nullptr;
        bord[originalRookPos.first][originalRookPos.second] = rookmoved[s->getKleur()][1].first;
        rookmoved[s->getKleur()][1].first->setKolom(originalRookPos.second);
    }
    if(promotion){
        Pieces[s->getKleur()].push_back(s);
        bord[originalR][originalK] = s;
        bord[p->getRij()][p->getKolom()] = nullptr;
        //zoekt het stuk in stukken
        for(int i = 0; i < Pieces[s->getKleur()].size(); i++) {
            if (Pieces[s->getKleur()][i] == p) {
                Pieces[s->getKleur()].erase(Pieces[s->getKleur()].begin() + i);
                break;
            }
        }
    }
    //zet het geslagen stuk terug
    if(take){
        if(!promotion){
            bord[originalR][originalK] = s;
        }
        Pieces[t->getKleur()].push_back(t);
        if(enPassant) {
            bord[r + ((s->getKleur()*2)-1)][k] = t;
            bord[r][k]= nullptr;
        }
        else{
            bord[r][k] = t;
        }
    }
    else{
        bord[r][k]= nullptr;
    }
    //zet alle variabelen terug naar orginele waarden
    bord[originalR][originalK] = s;
    s->setRij(originalR);
    s->setKolom(originalK);
    if(kingMove){
        kingPos[s->getKleur()] = make_pair(originalR, originalK);
        kingMove = false;
    }
    return legalMove;
}

//Verplaats stuk s naar positie (r,k)
void Game::move(SchaakStuk* s, int r, int k) {
    //OriginalR nodig voor enPassant
    int originalR = s->getRij();
    //verwijdert de vorige move
    undo[0].first = nullptr;
    if(undo[1].first != nullptr){
        delete undo[1].first;
    }
    undo[1].first = nullptr;
    undo[2].first = nullptr;
    redo.first = nullptr;
    if(kingmovedjustnow[0]){
        kingmovedjustnow[0] = false;
    }
    if(kingmovedjustnow[1]){
        kingmovedjustnow[1] = false;
    }
    //verwijder het geslagen stuk
    if(bord[r][k] != nullptr){
        //zoekt het stuk in alle stukken
        for(int i = 0; i < Pieces[s->getAndereKleur()].size(); i++){
            if(Pieces[s->getAndereKleur()][i] == bord[r][k]){
                //als het stuk een Toren is, moet het uit rookmoved verwijdert worden, en moet rookmoved op true komen te staan van deze toren
                if(bord[r][k]->getType() == "Toren"){
                    if(rookmoved[bord[r][k]->getKleur()][0].first == bord[r][k]){
                        rookmoved[bord[r][k]->getKleur()][0].first = nullptr;
                        rookmoved[bord[r][k]->getKleur()][0].second = true;
                    }
                    else if(rookmoved[bord[r][k]->getKleur()][1].first == bord[r][k]) {
                        rookmoved[bord[r][k]->getKleur()][1].first = nullptr;
                        rookmoved[bord[r][k]->getKleur()][1].second = true;
                    }
                }
                Pieces[s->getAndereKleur()].erase(Pieces[s->getAndereKleur()].begin()+i);
                break;
            }
        }
        //het geslagen stuk wordt opgeslagen in undo[1]
        undo[1].first = bord[r][k];
        undo[1].second.first = r;
        undo[1].second.second = k;
        bord[r][k] = nullptr;
    }
    //en Passant
    if(s->getType() == "Pion" && doubleStep != nullptr){
        //Pionnen moeten op dezelfde rij staan
        if(doubleStep->getRij() == s->getRij() && doubleStep->getKolom() == k){
            //zoek stuk in vector van stukken
            for(int i = 0; i < Pieces[s->getAndereKleur()].size(); i++){
                if(Pieces[s->getAndereKleur()][i] == doubleStep){
                    Pieces[s->getAndereKleur()].erase(Pieces[s->getAndereKleur()].begin()+i);
                    break;
                }
            }
            doubleStep = nullptr;
            //het geslagen stuk wordt opgeslagen in undo[1]
            undo[1].first = bord[r + ((s->getKleur()*2)-1)][k];
            undo[1].second.first = r + ((s->getKleur()*2)-1);
            undo[1].second.second = k;
            bord[r + ((s->getKleur()*2)-1)][k] = nullptr;
        }
    }
    //Verplaatsen van koning
    if(s->getRij() == kingPos[s->getKleur()].first && s->getKolom() == kingPos[s->getKleur()].second){
        //longCastle
        if(kingPos[s->getKleur()].second - 2 == k){
            bord[kingPos[s->getKleur()].first][kingPos[s->getKleur()].second - 1] = rookmoved[s->getKleur()][0].first;
            //toren wordt opgeslagen in undo[2]
            undo[2].first = bord[rookmoved[s->getKleur()][0].first->getRij()][rookmoved[s->getKleur()][0].first->getKolom()];
            undo[2].second.first = rookmoved[s->getKleur()][0].first->getRij();
            undo[2].second.second = rookmoved[s->getKleur()][0].first->getKolom();
            bord[rookmoved[s->getKleur()][0].first->getRij()][rookmoved[s->getKleur()][0].first->getKolom()] = nullptr;
            rookmoved[s->getKleur()][0].first->setKolom(kingPos[s->getKleur()].second - 1);
        }
        //shortCastle
        if(kingPos[s->getKleur()].second + 2 == k ){
            bord[kingPos[s->getKleur()].first][kingPos[s->getKleur()].second + 1] = rookmoved[s->getKleur()][1].first;
            //toren wordt opgeslagen in undo[2]
            undo[2].first = bord[rookmoved[s->getKleur()][1].first->getRij()][rookmoved[s->getKleur()][1].first->getKolom()];
            undo[2].second.first = rookmoved[s->getKleur()][1].first->getRij();
            undo[2].second.second = rookmoved[s->getKleur()][1].first->getKolom();
            bord[rookmoved[s->getKleur()][1].first->getRij()][rookmoved[s->getKleur()][1].first->getKolom()] = nullptr;
            rookmoved[s->getKleur()][1].first->setKolom(kingPos[s->getKleur()].second + 1);
        }
        //pas positie van koning aan
        kingPos[s->getKleur()] = make_pair(r,k);
        if(!kingmoved[s->getKleur()]){
            kingmovedjustnow[s->getKleur()] = true;
        }
        kingmoved[s->getKleur()] = true;
    }
    //promotie
    if(s->getType() == "Pion" && r == (s->getKleur()-1)*(-7)){
        //zoekt stuk in stukken en vervangt het met koningin
        for(int i = 0; i < Pieces[s->getKleur()].size(); i++){
            if(Pieces[s->getKleur()][i] == s){
                //pion wordt opgeslagen in undo[0]
                undo[0].first = s;
                undo[0].second.first = s->getRij();
                undo[0].second.second = s->getKolom();
                bord[s->getRij()][s->getKolom()] = nullptr;
                //delete s;
                SchaakStuk* p = new Koningin(s->getKleur(),r,k);
                Pieces[s->getKleur()][i] = p;
                bord[r][k] = p;
                //Koningin wordt opgeslagen in undo[2]
                undo[2].first = bord[p->getRij()][p->getKolom()];
                undo[2].second.first = p->getRij();
                undo[2].second.second = p->getKolom();
                break;
            }
        }
    }
    //verplaatsen van stuk
    else {
        //wordt opgeslagen in undo[0]
        undo[0].first = s;
        undo[0].second.first = s->getRij();
        undo[0].second.second = s->getKolom();
        bord[s->getRij()][s->getKolom()] = nullptr;
        bord[r][k] = s;
        s->setRij(r);
        s->setKolom(k);

    }
    //Als er een dubbele stap wordt gedaan door een pion
    if(s->getType() == "Pion" && originalR == r + ((s->getKleur()*4)-2)){
        doubleStep = s;
    } else doubleStep = nullptr;
    //Kijkt na of de andere speler schaak staat
    if(schaak(s->getAndereKleur())){
        //kijkt na of de andere speler schaakmat staat
        if(schaakmatOrPat(s->getAndereKleur())) {
            QMessageBox box1;
            if(s->getKleur() == wit) {
                box1.setText("Wit wint door schaakmat.");
            }else{
                box1.setText("Zwart wint door schaakmat.");
            }
            box1.exec();
            end();
        }
    }
    //kijkt na of de andere speler pat staat
    else if(schaakmatOrPat(s->getAndereKleur())) {
        QMessageBox box1;
        box1.setText("Pat.");
        box1.exec();
        end();
    }
    return;
}

//Kijkt na of een schaakstuk mogelijke zetten heeft
bool Game::possiblemoves(SchaakStuk* s){
    if(s->geldige_zetten(this).size() == 0){
        return false;
    }
    return true;
}

//Kijkt na of een kleur schaak staat
bool Game::schaak(zw kleur) {
    //loopt over het bord
    for(int r = 0; r < 8; r++){
        for(int k = 0; k < 8; k++ ){
            //Kijkt of de positie een stuk bevat
            if(bord[r][k] != nullptr){
                //stuk moet van andere kleur zijn
                if(bord[r][k]->getKleur() != kleur){
                    //zetten van dit stuk
                    vector<pair<int,int>> zetten = bord[r][k]->geldige_zetten(this, false, true);
                    for(int i = 0; i < zetten.size(); i++) {
                        //Als er een zet is die gelijk is aan de positie van de koning, staat de speler schaak
                        if(zetten[i].first == kingPos[kleur].first && zetten[i].second == kingPos[kleur].second){
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;


}

//Kijkt na of een kleur schaakmat of pat staat
bool Game::schaakmatOrPat(zw kleur) {
    //doorloopt heel het bord
    for(int r = 0; r < 8; r++){
        for(int k = 0; k < 8; k++ ){
            //kijkt na of er een stuk staat op deze plaats
            if(bord[r][k] != nullptr){
                //kijkt na of het de juiste kleur is (eigen kleur)
                if(bord[r][k]->getKleur() == kleur){
                    SchaakStuk* s = getPiece(r,k);
                    //Gaat elke move af om te kijken of hij nog steeds schaak staat
                    for(int i = 0; i < s->geldige_zetten(this).size(); i++) {
                        int newR = s->geldige_zetten(this)[i].first;
                        int newK = s->geldige_zetten(this)[i].second;
                        SchaakStuk* t = getPiece(newR, newK);
                        //verplaatsen van koning
                        if(r == kingPos[kleur].first && k == kingPos[kleur].second){
                            kingMove = true;
                            kingPos[kleur] = make_pair(newR,newK);
                        }
                        bord[newR][newK] = s;
                        bord[r][k] = nullptr;
                        s->setRij(newR);
                        s->setKolom(newK);
                        //als de speler niet schaak staat bij een move, staat hij niet schaakmat
                        if(!schaak(kleur)){
                            //undo
                            bord[r][k] = s;
                            bord[newR][newK] = t;
                            s->setRij(r);
                            s->setKolom(k);
                            if(kingMove){
                                kingPos[kleur] = make_pair(r,k);
                                kingMove = false;
                            }
                            return false;
                        }
                        //undo
                        bord[r][k] = s;
                        bord[newR][newK] = t;
                        s->setRij(r);
                        s->setKolom(k);
                        if(kingMove){
                            kingPos[kleur] = make_pair(r,k);
                            kingMove = false;
                        }
                    }
                }
            }
        }
    }
    return true;
}

//Geeft de vector van alle stukken van een gegeven kleur terug
vector<SchaakStuk*> Game::getPieces(zw kleur) const {
    return Pieces[kleur];
}

//undo functie om een zet terug te gaan
bool Game::Undo() {
    //Kijkt na of er kan worden teruggedraaid
    if(undo[0].first == nullptr){
        QMessageBox box1;
        box1.setText("cannot undo.");
        box1.exec();
        return false;
    }
    //Gaat een zet terug
    else{
        //de zet die wordt teruggedraaid wordt bijgehouden in redo
        redo.first = bord[undo[0].first->getRij()][undo[0].first->getKolom()];
        redo.second.first = undo[0].first->getRij();
        redo.second.second = undo[0].first->getKolom();
        //zet het stuk terug
        bord[undo[0].second.first][undo[0].second.second] = undo[0].first; //zet stuk terug naar orginele plaats
        bord[undo[0].first->getRij()][undo[0].first->getKolom()] = nullptr;
        undo[0].first->setRij(undo[0].second.first);
        undo[0].first->setKolom(undo[0].second.second);
        //Verplaatsen van koning
        if(undo[0].first->getType() == "Koning"){
            if(kingmovedjustnow[undo[0].first->getKleur()]){
                kingmoved[undo[0].first->getKleur()]=false;
            }
            kingPos[undo[0].first->getKleur()].first = undo[0].first->getRij();
            kingPos[undo[0].first->getKleur()].second = undo[0].first->getKolom();
        }
        //Als er een stuk geslagen is, ook terugzetten
        if(undo[1].first != nullptr){
            //enPassant
            if(undo[0].first->getType() == "Pion" && undo[1].first->getRij() == undo[0].first->getRij()){
                if(undo[1].first->getKolom() == undo[0].first->getKolom() + 1){
                    doubleStep = undo[1].first;
                }
                else if(undo[1].first->getKolom() == undo[0].first->getKolom() - 1){
                    doubleStep = undo[1].first;
                }
            }



            bord[undo[1].second.first][undo[1].second.second] = undo[1].first; //zet geslagen stuk terug naar orginele plaats
            Pieces[undo[1].first->getKleur()].push_back(undo[1].first);
        }
        //Als er een speciale move is gebeurd (promotie/rokade)
        if(undo[2].first != nullptr){
            //promotie
            if(undo[2].first->getType() == "Koningin"){
                bord[undo[0].first->getRij()][undo[0].first->getKolom()] = undo[0].first;
                redo.first = bord[undo[0].first->getRij()][undo[0].first->getKolom()];
                redo.second.first = undo[2].first->getRij();
                redo.second.second = undo[2].first->getKolom();
                //zoekt het stuk in alle stukken
                for(int i = 0; i < Pieces[undo[2].first->getKleur()].size(); i++) {
                    //vervangt de koningin met de pion
                    if (Pieces[undo[2].first->getKleur()][i] == undo[2].first) {
                        Pieces[undo[2].first->getKleur()][i] = undo[0].first;
                    }
                }
                //koningin wordt verwijderd
                bord[undo[2].second.first][undo[2].second.second] = nullptr;
                delete bord[undo[2].second.first][undo[2].second.second];
                //zet geslagen stuk terug naar orginele plaats
                if(undo[1].first != nullptr) {
                    bord[undo[1].second.first][undo[1].second.second] = undo[1].first;
                    Pieces[undo[1].first->getKleur()].push_back(undo[1].first);
                }
            }
            //Rokade
            else if(undo[2].first->getType() == "Toren"){
                //zet de move terug
                bord[undo[2].second.first][undo[2].second.second] = undo[2].first;
                bord[undo[2].first->getRij()][undo[2].first->getKolom()] = nullptr;
                undo[2].first->setRij(undo[2].second.first);
                undo[2].first->setKolom(undo[2].second.second);
                //longCastle
                if(kingPos[undo[2].first->getKleur()].second - 4 == undo[2].second.second) {
                    bord[rookmoved[undo[2].first->getKleur()][0].first->getRij()][rookmoved[undo[2].first->getKleur()][0].first->getKolom()] = undo[2].first;
                    rookmoved[undo[2].first->getKleur()][0].second = false;
                }
                //shortCastle
                else {
                    bord[rookmoved[undo[2].first->getKleur()][1].first->getRij()][rookmoved[undo[2].first->getKleur()][1].first->getKolom()] = undo[2].first;
                    rookmoved[undo[2].first->getKleur()][1].second = false;
                }
            }
        }
    }
    //maak undo leeg
    undo[0].first = nullptr;
    undo[1].first = nullptr;
    undo[2].first = nullptr;
    return true;
}

//redo functie om een zet verder te gaan (enkel als undo zojuist is uitgevoerd)
bool Game::Redo(){
    //Kijkt na of er kan worden verdergedraaid
    if(redo.first == nullptr){
        QMessageBox box1;
        box1.setText("cannot redo.");
        box1.exec();
        return false;
    }
    //voert de move terug uit
    else{
        move(redo.first,redo.second.first,redo.second.second);
    }
    return true;
}

//Eindigt het spel
void Game::end(){
    endOfGame = true;
    return;
}

//zet variabele endOfGame op true om aan te geven dat er geen schaakstukken meer verplaatst mogen worden
void Game::setEndOfGame(bool b){
    endOfGame = b;
    return;
}

//Kijkt na of er nog gespeeld mag worden of niet
bool Game::getEndOfGame() const {
    return endOfGame;
}
