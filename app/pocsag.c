#include "app/pocsag.h"
#include "audio.h"
#include "driver/bk4819.h"
#include "driver/crc.h"
#include "driver/eeprom.h"
#include "driver/system.h"
#include "driver/uart.h"
#include "frequencies.h"
#include "misc.h"
#include "radio.h"
#include "ui/helper.h"
#include "ui/inputbox.h"
#include "ui/ui.h"

#include <string.h>

void log(const char *str) {
    UART_Send(str, strlen(str));
}

void send_pocsag_data()
{
    uint32_t Timeout = 200000;

    log("send_pocsag_data\r\n");

    SYSTEM_DelayMs(20);

    struct reg_value {
        BK4819_REGISTER_t reg;
        uint16_t value;
    };

    struct reg_value POCSAG_Configuration [] = {
        { BK4819_REG_31, 0x0000 },	// Disable Scramble function
        { BK4819_REG_3F, BK4819_REG_3F_FSK_TX_FINISHED },  // FSK TX Finished interrupt enable
        { BK4819_REG_58, 0x03C1 },	// FSK Enable,
                                                // RX Bandwidth 000: FSK 1.2K
                                                // 0xAA or 0x55 Preamble
                                                // 11 RX Gain,
                                                // FSK RX mode selection: 000: FSK 1.2K
                                                // FSK TX mode selection: 000: FSK 1.2K

        {BK4819_REG_70, 0x0000 }, // Enable TONE1: 0, Enable TONE2: 0

        // 2<15:0> TONE2/FSK frequency control word: = freq (Hz) * 10.32444 for XTAL 13M/26M
        { BK4819_REG_72, 0x3065 }, // 1200 * 10.32444 MHz

        { BK4819_REG_5D, 0xD000 },	// Set FSK data length
        { BK4819_REG_59, 0x8068 },	// 4 byte sync length, 6 byte preamble, clear TX FIFO
        { BK4819_REG_59, 0x0068 },	// Same, but clear TX FIFO is now unset (clearing done)
        { BK4819_REG_5A, 0x5555 },	// First two sync bytes
        { BK4819_REG_5B, 0x55AA },	// End of sync bytes. Total 4 bytes: 555555aa
        { BK4819_REG_5C, 0xAA30 },	// Disable CRC
    };

    BK4819_SetAF(BK4819_AF_MUTE);

    // Set Deviation
    uint16_t gBK4819_DefaultDeviation;
    gBK4819_DefaultDeviation = BK4819_ReadRegister(BK4819_REG_40);
    BK4819_WriteRegister(BK4819_REG_40, (gBK4819_DefaultDeviation & 0xE000) | 0x1383);
    // Mute output audio and bypass all AF TX filters.
    //BK4819_WriteRegister(BK4819_REG_47, (2u << 12) | (BK4819_AF_MUTE << 8) | (1u << 6) | 1);

    // bypass all AF TX filters, AF output inverse mode: non-inverse
    //BK4819_WriteRegister(BK4819_REG_47, 0x4141);

    // Disable TX DC filter.
    BK4819_WriteRegister(BK4819_REG_7E, (BK4819_ReadRegister(BK4819_REG_7E) & 0xFFC7) | 0x8000);
    // Disable Voice FM AF TX filters
    BK4819_WriteRegister(BK4819_REG_2B, (BK4819_ReadRegister(BK4819_REG_2B) & 0xFFF8) | 0x7);
    // Disable ALC
    // FIXME: BK4819_SetRegValue(alcDisableRegSpec, 1);
    BK4819_WriteRegister(BK4819_REG_4B, 0x0010);

    for (unsigned int i = 0; i < ARRAY_SIZE(POCSAG_Configuration); i++) {
        BK4819_WriteRegister(POCSAG_Configuration[i].reg, POCSAG_Configuration[i].value);
    }


    //
    /*for (unsigned int i = 0; i < 102; i++) {
        BK4819_WriteRegister(BK4819_REG_5F, packet16[i]);
    }

    SYSTEM_DelayMs(20);

    BK4819_WriteRegister(BK4819_REG_59, 0x0868); // Enable FSK

    while (Timeout-- && (BK4819_ReadRegister(BK4819_REG_0C) & 1u) == 0)
        SYSTEM_DelayMs(5);

    log("done??\r\n");

    BK4819_WriteRegister(BK4819_REG_02, 0); // clear FIFO TX finished interrupt

    SYSTEM_DelayMs(20);

    BK4819_ResetFSK();*/

    SYSTEM_DelayMs(20);

    BK4819_WriteRegister(BK4819_REG_3F, BK4819_REG_3F_FSK_TX_FINISHED);
    BK4819_WriteRegister(BK4819_REG_59, 0x8068);
    BK4819_WriteRegister(BK4819_REG_59, 0x0068);

    uint16_t *packet16 = (uint16_t*)packet;
    for (unsigned int i = 0; i < 104; i++) {
        BK4819_WriteRegister(BK4819_REG_5F, ~packet16[i]);
    }

    SYSTEM_DelayMs(20);

    BK4819_WriteRegister(BK4819_REG_59, 0x0868);

    while (Timeout-- && (BK4819_ReadRegister(BK4819_REG_0C) & 1u) == 0)
        SYSTEM_DelayMs(5);

    BK4819_WriteRegister(BK4819_REG_02, 0);

    SYSTEM_DelayMs(20);

    BK4819_ResetFSK();
}

void pocsag_tx()
{
    log("RADIO_SetTxParameters\r\n");
    RADIO_SetTxParameters();

    // turn the RED LED on
    BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, true);

    log("send_pocsag_data\r\n");
    send_pocsag_data();

    BK4819_SetupPowerAmplifier(0, 0);
    BK4819_ToggleGpioOut(BK4819_GPIO1_PIN29_PA_ENABLE, false);

    // turn the RED LED on
    BK4819_ToggleGpioOut(BK4819_GPIO5_PIN1_RED, false);
}