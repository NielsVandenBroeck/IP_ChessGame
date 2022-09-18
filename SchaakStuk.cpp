//  Student: Niels Van den Broeck
//  Rolnummer: S0203844
//  Opmerkingen: (bvb aanpassingen van de opgave)
//
#include<vector>
#include "SchaakStuk.h"
#include"game.h"
#include<iostream>
using namespace std;

//constructor
SchaakStuk::SchaakStuk(zw kleur, int r, int k) : kleur(kleur), rij(r), kolom(k) {}
//geeft de geldige zetten van een stuk terug
vector<pair<int,int>> SchaakStuk::geldige_zetten(Game* g, bool slaanZetten, bool ilegalMoves) {
    return {};
}
//geeft de posities weer waar het stuk een ander stuk zou kunnen slaan
vector<pair<int,int>> SchaakStuk::slaan_zetten(Game* g) {
    return {};
}
//geeft de kleur van de andere speler terug
zw SchaakStuk::getAndereKleur() const {
    if(kleur == zwart){
        return wit;
    } return zwart;
}
//geeft de rij van het stuk terug
int SchaakStuk::getRij() const {
    return rij;
}
//geeft de kolom van het stuk terug
int SchaakStuk::getKolom() const {
    return kolom;
}
//past de rij van het stuk aan
void SchaakStuk::setRij(int rij) {
    SchaakStuk::rij = rij;
}
//past de kolom van het stuk aan
void SchaakStuk::setKolom(int kolom) {
    SchaakStuk::kolom = kolom;
}
//functie om het de zetten van Toren/Loper/Koningin te berekenen
vector<pair<int,int>> SchaakStuk::getPath(Game* g,bool schuin, bool recht, bool slaanZetten, bool ilegalMoves){
    vector<pair<int,int>> zetten;
    //Schuine zetten (Loper/Koningin)
    if(schuin == true){
        //gaat elke richting af
        for(int i = 0; i < 2; i++){
            for(int j = 0; j < 2; j++){
                //maakt de juiste richtingen
                int i_multiply = (i*2)-1;
                int j_multiply = (j*2)-1;
                //Kan maximaal 7 zetten hebben in deze richting
                for(int x = 1; x < 8; x++){
                    //Als de positie buiten het bord is moet er gebreaked worden
                    if(getRij()+(x*i_multiply) < 0 || getRij()+(x*i_multiply) > 7 || getKolom()+(x*j_multiply) < 0 || getKolom()+(x*j_multiply) > 7) break;
                    //Als er niets op deze positie staat, moet de positie toegevoegd worden aan zetten
                    else if(g->getPiece(getRij()+(x*i_multiply),getKolom()+(x*j_multiply)) == nullptr){
                        zetten.push_back(make_pair(getRij()+(x*i_multiply), getKolom()+(x*j_multiply)));
                    }
                    //Als er een stuk van de andere kleur staat, moet de positie toegevoegd worden, en daarna stoppen
                    else if(g->getPiece(getRij()+(x*i_multiply),getKolom()+(x*j_multiply))->getKleur() != getKleur() || slaanZetten){
                        zetten.push_back(make_pair(getRij()+(x*i_multiply), getKolom()+(x*j_multiply)));
                        break;
                    }
                    //Als er een stuk van eigen kleur staat, moet er gestopt worden
                    else break;
                }
            }
        }
    }
    //(rechte zetten(Toren/Koningin)
    if(recht == true){
        //gaat verschillende richtingen af
        for(int i = 0; i < 2; i++){
            for(int j = 0; j < 2; j++) {
                //juiste richting maken
                int j_multiply = (j * 2) - 1;
                //geval 1 -> rechts/links
                if (i == 0) {
                    //Kan maximaal 7 zetten hebben in deze richting
                    for (int x = 1; x < 8; x++) {
                        //Als de positie buiten het bord is moet er gebreaked worden
                        if (getKolom() + (x * j_multiply) < 0 || getKolom() + (x * j_multiply) > 7) break;
                        //Als er niets op deze positie staat, moet de positie toegevoegd worden aan zetten
                        else if (g->getPiece(getRij(), getKolom() + (x * j_multiply)) == nullptr ){
                            zetten.push_back(make_pair(getRij(), getKolom() + (x * j_multiply)));
                        }
                            //Als er een stuk van de andere kleur staat, moet de positie toegevoegd worden, en daarna stoppen
                        else if (g->getPiece(getRij(), getKolom() + (x * j_multiply))->getKleur() != getKleur() || slaanZetten) {
                            zetten.push_back(make_pair(getRij(), getKolom() + (x * j_multiply)));
                            break;
                        }
                            //Als er een stuk van eigen kleur staat, moet er gestopt worden
                        else break;
                    }
                }
                //geval 2 -> boven/onder
                else {
                    //Kan maximaal 7 zetten hebben in deze richting
                    for (int x = 1; x < 8; x++) {
                        //Als de positie buiten het bord is moet er gebreaked worden
                        if (getRij() + (x * j_multiply) < 0 || getRij() + (x * j_multiply) > 7) break;
                        //Als er niets op deze positie staat, moet de positie toegevoegd worden aan zetten
                        else if (g->getPiece(getRij() + (x * j_multiply), getKolom()) == nullptr ){
                            zetten.push_back(make_pair(getRij() + (x * j_multiply), getKolom()));
                        }
                            //Als er een stuk van de andere kleur staat, moet de positie toegevoegd worden, en daarna stoppen
                        else if (g->getPiece(getRij() + (x * j_multiply), getKolom())->getKleur() != getKleur() || slaanZetten) {
                            zetten.push_back(make_pair(getRij() + (x * j_multiply), getKolom()));
                            break;
                        }
                            //Als er een stuk van eigen kleur staat, moet er gestopt worden
                        else break;
                    }

                }
            }
        }
    }
    //ongeldige zetten eruit halen
    if(!ilegalMoves) {
        vector<pair<int,int>> legaleZetten;
        for (int i = 0; i < zetten.size(); i++) {
            if (g->testMove(this, zetten[i].first, zetten[i].second,getKleur())) {
                legaleZetten.push_back(zetten[i]);
            }
        }
        return(legaleZetten);
    }
    return zetten;
}
//geeft het type schaakstuk terug
string SchaakStuk::getType() const {
    return "";
}

//constructor
Pion::Pion(zw kleur, int r, int k) : SchaakStuk(kleur, r, k) {}
//geeft de geldige zetten van een stuk terug
vector<pair<int,int>> Pion::geldige_zetten(Game* g, bool slaanZetten, bool ilegalMoves) {
    vector<pair<int,int>> zetten;
    //1 stap naar boven
    if(this->getRij() < 7 && this->getRij() > 0) {
        if (g->getPiece(getRij() + 1 + (getKleur() * (-1) * 2), getKolom()) == nullptr) {
            zetten.push_back(make_pair(getRij() + 1 + (getKleur() * (-1) * 2), getKolom()));
            //2 stappen naar boven als ze nog op beginpositie staan
            if (this->getRij() == 1 + (getKleur() * 5) && g->getPiece(getRij() + 2 + (getKleur() * (-1) * 4), getKolom()) == nullptr) {
                zetten.push_back(make_pair(getRij() + 2 + (getKleur() * (-1) * 4), getKolom()));
            }
        }
    }
    //1 stap naar schuinboven om stuk te slaan
    if(this->getKolom() < 7 && this->getRij() < 7 && this->getRij() > 0) {
        if (g->getPiece(getRij() + 1 + (getKleur() * (-1) * 2), getKolom() + 1) != nullptr) {
            if (g->getPiece(getRij() + 1 + (getKleur() * (-1) * 2), getKolom() + 1)->getKleur() != getKleur()) {
                zetten.push_back(make_pair(getRij() + 1 + (getKleur() * (-1) * 2), getKolom() + 1));
            }
        }
    }
    //1 stap naar schuinboven om stuk te slaan
    if(this->getKolom() > 0 && this->getRij() < 7 && this->getRij() > 0) {
        if (g->getPiece(getRij() + 1 + (getKleur() * (-1) * 2), getKolom() - 1) != nullptr) {
            if (g->getPiece(getRij() + 1 + (getKleur() * (-1) * 2), getKolom() - 1)->getKleur() != getKleur()) {
                zetten.push_back(make_pair(getRij() + 1 + (getKleur() * (-1) * 2), getKolom() - 1));
            }
        }
    }
    //en passant
    if(this->getRij() == 4 - this->getKleur()  && g->getDoubleStep() != nullptr){
        if(g->getDoubleStep()->getKolom() == this->getKolom() + 1){
            zetten.push_back(make_pair(this->getRij() + (this->getKleur()*(-2)+1),this->getKolom() + 1));
        }
        else if(g->getDoubleStep()->getKolom() == this->getKolom() - 1){
            zetten.push_back(make_pair(this->getRij() + (this->getKleur()*(-2)+1),this->getKolom() - 1));
        }
    }
    //ongeldige zetten eruit halen
    if(!ilegalMoves) {
        vector<pair<int,int>> legaleZetten;
        for (int i = 0; i < zetten.size(); i++) {
            if (g->testMove(this, zetten[i].first, zetten[i].second,getKleur())) {
                legaleZetten.push_back(zetten[i]);
            }
        }
        return(legaleZetten);
    }
    return zetten;
}
//geeft de posities weer waar het stuk een ander stuk zou kunnen slaan
vector<pair<int,int>> Pion::slaan_zetten(Game* g) {
    vector<pair<int,int>> zetten;
    //schuin boven
    zetten.push_back(make_pair(getRij()+1+(getKleur()*(-1)*2), getKolom() + 1));
    zetten.push_back(make_pair(getRij()+1+(getKleur()*(-1)*2), getKolom() - 1));
    return(zetten);
}
//geeft het type schaakstuk terug
string Pion::getType()  const {
    return "Pion";
}

//constructor
Toren::Toren(zw kleur, int r, int k) : SchaakStuk(kleur, r, k) {}
//geeft de geldige zetten van een stuk terug
vector<pair<int,int>> Toren::geldige_zetten(Game* g, bool slaanZetten, bool ilegalMoves) {
    return getPath(g, false, true, slaanZetten, ilegalMoves);
}
//geeft de posities weer waar het stuk een ander stuk zou kunnen slaan
vector<pair<int,int>> Toren::slaan_zetten(Game* g) {
    return geldige_zetten(g, true, true);
}
//geeft het type schaakstuk terug
string Toren::getType() const {
    return "Toren";
}

//constructor
Paard::Paard(zw kleur, int r, int k) : SchaakStuk(kleur, r, k) {}
//geeft de geldige zetten van een stuk terug
vector<pair<int,int>> Paard::geldige_zetten(Game* g, bool slaanZetten, bool ilegalMoves) {
    vector<pair<int,int>> zetten;
    //2 naar links..
    if(getKolom()>1){
        //1 naar boven
        if(getRij() != 0) {
            if((g->getPiece(getRij()-1,getKolom()-2) == nullptr) || slaanZetten) {
                zetten.push_back(make_pair(getRij() - 1, getKolom() - 2));
            }
            else if(g->getPiece(getRij()-1,getKolom()-2)->getKleur() == getAndereKleur()){
                zetten.push_back(make_pair(getRij() - 1, getKolom() - 2));
            }
        }
        //1 naar onder
        if(getRij() != 7){
            if((g->getPiece(getRij()+1,getKolom()-2) == nullptr) || slaanZetten) {
                zetten.push_back(make_pair(getRij()+1,getKolom()-2));
            }
            else if(g->getPiece(getRij()+1,getKolom()-2)->getKleur() == getAndereKleur()){
                zetten.push_back(make_pair(getRij()+1,getKolom()-2));
            }

        }
    }
    //2 naar rechts
    if(getKolom()<6){
        //1 naar boven
        if(getRij() != 0){
            if((g->getPiece(getRij()-1,getKolom()+2) == nullptr) || slaanZetten) {
                zetten.push_back(make_pair(getRij()-1, getKolom()+2));
            }
            else if(g->getPiece(getRij()-1,getKolom()+2)->getKleur() == getAndereKleur()){
                zetten.push_back(make_pair(getRij()-1, getKolom()+2));
            }
        }
        //1 naar onder
        if(getRij() != 7){
            if((g->getPiece(getRij()+1,getKolom()+2) == nullptr) || slaanZetten) {
                zetten.push_back(make_pair(getRij()+1, getKolom()+2));
            }
            else if(g->getPiece(getRij()+1,getKolom()+2)->getKleur() == getAndereKleur()){
                zetten.push_back(make_pair(getRij()+1, getKolom()+2));
            }
        }
    }
    //2 naar boven
    if(getRij()>1){
        //1 naar links
        if(getKolom() != 0){
            if((g->getPiece(getRij()-2,getKolom()-1) == nullptr) || slaanZetten) {
                zetten.push_back(make_pair(getRij()-2, getKolom()-1));
            }
            else if(g->getPiece(getRij()-2,getKolom()-1)->getKleur() == getAndereKleur()){
                zetten.push_back(make_pair(getRij()-2, getKolom()-1));
            }
        }
        //1 naar rechts
        if(getKolom() != 7){
            if((g->getPiece(getRij()-2,getKolom()+1) == nullptr) || slaanZetten) {
                zetten.push_back(make_pair(getRij()-2, getKolom()+1));
            }
            else if(g->getPiece(getRij()-2,getKolom()+1)->getKleur() == getAndereKleur()){
                zetten.push_back(make_pair(getRij()-2, getKolom()+1));
            }
        }
    }
    //2 naar onder
    if(getRij()<6){
        //1 naar links
        if(getKolom() != 0){
            if((g->getPiece(getRij()+2,getKolom()-1) == nullptr) || slaanZetten) {
                zetten.push_back(make_pair(getRij()+2, getKolom()-1));
            }
            else if(g->getPiece(getRij()+2,getKolom()-1)->getKleur() == getAndereKleur()){
                zetten.push_back(make_pair(getRij()+2, getKolom()-1));
            }
        }
        //1 naar rechts
        if(getKolom() != 7){
            if((g->getPiece(getRij()+2,getKolom()+1) == nullptr) || slaanZetten) {
                zetten.push_back(make_pair(getRij()+2, getKolom()+1));
            }
            else if(g->getPiece(getRij()+2,getKolom()+1)->getKleur() == getAndereKleur()){
                zetten.push_back(make_pair(getRij()+2, getKolom()+1));
            }
        }
    }
    //ongeldige zetten eruit halen
    if(!ilegalMoves) {
        vector<pair<int,int>> legaleZetten;
        for (int i = 0; i < zetten.size(); i++) {
            if (g->testMove(this, zetten[i].first, zetten[i].second, getKleur())) {
                legaleZetten.push_back(zetten[i]);
            }
        }
        return(legaleZetten);
    }
    return zetten;
}
//geeft de posities weer waar het stuk een ander stuk zou kunnen slaan
vector<pair<int,int>> Paard::slaan_zetten(Game* g) {
    return geldige_zetten(g, true, true);
}
//geeft het type schaakstuk terug
string Paard::getType() const {
    return "Paard";
}

//constructor
Loper::Loper(zw kleur, int r, int k) : SchaakStuk(kleur, r, k) {}
//geeft de geldige zetten van een stuk terug
vector<pair<int,int>> Loper::geldige_zetten(Game* g, bool slaanZetten, bool ilegalMoves) {
    return getPath(g, true, false, slaanZetten, ilegalMoves);
}
//geeft de posities weer waar het stuk een ander stuk zou kunnen slaan
vector<pair<int,int>> Loper::slaan_zetten(Game* g) {
    return geldige_zetten(g, true, true);
}
//geeft het type schaakstuk terug
string Loper::getType()  const {
    return "Loper";
}

//constructor
Koning::Koning(zw kleur, int r, int k) : SchaakStuk(kleur, r, k) {}
//geeft de geldige zetten van een stuk terug
vector<pair<int,int>> Koning::geldige_zetten(Game* g, bool slaanZetten, bool ilegalMoves) {
    vector<pair<int,int>> zetten;
    //loopt over vierkant rond koning
    for (int i = getRij()-1; i <= getRij()+1; i++){
        for (int j = getKolom()-1; j <= getKolom()+1; j++){
            //positie van koning zelf niet toevoegen
            if(i == getRij() && j == getKolom()) continue;
            //posities buiten het bord ook niet toevoegen
            else if(i < 0 || i > 7 || j < 0 || j > 7) continue;
            else if(g->getPiece(i,j) == nullptr || slaanZetten) zetten.push_back(make_pair(i,j));
            else if(g->getPiece(i,j)->getKleur() != getKleur()) zetten.push_back(make_pair(i,j));
        }
    }
    //rokade:
    if(!g->kingMoved(this->getKleur())){
        if(this->getKleur() == zwart){
            if(!g->rookMoved(this->getKleur(), 0) && g->getPiece(0,1) == nullptr && g->getPiece(0,2) == nullptr && g->getPiece(0,3) == nullptr){
                zetten.push_back(make_pair(0,2));
            }
            else if(!g->rookMoved(this->getKleur(), 1) && g->getPiece(0,5) == nullptr && g->getPiece(0,6) == nullptr){
                zetten.push_back(make_pair(0,6));
            }
        }
        else{
            if(!g->rookMoved(this->getKleur(), 0) && g->getPiece(7,1) == nullptr && g->getPiece(7,2) == nullptr && g->getPiece(7,3) == nullptr){
                zetten.push_back(make_pair(7,2));
            }
            if(!g->rookMoved(this->getKleur(), 1) && g->getPiece(7,5) == nullptr && g->getPiece(7,6) == nullptr){
                zetten.push_back(make_pair(7,6));
            }
        }
    }
    //ongeldige zetten eruit halen
    if(!ilegalMoves) {
        vector<pair<int,int>> legaleZetten;
        for (int i = 0; i < zetten.size(); i++) {
            if (g->testMove(this, zetten[i].first, zetten[i].second,getKleur())) {
                legaleZetten.push_back(zetten[i]);
            }
        }
        return(legaleZetten);
    }
    return zetten;
}
//geeft de posities weer waar het stuk een ander stuk zou kunnen slaan
vector<pair<int,int>> Koning::slaan_zetten(Game* g) {
    return geldige_zetten(g, true, true);
}
//geeft het type schaakstuk terug
string Koning::getType() const {
    return "Koning";
}

//constructor
Koningin::Koningin(zw kleur, int r, int k) : SchaakStuk(kleur, r, k) {}
//geeft de geldige zetten van een stuk terug
vector<pair<int,int>> Koningin::geldige_zetten(Game* g, bool slaanZetten, bool ilegalMoves) {
    return getPath(g, true, true, slaanZetten, ilegalMoves);
}
//geeft de posities weer waar het stuk een ander stuk zou kunnen slaan
vector<pair<int,int>> Koningin::slaan_zetten(Game* g) {
    return geldige_zetten(g, true, true);
}
//geeft het type schaakstuk terug
string Koningin::getType() const {
    return "Koningin";
}

