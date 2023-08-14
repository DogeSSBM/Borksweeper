#ifndef SAVE_H
#define SAVE_H

Length boardFileLength(File *file)
{
    assertExpr(file);
    Length len = {0};
    int c;
    while((c = fgetc(file)) != EOF && c != '\n')
        len.x++;
    printf("x: %i\n", len.x);
    rewind(file);

    while((c = fgetc(file)) != EOF){
        len.y += c == '\n';
    }

    rewind(file);
    printf("y: %i\n", len.y);

    for(int y = 0; y < len.y; y++){
        for(int x = 0; x < len.x; x++){
            if((c = fgetc(file))=='\n'){
                fclose(file);
                panic("unexpected '\\n'. char pos: %i,%i", x, len.y);
            }else if(c == EOF){
                fclose(file);
                panic("unexpected EOF. char pos: %i,%i", x, len.y);
            }
        }
        if((c = fgetc(file)) != '\n'){
            fclose(file);
            panic("expected '\\n' at char pos: %i,%i", len.x, y);
        }
    }
    if((c = fgetc(file)) != EOF){
        fclose(file);
        panic("Expected EOF at pos: %i,%i", len.x, len.y);
    }

    rewind(file);
    printf("Board file len: %i,%i\n", len.x, len.y);
    return len;
}

Tile tileUnpack(char c)
{
    Tile tile = {0};
    tile.isBomb = 1 & (c >> 7);
    tile.num = 15 & (c >> 3);
    tile.state = 7 & c;
    return tile;
}

char tilePack(const Tile tile)
{
    return tile.state | (tile.num << 3) | (tile.isBomb << 7);
}

File* saveNewFile(Board *board)
{
    for(uint i = 0; i < 1000; i++){
        char path[64] = {0};
        sprintf(path, "./Saves/%u.swoop", i);
        File *file = fopen(path, "r");
        if(!file){
            printf("Saving game to: \"%s\"\n", path);
            File *file = fopen(path, "wb");
            assertExpr(file);
            board->saveNum = i;
            return file;
        }
        fclose(file);
    }
    return NULL;
}

File* boardOpen(const uint saveNum, char *mode)
{
    char buf[64] = {0};
    sprintf(buf, "./Saves/%u.swoop", saveNum);
    return fopen(buf, mode);
}

bool boardSave(Board *board, const bool newFile)
{
    File *file = NULL;
    if(!newFile)
        file = boardOpen(board->saveNum, "wb");
    if(newFile || !file)
        file = saveNewFile(board);
    for(int y = 0; y < board->len.y; y++){
        for(int x = 0; x < board->len.x; x++)
            fputc(tilePack(board->tile[x][y]), file);
        if(fputc('\n', file) == EOF){
            fclose(file);
            return false;
        }
    }
    fclose(file);
    return true;
}

Board boardLoad(const uint saveNum)
{
    File *file = boardOpen(saveNum, "rb");
    assertExprMsg(file, "Could not open board: \"%u\"", saveNum);
    Board board = {
        .saveNum = saveNum,
        .winLen = getWindowLen(),
        .len = boardFileLength(file),
        .scale = tileScale(board.winLen, board.len),
        .off = tileOffset(board.winLen, board.len, board.scale),
        .state = BS_PLAY,
        .tile = boardTileAlloc(board.len),
    };
    int c;
    for(int y = 0; y < board.len.y; y++){
        for(int x = 0; x < board.len.x; x++){
            if((c = fgetc(file))=='\n'){
                fclose(file);
                panic("unexpected '\\n'. char pos: %i,%i", x, y);
                printf("Board file len: %i,%i\n", board.len.x, board.len.y);
            }else if(c == EOF){
                fclose(file);
                panic("unexpected EOF. char pos: %i,%i", x, y);
                printf("Board file len: %i,%i\n", board.len.x, board.len.y);
            }
            board.tile[x][y] = tileUnpack(c);
        }
        if((c = fgetc(file)) != '\n'){
            fclose(file);
            panic("Expected newline at end of line %i\n", y);
        }
    }
    if((c = fgetc(file)) != EOF){
        fclose(file);
        panic("Expected EOF at end of line %i\n", board.len.y);
    }

    printBoardInfo(&board);
    printBoard(&board, true);
    printBoard(&board, false);
    return board;
}

#endif /* end of include guard: SAVE_H */
