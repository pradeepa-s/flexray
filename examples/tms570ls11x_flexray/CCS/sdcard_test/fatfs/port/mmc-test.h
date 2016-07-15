/*
 * mmc-test.h
 *
 *  Created on: Jan 9, 2016
 *      Author: USER
 */

#ifndef FATFS_PORT_MMC_TEST_H_
#define FATFS_PORT_MMC_TEST_H_
#include "mibspi.h"

void mmcSelectSpi(gioPORT_t *port, mibspiBASE_t *reg, uint32_t tf_group);


#endif /* FATFS_PORT_MMC_TEST_H_ */
