#include "../include/messanger.h"
void catch (const char ch)
{
    key_catcher[0] = key_catcher[1];
    key_catcher[1] = key_catcher[2];
    key_catcher[2] = ch;
}
