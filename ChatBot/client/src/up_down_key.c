#include "../include/messanger.h"

bool is_key_up()
{
    return key_catcher[0] == 27 && key_catcher[1] == 91 && key_catcher[2] == 65;
}

bool is_key_down()
{
    return key_catcher[0] == 27 && key_catcher[1] == 91 && key_catcher[2] == 66;
}
