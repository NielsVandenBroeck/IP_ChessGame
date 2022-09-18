//  Student: Niels Van den Broeck
//  Rolnummer: S0203844
//  Opmerkingen: (bvb aanpassingen van de opgave)
//

#include <iostream>
#include "mainwindow.h"
#include <QMessageBox>
#include <QtWidgets>
#include <thread>
using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    scene = new ChessBoard;
    QGraphicsView *view = new QGraphicsView(scene);
    setCentralWidget(view);

    connect(scene,SIGNAL(clicked(int,int)),this,SLOT(clicked(int,int)));

    createActions();
    createMenus();
    update();
}

//deze functie geeft alle mogelijke zetten van een stuk visueel weer
void MainWindow::showMoves(SchaakStuk* s){
    //Kijkt na of showMoves weergeven moeten worden
    if(display_moves->isChecked()) {
        vector<pair<int,int>> zetten = s->geldige_zetten(g.getGame());
        //Alle zetten van het gegeven stuk markeren
        for (int i = 0; i < zetten.size(); i++) {
            scene->setTileFocus(zetten[i].first, zetten[i].second,true);
        }
    }
}

//deze functie geeft alle posities visueel weer waar een gegeven stuk geslagen kan worden
void MainWindow::showThreats(SchaakStuk* s){
    //Kijkt na of showThreats weergeven moet worden
    if(display_threats->isChecked()) {
        vector<pair<int,int>> zetten = s->geldige_zetten(g.getGame());
        //Loopt over alle zetten van het gegeven stuk
        for (int x = 0; x < zetten.size(); x++) {
            //Loopt over alle stukken en zetten van de andere speler
            for (int y = 0; y < g.getPieces(s->getAndereKleur()).size(); y++) {
                for (int z = 0; z < g.getPieces(s->getAndereKleur())[y]->slaan_zetten(g.getGame()).size(); z++) {
                    //Als de zet in slaanzetten van een ander stuk behoord, moet die positie rood worden gekleurd
                    if (g.getPieces(s->getAndereKleur())[y]->slaan_zetten(g.getGame())[z] == zetten[x]) {
                        scene->setTileThreat(zetten[x].first,zetten[x].second, true);
                    }
                }
            }
        }
    }
}

//deze functie kleurt alle stukken in het rood als ze geslagen kunnen worden
void MainWindow::showPieceThreats(){
    //Kijkt na of de showPieceThreats weergeven moeten worden
    if(display_kills->isChecked()) {
        //Alle zetten van de andere speler worden rood (stuk wordt rood) dus als er geen stuk op staat dan gebeurt er niets
        for (int i = 0; i < g.getPieces(wit).size(); i++) {
            vector<pair<int,int>> zetten = g.getPieces(wit)[i]->geldige_zetten(g.getGame(), false, true);
            for (int j = 0; j < zetten.size(); j++) {
                scene->setPieceThreat(zetten[j].first,zetten[j].second, true);
            }
        }
        //Alle zetten van de andere speler worden rood (stuk wordt rood) dus als er geen stuk op staat dan gebeurt er niets
        for (int i = 0; i < g.getPieces(zwart).size(); i++) {
            vector<pair<int,int>> zetten = g.getPieces(zwart)[i]->geldige_zetten(g.getGame(), false, true);
            for (int j = 0; j < zetten.size(); j++) {
                scene->setPieceThreat(zetten[j].first,zetten[j].second, true);
            }
        }
    }
    return;
}

// Deze functie wordt opgeroepen telkens er op het schaakbord
// geklikt wordt. x,y geeft de positie aan waar er geklikt
// werd; r is de 0-based rij, k de 0-based kolom
void MainWindow::clicked(int r, int k) {
    //Kijkt na of het spel nog bezig is
    if(!g.getEndOfGame()) {
        //Als er nog geen stuk geselecteerd is
        if (selected == false) {
            //is de geselecteerde positie een stuk
            if (g.getPiece(r, k) == nullptr) {
                return;
            } else if (g.getPiece(r, k)->getKleur() == kleur) {
                //selecteer dit stuk
                scene->removeAllPieceThreats();
                scene->setTileSelect(r, k, true);
                selectedPiece = g.getPiece(r, k);
                selected = true;
                showMoves(selectedPiece);
                showThreats(selectedPiece);
            }
        }
        //stuk is al geselecteerd en wil nu verplaatst worden
        else {
            //deselect een stuk als er nog eens wordt op geklikt
            if (selectedPiece->getRij() == r && selectedPiece->getKolom() == k) {
                selected = false;
                selectedPiece = nullptr;

                scene->removeAllTileSelection();
                scene->removeAllTileFocus();
                scene->removeAllTileDanger();
                scene->removeAllMarking();
                scene->removeAllPieceThreats();
            }
            //Kijkt na of het naar de geselecteerde positie kan verplaatsen, zoja, doet het dat ook
            else if (g.canMove(selectedPiece, r, k)) {
                scene->removeAllTileSelection();
                scene->removeAllTileFocus();
                scene->removeAllTileDanger();
                scene->removeAllMarking();
                scene->removeAllPieceThreats();
                g.move(selectedPiece, r, k);
                if (kleur == wit) {
                    kleur = zwart;
                } else {
                    kleur = wit;
                }
                selected = false;
                selectedPiece = nullptr;
                update();

                showPieceThreats();
                //Als AI aanstaat, word de volgende zet geautomatiseerd
                if (blackAI) {
                    moveBlack();
                }
                if (whiteAI) {
                    moveWhite();
                }
                return;
            }
            update();
            showPieceThreats();
        }
    }
    return;
}

//Maakt een nieuw spel aan (reset)
void MainWindow::newGame()
{
    g.setEndOfGame(false);
    g.deleteBord();
    g.setStartBord();
    selected = false;
    selectedPiece = nullptr;
    kleur = wit;
    scene->removeAllTileSelection();
    scene->removeAllTileFocus();
    scene->removeAllTileDanger();
    scene->removeAllMarking();
    this->update();
}

//Slaat een spel op
void MainWindow::save() {
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save game"), "",
                                                    tr("Chess File (*.chs);;All Files (*)"));

    if (fileName.isEmpty())
        return;
    else {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        QDataStream out(&file);
        //Loopt over het bord en zet alle stukken in string vorm om op te slaan
        for(int r = 0; r < 8; r++) {
            for (int k = 0; k < 8; k++) {
                if(g.getPiece(r,k) == nullptr){
                    out << QString(".");
                }
                else if(g.getPiece(r,k)->getKleur() == zwart) {
                    if (g.getPiece(r, k)->getType() == "Pion") {
                        out << QString("Pb");
                    }
                    if (g.getPiece(r, k)->getType() == "Paard") {
                        out << QString("Hb");
                    }
                    if (g.getPiece(r, k)->getType() == "Loper") {
                        out << QString("Bb");
                    }
                    if (g.getPiece(r, k)->getType() == "Toren") {
                        out << QString("Rb");
                    }
                    if (g.getPiece(r, k)->getType() == "Koningin") {
                        out << QString("Qb");
                    }
                    if (g.getPiece(r, k)->getType() == "Koning") {
                        out << QString("Kb");
                    }
                }
                else if(g.getPiece(r,k)->getKleur() == wit) {
                    if (g.getPiece(r, k)->getType() == "Pion") {
                        out << QString("Pw");
                    }
                    if (g.getPiece(r, k)->getType() == "Paard") {
                        out << QString("Hw");
                    }
                    if (g.getPiece(r, k)->getType() == "Loper") {
                        out << QString("Bw");
                    }
                    if (g.getPiece(r, k)->getType() == "Toren") {
                        out << QString("Rw");
                    }
                    if (g.getPiece(r, k)->getType() == "Koningin") {
                        out << QString("Qw");
                    }
                    if (g.getPiece(r, k)->getType() == "Koning") {
                        out << QString("Kw");
                    }
                }
            }
        }
        //houdt bij wie er aan zet was
        if(this->kleur == wit){
            out << QString("w");
        }
        else{
            out << QString("z");
        }
    }
}

//opent een opgeslagen spel
void MainWindow::open() {
    g.setEndOfGame(false);
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Load game"), "",
                                                    tr("Chess File (*.chs);;All Files (*)"));
    if (fileName.isEmpty())
        return;
    else {

        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }

        try {
            QDataStream in(&file);
            g.deleteBord();
            selected = false;
            selectedPiece = nullptr;
            kleur = wit;
            scene->removeAllTileSelection();
            scene->removeAllTileFocus();
            scene->removeAllTileDanger();
            scene->removeAllMarking();
            //Loopt over het bord om de stukken erop te plaatsen (leest string per string en kijkt welk type dit stuk is)
            for (int r=0;r<8;r++) {
                for (int k=0;k<8;k++) {
                    QString piece;
                    in >> piece;
                    if(piece == "."){
                        g.setPiece(r,k, nullptr);
                    }
                    else if(piece == "Pb"){
                        SchaakStuk* s=new Pion(zwart,r,k);
                        g.setPiece(r,k, s);
                    }
                    else if(piece == "Bb"){
                        SchaakStuk* s=new Loper(zwart,r,k);
                        g.setPiece(r,k, s);
                    }
                    else if(piece == "Rb"){
                        SchaakStuk* s=new Toren(zwart,r,k);
                        g.setPiece(r,k, s);
                    }
                    else if(piece == "Hb"){
                        SchaakStuk* s=new Paard(zwart,r,k);
                        g.setPiece(r,k, s);
                    }
                    else if(piece == "Qb"){
                        SchaakStuk* s=new Koningin(zwart,r,k);
                        g.setPiece(r,k, s);
                    }
                    else if(piece == "Kb"){
                        SchaakStuk* s=new Koning(zwart,r,k);
                        g.setPiece(r,k, s);
                    }
                    else if(piece == "Pw"){
                        SchaakStuk* s=new Pion(wit,r,k);
                        g.setPiece(r,k, s);
                    }
                    else if(piece == "Bw"){
                        SchaakStuk* s=new Loper(wit,r,k);
                        g.setPiece(r,k, s);
                    }
                    else if(piece == "Rw"){
                        SchaakStuk* s=new Toren(wit,r,k);
                        g.setPiece(r,k, s);
                    }
                    else if(piece == "Hw"){
                        SchaakStuk* s=new Paard(wit,r,k);
                        g.setPiece(r,k, s);
                    }
                    else if(piece == "Qw"){
                        SchaakStuk* s=new Koningin(wit,r,k);
                        g.setPiece(r,k, s);
                    }
                    else if(piece == "Kw"){
                        SchaakStuk* s=new Koning(wit,r,k);
                        g.setPiece(r,k, s);
                    }
                    if (in.status()!=QDataStream::Ok) {
                        throw QString("Error reading file "+fileName);
                    }
                }
            }
            QString beurt;
            in >> beurt;
            //geeft aan wie aan beurt was
            if(beurt == "w"){
                kleur = wit;
            }
            else if(beurt == "z"){
                kleur = zwart;
            }
        } catch (QString& Q) {
            QMessageBox::information(this, tr("Error reading file"),
                                     Q);
        }
    }
    update();
}

//voert undo uit
void MainWindow::undo() {
    if(blackAI || whiteAI){
        QMessageBox box1;
        box1.setText("Cannot undo while playing against AI.");
        box1.exec();
    }
    else {
        //voert g.Undo aan, als het lukt worden alle tile selects verwijdert
        if (g.Undo()) {
            scene->removeAllTileSelection();
            scene->removeAllTileFocus();
            scene->removeAllTileDanger();
            scene->removeAllMarking();
            update();
            //van kleur wisselen
            if (kleur == wit) {
                kleur = zwart;
            } else {
                kleur = wit;
            }
        }
    }
}

//voert redo uit
void MainWindow::redo() {
    if(blackAI || whiteAI){
        QMessageBox box1;
        box1.setText("Cannot redo while playing against AI.");
        box1.exec();
    }
    else {
        //voert g.redo aan, als het lukt worden alle tile selects verwijdert
        if (g.Redo()) {
            scene->removeAllTileSelection();
            scene->removeAllTileFocus();
            scene->removeAllTileDanger();
            scene->removeAllMarking();
            update();
            //verwisselen van kleur
            if (kleur == wit) {
                kleur = zwart;
            } else {
                kleur = wit;
            }
        }
    }
}

//Verplaatst een zwart stuk automatisch
void MainWindow::moveBlack() {
    vector<SchaakStuk*> p = g.getPieces(zwart);
    //zetten waardoor er stuk geslagen kan worden/ schaak zetten
    for(int i = 0; i < p.size(); i++) {
        if (g.possiblemoves(p[i])) {
            vector<pair<int, int>> z = p[i]->geldige_zetten(g.getGame());
            for (int j = 0; j < z.size(); j++) {
                //Als deze zet een ander stuk neem of schaak kan zetten, voert het deze zet uit
                if (g.getPiece(z[j].first, z[j].second) != nullptr || !g.testMove(p[i], z[j].first, z[j].second, p[i]->getAndereKleur())) {
                    g.move(p[i], z[j].first, z[j].second);
                    // reset alle tile selects en update
                    scene->removeAllTileSelection();
                    scene->removeAllTileFocus();
                    scene->removeAllTileDanger();
                    scene->removeAllMarking();
                    kleur = wit;
                    update();
                    if(whiteAI){
                        moveWhite();
                    }
                    return;
                }
            }
        }
    }

    //random zet
    int randIndex = rand() % p.size();
    for(int i = 0; i< p.size(); i++){
        //Als de zet gespeeld kan worden, voer de zet uit
        if(g.possiblemoves(p[(randIndex + i)% p.size()])){
            vector<pair<int,int>> z = p[(randIndex + i)% p.size()]->geldige_zetten(g.getGame());
            pair<int,int> move = z[rand() % z.size()];
            g.move(p[(randIndex + i)% p.size()], move.first, move.second);
            break;
        }
    }
    // reset alle tile selects en update
    scene->removeAllTileSelection();
    scene->removeAllTileFocus();
    scene->removeAllTileDanger();
    scene->removeAllMarking();
    kleur = wit;
    update();
    if(whiteAI){
        moveWhite();
    }
    return;
}

//verplaatst en wit stuk automatisch
void MainWindow::moveWhite(){
    vector<SchaakStuk*> p = g.getPieces(wit);
    //zetten waardoor er stuk geslagen kan worden/ schaak zetten
    for(int i = 0; i < p.size(); i++) {
        if (g.possiblemoves(p[i])) {
            vector<pair<int, int>> z = p[i]->geldige_zetten(g.getGame());
            for (int j = 0; j < z.size(); j++) {
                //Als deze zet een ander stuk neem of schaak kan zetten, voert het deze zet uit
                if (g.getPiece(z[j].first, z[j].second) != nullptr || !g.testMove(p[i], z[j].first, z[j].second, p[i]->getAndereKleur())) {
                    g.move(p[i], z[j].first, z[j].second);
                    // reset alle tile selects en update
                    scene->removeAllTileSelection();
                    scene->removeAllTileFocus();
                    scene->removeAllTileDanger();
                    scene->removeAllMarking();
                    kleur = zwart;
                    update();
                    //std::this_thread::sleep_for(std::chrono::seconds(1));
                    if(blackAI){
                        moveBlack();
                    }
                    return;
                }
            }
        }
    }

    //random zet
    int randIndex = rand() % p.size();
    for(int i = 0; i< p.size(); i++){
        //Als de zet gespeeld kan worden, voer de zet uit
        if(g.possiblemoves(p[(randIndex + i)% p.size()])){
            vector<pair<int,int>> z = p[(randIndex + i)% p.size()]->geldige_zetten(g.getGame());
            pair<int,int> move = z[rand() % z.size()];
            g.move(p[(randIndex + i)% p.size()], move.first, move.second);
            break;
        }
    }
    // reset alle tile selects en update
    scene->removeAllTileSelection();
    scene->removeAllTileFocus();
    scene->removeAllTileDanger();
    scene->removeAllMarking();
    kleur = zwart;
    update();
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    if(blackAI){
        moveBlack();
    }
    return;
}

//verandert de mogelijkheid om showmoves/showthreats/showplayerthreads weer te geven of niet
void MainWindow::visualizationChange() {
    QMessageBox box;
    QString visstring = QString(display_moves->isChecked()?"T":"F")+(display_kills->isChecked()?"T":"F")+(display_threats->isChecked()?"T":"F");
    box.setText(QString("Visualization changed : ")+visstring);
    box.exec();
}

//zet AI aan of uit
void MainWindow::setBlackAI(bool b){
    //Als wit al ingesteld is als AI, kan het niet worden uitgevoerd
    if (whiteAI == true) {
        QMessageBox box1;
        box1.setText("You are already playing against the computer.");
        box1.exec();
    } else {
        //Zorg ervoor dat wit niet als AI kan worden ingesteld
        blackAI = b;
        if (kleur == zwart) {
            moveBlack();
        }
        White->setCheckable(!b);
    }
}

//zet AI aan of uit
void MainWindow::setWhiteAI(bool b) {
    //Als zwart al ingesteld is als AI, kan het niet worden uitgevoerd
    if (blackAI == true) {
        QMessageBox box1;
        box1.setText("You are already playing against the computer.");
        box1.exec();
    } else {
        //Zorg ervoor dat wit niet als AI kan worden ingesteld
        whiteAI = b;
        if (kleur == wit) {
            moveWhite();
        }
        Black->setCheckable(!b);
    }
}

// Update de inhoud van de grafische weergave van het schaakbord (scene)
// en maak het consistent met de game state in variabele g.
void MainWindow::update() {
    scene->clearBoard();
    //Loopt ober het bord
    for(int r = 0; r < 8; r++){
        for(int k = 0; k < 8; k++ ){
            //zet het item visueel in scene
            if(g.getPiece(r,k)!=nullptr){
                scene->setItem(r,k,g.getPiece(r,k)->piece());
            }
        }
    }
}





////////////////






void MainWindow::createActions() {
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Start a new game"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newGame);

    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Read game from disk"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save game to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);

    exitAct = new QAction(tr("&Exit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Abandon game"));
    connect(exitAct, &QAction::triggered, this, &MainWindow::on_actionExit_triggered);

    undoAct = new QAction(tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo last move"));
    connect(undoAct, &QAction::triggered, this, &MainWindow::undo);

    redoAct = new QAction(tr("&redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo last undone move"));
    connect(redoAct, &QAction::triggered, this, &MainWindow::redo);

    display_moves= new QAction(tr("&valid moves"), this);
    display_moves->setStatusTip(tr("Show valid moves"));
    display_moves->setCheckable(true);
    display_moves->setChecked(true);
    connect(display_moves, &QAction::triggered, this, &MainWindow::visualizationChange);

    display_kills= new QAction(tr("threathed &enemy"), this);
    display_kills->setStatusTip(tr("Highlight threathened pieces (enemy)"));
    display_kills->setCheckable(true);
    display_kills->setChecked(true);
    connect(display_kills, &QAction::triggered, this, &MainWindow::visualizationChange);

    display_threats= new QAction(tr("threathed &player"), this);
    display_threats->setStatusTip(tr("Highlight threathened pieces (player)"));
    display_threats->setCheckable(true);
    display_threats->setChecked(true);
    connect(display_threats, &QAction::triggered, this, &MainWindow::visualizationChange);

    Black= new QAction(tr("Black AI"), this);
    Black->setStatusTip(tr("Black AI"));
    Black->setCheckable(true);
    Black->setChecked(false);
    connect(Black, &QAction::triggered, this, &MainWindow::setBlackAI);

    White = new QAction(tr("White AI"), this);
    White->setStatusTip(tr("White AI"));
    White->setCheckable(true);
    White->setChecked(false);
    connect(White, &QAction::triggered, this, &MainWindow::setWhiteAI);
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(exitAct);

    gameMenu = menuBar()->addMenu(tr("&Game"));
    gameMenu->addAction(undoAct);
    gameMenu->addAction(redoAct);

    visualizeMenu = menuBar()->addMenu(tr("&Visualize"));
    visualizeMenu->addAction(display_moves);
    visualizeMenu->addAction(display_kills);
    visualizeMenu->addAction(display_threats);

    AI = menuBar()->addMenu(tr("&AI"));
    AI->addAction(Black);
    AI->addAction(White);

}

void MainWindow::on_actionExit_triggered() {
    if (QMessageBox::Yes == QMessageBox::question(this,
                                                  tr("Spel verlaten"),
                                                  tr("Bent u zeker dat u het spel wil verlaten?\nNiet opgeslagen wijzigingen gaan verloren.")))
    {
        QApplication::quit();
    }
}

//MainWindow::~MainWindow() {
//    delete g.getGame();
//}

