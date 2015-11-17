#ifndef CREATURE_H
#define CREATURE_H

#include <Qt>
#include <QGraphicsItem>
#include <QRadialGradient>

// Items to be drawn are derived from QGraphicsItem class
// We must override the boundingRect() function to provide a bounding rectangle (allows framework to draw efficiently)
// We must also override the paint() function which performs the actual drawing


// Bubble object encapsulates a bubble
class Bubble : public QGraphicsItem
{
public:
    Bubble(Qt::GlobalColor c, double a); // Constructor takes a colour and angle of travel

    QRectF boundingRect() const;    // Tells the framework the area that this object occupies
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget); // Draw the object

    void step(int delay);           // move the bubble
    void drop(int delay);           //move the disconnected bubbles
    Qt::GlobalColor returnColor();  //returns colour of bubble
    enum {R = 15};                  // radius of bubble
    int reflection, multiplier;                 // bubble bouncing off the sides or hitting top
    void SetTrue(Bubble *bub);            // sets private member "connected"
    bool returnConnected();                    //returns if bubble is connected or not
    void SetFalse(Bubble *bub);                     // check if connected bubbles are touching other bubble

private:
    QRadialGradient gradient;       // make a brush to paint a spehere
    Qt::GlobalColor colour;         // colour of bubble
    double angle;                   // direction of movement in radians
    bool connected;                 // is bubble connected to the walls of the board

};

#endif // CREATURE_H
