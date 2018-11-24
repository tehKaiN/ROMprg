# ROMprg - Universal ROM/Flash programmer

This is a project of budget-friendly ROM/flash chip programmer. It programs only the chips you need by using dedicated shields. Also it can be used to flash certain devices, e.g. game cartridges.

The project is based on Arduino Mega and can be used on any compatible board.

## Supported chips / devices

- Sega Megadrive / Genesis - dump any cartridge, flash programmable carts from [smdRomCart](https://github.com/tehKaiN/smdCart) project

More chips and devices will be added later.

## Usage

1. Plug in chip/device into slot
2. Connect ROMprg to computer of your choice
3. Execute commands from `romprg` or any other compatible app
4. Disconnect ROMprg from your computer
5. Remove chip/device from slot

How-to for `romprg` can be accessed by running it without additional parameters

### Megadrive shield examples

- Dump cartridge using auto name and auto size: `romprg COM4 -c megadrive -dd -an`
- Dump cartridge using specified name and size: `romprg COM4 -c megadrive -dd -n name.bin -s 512k`
- Clear programmable cartidge: `romprg COM4 -c megadrive -dc`
- Burn data to cartridge from selected file: `romprg COM4 -c megadrive -db -n file.bin`
