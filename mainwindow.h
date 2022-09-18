//  Student: Niels Van den Broeck
//  Rolnummer: S0203844
//  Opmerkingen: (bvb aanpassingen van de opgave)
//
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "game.h"
#include "SchaakStuk.h"
#include <QCloseEvent>
class ChessBoard;
class QSettings;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

    //virtual ~MainWindow();

    void update();
    void showMoves(SchaakStuk* s);
    void showThreats(SchaakStuk* s);
    void showPieceThreats();
private:
    ChessBoard* scene;
    Game g;
    zw kleur = wit;
    bool selected = false;
    SchaakStuk *selectedPiece = nullptr;
    bool blackAI = false;
    bool whiteAI = false;
private slots:
    void clicked(int x, int y);
    void newGame();
    void open();
    void save();
    void undo();
    void redo();
    void moveBlack();
    void moveWhite();
    void visualizationChange();
    void setBlackAI(bool b);
    void setWhiteAI(bool b);

    // De volgende lijnen kunnen genegeerd worden voor de opdracht

    void on_actionExit_triggered();
    void closeEvent (QCloseEvent *event)
    {
        on_actionExit_triggered();
        event->ignore();
    }

private:
    void createActions();
    void createMenus();
    QMenu *fileMenu;
    QMenu *gameMenu;
    QMenu *visualizeMenu;
    QMenu *AI;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *exitAct;

    QAction* display_moves;
    QAction* display_kills;
    QAction* display_threats;

    QAction* Black;
    QAction* White;
};

#endif // MAINWINDOW_H
