/*
 * prometheus_pusher.h
 *
 *  Created on: Oct 25, 2024
 *      Author: bespsm
 */

#ifndef PROMETHEUS_PUSHER_H_
#define PROMETHEUS_PUSHER_H_

#define PROMETHEUS_PUSHGATEWAY_HOST "192.168.178.51"
#define PROMETHEUS_PUSHGATEWAY_PORT 9091

/**
 * Starts prometheus pusher task.
 */
void prometheus_pusher_start(void);


#endif /* PROMETHEUS_PUSHER_H_ */
