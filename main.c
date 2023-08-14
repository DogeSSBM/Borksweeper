#define NUM_THREADS     4
#include "DogeLib/Includes.h"
#include <stdatomic.h>
#include <pthread.h>
#include "Borksweeper.h"

int main(int argc, char **argv)
{
    init();
    Board board = boardArgs(argc, argv);
    while(1){
        const uint t = frameStart();

        boardUpdate(&board);
        drawBoard(&board);

        frameEnd(t);
    }
    return 0;
}
