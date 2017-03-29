del Total.hex
mergehex.exe --merge s110_nrf51_8.0.0_softdevice.hex bootloader.hex --output OTATotal.hex
mergehex.exe --merge OTATotal.hex application.hex --output Total.hex
del OTATotal.hex
pause
