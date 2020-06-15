#include<gy86/gy86.h>

typedef struct HMCdata HMCdata;
struct HMCdata
{
    int16_t magnx;
    int16_t magnz;
    int16_t magny;
};

// while offsets magnetometer values will help us get final rotation
static HMCdatascaled offsets = {.magn = {.xi = 149.04, .yj = -254.38, .zk = -115.92}};

// while initial magnetometer values will help us get final rotation
static HMCdatascaled initial = {.magn = {.xi = 0.0, .yj = 0.0, .zk = 0.0}};

const HMCdatascaled* hmc_init()
{
    uint8_t data;

    // acess HMC5883 magnetometer here, and configure them
    data = 0x18;
    i2c_write(HMC5883_ADDRESS, 0x00, &data, 1);

    data = 0x20;
    i2c_write(HMC5883_ADDRESS, 0x01, &data, 1);

    data = 0x00;
    i2c_write(HMC5883_ADDRESS, 0x02, &data, 1);
    // magnetometer settings done

    // a small delay before we start reading the sensors
    delay_for_ms(200);

    uint16_t i;
    for(i = 0; i < 200; i++)
    {
        HMCdatascaled datasc;
        get_scaled_HMCdata(&datasc);
        initial.magn.xi += (datasc.magn.xi/200);
        initial.magn.yj += (datasc.magn.yj/200);
        initial.magn.zk += (datasc.magn.zk/200);
        delay_for_ms(14);
    }

    return &initial;
}

int get_scaled_HMCdata(HMCdatascaled* result)
{
    HMCdata data;

    i2c_read(HMC5883_ADDRESS, 0x03, &data, 6);

    // we have to change the data from network ordr to host order integers
    data.magnx = (data.magnx << 8) | ((data.magnx >> 8) & 0x00ff);
    data.magny = (data.magny << 8) | ((data.magny >> 8) & 0x00ff);
    data.magnz = (data.magnz << 8) | ((data.magnz >> 8) & 0x00ff);

    // in mG, milli Gauss
    result->magn.xi = ((((float)(data.magnx)) * 0.92) - offsets.magn.xi)/1166.56;
    result->magn.yj = ((((float)(data.magny)) * 0.92) - offsets.magn.yj)/1219.04;
    result->magn.zk = ((((float)(data.magnz)) * 0.92) - offsets.magn.zk)/997.28;

    return 1;
}