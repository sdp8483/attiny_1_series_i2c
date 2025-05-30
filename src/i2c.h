#ifndef INC_I2C_H
#define INC_I2C_H

// This library is based on the following examples
//      https://www.avrfreaks.net/s/topic/a5C3l000000UZleEAG/t151284
//      https://www.avrfreaks.net/s/topic/a5C3l000000UcBFEA0/t160557?comment=P-1319715

/* Includes ------------------------------------------------------------------ */
#include <avr/io.h>
#include <stdint.h>

/* Defines ------------------------------------------------------------------- */
#ifndef I2C_PINS_ENABLE_INTERNAL_PULLUPS        /* define this using build flags to change in platformio */
#define I2C_PINS_ENABLE_INTERNAL_PULLUPS    0   /* enable weak internal pullups on SDA and SCL pins */
#endif

#define I2C_SCK_100kHz  100000UL                /* i2c clock frequency of 100kHz, standard mode (Sm) */
#define I2C_SCK_400kHz  400000UL                /* i2c clock frequency of 400hHz, fast mode (Fm) */
#define I2C_SCK_1MHz    1000000UL               /* i2c clock frequency of 1MHz, fast mode plus (Fm+) */
#ifndef I2C_SCK_BAUD
#define I2C_SCK_BAUD    I2C_SCK_100kHz          /* default i2c clock frequency if not set using build flags */
#endif

#if (BOARD == ATtiny212) || (BOARD == ATtiny212)
/* ATtiny212 and ATtiny412 use PA1 and PA2 as I2C pins*/
#define I2C_PORT            PORTA
#define SDA_PIN             PIN1_bm
#define SCL_PIN             PIN2_bm
#else
#define I2C_PORT            PORTB
#define SDA_PIN             PIN1_bm
#define SCL_PIN             PIN0_bm
#endif

/* Macros -------------------------------------------------------------------- */
#define I2C_BAUD(F_SCL)     (((float) F_CPU / (2 * (float) F_SCL)) - 5)
#define I2C_HOST_NOP()      asm volatile(" nop \r\n")

/* typedef ------------------------------------------------------------------- */

/* Globals ------------------------------------------------------------------- */

/* External Globals ---------------------------------------------------------- */

/* Function Prototypes ------------------------------------------------------- */

/* Class --------------------------------------------------------------------- */
class I2C_HOST {
    public:
        enum i2c_host_direction {
            I2C_HOST_WRITE = 0,         /* host places address on bus with write flag set */
            I2C_HOST_READ = 1           /* host places address on bus with read flag set */
        };

        enum i2c_host_response {
            I2C_HOST_ACK,               /* after client sends a byte, host responds with ACK for another byte */
            I2C_HOST_NACK               /* after client sends a byte, host responds with NACK and stops data transfer */
        };

        enum i2c_state {
            I2C_HOST_OK,                /* i2c bus is ok */
            I2C_HOST_ERROR_BUS,         /* i2c bus error */
            I2C_HOST_ERROR_ARBLOST,     /* i2c host lost bus */
            I2C_HOST_ERROR_NOT_OWNER,   /* i2c host does not control bus */

            I2C_CLIENT_ACK,             /* i2c client responded ack */
            I2C_CLIENT_NACK,            /* i2c client responded nack */

            I2C_NO_TIMEOUT,             /* no timeout occurred */
            I2C_WIF_TIMEOUT,            /* timeout during wait for write interrupt flag */
            I2C_RIF_TIMEOUT,            /* timeout during wait for read interrup flag */
            I2C_WIF_RIF_TIMEOUT,        /* timeout during wait for read/write interrupt flag */

            I2C_UKNOWN_ERROR
        };

        void init(void);                /* set sck using I2C_SCK_BAUD and build flags */

        i2c_state start(uint8_t address, i2c_host_direction direction);
        i2c_state write(uint8_t *data);
        i2c_state read(uint8_t *data, i2c_host_response acknack = i2c_host_response::I2C_HOST_NACK);
        void stop(void);

    private:
        i2c_state _wait_wif_rif(void);      /* wait for read/write interrupt flag to be set with timeout */
        i2c_state _wait_wif(void);          /* wait for write interrupt flag to be set with timeout */
        i2c_state _wait_rif(void);          /* wait for read interrupt flag to be set with timeout */

        const uint16_t _max_timeout_count = 1000;
};

static I2C_HOST i2c_host;

#endif