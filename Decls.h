#ifndef DECLS_H
#define DECLS_H

// Args.h
void usage(void);
uint parseUint(char *, const uint);
bool sameStrLower(char *, char *);
ArgType parseArgType(char *);
Length parseLen(char *);
uint parseBombs(char *);
BoardType parseBoardType(char *);
Arg parseArg(char *);
Board boardArgs(int, char **);

// Board.h
Board boardDefault(void);
void boardFree(Board *);
void boardAlloc(Board *);
bool validTilePos(const Coord, const Length);
uint adjBombs(const Board *, const Coord);
uint adjTileState(const Board *, const Coord, const TileState);
uint floodFill(Board *, const Coord);
void boardCalcNums(Board *);
void boardResetFirstClick(Board *);
uint boardRemaining(Board *);
void boardRngBombs(Board *, const Coord);
bool boardPlaceBombs(Board *);

// Draw.h
uint tileScale(const Length, const Length);
Offset tileOffset(const Length, const Length, const uint);
void drawNum(const Coord, const uint, const uint);
void drawUnrevealedTile(const Coord, const uint);
void drawFlag(const Coord, const uint);
void drawQuest(const Coord);
void drawBoard(Board *);

// Print.h
void printBoardInfo(const Board *);
void printBoard(const Board *, const bool);
void printDecalsPos(const Board *, const Coord);
uint printCleared(const uint);
void printTile(const Tile);

// Solve.h
bool clear121(Board *, const Coord);
uint flagAdj(Board *, const Coord);
uint setAdj(Board *, const Coord, const TileState);
uint clearAdj(Board *, const Coord);
Board* resetQuests(Board *);
bool isAbsurd(Board *);
bool reductioAdAbsurdum(Board *);
bool satSolve(Board *);
bool solve(Board *);

// Update.h
Coord winTilePos(const Coord, const int, const Offset);

// Save.h
Length boardFileLength(File *);
Tile tileUnpack(char);
char tilePack(const Tile);
File* saveNewFile(Board *);
bool boardSave(Board *, const bool);
Board boardLoad(const uint);

#endif /* end of include guard: DECLS_H */
