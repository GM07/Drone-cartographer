#ifndef CRAZYFLIE_TASK_H  // NOLINT
#define CRAZYFLIE_TASK_H

/**
 * This header will be included in a c source file.
 * So we need to declare them in a different header to make sure that
 * compilation succeeds.
 */

void communicationManagerInit();
bool communicationManagerTest();
void initColorGradient();
bool testColorGradient();

void p2pTaskInit();
bool p2pTaskTest();

#endif