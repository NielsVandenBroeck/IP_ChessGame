//  Student: Niels Van den Broeck
//  Rolnummer: S0203844
//  Opmerkingen: (bvb aanpassingen van de opgave)
//

#ifndef SCHAKEN_SCHAAKSTUK_H
#define SCHAKEN_SCHAAKSTUK_H
#include "chessboard.h"

using namespace std;

class Game;

enum zw{zwart,wit};

class SchaakStuk {
public:
    SchaakStuk(zw kleur, int rij, int kolom);
    virtual Piece piece() const=0;      // Deze functie krijg je cadeau
                                        // Je hebt die nodig om het stuk in de
                                        // GUI te tekenen
                                        // Meer info: kijk naar mainwindow.cpp, lijn 27
    zw getKleur() const { return kleur; }
    zw getAndereKleur() const;
    int getRij() const;
    int getKolom() const;
    void setRij(int rij);
    void setKolom(int kolom);
    virtual vector<pair<int,int>> geldige_zetten(Game* g, bool slaanZetten = false, bool ilegalMoves = false);
    virtual vector<pair<int,int>> slaan_zetten(Game* g);
    vector<pair<int,int>> getPath( Game* g,bool schuin, bool recht, bool slaanZetten = false, bool ilegalMoves = false);
    virtual string getType() const;
private:

    zw kleur;
    int rij;
    int kolom;
};

class Pion:public SchaakStuk {
public:
    Pion(zw kleur, int r, int k);
    virtual Piece piece() const override {
        return Piece(Piece::Pawn,getKleur()==wit?Piece::White:Piece::Black);
    }
    vector<pair<int,int>> geldige_zetten(Game* g, bool slaanZetten = false, bool ilegalMoves = false) override;
    vector<pair<int,int>> slaan_zetten(Game* g) override;
    string getType() const override;
};

class Toren:public SchaakStuk {
public:
    Toren(zw kleur, int r, int k);
    Piece piece() const override {
        return Piece(Piece::Rook,getKleur()==wit?Piece::White:Piece::Black);
    }
    vector<pair<int,int>> geldige_zetten(Game* g, bool slaanZetten = false, bool ilegalMoves = false) override;
    vector<pair<int,int>> slaan_zetten(Game* g) override;
    string getType() const override;

};

class Paard:public SchaakStuk {
public:
    Paard(zw kleur, int r, int k);
    Piece piece() const override {
        return Piece(Piece::Knight,getKleur()==wit?Piece::White:Piece::Black);
    }
    vector<pair<int,int>> geldige_zetten(Game* g, bool slaanZetten = false, bool ilegalMoves = false) override;
    vector<pair<int,int>> slaan_zetten(Game* g) override;
    string getType() const override;
};

class Loper:public SchaakStuk {
public:
    Loper(zw kleur, int r, int k);
    Piece piece() const override {
        return Piece(Piece::Bishop,getKleur()==wit?Piece::White:Piece::Black);
    }
    vector<pair<int,int>> geldige_zetten(Game* g, bool slaanZetten = false, bool ilegalMoves = false) override;
    vector<pair<int,int>> slaan_zetten(Game* g) override;
    string getType() const override;
};

class Koning:public SchaakStuk {
public:
    Koning(zw kleur, int r, int k);
    Piece piece() const override {
        return Piece(Piece::King,getKleur()==wit?Piece::White:Piece::Black);
    }
    vector<pair<int,int>> geldige_zetten(Game* g, bool slaanZetten = false, bool ilegalMoves = false) override;
    vector<pair<int,int>> slaan_zetten(Game* g) override;
    string getType() const override;
};

class Koningin:public SchaakStuk {
public:
    Koningin(zw kleur, int r, int k);
    Piece piece() const override {
        return Piece(Piece::Queen,getKleur()==wit?Piece::White:Piece::Black);
    }
    vector<pair<int,int>> geldige_zetten(Game* g, bool slaanZetten = false, bool ilegalMoves = false) override;
    vector<pair<int,int>> slaan_zetten(Game* g) override;
    string getType() const override;
};

#endif //SCHAKEN_SCHAAKSTUK_H
