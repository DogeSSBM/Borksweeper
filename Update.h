#ifndef UPDATE_H
#define UPDATE_H

Coord winTilePos(const Coord pos, const int scale, const Offset boardOff)
{
    return coordDivi(coordSub(pos, boardOff), scale);
}

Coord mouseTilePos(const int scale, const Offset boardOff)
{
    return winTilePos(mouse.pos, scale, boardOff);
}

Coord tileMousePos(const uint scale, const Offset boardOff, const Coord tpos)
{
    return coordAdd(coordMuli(tpos, scale), boardOff);
}

bool checkCtrlKey(const Scancode key)
{
    return keyReleased(key) && (keyState(SDL_SCANCODE_LCTRL) || keyState(SDL_SCANCODE_RCTRL));
}

void checkQuit(Board *board)
{
    bool quit = false;
    if((quit = keyPressed(SDL_SCANCODE_ESCAPE)))
        printf("Escape pressed, freeing board and exiting\n");
    else if((quit =
        keyPressed(SDL_SCANCODE_Q) &&
        (keyState(SDL_SCANCODE_LCTRL) || keyState(SDL_SCANCODE_RCTRL))
    ))
        printf("Ctrl+Q pressed, freeing board and exiting\n");
    if(quit){
        if(board->tile)
            boardFree(board);
        exit(EXIT_SUCCESS);
    }
}

ClickType boardUpdateClick(Board *board)
{
    const uint ticks = getTicks();
    const Coord pos = mouseTilePos(board->scale, board->off);

    if(!validTilePos(pos, board->len))
        return C_NONE;

    if(mouseBtnPressed(MOUSE_L)){
        board->rDown.x = -1;
        board->rDown.y = -1;
        if(
            board->state == BS_PLAY &&
            coordSame(winTilePos(board->lDown, board->scale, board->off), pos) &&
            ticks - board->lastClickTick < 400 &&
            board->tile[pos.x][pos.y].state == S_NUM &&
            board->tile[pos.x][pos.y].num == adjTileState(board, pos, S_FLAG)
        )
            return C_DOUB;
        board->lDown = mouse.pos;
        board->lastClickTick = ticks;
    }

    if(mouseBtnPressed(MOUSE_R)){
        board->rDown = mouse.pos;
        board->lDown.x = -1;
        board->lDown.y = -1;
    }

    if(
        mouseBtnReleased(MOUSE_L) &&
        coordSame(board->lDown, mouse.pos)
    )
        return C_LEFT;

    if(
        mouseBtnReleased(MOUSE_R) &&
        coordSame(winTilePos(board->rDown, board->scale, board->off), pos) &&
        (
            board->tile[pos.x][pos.y].state == S_TILE ||
            board->tile[pos.x][pos.y].state == S_FLAG ||
            board->tile[pos.x][pos.y].state == S_QEST
        )
    )
        return C_RIGHT;

    return C_NONE;
}

void boardUpdateSaveNum(Board *board)
{
    const uint oldSaveNum = board->saveNum;
    board->saveNum = imax(0,
        board->saveNum + keyPressed(SDL_SCANCODE_UP) - keyPressed(SDL_SCANCODE_DOWN)
    );
    if(oldSaveNum != board->saveNum)
        printf("saveNum: %u\n", board->saveNum);
}

bool boardCheckLoad(Board *board)
{
    if(checkCtrlKey(SDL_SCANCODE_L)){
        if(board->tile)
            boardFree(board);
        *board = boardLoad(board->saveNum);
        return true;
    }
    return false;
}

void boardUpdate(Board *board)
{
    checkQuit(board);
    if(board->rescale || board->scale == 0){
        board->rescale = false;
        board->winLen = getWindowLen();
        board->scale = tileScale(board->winLen, board->len);
        board->off = tileOffset(board->winLen, board->len, board->scale);
    }

    const Coord tpos = mouseTilePos(board->scale, board->off);

    uint cleared = 0;
    switch(board->state){
        case BS_FIRST:
            boardUpdateSaveNum(board);
            if(boardUpdateClick(board) == C_LEFT){
                boardPlaceBombs(board);
            }
            boardCheckLoad(board);
            break;
        case BS_PLAY:
            boardUpdateSaveNum(board);
            if(checkCtrlKey(SDL_SCANCODE_S)){
                if(boardSave(board, (keyState(SDL_SCANCODE_LSHIFT)||keyState(SDL_SCANCODE_RSHIFT))))
                    printf("Saved board %u!\n", board->saveNum);
                else
                    printf("Error saving board\n");
            }
            boardCheckLoad(board);

            switch(boardUpdateClick(board)){
                case C_LEFT:
                    if(board->tile[tpos.x][tpos.y].isBomb){
                        boardFree(board);
                        exit(EXIT_SUCCESS);
                    }
                    cleared = floodFill(board, tpos);
                    break;
                case C_RIGHT:
                    if(board->tile[tpos.x][tpos.y].state == S_TILE)
                        board->tile[tpos.x][tpos.y].state = S_FLAG;
                    else if(board->tile[tpos.x][tpos.y].state == S_FLAG)
                        board->tile[tpos.x][tpos.y].state = S_QEST;
                    else
                        board->tile[tpos.x][tpos.y].state = S_TILE;
                    break;
                case C_DOUB:
                    cleared = floodAdj(board, tpos);
                    break;
                default:
                    break;
            }
            if(boardRemaining(board) == 0){
                printf("You win!\n");
                boardFree(board);
                exit(EXIT_SUCCESS);
                board->state = BS_FIRST;
            }
            break;
        default:
            break;
    }
    if(cleared != 0)
        printf("Cleared %u tiles\n", cleared);
}

#endif /* end of include guard: UPDATE_H */
