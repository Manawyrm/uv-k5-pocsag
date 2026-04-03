#include "app/pocsag.h"
#include "audio.h"
#include "driver/bk4819.h"
#include "driver/crc.h"
#include "driver/eeprom.h"
#include "driver/system.h"
#include "frequencies.h"
#include "misc.h"
#include "radio.h"
#include "ui/helper.h"
#include "ui/inputbox.h"
#include "ui/ui.h"

void send_pocsag_data()
{
    uint8_t Timeout = 200;

    SYSTEM_DelayMs(20);

    struct reg_value {
        BK4819_REGISTER_t reg;
        uint16_t value;
    };

    struct reg_value POCSAG_Configuration [] = {
        { BK4819_REG_3F, BK4819_REG_3F_FSK_TX_FINISHED },  // FSK TX Finished interrupt enable
        { BK4819_REG_58, 0x03C3 },	// FSK Enable,
                                                // RX Bandwidth FFSK 1200/1800 (unused)
                                                // 0xAA or 0x55 Preamble
                                                // 11 RX Gain,
                                                // FSK RX mode selection: 000: FSK 1.2K
                                                // FSK TX mode selection: 000: FSK 1.2K
        // REG_71<15:0> TONE1/Scramble frequency control word: = freq (Hz) * 10.32444 for XTAL 13M/26M
        { BK4819_REG_71, 0xB57C }, // POCSAG FSK deviation 4500Hz
        { BK4819_REG_5D, 0x0D00 },	// Set FSK data length to 13 bytes
        { BK4819_REG_59, 0x8068 },	// 4 byte sync length, 6 byte preamble, clear TX FIFO
        { BK4819_REG_59, 0x0068 },	// Same, but clear TX FIFO is now unset (clearing done)
        { BK4819_REG_5A, 0x5555 },	// First two sync bytes
        { BK4819_REG_5B, 0x55AA },	// End of sync bytes. Total 4 bytes: 555555aa
        { BK4819_REG_5C, 0xAA30 },	// Disable CRC
    };

    BK4819_SetAF(BK4819_AF_MUTE);

    for (unsigned int i = 0; i < ARRAY_SIZE(POCSAG_Configuration); i++) {
        BK4819_WriteRegister(POCSAG_Configuration[i].reg, POCSAG_Configuration[i].value);
    }

    for (unsigned int i = 0; i < 13; i++) {
        BK4819_WriteRegister(BK4819_REG_5F, 0x55);
    }

    SYSTEM_DelayMs(20);

    //BK4819_WriteRegister(BK4819_REG_59, 0x2868);

    while (Timeout-- && (BK4819_ReadRegister(BK4819_REG_0C) & 1u) == 0)
        SYSTEM_DelayMs(5);

    BK4819_WriteRegister(BK4819_REG_02, 0); // clear FIFO TX finished interrupt

    SYSTEM_DelayMs(20);

    BK4819_ResetFSK();
}

void pocsag_tx()
{
    //send_pocsag_data()
    //BK4819_SetupPowerAmplifier(0, 0);
    //BK4819_ToggleGpioOut(BK4819_GPIO1_PIN29_PA_ENABLE, false);

    send_pocsag_data();
}