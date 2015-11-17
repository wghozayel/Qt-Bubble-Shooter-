#ifndef TRACK_H
#define TRACK_H

#include <QGraphicsItem>
#include <QObject>
#include "bubble.h"
#include <fstream>
#include <iostream>

class QPushButton;                          // forward declaration for the button pointer

// Items to be drawn are derived from QGraphicsItem class
// We must override the boundingRect() function to provide a bounding rectangle (allows framework to draw efficiently)
// We must also override the paint() function which performs the actual drawing
// Derive from QObject class as well for the timer functionality

// Board object handles drawing game board, creating bubbles and moving them around

class Board : public QObject, public QGraphicsItem
{
    Q_OBJECT                                // add support for signals and slots
    Q_INTERFACES(QGraphicsItem)             // needed for signals and slots

public:
    Board();

    void init();                            // Do the initialization tasks
    void setAngle(const QPointF &p);        // set angle based on mouse coordinates
    QRectF boundingRect() const;            // Tell the framework the area occupied by this object
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget); // How this object is drawn
    Qt::GlobalColor random( Qt::GlobalColor);// randomize the colour of the bubble
    enum {DELAY=20, W=Bubble::R*17*2+Bubble::R+5, H=Bubble::R*18*2, T=5,X=40};     // Timer fires every 20 ms, Width is 300, Height is 400, Thickness is 5
    void fitToGrid(int);                       // adds new shot bubble to start bubble in grid structure
    void checkMatches();                    //checks if bubbles are same colours and deletes them
    void deleteBubbles (QList<Bubble*> matches);  //deletes the bubbles
    QList<Bubble *> neighbours(QList<Bubble *> touching);   //checks is bubbles that are touching originals are also same color
    void shoot();                           //shoots the bubbles
    void deleteConnected();                 // deletes the bubbles that are not connected to the board game
    void gameStart();                       //hows high scores at start of game

public slots:
    void restartGame();                     // Slot for restarting game
    void topTen();                          // Slot for showing top 10 high scores
    void help();                            // Slot for showing helpful instructions
    void Expert();                          // Slot for Expert mode
    void Novice();                          // Slot for Novice mode
    void Master();                          // Slot for Master mode

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
private:
    void timerEvent(QTimerEvent *event);    // Timer function handles movement of bubbles. Called every DELAY msec.
    double shootAngle, length ;
    QList<Bubble *> current, start,FirstandSecond,           // store the bubble
    temp,same,temp2,temp3,temp4,temp5,temp6,temp7,
    temp8,temp9,temp10,temp11,temp12,temp13,temp14,
    temp15,temp16,temp17,temp18,temp19,temp20,notcon,con;        //stores one level of bubbles touching the previous bubble starting at current
    QList<QString> names, line, all;                                // lists that hold player names
    QList<int> scores;                                   // list that hold player scores
};

#endif // TRACK_H
