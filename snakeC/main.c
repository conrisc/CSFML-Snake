#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include <SFML/System.h>
#include <SFML/Window.h>
#include <SFML/Graphics.h>
#include <stdbool.h>
//*********************************************************************************************************************
//Deklaracje struktur:
typedef struct elementListy {
    struct elementListy* next;
    struct elementListy* prev;
    int val;
} element;

typedef struct lista {
    element* last;
    element* first;
    int score,frozen;
    bool up,lose,horizontal;
    sfTime speed; //wieksza wartosc = wolniejszy snake
} list;
//*********************************************************************************************************************
//Spis funkcji:
void mainMenu(); //FUNKCJA GLOWNEGO MENU
void game();    //FUNKCJA OBSLUGUJACA GRE (SNAKE)
void settings();   //FUNKCJA WYBORU POZIOMU TRUDNOSCI GRY
bool isOptionMarked(sfText* text, sfColor on, sfColor off);  //FUNKCJA SPRAWDZAJACA CZY DANA OPCJA Z MENU JEST ZAZNACZONA (CZY JEST NA NIEJ KURSOR)
void showMenu();    //FUNKCJA ANIMACJI POJAWIANIA SIE MENU
void hideMenu();    //FUNKCJA ANIMACJI ZNIKANIA MENU
void addToSnake(int val); //FUNKCJA DODAJACA JEDNA CZESC DO WEZA (JEDEN PROSTOKAT)
void removeFirst(); //FUNKCJA USUWAJACA OGON (OSTATNI PROSTOKAT KTORY ZNIKA)
void clearSnake();  //FUNKCJA CZYSZCZACA LISTE SNAKE
void startSnake();  //FUNKCJA USTAWIAJACA WSZYSTKIE POCZATKOWE WARTOSCI PRZY ROZPOCZECIU GRY
void setApple(); //FUNKCJA USTAWIAJACA JABLKO W ODPOWIEDNIM MIEJSCU
bool isCorPos(); //FUNKCJA SPRAWDZAJACA CZY WYBRANA POZYCJA JABLKA JEST POPRAWNA
bool isEaten(); //FUNKCJA SPRAWDZAJACA CZY JABLKO ZOSTALO ZJEDZONE
bool isConflict(); //FUNKCJA SPRAWDZAJACA CZY WAZ SIE Z SOBA ZDERZYL
bool collisionWall(); //FUNKCJA SPRAWDZAJACA CZY WAZ ZDERZYL SIE ZE SCIANA
void intToString(int a); //FUNKCJA ZAMIENIAJACA INT OD LICZBY UZYSKANYCH PKT. W NAPIS
//*********************************************************************************************************************
//Zmienne globalne:
int menu = 1,posMenuRect = -40;
unsigned int textSize = 45;
const int width = 700,
          height = 500;
bool enter = true,hor;
char stringS[3];
sfUint8 transparent = 0;
sfVideoMode mode = {700, 500, 32};
sfRenderWindow* App;
sfVector2f wektor;
sfVector2i cursor;
sfEvent event,moveEvent[2];
sfText* backButton;
sfText* textString;
sfText* scoreString;
sfText* option1;
sfText* option2;
sfText* option3;
sfColor optColorOff;
sfColor optColorOn;
sfTexture* mTitTexture;
sfSprite* mainTitle;
sfRectangleShape* menuRect;
sfTime sleepTime;
sfRectangleShape* board;
sfRectangleShape* boardLine;
///WEZYKOWE ZMIENNE**********************
list snake;
sfRectangleShape* recSnake;
sfCircleShape* apple;
//*********************************************************************************************************************
int main()  {
    ///**********************************
    App = sfRenderWindow_create( mode, "Snake 2.1 - Konrad Cielecki", sfClose, NULL);
    //CZAS *********************************************
    sleepTime.microseconds = 500;
    srand( time( NULL ) );
    //IKONA APLIKACJI***********************************
    sfImage* icon;
    icon = sfImage_createFromFile ("icon.png");
    sfWindow_setIcon (App, 256, 256, sfImage_getPixelsPtr(icon));
    //TLO***********************************************
    sfTexture* bgTexture;
    sfSprite* background;
    bgTexture = sfTexture_createFromFile("background.jpg", NULL);
    background = sfSprite_create();
    sfSprite_setTexture(background, bgTexture, sfTrue);
    wektor.x = 0;
    wektor.y = 0;
    sfSprite_setPosition(background, wektor);
    wektor.x = (double)width/sfTexture_getSize(bgTexture).x;
    wektor.y = (double)height/sfTexture_getSize(bgTexture).y;
    sfSprite_setScale (background, wektor);
    //WCZYTYWANIE OBRAZKA Z NAPISEM SNAKE****************
    mTitTexture = sfTexture_createFromFile("snake.png",NULL);
    mainTitle = sfSprite_create();
    sfSprite_setTexture(mainTitle, mTitTexture,sfTrue);
    wektor.x = 190;
    wektor.y = 50;
    sfSprite_setPosition(mainTitle, wektor);
    //WCZYTYWANIE CZCIONKI*******************************
    sfFont* customFont;
    customFont = sfFont_createFromFile( "font.otf" );
    //USTAWIANIE NAPISOW W MENU**************************
    option1 = sfText_create();
    sfText_setString(option1, "PLAY");
    sfText_setFont(option1,customFont);
    sfText_setCharacterSize (option1, textSize);
    wektor.x = 50;
    wektor.y = 230;
    sfText_setPosition (option1, wektor);
        //**************
    option2 = sfText_create();
    sfText_setString(option2, "DIFFICULTY");
    sfText_setFont(option2,customFont);
    sfText_setCharacterSize (option2, textSize);
    wektor.x = 50;
    wektor.y = 300;
    sfText_setPosition (option2, wektor);
        //**************
    option3 = sfText_create();
    sfText_setString(option3, "EXIT");
    sfText_setFont(option3,customFont);
    sfText_setCharacterSize (option3, textSize);
    wektor.x = 50;
    wektor.y = 370;
    sfText_setPosition (option3,wektor);
    //USTAWIANIE KOLOROW NAPISOW*************************
    optColorOff = sfColor_fromRGBA(27,27,27,transparent);
    optColorOn = sfColor_fromRGBA(0,235,10,255);
    //USTAWIANIE PROSTOKATA PO LEWEJ(MENU)***************
    menuRect = sfRectangleShape_create();

    sfRectangleShape_setFillColor (menuRect, sfColor_fromRGBA(27,27,27,255));
    wektor.x = posMenuRect;
    wektor.y = 0;
    sfRectangleShape_setPosition (menuRect,wektor);
    wektor.x = 40;
    wektor.y = height;
    sfRectangleShape_setSize (menuRect, wektor);
    //USTAWIANIE PLANSZY(GAME)***************************
    board = sfRectangleShape_create();
    sfRectangleShape_setFillColor (board, sfColor_fromRGBA(27,27,27,255));
    wektor.x = 420;
    wektor.y = 375;
    sfRectangleShape_setSize (board, wektor);
    wektor.x = ((double)width-wektor.x)/2 - 5;
    wektor.y = 105;
    sfRectangleShape_setPosition (board,wektor);
    sfRectangleShape_setOutlineThickness(board,15);
    //USTAWIANIE LINI NA PLANSZY(GAME)*********************
    boardLine = sfRectangleShape_create();
    sfRectangleShape_setFillColor (boardLine, sfColor_fromRGBA(255,255,255,20));
    //USTAWIANIE SNAKE*************************************
    snake.first = NULL;
    snake.last = NULL;
    snake.speed.microseconds = 10000;
    recSnake = sfRectangleShape_create();
    sfRectangleShape_setFillColor (recSnake, optColorOn);
    //USTAWIANIE JABLKA************************************
    apple = sfCircleShape_create();
    sfCircleShape_setFillColor (apple, sfRed);
    sfCircleShape_setRadius (apple, 5);
    //USTAWIANIE TEKSTU WYNIKU*****************************
    scoreString = sfText_create();
    sfText_setFont(scoreString,customFont);
    //NAPIS "SCORE:"*************
    textString = sfText_create();
    sfText_setFont(textString,customFont);
    //BUTTON "BACK"*************
    backButton = sfText_create();
    sfText_setString (backButton, "BACK");
    sfText_setFont(backButton,customFont);
    sfText_setCharacterSize (backButton, textSize);
    wektor.x = 10;
    wektor.y = 10;
    sfText_setPosition (backButton,wektor);
    ///*****APLIKACJA*********************************************************
     while( sfRenderWindow_isOpen(App) ) {
        cursor = sfMouse_getPosition(App);
        sfRenderWindow_clear (App, optColorOff);
        sfRenderWindow_drawSprite(App, background, NULL);
        if (menu == 1) mainMenu();
        else if(menu == 2) game();
        else if(menu == 3) settings();
        sfRenderWindow_display(App);
    }

    return 0;
}
//*********************************************************************************************************************
///FUNKCJE:
void mainMenu() {
    if (enter == true)
        showMenu();
    else {
        while( sfRenderWindow_pollEvent(App, &event) )  {
            if( event.type == sfEvtClosed || (event.type == sfEvtKeyPressed && event.key.code == sfKeyEscape) )
                sfRenderWindow_close(App);
            else  {
                if (isOptionMarked(option1, optColorOn, optColorOff)) {
                    menu = 2;
                    startSnake();
                }
                if (isOptionMarked(option2, optColorOn, optColorOff)) {
                        menu = 3;
                        sfText_setString(option1, "EASY");
                        sfText_setString(option2, "NORMAL");
                        sfText_setString(option3, "HARD");
                }
                if (isOptionMarked(option3, optColorOn, optColorOff)) {
                    sfRenderWindow_close(App);
                }
            }
        }
    }
    ///RYSOWANIE
    sfRenderWindow_drawSprite(App, mainTitle, NULL);
    sfRenderWindow_drawRectangleShape (App, menuRect, NULL);
    sfRenderWindow_drawText(App, option1, NULL);
    sfRenderWindow_drawText(App, option2, NULL);
    sfRenderWindow_drawText(App, option3, NULL);
}
void game() {
    while(sfRenderWindow_pollEvent(App, &event))  {
        if( event.type == sfEvtClosed )
            sfRenderWindow_close(App);
        else if (event.type == sfEvtKeyPressed && event.key.code == sfKeyEscape) {
            menu = 1;
            enter = true;
        }
        else  if (enter==true && snake.lose == false && event.type == sfEvtKeyPressed){
                moveEvent[0] = moveEvent[1];
                moveEvent[1] = event;
        }
    }
    if (enter == false)
        hideMenu();
    else {
        //***********************************************************************************
        ///WYKONYWANIE RUCHU*********************************************************************************
        if (snake.last -> val%15 == 0) {
            if (moveEvent[0].type != sfEvtKeyPressed) {
                moveEvent[0] = moveEvent[1];
                moveEvent[1].type = sfEvtKeyReleased;
            }
            if (moveEvent[0].type == sfEvtKeyPressed) {
                event = moveEvent[0];
                moveEvent[0] = moveEvent[1];
                moveEvent[1].type = sfEvtKeyReleased;
                if (snake.horizontal == true && event.key.code == sfKeyW) {
                    addToSnake(snake.last -> prev -> val);
                    snake.up = false;
                }
                else if (snake.horizontal == true && event.key.code == sfKeyS) {
                    addToSnake(snake.last -> prev -> val);
                    snake.up = true;
                }
                else if (snake.horizontal == false && event.key.code == sfKeyD) {
                    addToSnake(snake.last -> prev -> val);
                    snake.up = true;
                }
                else if (snake.horizontal == false && event.key.code == sfKeyA) {
                    addToSnake(snake.last -> prev -> val);
                    snake.up = false;
                }
            }
        }
        ///DODAWANIE RUCHU***********************************************************************************
        if (snake.lose == false) {
            //RUCH DO PRZODU
            if(snake.up == true) snake.last -> val++;
            else snake.last -> val--;
            //RUCH OSTATNIEGO ELEMENTU
            if (snake.frozen>0) snake.frozen--;
            if (snake.frozen == 0) {
                if (snake.first->val<snake.first->next->next->val) snake.first -> val++;
                else snake.first -> val--;
            }
            //USUWANIE OSTATNIEJ CZESCI WEZA JEZELI MA 0px
            if (snake.first -> val == snake.first -> next -> next -> val) removeFirst();
            //SPRAWDZANIE CZY JABLKO ZOSTALO ZJEDZONE
            if (isEaten()) {
                setApple();
                snake.score++;
                snake.frozen += 15;
                intToString(snake.score);
            }
        }
        if (collisionWall()) snake.lose = true;
        ///RYSOWANIE*****************************************************************************************
        sfRenderWindow_drawRectangleShape (App, board, NULL);
        sfFloatRect bRec =	sfRectangleShape_getGlobalBounds (board);
        //PIONOWE LINIE NA PLANSZY
        wektor.x = 1;
        wektor.y = bRec.height;
        sfRectangleShape_setSize (boardLine, wektor);
        wektor.y = bRec.top;
        for (int i=bRec.left+15;i<bRec.left+bRec.width;i+=15) {
            wektor.x = i;
            sfRectangleShape_setPosition (boardLine,wektor);
            sfRenderWindow_drawRectangleShape (App, boardLine, NULL);
        }
        //POZIOME LINIE NA PLANNSZY
        wektor.x = bRec.width;
        wektor.y = 1;
        sfRectangleShape_setSize (boardLine, wektor);
        wektor.x = bRec.left;
        for (int i=bRec.top+15;i<bRec.top+bRec.height;i+=15) {
            wektor.y = i;
            sfRectangleShape_setPosition (boardLine,wektor);
            sfRenderWindow_drawRectangleShape (App, boardLine, NULL);
        }
        sfRenderWindow_drawCircleShape (App, apple, NULL);
        //RYSOWANIE SNAKE
        hor = snake.horizontal;
        for (element* iter = snake.last ; iter!=snake.first -> next ; iter = iter -> prev) {
            if (hor == true) {
                wektor.x = abs((iter -> val) - (iter -> prev -> prev -> val))+15;
                wektor.y = 15;
                sfRectangleShape_setSize (recSnake, wektor);
                wektor.x = min(iter -> val, iter -> prev -> prev -> val);
                wektor.y = iter -> prev -> val;
                sfRectangleShape_setPosition (recSnake,wektor);
                hor = false;
            }
            else {
                wektor.x = 15;
                wektor.y = abs((iter -> val) - (iter -> prev -> prev -> val))+15;
                sfRectangleShape_setSize (recSnake, wektor);
                wektor.x = iter -> prev -> val;
                wektor.y = min(iter -> val, iter -> prev -> prev -> val);
                sfRectangleShape_setPosition (recSnake,wektor);
                hor = true;
            }
            sfRenderWindow_drawRectangleShape (App, recSnake, NULL);
            if(snake.lose == false && iter != snake.last && isConflict()) snake.lose = true;
        }
        //RYSOWANIE TEKSTU
        if (isOptionMarked(backButton, optColorOn, sfColor_fromRGB(27,27,27))) menu = 1;
        if (snake.lose == true) {
                sfRectangleShape* scoreBoard;
                scoreBoard = sfRectangleShape_create();
                wektor.x = 390;
                wektor.y = 300;
                sfRectangleShape_setSize(scoreBoard,wektor);
                wektor.x = width/2-wektor.x/2 - 5;
                wektor.y = height/2-wektor.y/2+30;
                sfRectangleShape_setPosition(scoreBoard,wektor);
                sfRectangleShape_setFillColor(scoreBoard,sfColor_fromRGBA(255,255,255,150));
                sfRenderWindow_drawRectangleShape(App,scoreBoard,NULL);
                sfText_setString (textString, "YOUR SCORE:");
                sfText_setColor (textString,sfColor_fromRGB(27,27,27));
                sfText_setColor (scoreString,sfColor_fromRGB(27,27,27));
                sfText_setCharacterSize (scoreString, textSize);
                sfText_setCharacterSize (textString, textSize);
                wektor.x = width/2-sfText_getGlobalBounds(textString).width/2;
                wektor.y = sfRectangleShape_getGlobalBounds(scoreBoard).top + sfRectangleShape_getGlobalBounds(scoreBoard).height/2 - 50;
                sfText_setPosition (textString,wektor);
                wektor.x = width/2-sfText_getGlobalBounds(scoreString).width/2;
                wektor.y = wektor.y+textSize;
                sfText_setPosition (scoreString,wektor);
        }
        sfRenderWindow_drawText(App, backButton, NULL);
        sfRenderWindow_drawText(App, textString, NULL);
        sfRenderWindow_drawText(App, scoreString, NULL);
        ///SPOWALNIANIE PETLI
        sfSleep(snake.speed);
    }
}
void settings() {
    while(sfRenderWindow_pollEvent(App, &event))  {
        if( event.type == sfEvtClosed )
            sfRenderWindow_close(App);
        else if (event.type == sfEvtKeyPressed && event.key.code == sfKeyEscape)
            menu = 1;
        else  {
            if (isOptionMarked(option1, optColorOn, optColorOff)) {
                menu = 1;
                snake.speed.microseconds = 16000;
            }
            if (isOptionMarked(option2, optColorOn, optColorOff)) {
                menu = 1;
                snake.speed.microseconds = 10000;
            }
            if (isOptionMarked(option3, optColorOn, optColorOff)) {
                menu = 1;
                snake.speed.microseconds = 5000;
            }
        }
    }
    sfRenderWindow_drawSprite(App, mainTitle, NULL);
    sfRenderWindow_drawRectangleShape (App, menuRect, NULL);
    sfRenderWindow_drawText(App, option1, NULL);
    sfRenderWindow_drawText(App, option2, NULL);
    sfRenderWindow_drawText(App, option3, NULL);
    if (menu == 1) {
        sfText_setString(option1, "PLAY");
        sfText_setString(option2, "DIFFICULTY");
        sfText_setString(option3, "EXIT");
    }
}
bool isOptionMarked(sfText* text, sfColor on, sfColor off) {
    sfFloatRect textRectangle = sfText_getGlobalBounds(text);
    bool isItMarked = isItMarked = ( cursor.x>=textRectangle.left && cursor.x<=textRectangle.left + textRectangle.width && cursor.y>=textRectangle.top && cursor.y<=textRectangle.top + textRectangle.height);
    sfText_setColor (text,  isItMarked ? on :  off );
    if (isItMarked && event.type == sfEvtMouseButtonPressed && event.mouseButton.button == sfMouseLeft) {
        return 1;
    }
    return 0;
}
void showMenu() {
    transparent++;
    while( sfRenderWindow_pollEvent(App, &event) )  {
            if( event.type == sfEvtClosed || (event.type == sfEvtKeyPressed && event.key.code == sfKeyEscape) )
                sfRenderWindow_close(App);
    }
    sfSleep(sleepTime);
    if (transparent%7 == 0) posMenuRect++;
    wektor.x = posMenuRect;
    wektor.y = 0;
    sfRectangleShape_setPosition (menuRect,wektor);

    optColorOff = sfColor_fromRGBA(27,27,27,transparent);
    sfText_setColor (option1, optColorOff );
    sfText_setColor (option2, optColorOff );
    sfText_setColor (option3, optColorOff );
    if(transparent == 255)
        enter = false;
}
void hideMenu() {
    sfSleep(sleepTime);
    if (transparent%7 == 0) posMenuRect--;
    wektor.x = posMenuRect;
    wektor.y = 0;
    sfRectangleShape_setPosition (menuRect,wektor);
    optColorOff = sfColor_fromRGBA(27,27,27,transparent);
    sfText_setColor (option1, optColorOff );
    sfText_setColor (option2, optColorOff );
    sfText_setColor (option3, optColorOff );
    transparent--;
    sfRenderWindow_drawSprite(App, mainTitle, NULL);
    sfRenderWindow_drawRectangleShape (App, menuRect, NULL);
    sfRenderWindow_drawText(App, option1, NULL);
    sfRenderWindow_drawText(App, option2, NULL);
    sfRenderWindow_drawText(App, option3, NULL);
    if(transparent == 0)
        enter = true;
}
void addToSnake(int val) {
    snake.last -> next = (element*)malloc(sizeof(element));
    snake.last -> next -> next = NULL;
    snake.last -> next -> prev = snake.last;
    snake.last = snake.last -> next;
    snake.last -> val = val;
    if (snake.horizontal == true) snake.horizontal = false;
    else snake.horizontal = true;
}
void removeFirst() {
    snake.first = snake.first -> next;
    free(snake.first -> prev);
    snake.first -> prev = NULL;
}
void clearSnake() {
    if (snake.last!=NULL) {
        while (snake.last!=snake.first) {
            snake.last = snake.last -> prev;
            free(snake.last -> next);
        }
        free(snake.last);
        snake.first = NULL;
        snake.last = NULL;
    }
}
void startSnake() {
    clearSnake();
    snake.horizontal = true;
    snake.up = true;
    snake.frozen = 0;
    snake.lose = false;
    element* pom = (element*)malloc(sizeof(element));
    snake.first = (element*)malloc(sizeof(element));
    snake.last = (element*)malloc(sizeof(element));
    snake.first -> prev = NULL;
    snake.first -> next = pom;
    pom -> prev = snake.first;
    pom -> next = snake.last;
    snake.last -> prev = pom;
    snake.last -> next = NULL;
    snake.first -> val = 195 + sfRectangleShape_getGlobalBounds(board).left;
    pom -> val = 195 + sfRectangleShape_getGlobalBounds(board).top;
    snake.last -> val = 240 + sfRectangleShape_getGlobalBounds(board).left;
    snake.score = 0;
    intToString(snake.score);
    setApple();
    moveEvent[0].type = sfEvtKeyReleased;
    moveEvent[1].type = sfEvtKeyReleased;
    //USTAWIANIE NAPISOW WYNIKU*****************************************
    sfText_setColor (textString,sfColor_fromRGB(255,255,255));
    sfText_setColor (scoreString,sfColor_fromRGB(255,255,255));
    sfText_setString (textString, "SCORE:");
    sfText_setCharacterSize (scoreString, textSize-10);
    sfText_setCharacterSize (textString, textSize-10);
    wektor.x = width/2-sfText_getGlobalBounds(textString).width/2;
    wektor.y = 10;
    sfText_setPosition (textString,wektor);
    wektor.x = width/2-sfText_getGlobalBounds(scoreString).width/2;
    wektor.y = textSize;
    sfText_setPosition (scoreString,wektor);
}
void setApple() {
    do  {
        int random = 15*(rand()%(int)((sfRectangleShape_getGlobalBounds(board).width - 2*sfRectangleShape_getOutlineThickness(board))/15));
        wektor.x = 3+sfRectangleShape_getGlobalBounds(board).left + sfRectangleShape_getOutlineThickness(board)+random;
        random = 15*(rand()%(int)((sfRectangleShape_getGlobalBounds(board).height - 2*sfRectangleShape_getOutlineThickness(board))/15));
        wektor.y = 3+sfRectangleShape_getGlobalBounds(board).top + sfRectangleShape_getOutlineThickness(board)+random;
        sfCircleShape_setPosition (apple, wektor);
    } while (!isCorPos());
}
bool isCorPos() {
        sfFloatRect pieceOfS;
        sfFloatRect applePos = sfCircleShape_getGlobalBounds(apple);
        hor = snake.horizontal;
        for (element* iter = snake.last ; iter!=snake.first -> next ; iter = iter -> prev) {
            if (hor == true) {
                pieceOfS.width = abs((iter -> val) - (iter -> prev -> prev -> val))+15;
                pieceOfS.height = 15;
                pieceOfS.left = min(iter -> val, iter -> prev -> prev -> val);
                pieceOfS.top = iter -> prev -> val;
                hor = false;
            }
            else {
                pieceOfS.width = 15;
                pieceOfS.height = abs((iter -> val) - (iter -> prev -> prev -> val))+15;
                pieceOfS.left = iter -> prev -> val;
                pieceOfS.top = min(iter -> val, iter -> prev -> prev -> val);
                hor = true;
            }
            if ( (pieceOfS.top<=applePos.top && applePos.top<=pieceOfS.top+pieceOfS.height && pieceOfS.left<=applePos.left && applePos.left<=pieceOfS.left+pieceOfS.width) ||
                 (pieceOfS.top<=applePos.top && applePos.top<=pieceOfS.top+pieceOfS.height && pieceOfS.left<=applePos.left+applePos.width && applePos.left+applePos.width<=pieceOfS.left+pieceOfS.width) ||
                 (pieceOfS.top<=applePos.top+applePos.height && applePos.top+applePos.height<=pieceOfS.top+pieceOfS.height && pieceOfS.left<=applePos.left && applePos.left<=pieceOfS.left+pieceOfS.width) ||
                 (pieceOfS.top<=applePos.top+applePos.height && applePos.top+applePos.height<=pieceOfS.top+pieceOfS.height && pieceOfS.left<=applePos.left+applePos.width && applePos.left+applePos.width<=pieceOfS.left+pieceOfS.width)) return 0;
        }

        return 1;
}
bool isEaten() {
    sfFloatRect pieceOfS;
    sfFloatRect applePos = sfCircleShape_getGlobalBounds(apple);
    element* iter = snake.last;
    if (snake.horizontal == true) {
        pieceOfS.width = abs((iter -> val) - (iter -> prev -> prev -> val))+15;
        pieceOfS.height = 15;
        pieceOfS.left = min(iter -> val, iter -> prev -> prev -> val);
        pieceOfS.top = iter -> prev -> val;
    }
    else {
        pieceOfS.width = 15;
        pieceOfS.height = abs((iter -> val) - (iter -> prev -> prev -> val))+15;
        pieceOfS.left = iter -> prev -> val;
        pieceOfS.top = min(iter -> val, iter -> prev -> prev -> val);
    }
    if ( (pieceOfS.top<=applePos.top && applePos.top<=pieceOfS.top+pieceOfS.height && pieceOfS.left<=applePos.left && applePos.left<=pieceOfS.left+pieceOfS.width) ||
         (pieceOfS.top<=applePos.top && applePos.top<=pieceOfS.top+pieceOfS.height && pieceOfS.left<=applePos.left+applePos.width && applePos.left+applePos.width<=pieceOfS.left+pieceOfS.width) ||
         (pieceOfS.top<=applePos.top+applePos.height && applePos.top+applePos.height<=pieceOfS.top+pieceOfS.height && pieceOfS.left<=applePos.left && applePos.left<=pieceOfS.left+pieceOfS.width) ||
         (pieceOfS.top<=applePos.top+applePos.height && applePos.top+applePos.height<=pieceOfS.top+pieceOfS.height && pieceOfS.left<=applePos.left+applePos.width && applePos.left+applePos.width<=pieceOfS.left+pieceOfS.width))
         return 1;
    else return 0;
}
bool isConflict() {
    sfFloatRect firstPiece; //POCZATEK WEZA
    sfFloatRect otherPiece = sfRectangleShape_getGlobalBounds(recSnake); //KTORYS Z ELEMENTOW WEZA
    element* iter = snake.last;
    if (snake.horizontal == true) {
        firstPiece.width = abs((iter -> val) - (iter -> prev -> prev -> val))+15;
        firstPiece.height = 15;
        firstPiece.left = min(iter -> val, iter -> prev -> prev -> val);
        firstPiece.top = iter -> prev -> val;
        if (snake.up == true) firstPiece.left += 15;
        firstPiece.width -= 15;
    }
    else {
        firstPiece.width = 15;
        firstPiece.height = abs((iter -> val) - (iter -> prev -> prev -> val))+15;
        firstPiece.left = iter -> prev -> val;
        firstPiece.top = min(iter -> val, iter -> prev -> prev -> val);
        if (snake.up == true) firstPiece.top += 15;
        firstPiece.height -= 15;
    }
    if ( (otherPiece.top<firstPiece.top && firstPiece.top<otherPiece.top+otherPiece.height && otherPiece.left<firstPiece.left && firstPiece.left<otherPiece.left+otherPiece.width) ||
         (otherPiece.top<firstPiece.top && firstPiece.top<otherPiece.top+otherPiece.height && otherPiece.left<firstPiece.left+firstPiece.width && firstPiece.left+firstPiece.width<otherPiece.left+otherPiece.width) ||
         (otherPiece.top<firstPiece.top+firstPiece.height && firstPiece.top+firstPiece.height<otherPiece.top+otherPiece.height && otherPiece.left<firstPiece.left && firstPiece.left<otherPiece.left+otherPiece.width) ||
         (otherPiece.top<firstPiece.top+firstPiece.height && firstPiece.top+firstPiece.height<otherPiece.top+otherPiece.height && otherPiece.left<firstPiece.left+firstPiece.width && firstPiece.left+firstPiece.width<otherPiece.left+otherPiece.width) )
		   return 1;
    return 0;
}
bool collisionWall() {
    sfFloatRect firstPiece; //POCZATEK WEZA
    sfFloatRect boardPos = sfRectangleShape_getGlobalBounds(board); //POZYCJA PLANSZY
    float thick = sfRectangleShape_getOutlineThickness(board);  //GRUBOSC RAMKI PLANSZY

    element* iter = snake.last;
    if (snake.horizontal == true) {
        firstPiece.width = abs((iter -> val) - (iter -> prev -> prev -> val))+15;
        firstPiece.height = 15;
        firstPiece.left = min(iter -> val, iter -> prev -> prev -> val);
        firstPiece.top = iter -> prev -> val;

    }
    else {
        firstPiece.width = 15;
        firstPiece.height = abs((iter -> val) - (iter -> prev -> prev -> val))+15;
        firstPiece.left = iter -> prev -> val;
        firstPiece.top = min(iter -> val, iter -> prev -> prev -> val);
    }
    if ((firstPiece.left<boardPos.left+thick) ||
        (firstPiece.top<boardPos.top+thick) ||
        (firstPiece.left+firstPiece.width>boardPos.left+boardPos.width-thick) ||
        (firstPiece.top+firstPiece.height>boardPos.top+boardPos.height-thick) )
        return 1;
    return 0;
}
void intToString(int a) {
    for (int i=2;i>=0;i--) {
        stringS[i] = (char)((a%10)+48);
        a /= 10;
    }
    sfText_setString (scoreString, stringS);
}
