# LED Demo

## Description
This demo prints a string on a 8x8 LED dot-matrix display.

## Resources
- Getting Started:  
    * Teaching Material: https://wiki.hensoldt-cyber.systems/display/HEN/TRENTOS-M+Teaching+Material
    * seL4 and CAmkES tutorial: https://docs.sel4.systems/Tutorials/
    * CAmkES manifest: https://github.com/seL4/camkes-tool/blob/master/docs/index.md  

- Source Code resources:  
    * Look at RPI\_SPI\_FLASH component in TRENTOS-M  
    * Driver component translated from https://os.mbed.com/teams/Maxim-Integrated/code/MAX7219/    
    * Code that runs on max7219: https://os.mbed.com/components/MAX7219-64-LED-Display-Driver-or-8-Digit/  

- Datasheets:
    * MAX7219: https://datasheets.maximintegrated.com/en/ds/MAX7219-MAX7221.pdf    
    * 1088AS LED: https://www.velleman.eu/downloads/29/infosheets/vmp502\_led8x8\_1088as.pdf  

## Application 
- This demo is build using the SD-card switcher: https://wiki.hensoldt-cyber.systems/display/HEN/SD-Card+Switcher
- Build demo:  
```
sdk/scripts/open_trentos_build_env.sh sdk/build-system.sh sdk/demos/demo_led_app_rpi3/src rpi3 build-rpi3-Debug-demo_led_app -DCMAKE_BUILD_TYPE=Debug
```
- Get `<sd_device_number>` with:
```
sudo sd-mux-ctrl -l
```
- Get `<block_device_name>` with:
```
sudo sd-mux-ctrl -e <sd_device_number> -s
lsblk
```
- Copy `os_image.bin` to Raspberry Pi with SD-card switcher:
```
sudo sd-mux-ctrl -e <sd_device_number> -s
sudo mount /dev/<block_device_name> /mnt
cd <path_to_trentos_dir>/build-rpi3-Debug-demo_led_app/images
sudo cp os_image.bin /mnt
sync
sudo umount /mnt
sudo sd-mux-ctrl -e <sd_device_number> -d
```
- When booting up the Raspberry Pi, the demo application should run now.

## For the future
- implement the scrolling function with framebuffer and viewport -> what interface functions do we want?
- separate struct for framebuffer and viewport -> implement functions: drawPixel(),dumpViewport(),drawCharacter()
