#include "board.h"
#include <QGraphicsScene>
#include <QtGui>
#include <cmath>
#include <QMessageBox>
#include "time.h"
#include "QFile"
#include <QTextStream>

int Color,Offset=1,Y2=Bubble::R+2, chancesleft=5,score=0,Multi=1,timeleft=5000,timerexists=0, NotValidFile=0;
Qt::GlobalColor first,next,rand1,rand2;
QLCDNumber *turn,*showscore,*timer;
QRadioButton *rbn,*rbe,*rbm;
QVBoxLayout *layout;
QLabel *showtimer;
QString player;

// initialize the parameters as appropriate
Board::Board() : shootAngle(-3.1415/4)
{
    srand(time(NULL));
    startTimer(DELAY); // wakeup every DELAY millisec
}

// Tell the area occupied by the track object
QRectF Board::boundingRect() const
{
    return QRectF(0,0, W,H);
}

// Do the initialization tasks. This is where all the buttons are created
void Board::init()
{
    layout = new QVBoxLayout;          // Create a vertical layout for a column of buttons
    QLabel *status =new QLabel("        Score");
    QLabel *status2 = new QLabel("    Chances Left");
    QLabel *Name =new QLabel("         Player Name");

    layout->addWidget(Name);
    QPushButton *b = new QPushButton(tr("Restart")); // create a button for restarting the game
    connect(b, SIGNAL(clicked()), this, SLOT(restartGame()));           // connect restart button to restartGame function
    layout->addWidget(b);        // add the restart button to the button row

    b = new QPushButton(tr("Top 10"));   // Create a button for showing top 10 high scores
    connect(b, SIGNAL(clicked()), this, SLOT(topTen()));   // connect this button to high score function
    layout->addWidget(b);                           // Add it to the row of buttons

    b = new QPushButton(tr("Help"));    //Creat a button for showing helpful instructions
    connect(b, SIGNAL(clicked()), this, SLOT(help()));  //connect this button to help function
    layout->addWidget(b);                               //add it to the row of buttons

    QVBoxLayout *vbox = new QVBoxLayout;
    rbn = new QRadioButton(tr("Novice"));
    connect(rbn, SIGNAL(clicked()), this, SLOT(Novice()));
    rbn->setCheckable(true);
    rbn->setChecked(true);
    rbn->setDown(true);
    vbox->addWidget(rbn);                           // Add it to the row of buttons

    rbe = new QRadioButton(tr("Expert"));
    connect(rbe, SIGNAL(clicked()), this, SLOT(Expert()));
    rbe->setCheckable(true);
    vbox->addWidget(rbe);                           // Add it to the row of buttons

    rbm = new QRadioButton(tr("Master"));
    connect(rbm, SIGNAL(clicked()), this, SLOT(Master()));
    rbm->setCheckable(true);
    vbox->addWidget(rbm);                           // Add it to the row of buttons

    QGroupBox *bg = new QGroupBox(tr("Game Mode"));
    bg->setLayout(vbox);
    layout->addWidget(bg);

    QWidget *window = new QWidget;      // create a window to whold the row of buttons
    window->setLayout(layout);          // add the row of buttons to this window
    window->move(W+2*T,0);                // position it at the right edge
    this->scene()->addWidget(window);   // add the button window to the scene

    layout->addWidget(status2);
    turn=new QLCDNumber(1);     //creates QLCDNumber to hold remaining shots
    turn->setSegmentStyle(QLCDNumber::Filled);
    layout->addWidget(turn);
    turn->display(5);

    layout->addWidget(status);                  //adds text label to say score
    showscore=new QLCDNumber(10);               //creates QLCDNumber to show score
    showscore->setSegmentStyle(QLCDNumber::Filled);
    layout->addWidget(showscore);                //adds a box to display the score
    showscore->display(0);                      //sets the score

    showtimer=new QLabel("     Time Left (ms)");    //label to say time left
    layout->addWidget(showtimer);


    int y=Bubble::R*17+2;

    for(int i=0;i<9;i++)                //draws 9 rows of random coloured bubbles
    {
        int x=Bubble::R+3;
        if(i%2==0)
            x+=Bubble::R;
        for(int p=0;p<17;p++)           // draws 17 column of random coloured bubbles
        {
            start.append(new Bubble(random(rand1),shootAngle));
            start.last()->setParentItem(this);
            start.last()->setPos(x,y);
            x+=Bubble::R*2;
        }
        y-=Bubble::R*2;
    }
    first=random(first);            //gives first a new random colour
    next=random(next);              //gives next a new random colour
    FirstandSecond.append(new Bubble(first,shootAngle));    //adds new bubble to be shot next
    FirstandSecond.back()->setParentItem(this);
    FirstandSecond.back()->setPos(W/2,H);
    FirstandSecond.append(new Bubble(next,shootAngle));     //adds new bubble to be shot after that
    FirstandSecond.back()->setParentItem(this);
    FirstandSecond.back()->setPos(W+Bubble::R*4,H-Bubble::R*8);

    gameStart();                                    // calls function to diplay ten highest scores

}

Qt::GlobalColor Board::random( Qt::GlobalColor col)     //generates random colour
{

    Color=rand() % 6+1;
    if(Color==1)
        col=Qt::yellow;
    if(Color==2)
        col=Qt::blue;
    if(Color==3)
        col=Qt::red;
    if(Color==4)
        col=Qt::cyan;
    if(Color==5)
        col=Qt::magenta;
    if(Color==6)
        col=Qt::green;

    return col;
}

void Board::setAngle(const QPointF &p)
{
    shootAngle= atan2(H-p.y(),-W/2+p.x()); // calculate angle and store it in shootAngle
    length=W/2 +(cos(shootAngle)*X);
}

// Draw the game board
void Board::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Set drawing parameters
    painter->setBrush(Qt::darkGray);   // Black backround
    painter->setPen(QPen(Qt::darkGreen, T, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin)); // Gray, T pixels wide solid line with flat ends and round joins
    painter->drawRect(0,0,W,H);    // Draw the game board
    painter->drawRect(W+Bubble::R*2+5,H-Bubble::R*11+5,Bubble::R*4-10,Bubble::R*5);//draws rectangle to draw the next bubble in
    painter->setPen(QPen(Qt::red, T, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));//Red, T pixles wide solid line with flat ends and round joins
    painter->drawLine(W/2,H,length,H-(sin(shootAngle)*40));  //Draws the shooter
    painter->setPen(QPen(Qt::white, T, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));//change colour of pen
    painter->drawText(W+Bubble::R*4-10, H-Bubble::R*10+5, "Next");                 //draws the text saying next

}

// Timer function that handles movement of bubbles
void Board::timerEvent(QTimerEvent *)
{
    if(timeleft==5000 && timerexists==1)    //checks if we are in master mode and reduces timer accordingly
        timer->display(5);
    if(timeleft==4000 && timerexists==1)    //checks if we are in master mode and reduces timer accordingly
        timer->display(4);
    if(timeleft==3000 && timerexists==1)    //checks if we are in master mode and reduces timer accordingly
        timer->display(3);
    if(timeleft==2000 && timerexists==1)    //checks if we are in master mode and reduces timer accordingly
        timer->display(2);
    if(timeleft==1000 && timerexists==1)    //checks if we are in master mode and reduces timer accordingly
        timer->display(1);
    if(timerexists==1)                      //decrements the second left according to the amount of times timer even is called
        timeleft-=Board::DELAY;
    if(timeleft==0)                         //shoots bubble if time left is 0
    {
        shoot();
        timeleft=5000;                      //resets timer
    }
    if(!current.isEmpty())
        timeleft=5000;                      //resets timer if bubble is shot

    if(!current.isEmpty())
    {
        current.last()->step(DELAY);            //moves bubble
        for(int i=0;i<start.size();i++)
        {
            if(!current.isEmpty() && start[i]!=NULL && !start.isEmpty())    //if the bubble isn't null
            {
                if(current.last()->collidesWithItem(start[i]))      //if bubble collide
                {
                    if(current.last()->Bubble::multiplier==1)      //if bubble hasnt hit wall
                        Multi=1;
                    if(current.last()->Bubble::multiplier==2)      //if bubble is coming off reflection from wall once
                        Multi=2;
                    if(current.last()->Bubble::multiplier>=3)      //if bubble is coming off reflection from wall once
                        Multi=4;
                    current.last()->Bubble::reflection=0;           //stops bubble
                    if(current.last()->Bubble::reflection==0)
                    {
                        start.append(current.last());                   //adds bubble to start bubbles
                        start.last()->setParentItem(this);              //sets parent item
                        fitToGrid(i);                                   //fits it to grid
                        for(int g=0;g<start.size();g++)
                            start.at(g)->SetFalse(start.at(g));         //sets all the bubbles to false
                        checkMatches();                                 //checks if colours are same
                        if(!current.isEmpty())
                            current.takeLast();                         //removes current is it wasnt already removed

                    }
                }
            }
        }
        if(!current.isEmpty()&&current.last()->Bubble::reflection==0)       //if doesnt collide with any bubble ie hits top of board
        {
            start.append(current.last());                   //adds bubble to start bubbles
            start.last()->setParentItem(this);              //sets parent item
            fitToGrid(0);                                   //fits it to grid
            checkMatches();                                 //checks if colours are same
            if(!current.isEmpty())
                current.takeLast();                         //removes current is it wasnt already removed
        }
    }

    if(chancesleft==0)                                        //if timer gets to zero, all rows advance down one row
    {


        if(rbn->isDown())       //resets chnces left to 5 is novice
            chancesleft=5;
        else
            chancesleft=3;      //resets chances left to 3 for expert and master
        turn->display(chancesleft);     //displays chances left
        for(int i=0;i<start.size();i++) //moves ll bubbles down one row
        {
            int curx,cury;
            curx=start.at(i)->pos().x();
            cury=start.at(i)->pos().y()+Bubble::R*2;
            start.at(i)->setPos(curx,cury);
        }

        int x2=Bubble::R+3;
        if(Offset%2==0)
            x2+=Bubble::R;
        for (int j=0;j<17;j++)      //draws new row of bubbles at the top
        {
            start.append(new Bubble(Board::random(first),shootAngle));
            start.last()->setParentItem(this);
            start.last()->setPos(x2,Y2);
            start.last()->SetTrue(start.last());
            x2+=Bubble::R*2;

        }
        Offset++;

    }
    for(int i=0;i<start.size();i++) //checks if bubble passes below the bottom of the board and ends game if so
    {
        if(start.at(i)->pos().y()>=Board::H||(!current.isEmpty()&&current.last()->pos().y()>=Board::H+Bubble::R))
        {


            bool ok;
            player = QInputDialog::getText(0, "GAME OVER",      //displays end of game text
                                           "Game over!\nYour score: " + QString::number(score,10) + "\nPlease enter your name:", QLineEdit::Normal,
                                           QDir::home().dirName(), &ok);
            while (ok && player.size()==0)  //doesnt allow you to enter a blank name
            {
                player = QInputDialog::getText(0, "GAME OVER",
                                               "Game over!\nYour score: " + QString::number(score,10) + "\nPlease enter your name:", QLineEdit::Normal,
                                               QDir::home().dirName(), &ok);
                if(ok && player.size()!=0)
                    break;
            }
            if(NotValidFile==0)         //if the file is valid
            {
                if (ok && player.size()<=10 && !player.isEmpty())   //if name is less then ten letters
                {
                    QFile file("C:/Users/Waseem/Desktop/BUBBLE/bubble_highscore.txt");
                    file.open(QIODevice::Append);

                    QTextStream t(&file);
                    t.setCodec("UTF-8");

                    t << "\n" + player + ":" + QString::number(score) ;

                    file.close();

                }
                if(ok && player.size()>10 && !player.isEmpty())     //if name is greater then ten letters only saves first ten
                {
                    QFile file("C:/Users/Waseem/Desktop/BUBBLE/bubble_highscore.txt");
                    file.open(QIODevice::Append);

                    QTextStream t(&file);
                    t.setCodec("UTF-8");

                    player.resize(10);

                    t <<  "\n" + player + ":" + QString::number(score) ;

                    file.close();
                }
            }
            else        //if file in invalid
            {
                if (ok && player.size()<=10 && !player.isEmpty())   //if name is less then ten letters
                {
                    QFile file("C:/Users/Waseem/Desktop/BUBBLE/bubble_highscore.txt");
                    file.open(QIODevice::Append);
                    QByteArray nom;
                    nom.append(player);
                    QByteArray num;
                    num.append(QString::number(score));
                    file.remove();              //removes invalid file
                    file.open(QIODevice::Append);   //creates new file

                    file.write(nom);        //writes name and highscore to new file
                    file.write(":");
                    file.write(num);

                    file.close();
                    NotValidFile=0;     //makes file valid

                }
                if(ok && player.size()>10 && !player.isEmpty())     //if name is greater then ten letters only saves first ten
                {
                    QFile file("C:/Users/Waseem/Desktop/BUBBLE/bubble_highscore.txt");
                    file.open(QIODevice::Append);
                    player.resize(10);
                    QByteArray nom;
                    nom.append(player);
                    QByteArray num;
                    num.append(QString::number(score));
                    file.remove();      //deletes invalid file
                    file.open(QIODevice::Append);       //makes new file

                    file.write(nom);            //adds name and score to new file
                    file.write(":");
                    file.write(num);

                    file.close();
                    NotValidFile=0;         //makes file valid
                }
            }
            restartGame();      //restarts game

        }

    }
    if(!notcon.isEmpty())   //makes all the bubbles not connected to the board drop down
    {
        for(int r=0;r<notcon.size();r++)
        {
            notcon.at(r)->drop(DELAY);
            if(notcon.at(r)->pos().y()>Board::H-10||notcon.at(r)->pos().y()<Board::H-Board::H   //once bubbles pass lower boundry they are deleted
                    ||notcon.at(r)->pos().x()>W+Bubble::R||notcon.at(r)->pos().x()<W-W-Bubble::R)
            {
                deleteBubbles(notcon);
                while(!notcon.isEmpty())    //removes and bubbles from list
                    notcon.takeLast();
            }
        }
    }
}

// slot for the game restart button
void Board::restartGame()
{//empties all the lists of bubbles
    while( !start.isEmpty())
        delete start.takeLast();
    while( !current.isEmpty())
        delete current.takeLast();
    while( !FirstandSecond.isEmpty())
        delete FirstandSecond.takeLast();
    while(!same.isEmpty())
        delete same.takeLast();
    while(!temp.isEmpty())
        delete temp.takeLast();
    layout->removeWidget(timer);    //removes timer
    chancesleft=5;                  //resets chances left
    Offset=1;                       //resets variable to determine weather the row should be drawn off axis
    score=0;                        //resets core
    timerexists=0;                  //resets variable to determine if in master mode
    timeleft=5000;                  //resets time left
    init();                         //calls init
}

void Board::gameStart()     //draws the top ten at the begining of the game
{
    QFile file("C:/Users/Waseem/Desktop/BUBBLE/bubble_highscore.txt");
    QMessageBox newMB;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text) )
    { // failed to open file
        std::cerr << "Could not open file\n";
        NotValidFile=1;
        return;
    }
    int counter=0,lines=0, invalid=0;
    QTextStream t( &file );        // use a text stream

    QString all2;
    while ( !t.atEnd() )    // loop until end of file...
    {
        t.setCodec("UTF-8");
        line.append(t.readLine());         // line of text excluding '\n'
        lines++;

        if ((line.last()).count(':') !=1)           //checks if line contains any":"
        {
            counter=-1;
            invalid++;        // make the file invalid
        }
        else
        {
            counter=0;
        }

        if(counter==0)
        {
            QStringList fields = (line.last()).split(':'); // Read help on QstringList
            int score = fields.at(1).toInt(); // convert the second field to int
            if(fields.size()<=10)
            {
                names.append(fields.first());
                scores.append(score);
            }


            if(!scores.isEmpty()&&(scores.last()<0||scores.last()>2147483647))      //if score is negative or greater then that number
            {
                invalid++;
            }
        }
    }
    if(invalid>0)
    {
        newMB.setText("WARNING:\nInvalid High-Score File");
        newMB.exec();
        NotValidFile=1;
    }
    if(invalid==0)
    {
        int j=1;
        while (!(j==0))     //orders the to ten highest scores
        {
            j=0;
            for (int i=0; i<scores.size()-1; i++)
            {
                if (scores.at(i)<scores.at(i+1))
                {
                    scores.swap(i,(i+1));
                    names.swap(i,(i+1));
                    j++;
                }
            }
        }
        for(int i=0;i<scores.size();i++)    //adds names and scores to a list
        {
            all.append(names.at(i));
            all.append(":");
            all.append(QString::number(scores.at(i)));
            all.append("\n");
        }
        for(int i=10;line.size()>10;i++)
        {
            if(i!=line.size())      //removes names and scores if more then ten lines
                all.takeLast();
            else
                break;
        }
        int k=0;

        for(int i=0;i<(all.size()/4);i++)       //adds the names and score of top ten to a Qstring
        {
            if(i>9)
                break;
            all2.append(all.at(i+k));
            all2.append(all.at(i+1+k));
            all2.append(all.at(i+k+2));
            all2.append(all.at(i+3+k));
            k+=3;
        }

        newMB.setText(all2);        //displays top ten in message box
        newMB.exec();
    }
    while(!all.isEmpty())       //clears lists of names and scores
        all.clear();
    while(!names.isEmpty())     //clears list of names
        names.takeLast();
    while(!scores.isEmpty())    //clears list of scores
        scores.takeLast();
    while(!line.isEmpty())      //clears list of lines
        line.takeLast();
}

// slot for loading a map file
void Board::topTen()
{
    gameStart();        //calls gamestart
    qDebug("clicked Top 10");
}

//Slot for the game help button
void Board::help()
{
    QLabel *instructions = new QLabel("                                    Game rules and instructions:\n                      Point the mouse curser in the direction you wish to shoot the bubble\n                      If the bubble forms a group with 3 or more same colour bubbles,\n                      they will be deleted and you will recieve points.");
    instructions->setWindowTitle("Bubble Shooter - Help");      //writes instruction and displayes them
    instructions->setFixedSize(400,200);
    instructions->show();
    qDebug("clicked Help");
}

void Board::Novice()        //novice mode
{
    if(score!=0 || start.size()>153)
    {
        QMessageBox newMB;
        QAbstractButton *YES;
        YES=newMB.addButton(QMessageBox::Yes);
        newMB.addButton(QMessageBox::No);
        newMB.setText("The game will be reset");            //displays warning message is score is not 0 or bubble has been shot
        newMB.setInformativeText("Are You Sure You Want To Continue?");
        newMB.exec();
        if(newMB.clickedButton()==YES)              //if yes is clicked, changes mode
        {
            layout->removeWidget(timer);

            restartGame();
            chancesleft=5;
            turn->display(chancesleft);
            rbn->setChecked(true);
            rbm->setDown(false);
            rbe->setDown(false);
            rbn->setDown(true);
            timerexists=0;
            timeleft=5000;
        }
        else            //reverts mode to one it was just at
        {
            if(rbn->isDown())
                rbn->setChecked(true);
            if(rbe->isDown())
                rbe->setChecked(true);
            if(rbm->isDown())
                rbm->setChecked(true);
        }
    }
    else            //changes game mode
    {
        layout->removeWidget(timer);

        chancesleft=5;
        timeleft=5000;
        timerexists=0;
        turn->display(chancesleft);
        rbm->setDown(false);
        rbe->setDown(false);
        rbn->setDown(true);
    }

}

void Board::Expert()        //expert game mode
{
    if(score!=0 || start.size()>153)        //everything works the same as in novice except chances left is 3
    {
        QMessageBox newMB;
        QAbstractButton *YES;
        YES=newMB.addButton(QMessageBox::Yes);
        newMB.addButton(QMessageBox::No);
        newMB.setText("The game will be reset");
        newMB.setInformativeText("Are You Sure You Want To Continue?");
        newMB.exec();
        if(newMB.clickedButton()==YES)
        {
            layout->removeWidget(timer);

            restartGame();
            chancesleft=3;
            turn->display(chancesleft);
            rbe->setChecked(true);
            timerexists=0;
            rbn->setDown(false);
            rbm->setDown(false);
            rbe->setDown(true);
            timeleft=5000;
        }
        else
        {
            if(rbn->isDown())
                rbn->setChecked(true);
            if(rbe->isDown())
                rbe->setChecked(true);
            if(rbm->isDown())
                rbm->setChecked(true);
        }
    }
    else
    {
        layout->removeWidget(timer);

        chancesleft=3;
        timeleft=5000;
        timerexists=0;
        turn->display(chancesleft);
        rbn->setDown(false);
        rbm->setDown(false);
        rbe->setDown(true);
    }
}

void Board::Master()            //master game mode
{
    if(score!=0 || start.size()>153)        //everthing works the same as expert except bubbles are shot after 5 second
    {
        QMessageBox newMB;
        QAbstractButton *YES;
        YES=newMB.addButton(QMessageBox::Yes);
        newMB.addButton(QMessageBox::No);
        newMB.setText("The game will be reset");
        newMB.setInformativeText("Are You Sure You Want To Continue?");
        newMB.exec();
        if(newMB.clickedButton()==YES)
        {
            restartGame();
            timeleft=5000;
            chancesleft=3;
            turn->display(chancesleft);
            rbm->setChecked(true);
            timer=new QLCDNumber(4);

            timer->setSegmentStyle(QLCDNumber::Filled);

            layout->addWidget(timer);
            timer->display(timeleft);
            timerexists=1;
            rbn->setDown(false);
            rbe->setDown(false);
            rbm->setDown(true);
        }
        else
        {
            if(rbn->isDown())
                rbn->setChecked(true);
            if(rbe->isDown())
                rbe->setChecked(true);
            if(rbm->isDown())
                rbm->setChecked(true);
        }
    }
    else
    {
        chancesleft=3;
        timeleft=5000;
        turn->display(chancesleft);
        rbm->setChecked(true);

        timer=new QLCDNumber(4);
        timer->setSegmentStyle(QLCDNumber::Filled);

        layout->addWidget(timer);
        timer->display(timeleft);
        timerexists=1;
        rbn->setDown(false);
        rbe->setDown(false);
        rbm->setDown(true);
    }
}

void Board::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);               // call base class function to allow further processing
    shoot();                            //calls function to shoot bubble

}

void Board::shoot()             //shoots bubble
{
    double angle= shootAngle;                            // sets angle to shootAngle


    if(FirstandSecond.size()==2 && current.size()==0)   //checks to make sure no bubble is still moving and there are 2 bubbles in FirstandSecond
    {
        FirstandSecond.at(1)->setPos(W/2,H);
        delete FirstandSecond.takeAt(0);
    }

    if(current.size()==0)                               //checks to make sure no bubble is still moving
    {
        current.append(new Bubble(first,angle));           // adds a randomly selected bubble to the list
        current.last()->setParentItem(this);               // Sets bubble as Parent
        current.last()->setPos(W/2,H);                     // Set it at the center of the shooter.
        first=next;
        next=random(next);
        FirstandSecond.append(new Bubble(next,shootAngle));         //adds the second randomly selected bubble to the list
        FirstandSecond.back()->setParentItem(this);                 // Sets bubble as Parent
        FirstandSecond.back()->setPos(W+Bubble::R*4,H-Bubble::R*8);                  //sets position in the box next
    }
}

void Board::fitToGrid( int p)                   //checks positions on current and start bubbles and decides which place in the grid to snap it to
{
    int x_shot, y_shot, x_start, y_start/*,xlength,ylength,hyp,angle*/;
    x_shot=current.last()->pos().x();
    y_shot=current.last()->pos().y();
    x_start=start.at(p)->pos().x();
    y_start=start.at(p)->pos().y();
    //    xlength=x_shot-x_start;
    //    ylength=y_shot-y_start;
    //    hyp=sqrt(((xlength*xlength)+(ylength*ylength)));
    //    angle=atan(ylength/xlength);
    if(x_shot<x_start)          //if approaches from left
    {
        x_shot=x_start-Bubble::R;
    }
    if(x_shot>=x_start)           //if approaches from right
    {
        x_shot=x_start+Bubble::R;

    }
    if(y_shot<y_start && x_shot>x_start)        //approaches from top right
    {
        for(int i=0;i<start.size();i++)
        {
            if(start.at(i)->pos().y()==y_start-Bubble::R*2 &&start.at(i)->pos().x()==x_start+Bubble::R)
            {
                y_shot=y_start;
                x_shot=x_start+Bubble::R*2;
                break;
            }
            else
            {
                y_shot=y_start-Bubble::R*2;
                x_shot=x_start+Bubble::R;
            }
        }
        start.last()->setPos(x_shot,y_shot);
    }
    if(y_shot<y_start && x_shot<x_start)        //approaches from top left
    {
        for(int i=0;i<start.size();i++)
        {
            if(start.at(i)->pos().y()==y_start-Bubble::R*2 &&start.at(i)->pos().x()==x_start-Bubble::R)
            {
                y_shot=y_start;
                x_shot=x_start-Bubble::R*2;
                break;
            }
            else
            {
                y_shot=y_start-Bubble::R*2;
                x_shot=x_start-Bubble::R;
            }
        }
        start.last()->setPos(x_shot,y_shot);
    }

    if(x_shot<x_start&&x_start==18)          //if approaches from left but at wall
    {
        x_shot=x_start+Bubble::R;
    }
    if(x_shot>=x_start&&x_start==513)           //if approaches from right but at wall
    {
        x_shot=x_start-Bubble::R;

    }


    if(y_shot<y_start+Bubble::R-5&&y_shot>y_start-Bubble::R+5 && x_shot>x_start)       // if approaches from same height and right
    {
        y_shot=y_start;
        x_shot=x_start+Bubble::R*2;
        start.last()->setPos(x_shot,y_shot);
    }
    if(y_shot<y_start+Bubble::R-5&&y_shot>y_start-Bubble::R+5 && x_shot<x_start)       // if approaches from same height and left
    {
        y_shot=y_start;
        x_shot=x_start-Bubble::R*2;
        start.last()->setPos(x_shot,y_shot);
    }
    if(y_shot==y_start&&x_shot==x_start)
        start.last()->setPos(x_shot,y_shot);

    if(y_shot>y_start)          //if approaches from bottem
    {
        y_shot=y_start+2*Bubble::R;
        start.last()->setPos(x_shot,y_shot);
    }
}

void Board::checkMatches()      //checks if bubbles touching are same color
{

    same=neighbours(current);

    if(!same.isEmpty())
    {
        if(same.size()>2)     //if there are at least 3 same color bubbles
        {                                                      //deletes all the same color bubbles touching
            deleteBubbles(same);

            deleteConnected();

        }
        else           //if same color bubbles touhing is 2 or less
        {
            chancesleft--;         //reduces turn
            if(chancesleft<0)      //checks if turn gets to zero
                chancesleft=5;     //resets tunn
            turn->display(chancesleft);     //displays new turns left

        }
    }
    else
    {
        chancesleft--;                 //reduce turn
        if(chancesleft<0)              //check if tun is less than 0
            chancesleft=5;             //resets turn
        turn->display(chancesleft);    //displays new turn

    }
    while(!same.isEmpty())                 //empty all lists of bubbles touching
        same.takeLast();
    while(!temp.isEmpty())
        temp.takeLast();
    while(!temp2.isEmpty())
        temp2.takeLast();

}

void Board::deleteBubbles(QList<Bubble *> matches)  //deletes all matching touching bubbles
{
    for(int k=0; k<start.size();k++)
    {
        for(int j=0; j<matches.size();j++)
        {
            if(start.at(k)==matches.at(j))
            {
                delete start[k];                        //deletes the bubble
                start.replace(k,NULL);                  //sets bubble to null
                score=score+10*Multi;                   //increases score
                showscore->display(score);              //shows new score
            }
        }
    }
    start.removeAll(NULL);                              //remove all null bubbles
}

QList<Bubble *> Board::neighbours(QList<Bubble *> Touch)  //checks level of bubbles to see if touching and same colour
{
    for(int g=0;g<start.size();g++)
        start.at(g)->SetFalse(start.at(g));         //sets all the bubbles to false
    current.last()->SetTrue(current.last());

    int cur_x,cur_y,str_x,str_y;
    if(!Touch.isEmpty())
    {
        for(int t=0;t<Touch.size();t++)          // checks if bubbles that are touching current, are touching other bubbles of same colour
        {
            for(int s=0;s<start.size();s++)
            {

                cur_x=Touch.at(t)->pos().x();
                cur_y=Touch.at(t)->pos().y();
                str_x=start.at(s)->pos().x();
                str_y=start.at(s)->pos().y();

                if((((str_x==cur_x+Bubble::R||str_x==cur_x-Bubble::R)               //checks all bubbles next to originals if same colour
                     && (str_y==cur_y+Bubble::R*2||str_y==cur_y-Bubble::R*2))
                    &&start.at(s)->returnColor()==current.last()->returnColor() &&
                    start.at(s)->returnConnected()==false)
                        ||(((str_x==cur_x+Bubble::R*2||str_x==cur_x-Bubble::R*2)
                            && (str_y==cur_y))
                           && start.at(s)->returnColor()==current.last()->returnColor()&&
                           start.at(s)->returnConnected()==false))
                {
                    start.at(s)->SetTrue(start.at(s));
                    Touch.append(start.at(s));           //adds the touching bubbles to a list

                    for(int t=0;t<Touch.size();t++)
                    {
                        Touch.at(t)->SetTrue(Touch.at(t));
                    }
                    //                    if(!Touch.isEmpty())
                    //                    {
                    //                        for(int g=0;g<Touch.size()-1;g++)            //removes the same bubble if it was added more than once
                    //                        {
                    //                            for(int f=1+g;f<Touch.size();f++)
                    //                            {
                    //                                if(Touch.at(g)->pos().x()==Touch.at(f)->pos().x()
                    //                                        && Touch.at(g)->pos().y()==Touch.at(f)->pos().y())
                    //                                {
                    //                                    Touch.takeAt(g);
                    //                                    break;
                    //                                }
                    //                            }
                    //                        }
                    //                    }
                }
            }
        }
    }

    return Touch;
}

void Board::deleteConnected()       //deletes all bubbles that are not connected to board
{
    for(int g=0;g<start.size();g++)
    {
        start.at(g)->SetFalse(start.at(g));
        int ypos=start.at(g)->pos().y();
        if(ypos==17)
        {
            start.at(g)->SetTrue(start.at(g));
            con.append(start.at(g));
        }
    }


    for(int y=0;y<con.size();y++)
    {
        for(int z=0;z<start.size();z++)
        {
            int cur_x,cur_y,str_x,str_y;
            cur_x=con.at(y)->pos().x();
            cur_y=con.at(y)->pos().y();
            str_x=start.at(z)->pos().x();
            str_y=start.at(z)->pos().y();

            if((((str_x==cur_x+Bubble::R||str_x==cur_x-Bubble::R)
                 && (str_y==cur_y+Bubble::R*2||str_y==cur_y-Bubble::R*2))
                && start.at(z)->returnConnected()==false)
                    ||(((str_x==cur_x+Bubble::R*2||str_x==cur_x-Bubble::R*2)
                        && (str_y==cur_y))
                       && start.at(z)->returnConnected()==false))
            {
                start.at(z)->SetTrue(start.at(z));           //sets the ones that are connected to true
                con.append(start.at(z));
            }

        }
    }


    for(int e=0;e<start.size();e++)
    {
        if(start.at(e)->returnConnected()==false)           //adds all the bubbles that are not connected to other bubbles to a list
            notcon.append(start.at(e));
    }

    if(current.last()->Bubble::multiplier==1)      //if bubble hasnt hit wall
        Multi=2;
    if(current.last()->Bubble::multiplier==2)      //if bubble is coming off reflection from wall once
        Multi=4;
    if(current.last()->Bubble::multiplier>=3)      //if bubble is coming off reflection from wall once
        Multi=8;
    //if(!notcon.isEmpty())
    //  DeleteBubbles(notcon);                                  //deletes none connected bubbles

    //    while(!notcon.isEmpty())
    //        notcon.takeLast();
    while(!con.isEmpty())
        con.takeLast();
}
