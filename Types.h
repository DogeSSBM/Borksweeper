#ifndef TYPES_H
#define TYPES_H

typedef enum{
    S_TILE,
    S_NUM,
    S_FLAG,
    S_QEST,
    S_QEST_SAFE,
    S_N
}TileState;
const char *TileStateStr[S_N] = {
    "TILE",
    "NUM",
    "FLAG",
    "QEST",
    "S_QEST_SAFE",
};
const char TileStateChar[S_N] = {
    '#',
    ' ',
    '>',
    '?',
    'S'
};
typedef struct{
    bool isBomb;
    uint num;
    TileState state;
}Tile;

typedef enum{
    B_RNG,
    B_ADJ,
    B_SAT,
    B_N
}BoardType;
const char *BoardTypeStr[B_N] = {
    "B_RNG",
    "B_ADJ",
    "B_SAT"
};
typedef enum{
    BS_MENU,
    BS_FIRST,
    BS_PLACE,
    BS_PLAY,
    BS_END,
    BS_N
}BoardState;
const char *BoardStateStr[BS_N] = {
    "BS_MENU",
    "BS_FIRST",
    "BS_PLACE",
    "BS_PLAY",
    "BS_END"
};

typedef enum{
    A_LEN,
    A_MIN,
    A_TYP,
    A_CHEAT,
    A_ERR
}ArgType;
typedef struct Arg{
    ArgType type;
    union{
        Length len;
        uint min;
        BoardType typ;
        bool cheat;
    };
}Arg;

typedef struct{
    uint showUntil;
    Coord pos;
    TileState action;
}Hint;

typedef struct{
    Hint hint;
    uint saveNum;
    bool rescale;
    Length winLen;
    Length len;
    uint scale;
    Offset off;
    uint numBombs;
    bool cheat;
    uint lastClickTick;
    Coord lDown;
    Coord rDown;
    BoardState state;
    BoardType type;
    Tile **tile;
}Board;

typedef enum{C_NONE, C_LEFT, C_DOUB, C_RIGHT}ClickType;

typedef struct{
    Board board;
    Coord tpos;
    int index;
}ThreadData;

#endif /* end of include guard: TYPES_H */
