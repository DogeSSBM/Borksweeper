#ifndef BOARD_H
#define BOARD_H

Board boardDefault(void)
{
    Board board = {
        .winLen = getWindowLen(),
        .len = iC(30, 16),
        .scale = tileScale(board.winLen, board.len),
        .off = tileOffset(board.winLen, board.len, board.scale),
        .numBombs = 99,
        .lDown = iC(-1,-1),
        .rDown = iC(-1,-1),
        .state = BS_FIRST,
        .type = B_SAT
    };
    return board;
}

void boardCpy(Board *dst, Board *src)
{
    assertExpr(dst->tile != NULL && src->tile != NULL);
    if(!coordSame(dst->len, src->len))
        printf("dst->len: (%2i,%2i) src->len: (%2i,%2i)\n", dst->len.x, dst->len.y, src->len.x, src->len.y);
    assertExpr(coordSame(dst->len, src->len));
    for(int x = 0; x < dst->len.x; x++){
        for(int y = 0; y < dst->len.y; y++){
            dst->tile[x][y] = src->tile[x][y];
        }
    }
}

void boardFree(Board *board)
{
    assertExpr(board->tile != NULL);
    printf("Freeing board: {%i,%i}\n", board->len.x, board->len.y);
    for(int x = 0; x < board->len.x; x++)
        free(board->tile[x]);
    free(board->tile);

    board->tile = NULL;
    printf("Free'd board: {%i,%i}\n", board->len.x, board->len.y);
}

Tile** boardTileAlloc(const Length len)
{
    assertExprMsg(coordMin(len) >= 4, "Cannot boardAlloc anything < {4,4}, len: {%i,%i}", len.x, len.y);
    Tile **tile = calloc(len.x, sizeof(Tile*));
    for(int x = 0; x < len.x; x++)
        tile[x] = calloc(len.y, sizeof(Tile));
    return tile;
}

void boardAlloc(Board *board)
{
    assertExprMsg(!board->tile, "Tried to boardAlloc when board->tile was not NULL");
    printf("allocating: (%2i,%2i)\n", board->len.x, board->len.y);
    board->tile = boardTileAlloc(board->len);
}

bool validTilePos(const Coord pos, const Length bounds)
{
    return inBound(pos.x, 0, bounds.x) && inBound(pos.y, 0, bounds.y);
}

uint adjBombs(const Board *board, const Coord pos)
{
    uint count = 0;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = {.x = pos.x+xo, .y = pos.y+yo};
            count += validTilePos(adj, board->len) && !coordSame(pos, adj) && board->tile[adj.x][adj.y].isBomb;
        }
    }
    return count;
}

uint adjTileState(const Board *board, const Coord pos, const TileState state)
{
    uint count = 0;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = {.x = pos.x+xo, .y = pos.y+yo};
            count += (
                !coordSame(pos, adj) &&
                validTilePos(adj, board->len) &&
                board->tile[adj.x][adj.y].state == state
            );
        }
    }
    return count;
}

uint floodFill(Board *board, const Coord pos)
{
    if(!validTilePos(pos, board->len) || board->tile[pos.x][pos.y].state != S_TILE)
        return 0;
    if(board->tile[pos.x][pos.y].isBomb){
        printBoard(board, false);
        printf("\n");
        printBoard(board, true);
        printf("\n");
        printDecalsPos(board, pos);
        panic("tried to prop bomb at %i,%i\n", pos.x, pos.y);
    }
    board->tile[pos.x][pos.y].state = S_NUM;
    if(board->tile[pos.x][pos.y].num > 0)
        return 1;

    uint cleared = 1;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = {.x = pos.x+xo, .y = pos.y+yo};
            if(
                !coordSame(pos, adj) &&
                validTilePos(adj, board->len) &&
                board->tile[adj.x][adj.y].state == S_TILE
            )
                cleared += floodFill(board, adj);
        }
    }

    return cleared;
}

uint floodAdj(Board *board, const Coord pos)
{
    uint count = 0;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = {.x = pos.x+xo, .y = pos.y+yo};
            if(
                !coordSame(pos, adj) &&
                validTilePos(adj, board->len) &&
                board->tile[adj.x][adj.y].state == S_TILE
            ){
                if(board->tile[adj.x][adj.y].isBomb){
                    board->state = BS_MENU;
                    return 0;
                }
                count += floodFill(board, adj);
            }
        }
    }
    return count;
}

void boardCalcNums(Board *board)
{
    for(int y = 0; y < board->len.y; y++)
        for(int x = 0; x < board->len.x; x++)
            board->tile[x][y].num = adjBombs(board, iC(x,y));
}

void old_boardResetFirstClick(Board *board)
{
    assertExprMsg(board->tile, "Must alloc board->tile before resetting to first click state.");
    for(int y = 0; y < board->len.y; y++){
        for(int x = 0; x < board->len.x; x++){
            board->tile[x][y].state = S_TILE;
            board->tile[x][y].isBomb = false;
            board->tile[x][y].num = 0;
        }
    }
    board->state = BS_FIRST;
}

void boardResetFirstClick(Board *board)
{
    assertExprMsg(board->tile, "Must alloc board->tile before resetting to first click state.");
    for(int x = 0; x < board->len.x; x++)
        memset(board->tile[x], 0, board->len.y * sizeof(Tile));
    board->state = BS_FIRST;
}

void boardResetTileStates(Board *board)
{
    assertExprMsg(board->tile, "Must alloc board->tile before resetting tiles.");
    for(int y = 0; y < board->len.y; y++)
        for(int x = 0; x < board->len.x; x++)
            board->tile[x][y].state = S_TILE;
}

uint boardRemaining(Board *board)
{
    uint total = 0;
    for(int y = 0; y < board->len.y; y++)
        for(int x = 0; x < board->len.x; x++)
            total += board->tile[x][y].state != S_NUM;
    return total - board->numBombs;
}

void boardRngBombs(Board *board, const Coord tpos)
{
    for(int x = 0; x < board->len.x; x++)
        memset(board->tile[x], 0, board->len.y * sizeof(Tile));
    board->state = BS_FIRST;
    for(uint i = 0; i < board->numBombs; i++){
        Coord pos;
        do{
            pos.x = rand()%board->len.x;
            pos.y = rand()%board->len.y;
        }while(
            board->tile[pos.x][pos.y].isBomb || (
                inBound(pos.x, tpos.x-1,tpos.x+2) &&
                inBound(pos.y, tpos.y-1,tpos.y+2)
            )
        );
        board->tile[pos.x][pos.y].isBomb = true;
    }
}

volatile _Atomic int *done;

void* boardPlaceBombsThread(void *voidData)
{
    ThreadData *data = voidData;
    Board *board = &(data->board);
    ull n = 0;
    do{
        for(uint i = 0; i < 50; i++){
            boardRngBombs(board, data->tpos);
            boardCalcNums(board);
            floodFill(board, data->tpos);
            if(solve(board)){
                if(atomic_load(done) != -1)
                    return NULL;
                atomic_store(done, data->index);
                printf("Storing %i\n", data->index);
                return NULL;
            }
        }
        n++;
        printf("Thread[%i]: %llu\n", data->index, n*50);
    }while(atomic_load(done) == -1);
    return NULL;
}

bool boardPlaceBombs(Board *board)
{
    const Length len = board->len;
    _Atomic int d;
    done = &d;
    atomic_init(done, -1);
    Coord tpos = winTilePos(board->lDown, board->scale, board->off);
    assertExpr(validTilePos(tpos, board->len));
    printf("placing bombs\n");
    assertExpr(board->state == BS_FIRST);
    for(Direction d = 0; d < 4; d++)
        if(!validTilePos(coordShift(tpos, d, 1), board->len))
            tpos = coordShift(tpos, dirINV(d), 1);
    uint ticks = getTicks();
    if(board->type != B_RNG){
        ThreadData data[NUM_THREADS] = {0};
        pthread_t thread[NUM_THREADS] = {0};
        for(int i = 0; i < NUM_THREADS; i++){
            data[i].board = *board;
            data[i].board.len = board->len;
            boardAlloc(&(data[i].board));
            boardResetFirstClick(&(data[i].board));
            data[i].tpos = tpos;
            data[i].index = i;
            pthread_create(&(thread[i]), NULL, boardPlaceBombsThread, &(data[i]));
        }
        for(int i = 0; i < NUM_THREADS; i++){
            pthread_join(thread[i], NULL);
        }
        const int index = atomic_load(done);
        printf("index: %i\n", index);
        ticks = getTicks()-ticks;
        printf("In %isec\n", ticks/1000);
        board->len = len;
        boardAlloc(board);
        boardCpy(board, &(data[index].board));
        for(int i = 0; i < NUM_THREADS; i++){
            boardFree(&(data[i].board));
        }
    }else{
        boardAlloc(board);
        boardResetFirstClick(board);
    }
    printBoardInfo(board);
    printBoard(board, false);
    printBoard(board, true);
    boardResetTileStates(board);
    floodFill(board, tpos);
    board->state = BS_PLAY;
    printf("Tiles remaining: %u\n", boardRemaining(board));
    return true;
}

#endif /* end of include guard: BOARD_H */
