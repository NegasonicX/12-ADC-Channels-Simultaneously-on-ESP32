# Running 12 built-in ADC Channels Simultaneously on ESP32
A simple code to simulateously run 12 built-in ADCs on ESP32 at the same time.

# GPIO Functions:
I am using 12 GPIOs which have multiplexed functioning of ADCs

* GPIO36 ( ADC1 CHANNEL 0 )
* GPIO39 ( ADC1 CHANNEL 3 )
* GPIO34 ( ADC1 CHANNEL 6 )
* GPIO35 ( ADC1 CHANNEL 7 )
* GPIO32 ( ADC1 CHANNEL 4 )
* GPIO33 ( ADC1 CHANNEL 5 )
* GPIO25 ( ADC2 CHANNEL 8 )
* GPIO26 ( ADC2 CHANNEL 9 )
* GPIO27 ( ADC2 CHANNEL 7 )
* GPIO14 ( ADC2 CHANNEL 6 )
* GPIO4  ( ADC2 CHANNEL 0 )
* GPIO13 ( ADC2 CHANNEL 4 )

# Understanding the Flow:
* This code is developed for ESP32 on Embedded C Language using FreeRTOS.
* ESP32 is integrated with 2 SAR (Successive Approximation Register) which supports upto 18 channels as per espressif.
* All the ADCs have a maximum resolution of 12-bit ( variable among 9-bit, 10-bit, 11-bit & 12-bit ) and input voltage limit to 3.3v
* Among 18 channels, not all the ADC GPIOs are accessible by user due to several reasons like flash for WiFi drivers. Hence out of 18 channels, only 15 channels are truly free for use without any GPIO used in boot-up or Hall Sensor.
* You can set various attenuation dB to get desired range of voltages :

| Attenuation   | Measurable input voltage range |
| ------------- | ------------- |
| ADC_ATTEN_DB_0 | 100 mV ~ 950 mV |
| ADC_ATTEN_DB_2_5 | 100 mV ~ 1250 mV |
| ADC_ATTEN_DB_6 | 150 mV ~ 1750 mV |
| ADC_ATTEN_DB_11 | 150 mV ~ 2450 mV |

* Both ADC1 and ADC2 have a different way of execution and getting the raw value which I have clearly explained in the code.
* At the end, I am printing the raw ADC1 and raw ADC2 values at the end as well as the specified the channel for the same.

# Conclusion:
* There is nothing to conclude this time, it's super cool and works flawlessly.
* I hope you all like it :-D
  
