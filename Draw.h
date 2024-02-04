#ifndef DRAW_H
#define DRAW_H

uint tileScale(const Length winLen, const Length boardLen)
{
    return imin(winLen.x/boardLen.x, winLen.y/boardLen.y);
}

Offset tileOffset(const Length winLen, const Length boardLen, const uint scale)
{
    return coordDivi(coordSub(winLen, coordMuli(boardLen, scale)), 2);
}

void drawNum(const Coord winPos, const uint scale, const uint num)
{
    static const Color numColor[8] = {
        {0x00, 0x00, 0xAA, 0xFF},
        {0x00, 0x52, 0x00, 0xFF},
        {0xAA, 0x00, 0x00, 0xFF},
        {0x00, 0x00, 0x52, 0xFF},
        {0x52, 0x00, 0x00, 0xFF},
        {0x00, 0x52, 0x52, 0xFF},
        {0x00, 0x00, 0x00, 0xFF},
        {0x52, 0x52, 0x52, 0xFF},
    };

    setColor(GREY3);
    fillSquareCoordResize(winPos, scale, -1);
    if(num > 0){
        setTextColor(numColor[num - 1]);
        char txt[2] = {'0' + num, '\0'};
        drawTextCenteredCoord(txt, coordAddi(winPos, scale/2));
    }
}

void drawUnrevealedTile(const Coord winPos, const uint scale)
{
    setColor(GREY1);
    fillSquareCoordResize(winPos, scale, -1);
    setColor(GREY2);
    fillSquareCoordResize(winPos, scale, -1 - scale/8);
}

void drawFlag(const Coord winPos, const uint scale)
{
    const uint t1 = scale/3;
    const uint t2 = t1*2;
    const uint n1 = t1/3;
    const uint n2 = n1*2;

    setColor(BLACK);
    const Coord p1 = coordShift(coordShift(winPos, DIR_R, t1+n1), DIR_D, n1);
    const Coord p2 = coordShift(coordShift(winPos, DIR_R, t1+n2), DIR_D, t2+n1);
    fillRectCoords(p1, p2);
    fillRectCoords(
        coordShift(coordShift(winPos, DIR_R, t1), DIR_D, t2+n1),
        coordShift(coordShift(winPos, DIR_R, t2), DIR_D, t2+n2)
    );
    setColor(RED);
    Coord f[3] = {
        coordShift(p1, DIR_R, n1),
        coordShift(coordShift(p1, DIR_R, t1+n1), DIR_D, n2),
        coordShift(coordShift(p1, DIR_R, n1), DIR_D, t1+n1)
    };
    fillPoly(f, 3);
}

void drawQuest(const Coord winPosCentered)
{
    setTextColor(BLACK);
    drawTextCenteredCoord("?", winPosCentered);
}

void drawBoard(Board *board)
{
    if(board->rescale || windowResized()){
        board->winLen = getWindowLen();
        board->scale = tileScale(board->winLen, board->len);
        board->off = tileOffset(board->winLen, board->len, board->scale);
        setTextSize(board->scale);
    }

    const uint hscale = board->scale/2;

    for(int y = 0; y < board->len.y; y++){
        for(int x = 0; x < board->len.x; x++){
            const Coord winPos = tileMousePos(board->scale, board->off, iC(x,y));

            if(board->state < BS_PLAY || board->tile == NULL){
                drawUnrevealedTile(winPos, board->scale);
            }else if(board->tile[x][y].state == S_NUM){
                drawNum(winPos, board->scale, board->tile[x][y].num);
            }else{
                drawUnrevealedTile(winPos, board->scale);

                if(board->cheat || board->state == BS_END){
                    if(board->tile[x][y].isBomb){
                        setTextColor(BLACK);
                        drawTextCenteredCoord("B", coordAddi(winPos, hscale));
                    }else if(board->tile[x][y].num > 0){
                        drawNum(winPos, board->scale, board->tile[x][y].num);
                    }
                }

                if(board->tile[x][y].state == S_FLAG)
                    drawFlag(winPos, board->scale);
                else if(board->tile[x][y].state == S_QEST)
                    drawQuest(coordAddi(winPos, hscale));
            }
        }
    }

    if(getTicks() < board->hint.showUntil){
        setColor(board->hint.action == S_NUM?BLUE:RED);
        fillCircleCoord(coordAddi(tileMousePos(board->scale, board->off, iC(board->hint.pos.x, board->hint.pos.y)), board->scale/2), board->scale/2);
    }
}

#endif /* end of include guard: DRAW_H */
