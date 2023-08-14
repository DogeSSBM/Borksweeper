#ifndef PRINT_H
#define PRINT_H

uint boardCountBombs(const Board *board)
{
    assertExpr(board);
    uint total = 0;
    for(int y = 0; y < board->len.y; y++)
        for(int x = 0; x < board->len.x; x++)
            total += board->tile[x][y].isBomb;
    return total;
}

void printBoardInfo(const Board *board)
{
    assertExpr(board);
    printf("Board -\n");
    printf("saveNum: %u\n", board->saveNum);
    printf("len: (%03i,%03i)\n", board->len.x, board->len.y);
    printf("numBombs: %u\n", boardCountBombs(board));
}

void printBoard(const Board *board, const bool decals)
{
    assertExpr(board->tile);
    for(int y = 0; y < board->len.y; y++){
        for(int x = 0; x < board->len.x; x++){
            if(decals){
                if(board->tile[x][y].state == S_NUM)
                    putchar(board->tile[x][y].num ? '0' + board->tile[x][y].num : ' ');
                else
                    putchar(TileStateChar[board->tile[x][y].state]);
            }else{
                if(board->tile[x][y].isBomb)
                    putchar('B');
                else
                    putchar(board->tile[x][y].num ? '0' + board->tile[x][y].num : ' ');
            }
            putchar(' ');
        }
        putchar('\n');
    }
    putchar('\n');
}

void printDecalsPos(const Board *board, const Coord pos)
{
    assertExpr(board->tile);
    for(int y = 0; y < board->len.y; y++){
        for(int x = 0; x < board->len.x; x++){
            if(x == pos.x && y == pos.y)
                putchar('X');
            else if(board->tile[x][y].state == S_NUM)
                putchar(board->tile[x][y].num ? '0'+board->tile[x][y].num : ' ');
            else
                putchar(TileStateChar[board->tile[x][y].state]);
            putchar(' ');
        }
        putchar('\n');
    }
}

uint printCleared(const uint cleared)
{
    if(cleared)
        printf("Cleared %u tiles.\n", cleared);
    return cleared;
}

void printTile(const Tile tile)
{
    printf("(Tile){\n");
    printf("\t.isBomb = %s,\n", tile.isBomb ? "true" : "false");
    printf("\t.num = %u,\n", tile.num);
    printf("\t.state = %s\n", TileStateStr[tile.state]);
    printf("}\n");
}

#endif /* end of include guard: PRINT_H */
