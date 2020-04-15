#include<gy86/gy86.h>

typedef struct MPUdata MPUdata;
struct MPUdata
{
    int16_t acclx;
    int16_t accly;
    int16_t acclz;
    int16_t temp;
    int16_t gyrox;
    int16_t gyroy;
    int16_t gyroz;
};

// gyroscope steady state initial values may not be 0
// while initial accelerometer values will help us get final rotation
static MPUdatascaled offsets = {.accl = {.xi = 0.154, .yj = -0.005, .zk = -0.729}, .temp = 0.0, .gyro = {.xi = -2.16, .yj = 1.6, .zk = 0.21}};

// gyroscope steady state initial values may not be 0
// while initial accelerometer values will help us get final rotation
static MPUdatascaled initial = {.accl = {.xi = 0.0, .yj = 0.0, .zk = 0.0}, .temp = 0.0, .gyro = {.xi = 0.0, .yj = 0.0, .zk = 0.0}};

const MPUdatascaled* mpu_init()
{
    uint8_t data;

    // write 0 to pwr_mgmt_1 register to wake it up
    data = 0x00;
    i2c_write(MPU6050_ADDRESS, 0x6b, &data, 1);

    // below wries will enable us to talk to HMC5883 sensor
        // turn on master of i2c, and reset accl and gyro data path and its registers
        data = 0x01;
        i2c_write(MPU6050_ADDRESS, 0x6a, &data, 1);

        // turnon i2c bypass mode
        data = 0x02;
        i2c_write(MPU6050_ADDRESS, 0x37, &data, 1);
    // yay we can now talk to magnetometer, or anyother sensor attached on the auxilary bus of mpu6050 directly

    // set gyroscope sensitivity => 0 = +/-250 dps (degrees per second)
    data = 0x00;
    i2c_write(MPU6050_ADDRESS, 0x1b, &data, 1);

    // set accelerometer sensitivity => 0 = +/-19.6 (meters per second square)
    data = 0x00;
    i2c_write(MPU6050_ADDRESS, 0x1c, &data, 1);

    // Below configuration will set your sample rate to 8kHz
        // set EXT_SYNC_SET and DLPF_CFG => 0 = (input disabled and 260 BW)
        data = 0x00;
        i2c_write(MPU6050_ADDRESS, 0x1a, &data, 1);

        // set SMPLRT_DIV = 0 => sensor registers are clocked at 8Khz
        data = 0x00;
        i2c_write(MPU6050_ADDRESS, 0x19, &data, 1);
    // mpu6050 going zoo00000mm

    // a small delay before we start reading the sensors
    delay_for(200);

    uint16_t i;
    for(i = 0; i < 500; i++)
    {
        MPUdatascaled datasc;
        get_scaled_MPUdata(&datasc);
        initial.accl.xi += (datasc.accl.xi/500);
        initial.accl.yj += (datasc.accl.yj/500);
        initial.accl.zk += (datasc.accl.zk/500);
        initial.gyro.xi += (datasc.gyro.xi/500);
        initial.gyro.yj += (datasc.gyro.yj/500);
        initial.gyro.zk += (datasc.gyro.zk/500);
        delay_for(1);
    }

    return &initial;
}

int get_scaled_MPUdata(MPUdatascaled* result)
{
    MPUdata data;

    i2c_read(MPU6050_ADDRESS, 0x3b, &data, 14);

    // we have to change the data from network ordr to host order integers
    data.acclx = (data.acclx << 8) | ((data.acclx >> 8) & 0x00ff);
    data.accly = (data.accly << 8) | ((data.accly >> 8) & 0x00ff);
    data.acclz = (data.acclz << 8) | ((data.acclz >> 8) & 0x00ff);
    data.temp = (data.temp << 8) | ((data.temp >> 8) & 0x00ff);
    data.gyrox = (data.gyrox << 8) | ((data.gyrox >> 8) & 0x00ff);
    data.gyroy = (data.gyroy << 8) | ((data.gyroy >> 8) & 0x00ff);
    data.gyroz = (data.gyroz << 8) | ((data.gyroz >> 8) & 0x00ff);

    // in m/s2, meter per second square => sensitivity = +/-2g = +/-19.6
    result->accl.xi = ((((double)(data.acclx)) * 19.6) / 32768.0) - offsets.accl.xi;
    result->accl.yj = ((((double)(data.accly)) * 19.6) / 32768.0) - offsets.accl.yj;
    result->accl.zk = ((((double)(data.acclz)) * 19.6) / 32768.0) - offsets.accl.zk;

    // temperature is in degree celcius
    result->temp  = (((double)(data.temp)) / 340) + 36.53;

    // in dps, degrees per second => sensitivity = +/-250
    result->gyro.xi = ((((double)(data.gyrox)) * 250.0) / 32768.0) - offsets.gyro.xi;
    result->gyro.yj = ((((double)(data.gyroy)) * 250.0) / 32768.0) - offsets.gyro.yj;
    result->gyro.zk = ((((double)(data.gyroz)) * 250.0) / 32768.0) - offsets.gyro.zk;

    return 1;
}

// the result gets stored in the raw quaternion change
void get_raw_quaternion_change_from_gyroscope(quat_raw* change, quaternion* previous_quaternion, vector* gyroscope, double time_in_seconds_since_last_reading)
{
    // vectr is now the unit vector in the direction of rotation wrt to local current axis
    unit_vector(&(change->vectr), gyroscope);

    // the angle of rotation since past reading of gyroscope
    change->theta = magnitude_vector(gyroscope) * time_in_seconds_since_last_reading;

    // rotate vectr, about the conjugate of previous_quaternion, to make change to represent rotation wrt initial ground frame of reference
    quaternion reverse_rotation;
    conjugate(&reverse_rotation, previous_quaternion);
    vector rotation_axis;
    rotate_vector(&rotation_axis, &reverse_rotation, &(change->vectr));
    change->vectr = rotation_axis;
}