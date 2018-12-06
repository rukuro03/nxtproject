/*
  カラーセンサAPI
*/

#include "monoatume_cfg.h"

U16 color_judge(void){
/*カラーIDを返す*/
return ecrobot_get_nxtcolorsensor_id(Color);　

}
