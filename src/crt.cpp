#ifdef _WIN32
#include <windows.h>
#include <iosfwd>
#include <conio.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#endif

#include "crt.h"

#ifdef _WIN32
HANDLE stdConOut;
#endif
ConsoleColor textColor;
ConsoleColor backgroundColor;
ConsoleColor origTextColor;
ConsoleColor origBackgroundColor;

#ifdef _WIN32

static void GetCurrentColors()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(stdConOut, &csbi);
    textColor = ConsoleColor(csbi.wAttributes & 15);
    backgroundColor = ConsoleColor((csbi.wAttributes & 0xf0)>>4);
}

static void SetCurrentColors()
{
    unsigned short wAttributes=((unsigned int)backgroundColor<<4) | (unsigned int)textColor;
    SetConsoleTextAttribute(stdConOut, wAttributes);
}

void ClrScr()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(stdConOut, &csbi);
    const DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    COORD homeCoords = { 0, 0 };
    DWORD count;
    FillConsoleOutputCharacter(stdConOut, (TCHAR) ' ', cellCount, homeCoords, &count);
    FillConsoleOutputAttribute(stdConOut, csbi.wAttributes, cellCount, homeCoords, &count);
    SetConsoleCursorPosition(stdConOut, homeCoords);
}

void CrtInit()
{
    stdConOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetCurrentColors();
    origTextColor = textColor;
    origBackgroundColor = backgroundColor;
}

void CrtClose()
{
    TextColor(origTextColor);
    TextBackground(origBackgroundColor);
}

void Delay(const int ms)
{
    Sleep(ms);
}

void GotoXY(const int x, const int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(stdConOut, coord);
}

char ReadKey()
{
    return _getch();
}

#else  // not _WIN32 ----------------------------------------------------------

static void SetCurrentColors()
{
    int fg;
    int bg;
    bool hi = false;
    switch(textColor) {
        case Black: fg = 30; break;
        case Blue: fg = 34; break;
        case Green: fg = 32; break;
        case Cyan: fg = 36; break;
        case Red: fg = 31; break;
        case Magenta: fg = 35; break;
        case Brown: fg = 33; break;
        case LightGray: fg = 37; break;
        case DarkGray: fg = 30; hi = true; break;
        case LightBlue: fg = 34; hi = true; break;
        case LightGreen: fg = 32; hi = true; break;
        case LightCyan: fg = 36; hi = true; break;
        case LightRed: fg = 31; hi = true; break;
        case LightMagenta: fg = 35; hi = true; break;
        case Yellow: fg = 33; hi = true; break;
        case White: fg = 37; hi = true; break;
    }
    switch(backgroundColor) {
        case Black: bg = 40; break;
        case Blue: bg = 44; break;
        case Green: bg = 42; break;
        case Cyan: bg = 46; break;
        case Red: bg = 41; break;
        case Magenta: bg = 45; break;
        case Brown: bg = 43; break;
        case LightGray: bg = 47; break;
    }
    if (hi) {
        printf("\033[%d;1;%dm", fg, bg);
    } else {
        printf("\033[0;%d;%dm", fg, bg);
    }
}

void ClrScr()
{
    printf("\033[2J\033[1;1H");
}

void CrtInit()
{
    textColor = LightGray;
    backgroundColor = Black;
    SetCurrentColors();
}

void CrtClose()
{
}

void Delay(const int ms)
{
    usleep(ms*1000);
}

void GotoXY(const int x, const int y)
{
    printf("\033[%d;%dH", y, x);
}

char ReadKey()
{
    struct termios orig_term_attr;
    struct termios new_term_attr;

    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);
    char ch = fgetc(stdin);
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);
    return ch;
}

#endif

void TextColor(const ConsoleColor col)
{
    textColor = col;
    SetCurrentColors();
}

void TextBackground(const ConsoleColor col)
{
    backgroundColor = col;
    SetCurrentColors();
}

void Backspace()
{
    char buf[4] = { 8, 32, 8, 0 };
    printf(buf);
}
