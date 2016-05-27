/*
 * NAME: Huisen Zhan
 * USERID: hszhan
 *
 * Fall 2015 CS349 Assignment 1:  An implementation of Breakout in C/C++ and Xlib.
 * 
 * 
 *
 * Commands to compile and run:
 *
 *  g++ -o a1 a1.cpp -L/usr/X11R6/lib -lX11 -lstdc++
 *  ./a1
 *
 * Note: the -L option and -lstdc++ may not be needed on some machines.
 */


#include <iostream>
#include <list>
#include <cstdlib>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>

/*
 * Header files for X functions
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;

int ballspeed = 5;
const int BufferSize = 10;
int FPS = 40;
const int Border = 10;
const int win_width = 800;
const int win_height = 500;
const int paddle_width = 100;
const int paddle_height = 10;
const int gap = 2;
bool win = false;
bool up = false;
int j = 0;
bool start = false;
int level = 1;
int row = 0;
int colmun = 0;
int windowWidth = win_width;
int windowHeight = win_height;
int diameter = windowWidth/90;
bool stopAnimation = false;
bool startAnimation = false;
int x_paddle;
int y_paddle;
int brick_width = windowWidth/20;
int brick_height = windowHeight/30;
int paddleSpeed = 100;
int score = 0;
int OldScore = 0;
int life = 3;
vector<XColor> color;
/*
 * Information to draw on the window.
 */
struct XInfo {
    Display* 	display;
    int		 screen;
    Window	 window;
    GC       gc[6];

    Pixmap  pixmap;
    int      width;
    int      height;
};


/*
 * Function to put out a message on error exits.
 */
void error( string str ) {
    cerr << str << endl;
    exit(0);
}

class Displayable {
    public:
        virtual void paint(XInfo &xinfo) = 0;
};  


class Paddle : public Displayable{
    public:
        void paint(XInfo &xinfo){
            if(!startAnimation){
                xlocation = windowWidth/2;
                ylocation = windowHeight-paddle_height;
                x_paddle = xlocation;
                y_paddle = ylocation;
            }else{
                ylocation = windowHeight-paddle_height;
            }
            XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[1],xlocation,ylocation,paddle_width,paddle_height);
        }
        void move_left(XInfo &xinfo){
            if(xlocation - paddle_width <= 0){
                xlocation = 0;
            }else{
                xlocation -= speed;
            }
            x_paddle = xlocation;
            y_paddle = ylocation;
        }
        void move_right(XInfo &xinfo){
            if(xlocation + paddle_width >= xinfo.width){
                xlocation = xinfo.width - paddle_width;
            }else{
                xlocation += speed;
            }
            x_paddle = xlocation;
            y_paddle = ylocation;
        }
        void mouse_move(int x, XInfo &xinfo){
        
           if(x < 0){
                x = 0;
                xlocation = x;
            }else{
                x -= speed;
                xlocation = x;
            }
            if(x >= xinfo.width - paddle_width){
                xlocation = xinfo.width - paddle_width;
            }else{
                x += speed;
                xlocation = x;
            }
            x_paddle = xlocation;
            y_paddle = ylocation;
            
        }
        Paddle(int paddle_width, int paddle_height):paddle_width(paddle_width),paddle_height(paddle_height){
            speed = paddleSpeed;
        }
    private:
        int xlocation;
        int ylocation;
        int paddle_width;
        int paddle_height;
        int speed;
};

class Brick : public Displayable{
    public:
        bool destoryed;
        int xlocation;
        int ylocation;
        int my_color;
        Brick(int x, int y): xlocation(x), ylocation(y), destoryed(false), my_color(4){

        }
        void paint(XInfo &xinfo){
            //cout << "test" << endl;
            if(destoryed) return;
           
            //cout << "here" << endl;
            
            XDrawRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[0], xlocation, ylocation, brick_width, brick_height);
            XSetForeground(xinfo.display, xinfo.gc[3], color[my_color].pixel);
            XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[3], xlocation, ylocation, brick_width, brick_height); 
            // if(destoryed) return;
            // xlocation = windowWidth/4;
            // ylocation = windowHeight/12;
            // BrickVector.clear();
            // while(ylocation < (windowHeight/4)){
            //     while(xlocation < (3*windowWidth/4)){
            //         if(!destoryinfo[make_pair(xlocation,ylocation)]){
            //             ++j;
            //             if(j < 5){
            //                 XSetForeground(xinfo.display, xinfo.gc[3], (color[j]).pixel);
            //             }
            //             XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[3], xlocation, ylocation, brick_width, brick_height);
            //         }
            //         xlocation += brick_width+gap;
            //         info.xlocation = xlocation;
            //         info.ylocation = ylocation;
            //         BrickVector.push_back(info);
            //     }
            //     ylocation += brick_height+gap;
            //     xlocation = windowWidth/4;
            // }
        } 
}; 

class Ball : public Displayable {
    private:
      int xlocation;
      int ylocation;
      int diameter;
      int speed;

    public:
        int directionX;
        int directionY;
        Ball(int diameter):diameter(diameter){
            
        }

        void setSpeed(int s){
            speed = s;
            if(!startAnimation){
                srand((unsigned)time(0));
                int random = (rand()%3)-2;
                if(random > 0){ 
                    directionX = speed;
                    directionY = speed;
                } else {
                    directionX = 0-speed;
                    directionY = speed;
                }
                    
                    startAnimation = true;
            }
            if(up){
              level += 1;
              if(directionX > 0){
                directionX += speed;
              }else{
                directionX -= speed;  
              }
              if(directionY > 0){
                directionY += speed;
              }else{
                directionY -= speed;  
              }
              up = false;
            }
        }
        void paint(XInfo &xinfo) {
          
            if(!startAnimation){
                xlocation = windowWidth/2+paddle_width/2;
                ylocation = windowHeight-paddle_height-diameter;
            }
            XFillArc(xinfo.display, xinfo.pixmap, xinfo.gc[2], xlocation, ylocation, diameter, diameter, 0, 360*64);
        }

        void move(Paddle &paddle, list<Brick*> bricks) {

            //hit the paddle
            if((ylocation+diameter >= y_paddle) && 
               (ylocation <= y_paddle+paddle_height) &&
               (xlocation+diameter >= x_paddle) && 
               (xlocation <= x_paddle+paddle_width)){
                directionY = 0-directionY;
            }
            //hit brick
            
            // if(!balltouched_brick){
                list<Brick*>::iterator begin = bricks.begin();
                list<Brick*>::iterator end = bricks.end();
                while(begin != end) {
                  Brick * b = * begin;
                  if(b->destoryed) {
                    begin ++;
                    continue;
                  };

                  if(this->ylocation + this->directionY + this->diameter >= b->ylocation&&
                     this->ylocation + this->directionY<= b->ylocation + brick_height){
                    //move from x right
                    if(this->xlocation < b->xlocation - diameter &&
                       this->xlocation + this->directionX + diameter >= b->xlocation){
                        if(b->my_color == 0){
                          b->destoryed = true;
                        }
                        score += 10;
                        this->directionX = 0 - directionX;
                        b->my_color--;
                      // if(!destoryinfo[make_pair(b->xlocation,b->ylocation)]){
                      //     destoryinfo[make_pair(b->xlocation,b->ylocation)] = true;
                      //     directionX = 0 - directionX;
                      //     score += 10;
                      // }
                    }
                    //move from x left
                    if(this->xlocation > b->xlocation + brick_width &&
                       this->xlocation + this->directionX <= b->xlocation + brick_width){
                        if(b->my_color == 0){
                          b->destoryed = true;
                        }
                        score += 10;
                        this->directionX = 0 - directionX;
                        b->my_color--;
                      // if(!destoryinfo[make_pair(b->xlocation,b->ylocation)]){
                      //     destoryinfo[make_pair(b->xlocation,b->ylocation)] = true;
                      //     directionX = 0 - directionX;
                      //     score += 10;
                      // }
                    }
                  }

                  if(xlocation + directionX >= b->xlocation && 
                     xlocation + diameter + directionX <= b->xlocation + brick_width){
                    if(ylocation > b->ylocation + brick_height &&
                       ylocation + directionY <= b->ylocation + brick_height){
                        if(b->my_color == 0){
                          b->destoryed = true;
                        }
                        score += 10;
                        directionY = 0 - directionY;
                        b->my_color--;
                      //  if(!destoryinfo[make_pair(b->xlocation,b->ylocation)]){
                      //     destoryinfo[make_pair(b->xlocation,b->ylocation)] = true;
                      //     directionY = 0 - directionY;
                      //     score += 10;
                      // }
                    }

                    if(ylocation < b->ylocation &&
                       ylocation + directionY >= b->ylocation){
                        if(b->my_color == 0){
                          b->destoryed = true;
                        }
                        score += 10;
                        directionY = 0 - directionY;
                        b->my_color--;
                      //  if(!destoryinfo[make_pair(BrickVector[i].xlocation,BrickVector[i].ylocation)]){
                      //     destoryinfo[make_pair(BrickVector[i].xlocation,BrickVector[i].ylocation)] = true;
                      //     directionY = 0 - directionY;
                      //     score += 10;
                      // }
                    }

                  }//end of if
                  begin ++;
                  //cout << "in loop" << endl;
                }//end of while
                //balltouched_brick = true;
                //cout << "here!" << endl;

            // }else{
            //   balltouched_brick= false;
            // }
            
            //hit wall
            
                if(xlocation + diameter >= windowWidth) {
                    directionX = 0-directionX;
                } 
                if(xlocation <= 0) {
                    directionX = 0-directionX;
                    xlocation = 0;
                } 
                if(ylocation + diameter > windowHeight) {            
                    if(life <= 0){
                      stopAnimation = true;
                    }else{
                      --life;
                      level = 1;
                      startAnimation = false;
                    }

                } 
                if(ylocation <= 0) {
                    directionY = 0-directionY;
                    ylocation = 0;
                }
            if(score >= OldScore + 100){
              ballspeed += 1;
              OldScore = score;
              up = true;
              this->setSpeed(ballspeed);
            }
            ylocation += directionY;
            xlocation += directionX;

     }
};


list<Displayable *> dList;
Paddle paddle(paddle_width,paddle_height);
//Brick brick(brick_width,brick_height);
list<Brick*> bricks;
Ball ball(diameter);

/*
 * Create a window
 */
void initX(int argc, char* argv[], XInfo& xInfo) {
    XSizeHints hints;

    /*
    * Display opening uses the DISPLAY	environment variable.
    * It can go wrong if DISPLAY isn't set, or you don't have permission.
    */
    xInfo.display = XOpenDisplay( "" );
    if ( !xInfo.display )	{
        error( "Can't open display." );
    }

    /*
    * Find out some things about the display you're using.
    */
    xInfo.screen = DefaultScreen( xInfo.display ); // macro to get default screen index

    unsigned long white, black;
    white = XWhitePixel( xInfo.display, xInfo.screen ); 
    black = XBlackPixel( xInfo.display, xInfo.screen );

    hints.x = 100;
    hints.y = 100;
    hints.width = win_width;
    hints.height = win_height;
    hints.flags = PPosition | PSize;

    xInfo.window = XCreateSimpleWindow(
       xInfo.display,				// display where window appears
       DefaultRootWindow( xInfo.display ), // window's parent in window tree
       hints.x, hints.y,            // upper left corner location
       hints.width, hints.height,               // size of the window
       Border,						     // width of window's border
       black,						// window border colour
       white );					    // window background colour

    // extra window properties like a window title
    XSetStandardProperties(
        xInfo.display,		// display containing the window
        xInfo.window,		// window whose properties are set
        "Huisen zhan's Breakout",	// window's title
        "OW",				// icon's title
        None,				// pixmap for the icon
        argv, argc,			// applications command line args
        None );			// size hints for the window
   
   Colormap screen_colormap;     /* color map to use for allocating colors.   */
   XColor red, brown, blue, yellow, green;
   /* get access to the screen's color map. */
   screen_colormap = DefaultColormap(xInfo.display, xInfo.screen);
   /* allocate the set of colors we will want to use for the drawing. */
   XAllocNamedColor(xInfo.display, screen_colormap, "red", &red, &red);
   XAllocNamedColor(xInfo.display, screen_colormap, "brown", &brown, &brown);
   XAllocNamedColor(xInfo.display, screen_colormap, "blue", &blue, &blue);
   XAllocNamedColor(xInfo.display, screen_colormap, "yellow", &yellow, &yellow);
   XAllocNamedColor(xInfo.display, screen_colormap, "green", &green, &green);
   color.push_back(red);
   color.push_back(yellow);
   color.push_back(blue);
   color.push_back(green);
   color.push_back(brown);
   

int i = 0;
    xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
    XSetForeground(xInfo.display, xInfo.gc[i], black);
    XSetBackground(xInfo.display, xInfo.gc[i], white);
    XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
    XSetLineAttributes(xInfo.display, xInfo.gc[i],
                         1, LineSolid, CapButt, JoinRound);

    // Reverse Video
    i = 1;
    xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
    XSetForeground(xInfo.display, xInfo.gc[i], white);
    XSetBackground(xInfo.display, xInfo.gc[i], black);
    XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
    XSetLineAttributes(xInfo.display, xInfo.gc[i],
                         0, LineSolid, CapButt, JoinRound);
    i = 2;
    xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
    XSetForeground(xInfo.display, xInfo.gc[i], blue.pixel);
    XSetBackground(xInfo.display, xInfo.gc[i], white);
    XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
    XSetLineAttributes(xInfo.display, xInfo.gc[i],
                         10, LineSolid, CapButt, JoinRound);

    i = 3;
    xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
    XSetBackground(xInfo.display, xInfo.gc[i], white);
    XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
    XSetLineAttributes(xInfo.display, xInfo.gc[i],
                         gap, LineSolid, CapButt, JoinMiter);
    
    i = 4;
    xInfo.gc[i] = XCreateGC( xInfo.display, xInfo.window, 0, 0 );
    XFontStruct * font;
    font = XLoadQueryFont (xInfo.display, "12x24");
    XSetFont (xInfo.display, xInfo.gc[i], font->fid);
    XSetForeground(xInfo.display, xInfo.gc[i], red.pixel);

    i = 5;
    xInfo.gc[i] = XCreateGC( xInfo.display, xInfo.window, 0, 0 );
    XSetForeground(xInfo.display, xInfo.gc[i], brown.pixel);


    int depth = DefaultDepth(xInfo.display, DefaultScreen(xInfo.display));
    xInfo.pixmap = XCreatePixmap(xInfo.display, xInfo.window, hints.width, hints.height, depth);
    xInfo.width = hints.width;
    xInfo.height = hints.height;

    XSelectInput(xInfo.display, xInfo.window, 
        ButtonPressMask | KeyPressMask | 
        PointerMotionMask | 
        EnterWindowMask | LeaveWindowMask |
        KeyReleaseMask | StructureNotifyMask); 

    XSetWindowBackgroundPixmap(xInfo.display, xInfo.window, None);
    /*
     * Put the window on the screen.
     */
    XMapRaised( xInfo.display, xInfo.window );

    XFlush(xInfo.display);
    sleep(2);	// let server get set up before sending drawing commands
                // this is a hack until we get to events
}


void show(XInfo &xinfo){
   string score_string; 
   stringstream convert_score; 
   convert_score << "Total score: " << score;
   score_string = convert_score.str();    
   XDrawString( xinfo.display, xinfo.pixmap, xinfo.gc[5], 15, 15, score_string.c_str(), score_string.length() ); 


   string life_string;
   stringstream convert_life;
   convert_life << "Rest Ball: " << life;
   life_string = convert_life.str();
   XDrawString( xinfo.display, xinfo.pixmap, xinfo.gc[5], 15, 30, life_string.c_str(), life_string.length() );
   
   
   string level_string;
   stringstream convert_level;
   convert_level << "Level: " << level;
   level_string = convert_level.str();
   XDrawString( xinfo.display, xinfo.pixmap, xinfo.gc[5], windowWidth-80, 15, level_string.c_str(), level_string.length() );

   if(!start){
      XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[0], 0, 0, xinfo.width, xinfo.height);
      XDrawString( xinfo.display, xinfo.pixmap, xinfo.gc[4], windowWidth/4, windowHeight/4, "Welcome to Breakout", 19); 
      XDrawString( xinfo.display, xinfo.pixmap, xinfo.gc[4], windowWidth/4, windowHeight/4+20, "press 's' to start", 18); 
      XDrawString( xinfo.display, xinfo.pixmap, xinfo.gc[4], windowWidth/4, windowHeight/4+40, "use 'a' to move paddle to left", 30); 
      XDrawString( xinfo.display, xinfo.pixmap, xinfo.gc[4], windowWidth/4, windowHeight/4+60,"use 'd' to move paddle to right", 31); 
      XDrawString( xinfo.display, xinfo.pixmap, xinfo.gc[4], windowWidth/4, windowHeight/4+80,"press 'q' to close", 18);
      XDrawString( xinfo.display, xinfo.pixmap, xinfo.gc[4], windowWidth/4, windowHeight/4+100,"you can use mouse to move paddle", 32);
      XDrawString( xinfo.display, xinfo.pixmap, xinfo.gc[4], windowWidth/4, windowHeight/4+120,"press 'space' to fire the ball", 31);
      XDrawString( xinfo.display, xinfo.pixmap, xinfo.gc[4], windowWidth/4, windowHeight/4+140,"press 'u' to levelup", 20); 
      XDrawString( xinfo.display, xinfo.pixmap, xinfo.gc[4], windowWidth/4, windowHeight/4+160,"press 'r' to restart", 20);
      XDrawString( xinfo.display, xinfo.pixmap, xinfo.gc[4], windowWidth/4, windowHeight/4+180,"hszhan", 6);
      XDrawString( xinfo.display, xinfo.pixmap, xinfo.gc[4], windowWidth/4, windowHeight/4+200,"20457524", 8);
   }
   if(stopAnimation && win){
      XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[0], 0, 0, xinfo.width, xinfo.height);
      XDrawString( xinfo.display, xinfo.pixmap, xinfo.gc[4], windowWidth/2, windowHeight/2, "Game over", 9);
   }
}

void repaint( XInfo &xinfo) {
    list<Displayable *>::const_iterator begin = dList.begin();
    list<Displayable *>::const_iterator end = dList.end();

    //XClearWindow( xinfo.display, xinfo.window );
    
    // get height and width of window (might have changed since last repaint)

    // XWindowAttributes windowInfo;
    // XGetWindowAttributes(xinfo.display, xinfo.window, &windowInfo);
    // unsigned int height = windowInfo.height;
    // unsigned int width = windowInfo.width;

    // big black rectangle to clear background
    XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[0], 0, 0, xinfo.width, xinfo.height);
    if(bricks.empty() || startAnimation){
      win = true;
    }
    /*if(!start){ 
        string text("Welcome to Breakout");
        string lineOne("press 's' to start");
        string lineTow("use 'a' to move paddle to left");
        string lineThree("use 'd' to move paddle to right");
        string lineFour("press space to start moving the ball");
        string lineFive("press 'q' to close");
        XDrawString(xinfo.display, xinfo.gc[4], 10, 50, text.c_str(), text.length()); 
        XDrawString(xinfo.display, xinfo.gc[4], 10, 60, lineOne.c_str(), lineOne.length());
        XDrawString(xinfo.display, xinfo.gc[4], 10, 70, lineTow.c_str(), lineTow.length());
        XDrawString(xinfo.display, xinfo.gc[4], 10, 80, lineThree.c_str(), lineThree.length());
        XDrawString(xinfo.display, xinfo.gc[4], 10, 90, lineFour.c_str(), lineFour.length());
        XDrawString(xinfo.display, xinfo.gc[4], 10, 100,lineFive.c_str(), lineFive.length());
    }else{*/
    // draw display list
       
        while( begin != end ) {
            Displayable *d = *begin;
            d->paint(xinfo);
            begin++;
        }
    //}
    show(xinfo);
    // copy buffer to window
    XCopyArea(xinfo.display, xinfo.pixmap, xinfo.window, xinfo.gc[0], 
        0, 0, xinfo.width, xinfo.height,  // region of pixmap to copy
        0, 0); // position to put top left corner of pixmap in window
    
    XFlush( xinfo.display );
}

void handleKeyPress(XInfo &xinfo, XEvent &event){
    KeySym key;
    char text[BufferSize];
    int i = XLookupString( 
        (XKeyEvent *)&event,    // the keyboard event
        text,                   // buffer when text will be written
        BufferSize,             // size of the text buffer
        &key,                   // workstation-independent key symbol
        NULL );   
    if(i=1){
        if(text[0] == 'd'){
            paddle.move_right(xinfo);
        }else if(text[0] == 'a'){
            paddle.move_left(xinfo);
        }
    } 
}

void handleAnimation(XInfo &xinfo) {
    ball.move(paddle, bricks);
    //paddle.move_left(xinfo);
    //paddle.move_right(xinfo);

}

void handleResize(XInfo &xinfo, XEvent &event) {
    XConfigureEvent xce = event.xconfigure;
    fprintf(stderr, "Handling resize  w=%d  h=%d\n", xce.width, xce.height);
    if (xce.width != xinfo.width || xce.height != xinfo.height) {
        xinfo.width = xce.width;
        xinfo.height = xce.height;
        windowHeight = xce.height;
        windowWidth = xce.width;
        brick_width = windowWidth/20;
        brick_height = windowHeight/30;
        for(list<Brick *>::iterator it = bricks.begin(); it != bricks.end(); ++it) {
             delete *it;
          }

          for (int i = 0; i < 10; i ++) {
            for(int j = 0; j < 5; j ++) {
              Brick * b = new Brick( i* brick_width + windowWidth/4 , j * brick_height + windowHeight/12);
              dList.push_front(b);
              bricks.push_front(b);  
            }
          }
    }
}

void handleMotion(XInfo &xinfo, XEvent &event, int inside) {
    if (inside) {
        paddle.mouse_move(event.xbutton.x, xinfo);    
    }
}

void handle_keyrelease(XEvent &event)
{
   KeySym key;
   char text[BufferSize];
   
    int i = XLookupString( (XKeyEvent *)&event,     // the keyboard event
                          text,                     // buffer when text will be written
                          BufferSize,          // size of the text buffer
                          &key,                     // workstation-independent key symbol
                          NULL );               // pointer to a composeStatus structure (unused)
    if ( i == 1) {
        if (text[0] == ' '){
            ball.setSpeed(ballspeed);
        }
        if(text[0] == 'q'){
            exit(0);
        }
        if(text[0] == 's'){
            start = true;
        }
        if(text[0] == 'u'){
            score += 50;
            OldScore = score;
            ballspeed += 1;
            up = true;
            ball.setSpeed(ballspeed);
        }
        if(text[0] == 'r'){
          stopAnimation = false;
          startAnimation = false;
          win = false;
          level = 1;
          score = 0;
          OldScore = 0;
          life = 3;
          ballspeed = 5;
          for(list<Brick *>::iterator it = bricks.begin(); it != bricks.end(); ++it) {
             delete *it;
          }

          for (int i = 0; i < 10; i ++) {
            for(int j = 0; j < 5; j ++) {
              Brick * b = new Brick( i* brick_width + windowWidth/4 , j * brick_height + windowHeight/12);
              dList.push_front(b);
              bricks.push_front(b);  
            }
          }
        }
      }
      
}


// get microseconds
unsigned long now() {
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

void eventLoop(XInfo &xinfo) {
    // Add stuff to paint to the display list
    dList.push_front(&paddle);
    //dList.push_front(&brick);
    for (int i = 0; i < 10; i ++) {
      for(int j = 0; j < 5; j ++) {
        Brick * b = new Brick( i* brick_width + windowWidth/4 , j * brick_height + windowHeight/12);
        dList.push_front(b);
        bricks.push_front(b);  
      }
      
    }
    
    dList.push_front(&ball);

    //dList.push_front(&right_eye);
    
    XEvent event;
    unsigned long last = 0;
    int inside = 0;

    while( true ) {
        /*
         * This is NOT a performant event loop!  
         * It needs help!
         */
        
        if (XPending(xinfo.display) > 0) {
            XNextEvent( xinfo.display, &event );
            //cout << "event.type=" << event.type << "\n";
            switch( event.type ) {
                /*case ButtonPress:
                    handleButtonPress(xinfo, event);
                    break;*/
                case KeyPress:
                    handleKeyPress(xinfo, event);
                    break;
                case MotionNotify:
                    handleMotion(xinfo, event, inside);
                    break;
                case EnterNotify:
                    inside = 1;
                    break;
                case LeaveNotify:
                    inside = 0;
                    break;
                case KeyRelease:
                    handle_keyrelease(event);
                    break;
                case ConfigureNotify:
                    handleResize(xinfo, event);
                    break;  
            }
        } 
        unsigned long end = now();
      if ( end - last > 1000000/FPS )
      {
         if(!stopAnimation)handleAnimation(xinfo);
         repaint(xinfo);
         last = now();
      } else if ( XPending(xinfo.display) == 0 ) {
         usleep(1000000/FPS - (end - last));
      }
        /*usleep(1000000/FPS);
        handleAnimation(xinfo, inside);
        repaint(xinfo);*/
    }
}
/*
 *   Start executing here.
 *	 First initialize window.
 *	 Next loop responding to events.
 *	 Exit forcing window manager to clean up - cheesy, but easy.
 */
int main ( int argc, char* argv[] ) {
    //cout << argc << endl;
    if(argc == 3){
      
      FPS = atoi(argv[1]);
      ballspeed = atoi(argv[2]);
      
    }
    XInfo xInfo;
    //bricks.push_front(&brick);
    initX(argc, argv, xInfo);
    eventLoop(xInfo);
    XCloseDisplay(xInfo.display);
}
