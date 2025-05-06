#include "ti_msp_dl_config.h"

#define DAC_ADDR            0x62
#define SD_PIN              DL_GPIO_PIN_9    /* PA9 = MCP4725 SD */
#define LED_PIN             DL_GPIO_PIN_13   /* PA13 = debug LED */

#define SW0_PIN             DL_GPIO_PIN_0    /* PA0 */
#define SW1_PIN             DL_GPIO_PIN_1    /* PA1 */
#define SW2_PIN             DL_GPIO_PIN_2    /* PA2 */
#define SW3_PIN             DL_GPIO_PIN_3    /* PA3 */
#define SW4_PIN             DL_GPIO_PIN_4    /* PA4 */
#define SW5_PIN             DL_GPIO_PIN_5    /* PA5 */
#define SW6_PIN             DL_GPIO_PIN_6    /* PA6 */

#define TONE_HIGH           4095U            /* full‑scale high = 3.3 V */
#define TONE_LOW                0U            /* full‑scale low  = 0 V */
#define SILENCE             2048U            /* mid‑scale = silence */

#define HALF_PERIOD_DELAYS0  6000U           /* highest freq */
#define HALF_PERIOD_DELAYS1  7000U
#define HALF_PERIOD_DELAYS2  8000U
#define HALF_PERIOD_DELAYS3  9000U
#define HALF_PERIOD_DELAYS4 10000U
#define HALF_PERIOD_DELAYS5 11000U
#define HALF_PERIOD_DELAYS6 12000U           /* lowest freq */

static inline void dac_write(uint16_t v)
{
    uint8_t buf[2] = { (uint8_t)(v >> 4), (uint8_t)(v << 4) };
    while (!(DL_I2C_getControllerStatus(I2C_0_INST) &
             DL_I2C_CONTROLLER_STATUS_IDLE)) { }
    DL_I2C_fillControllerTXFIFO(I2C_0_INST, buf, 2);
    DL_I2C_startControllerTransfer(I2C_0_INST,
                                   DAC_ADDR, 2, false);
    while (!(DL_I2C_getControllerStatus(I2C_0_INST) &
             DL_I2C_CONTROLLER_STATUS_IDLE)) { }
}

int main(void)
{
    SYSCFG_DL_init();  // sets up I²C_0, PA0–PA6 inputs, PA9/PA13 outputs

    // Ensure MCP4725 SD pin is high so the DAC is always enabled
    DL_GPIO_enableOutput(GPIOA, SD_PIN);
    DL_GPIO_setPins    (GPIOA, SD_PIN);

    // LED debug pin (PA13)
    DL_GPIO_enableOutput(GPIOA, LED_PIN);
    DL_GPIO_clearPins (GPIOA, LED_PIN);  // start OFF

    bool tone0 = false, tone1 = false, tone2 = false,
         tone3 = false, tone4 = false, tone5 = false, tone6 = false;

    while (1) {
        // 1) read all seven buttons
        bool p0 = (DL_GPIO_readPins(GPIOA, SW0_PIN) & SW0_PIN) != 0;
        bool p1 = (DL_GPIO_readPins(GPIOA, SW1_PIN) & SW1_PIN) != 0;
        bool p2 = (DL_GPIO_readPins(GPIOA, SW2_PIN) & SW2_PIN) != 0;
        bool p3 = (DL_GPIO_readPins(GPIOA, SW3_PIN) & SW3_PIN) != 0;
        bool p4 = (DL_GPIO_readPins(GPIOA, SW4_PIN) & SW4_PIN) != 0;
        bool p5 = (DL_GPIO_readPins(GPIOA, SW5_PIN) & SW5_PIN) != 0;
        bool p6 = (DL_GPIO_readPins(GPIOA, SW6_PIN) & SW6_PIN) != 0;

        // 2) mirror any press on the LED
        if (p0 || p1 || p2 || p3 || p4 || p5 || p6)
            DL_GPIO_setPins  (GPIOA, LED_PIN);
        else
            DL_GPIO_clearPins(GPIOA, LED_PIN);

        // 3) generate the tone for the first pressed switch
        if (p0) {
            tone0 = !tone0;
            dac_write(tone0 ? TONE_HIGH : TONE_LOW);
            for (volatile uint32_t i = 0; i < HALF_PERIOD_DELAYS0; ++i) { }
        }
        else if (p1) {
            tone1 = !tone1;
            dac_write(tone1 ? TONE_HIGH : TONE_LOW);
            for (volatile uint32_t i = 0; i < HALF_PERIOD_DELAYS1; ++i) { }
        }
        else if (p2) {
            tone2 = !tone2;
            dac_write(tone2 ? TONE_HIGH : TONE_LOW);
            for (volatile uint32_t i = 0; i < HALF_PERIOD_DELAYS2; ++i) { }
        }
        else if (p3) {
            tone3 = !tone3;
            dac_write(tone3 ? TONE_HIGH : TONE_LOW);
            for (volatile uint32_t i = 0; i < HALF_PERIOD_DELAYS3; ++i) { }
        }
        else if (p4) {
            tone4 = !tone4;
            dac_write(tone4 ? TONE_HIGH : TONE_LOW);
            for (volatile uint32_t i = 0; i < HALF_PERIOD_DELAYS4; ++i) { }
        }
        else if (p5) {
            tone5 = !tone5;
            dac_write(tone5 ? TONE_HIGH : TONE_LOW);
            for (volatile uint32_t i = 0; i < HALF_PERIOD_DELAYS5; ++i) { }
        }
        else if (p6) {
            tone6 = !tone6;
            dac_write(tone6 ? TONE_HIGH : TONE_LOW);
            for (volatile uint32_t i = 0; i < HALF_PERIOD_DELAYS6; ++i) { }
        }
        else {
            // no key pressed → silence
            dac_write(SILENCE);
        }
    }

    // never reached
    return 0;
}

