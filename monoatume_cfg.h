#ifndef MONOATUME_CFG_H
#define MONOATUME_CFG_H

#include <t_services.h>
#include "monoatume.h"

#ifndef _MACRO_ONLY

/* Tasks */
extern void InitTsk(VP_INT exinf);
extern void ButtonTsk(VP_INT exinf);
extern void MainTsk(VP_INT exinf);
extern void QuitTsk(VP_INT exinf);
extern void FuncTsk(VP_INT exinf);
extern void MoveTsk(VP_INT exinf);
extern void TimerTsk(VP_INT exinf);
extern void TimeOutTsk(VP_INT exinf);
extern void DispTsk(VP_INT exinf);
extern void ColsTsk(VP_INT exinf);
extern void MuscTsk(VP_INT exinf);

/* CyclicTimers */
extern void MoveCyc(VP_INT exinf);
extern void DispCyc(VP_INT exinf);

#endif

#endif
