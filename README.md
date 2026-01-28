pio -t menuconfig
    before test if deleted cmakelists/sdkconfig
pio test -vvv
    test whats in test

if its not detecting:
1. hold pwr button, press reset, lift pwr button
2. activate gpio2 (idk why this works sometimes)
3. reset board (press reset without pwr button)
4. disconnect from computer, hold pwr button, connect to computer, lift pwr button
5. retry from step 1

D0 = GPIO2
D1 = GPIO3
i cant find (?) GPIO 0/1

workingsleep.cpp puts it into deep sleep and can be activated via reed switch
reed switch has low refresh rate so need to hold it near magnet for a while
have not made timer enable yet