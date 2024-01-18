#include <SA/SA.h>
#include "film_gui/gui.h"

int main(int argc, char* argv[])
{
    SA_init();
    start_gui();
    SA_destroy();
}