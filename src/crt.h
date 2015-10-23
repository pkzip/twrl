#ifndef _CRT_H_
#define _CRT_H_

enum ConsoleColor
{
	Black        = 0,
	Blue         = 1,
	Green        = 2,
	Cyan         = 3,
	Red          = 4,
	Magenta      = 5,
	Brown        = 6,
	LightGray    = 7,
	DarkGray     = 8,
	LightBlue    = 9,
	LightGreen   = 10,
	LightCyan    = 11,
	LightRed     = 12,
	LightMagenta = 13,
	Yellow       = 14,
	White        = 15
};

void ClrScr();
void CrtInit();
void CrtClose();
void Delay(const int ms);
void GotoXY(const int x, const int y);
void TextColor(const ConsoleColor col);
void TextBackground(const ConsoleColor col);
char ReadKey();
void Backspace();

#endif
