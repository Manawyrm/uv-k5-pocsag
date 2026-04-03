# Quansheng UV-K5 POCSAG pager transmitter

This repository is a heavily modified version of the open-source firmware for the popular Quansheng UV-K5 ham radios.  

> [!WARNING]  
> This special firmware should __NOT__ be flashed onto handheld radios for normal use.  
> It might brick you radio, provides no user interface, no RX capability!

This repository is based on [DualTachyon/uv-k5-firmware](https://github.com/DualTachyon/uv-k5-firmware) / [OneOfEleven/uv-k5-firmware-custom](https://github.com/OneOfEleven/uv-k5-firmware-custom)

## Table of Contents

* [Main Features](#main-features)

## Main features:
* Digital POCSAG1200 pager transmitter
* USB-serial (UART) communication-only
* No analog audio / modulation anywhere in the process

## Compilation
```bash
sudo apt install git build-essential gcc-arm-none-eabi libnewlib-arm-none-eabi python3-crcmod
```

## Credits

* [Manawyrm](https://github.com/Manawyrm) (POCSAG pager TX)
* [OneOfEleven](https://github.com/OneOfEleven) (custom firmware)
* [DualTachyon](https://github.com/DualTachyon) (custom firmware)
* [Mikhail](https://github.com/fagci)
* [Andrej](https://github.com/Tunas1337)
* [Manuel](https://github.com/manujedi)
* @wagner
* @Lohtse Shar
* [@Matoz](https://github.com/spm81)
* @Davide
* @Ismo OH2FTG
* [OneOfEleven](https://github.com/OneOfEleven)
* @d1ced95
* and others I forget

## License

Copyright 2026 Manawyrm
https://github.com/Manawyrm  
Copyright 2023 Dual Tachyon
https://github.com/DualTachyon

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

