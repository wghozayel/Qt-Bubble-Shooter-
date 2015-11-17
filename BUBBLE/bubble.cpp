#include "bubble.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <cmath>
#include "board.h"

// Initialize the colour and direction of movement
Bubble::Bubble(Qt::GlobalColor c, double a) : reflection(1), multiplier(1), gradient(-R, -R, 2*R) , colour(c), angle(a)
{
    gradient.setColorAt(1, QColor(colour).lighter(30));
    gradient.setColorAt(0, QColor(colour).lighter(150));
    connected=true;
}

// Tell the area of the object to the framework
QRectF Bubble::boundingRect() const
{
    return QRectF(-R, -R, 2*R, 2*R);
}

// Draw the object. Framework calls this function whenever the object needs to be drawn.
// We have no control over when this actually gets called
// Add code here to make the bubble look the way you want
void Bubble::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Set a radial gradient to make it look like a sphere
    painter->setBrush(gradient);            // set this as the brush
    painter->setPen(QPen(Qt::black, 0));    // set a zero-width border

    painter->drawEllipse(-R,-R,R*2,R*2);// Draw the circle.
}

// This gets called periodically and avoids a separate timer for each creature.
// Add code here that handles the position of the bubbles
void Bubble::step(int delay)
{
    const double VEL(0.7);      // Velocity of the bubble
    double x = pos().x(); double y = pos().y();     // get current x,y coordinates
    double new_x, new_y;

    if (x<=Board::W-Board::W+R || x>=Board::W-R)
    {
        reflection*=-1;
        multiplier++;
    }

    if (y<=Board::H-Board::H+R+Board::T)
        reflection=0;

    if (reflection!=0)
    {
        new_x=x+reflection*(VEL*delay*cos(angle)); // A4: calculate new position based on 'VEL' and 'angle', uncomment following line and pass appropriate value
        new_y=y-(VEL*delay*sin(angle));
        setPos(new_x, new_y);
    }



    if (reflection==0)
        setPos(x,y);



}

void Bubble::drop(int delay)
{
    const double VEL(0.3);      // Velocity of the bubble
    double x = pos().x(); double y = pos().y();     // get current x,y coordinates
    double new_x, new_y;

    new_x=x/*+VEL*delay*cos(angle)*/; // A4: calculate new position based on 'VEL' and 'angle', uncomment following line and pass appropriate value
    new_y=y+(VEL*delay/**sin(angle)*/);
    setPos(new_x, new_y);

}

Qt::GlobalColor Bubble::returnColor()
{
return colour;
}

void Bubble::SetTrue (Bubble * bub)
{
        bub->connected=true;
}
bool Bubble::returnConnected()
{
    return connected;
}

void Bubble::SetFalse(Bubble * bub)
{
    bub->connected=false;
}
