#ifndef BACKUP_DATA_H
#define BACKUP_DATA_H

#include<regs/bkp.h>
#include<regs/rcc.h>
#include<regs/pwr.h>

#include<math.h>

void backup_data_init();

int compareBackupDoubleValues(double a, double b);

// returns 1,if write succeeded
int write_backup_data(uint8_t index, double data);

double read_backup_data(uint8_t index);

#endif