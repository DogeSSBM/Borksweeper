#ifndef SOLVE_H
#define SOLVE_H

bool clear121(Board *board, const Coord pos)
{
    for(uint i = 0; i < 2; i++){
        const Coord a = coordShift(pos, i,  1);
        const Coord b = coordShift(pos, dirINV(i), 1);
        if(
            validTilePos(a, board->len) &&
            board->tile[a.x][a.y].state == S_NUM &&
            board->tile[a.x][a.y].num == 1
            &&
            validTilePos(b, board->len) &&
            board->tile[b.x][b.y].state == S_NUM &&
            board->tile[b.x][b.y].num == 1
        ){
            Direction d = dirROR(i);
            for(uint j = 0; j < 2; j++){
                d = dirINV(d);
                const Coord perpa = coordShift(a,   d, 1);
                const Coord perpp = coordShift(pos, d, 1);
                const Coord perpb = coordShift(b,   d, 1);
                if(
                    validTilePos(perpa, board->len) &&
                    board->tile[perpa.x][perpa.y].state == S_TILE &&

                    validTilePos(perpp, board->len) &&
                    board->tile[perpp.x][perpp.y].state == S_TILE &&

                    validTilePos(perpb, board->len) &&
                    board->tile[perpb.x][perpb.y].state == S_TILE
                ){
                    floodFill(board, perpp);
                    return true;
                }
            }
        }
    }
    return false;
}

uint flagAdj(Board *board, const Coord pos)
{
    uint flagged = 0;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = iC(pos.x+xo, pos.y+yo);
            if(
                coordSame(pos, adj) ||
                !validTilePos(adj, board->len) ||
                board->tile[adj.x][adj.y].state != S_TILE
            )
                continue;

            board->tile[adj.x][adj.y].state = S_FLAG;
            flagged++;
        }
    }
    return flagged;
}

uint setAdj(Board *board, const Coord pos, const TileState state)
{
    uint set = 0;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = iC(pos.x+xo, pos.y+yo);
            if(
                coordSame(pos, adj) ||
                !validTilePos(adj, board->len) ||
                board->tile[adj.x][adj.y].state != S_TILE
            )
                continue;

            board->tile[adj.x][adj.y].state = state;
            set++;
        }
    }
    return set;
}

uint clearAdj(Board *board, const Coord pos)
{
    uint cleared = 0;
    for(int yo = -1; yo <= 1; yo++){
        for(int xo = -1; xo <= 1; xo++){
            const Coord adj = iC(pos.x+xo, pos.y+yo);
            if(
                !coordSame(pos, adj) &&
                validTilePos(adj, board->len) &&
                board->tile[adj.x][adj.y].state == S_TILE
            ){
                if(board->tile[adj.x][adj.y].isBomb){
                    printDecalsPos(board, adj);
                    panic("Cleared adj bomb at: (%i,%i)", adj.x, adj.y);
                }
                cleared += floodFill(board, adj);
            }
        }
    }
    return cleared;
}

Board* resetQuests(Board *board)
{
    for(int y = 0; y < board->len.y; y++)
        for(int x = 0; x < board->len.x; x++)
            if(board->tile[x][y].state == S_QEST || board->tile[x][y].state == S_QEST_SAFE)
                board->tile[x][y].state = S_TILE;
    return board;
}

bool isAbsurd(Board *board)
{
    for(int y = 0; y < board->len.y; y++){
        for(int x = 0; x < board->len.x; x++){
            const Coord pos = iC(x,y);
            if(board->tile[x][y].state == S_NUM && board->tile[x][y].num){
                const uint num = board->tile[x][y].num;
                const uint bombs = adjTileState(board, pos, S_FLAG) + adjTileState(board, pos, S_QEST);
                const uint tiles = adjTileState(board, pos, S_TILE);
                if(bombs > num || tiles < num - bombs)
                    return true;
            }
        }
    }
    return false;
}

bool reductioAdAbsurdum(Board *board)
{
    bool progress;
    do{
        progress = false;
        for(int y = 0; y < board->len.y; y++){
            for(int x = 0; x < board->len.x; x++){
                const Coord pos = iC(x,y);
                if(board->tile[x][y].state == S_NUM && board->tile[x][y].num){
                    if(adjTileState(board, pos, S_TILE) <=
                        board->tile[x][y].num - (adjTileState(board, pos, S_FLAG)+adjTileState(board, pos, S_QEST))
                    )
                        progress |= setAdj(board, pos, S_QEST);

                    if(adjTileState(board, pos, S_FLAG)+adjTileState(board, pos, S_QEST) == board->tile[x][y].num)
                        progress |= setAdj(board, pos, S_QEST_SAFE);
                }
            }
        }
        if(progress && isAbsurd(board)){
            board = resetQuests(board);
            return true;
        }
    }while(progress);
    board = resetQuests(board);
    return false;
}

bool satSolve(Board *board)
{
    for(int y = 0; y < board->len.y; y++){
        for(int x = 0; x < board->len.x; x++){
            // uint adjTile = 0;
            // uint adjFlag = 0;
            const Coord pos = {.x = x, .y = y};
            // for(int yo = -1; yo <= 1; yo++){
            //     for(int xo = -1; xo <= 1; xo++){
            //         const Coord adj = {.x = x + xo, .y = y + yo};
            //         if(
            //             (adj.x != x || adj.y != y) &&
            //             adj.x >= 0 && adj.y >= 0 &&
            //             adj.x < len.x && adj.y < len.y
            //         ){
            //             adjTile += tile[adj.x][adj.y].state == S_TILE;
            //             adjFlag += tile[adj.x][adj.y].state == S_FLAG;
            //         }
            //     }
            // }
            if(
                board->tile[x][y].state == S_NUM &&
                board->tile[x][y].num &&
                adjTileState(board, pos, S_FLAG) + 1 == board->tile[x][y].num
            ){
                for(int yo = -1; yo <= 1; yo++){
                    for(int xo = -1; xo <= 1; xo++){
                        const Coord adj = iC(pos.x+xo, pos.y+yo);
                        if(
                            !coordSame(pos, adj) &&
                            validTilePos(adj, board->len) &&
                            board->tile[adj.x][adj.y].state == S_TILE
                        ){
                            board->tile[adj.x][adj.y].state = S_QEST;
                            if(reductioAdAbsurdum(board)){
                                floodFill(board, adj);
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool solve(Board *board)
{
    bool progress;
    const Length len = board->len;
    Tile **tile = board->tile;
    // uint **adjTile = calloc(len.y, sizeof(uint*));
    // uint **adjFlag = calloc(len.y, sizeof(uint*));
    // for(int y = 0; y < len.y; y++){
    //     adjTile[y] = calloc(len.x, sizeof(uint));
    //     adjFlag[y] = calloc(len.x, sizeof(uint));
    // }
    // for(int y = 0; y < len.y; y++){
    //     for(int x = 0; x < len.x; x++){
    //         const Coord pos = {.x = x, .y = y};
    //         adjTile[pos.x][pos.y] = 8;
    //         for(int yo = -1; yo <= 1; yo++){
    //             for(int xo = -1; xo <= 1; xo++){
    //                 const Coord adj = {.x = pos.x+xo, .y = pos.y+yo};
    //                 adjTile[pos.x][pos.y] -= (
    //                     (adj.x != pos.x || adj.y != pos.y) &&
    //                     adj.x >= 0 && adj.y >= 0 &&
    //                     adj.x < len.x && adj.y < len.y &&
    //                     tile[adj.x][adj.y] == S_NUM
    //                 );
    //             }
    //         }
    //     }
    // }
    do{
        progress = false;
        for(int y = 0; y < len.y; y++){
            for(int x = 0; x < len.x; x++){
                if(tile[x][y].state == S_NUM && tile[x][y].num){
                    uint adjTile = 0;
                    uint adjFlag = 0;
                    const Coord pos = {.x = x, .y = y};
                    for(int yo = -1; yo <= 1; yo++){
                        for(int xo = -1; xo <= 1; xo++){
                            const Coord adj = {.x = x + xo, .y = y + yo};
                            if(
                                (adj.x != x || adj.y != y) &&
                                adj.x >= 0 && adj.y >= 0 &&
                                adj.x < len.x && adj.y < len.y
                            ){
                                adjTile += tile[adj.x][adj.y].state == S_TILE;
                                adjFlag += tile[adj.x][adj.y].state == S_FLAG;
                            }
                        }
                    }
                    if(adjTile <= tile[x][y].num - adjFlag){
                        const uint flagged = flagAdj(board, pos);
                        if(flagged){
                            progress = true;
                            adjFlag += flagged;
                            adjTile -= flagged;
                        }
                    }

                    if(adjFlag == tile[x][y].num){
                        const uint clearedadj = clearAdj(board, pos);
                        if(clearedadj){
                            progress = true;
                            adjTile = 0;
                        }
                    }

                    if(board->type == B_SAT && tile[x][y].num == 2)
                        progress |= clear121(board, pos);

                }
            }
        }
        if(board->type == B_SAT && !progress)
            progress |= satSolve(board);
    }while(progress);

    for(int y = 0; y < board->len.y; y++)
        for(int x = 0; x < board->len.x; x++)
            if(!board->tile[x][y].isBomb && board->tile[x][y].state != S_NUM)
                return false;
    return true;
}

#endif /* end of include guard: SOLVE_H */
