For hver motor er det 3 kabler, minst en av disse bør være input
Jeg setter av 1 pin per motor, fordi jeg mener på at de andre var unødvendige.

OutputTilHoyre => PA6 (Tim3 Ch1)
OutputTilVenstre => PB0 (Tim3 Ch3)



Potensimeter joysticken har to potensiometere i seg, jeg antar at disse gir ut to forsjellige verdier til to forskjellige pins.
Setter derfor av 2 ADC pins:

ADC_Xakse => PA3 (Gjennom ADC1 IN3)
ADC_Yakse => PA1 (Gjennom ADC1 In1)

Har gått in her på STM filen og satt "Scan conversion mode" til enable. "Number of Conversions" til 2. "rank 1" til Channel 1 og "rank 2" til Channel 3