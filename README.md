# lcd-billboard
The Billboard (LCD AD Scheduler)

Github repo:
https://github.com/NoJi-nx/lcd-billboard

Wokwi link:
https://wokwi.com/projects/458899997912236033


## Översikt

Detta är ett projekt för ett digitalt reklamskyltsystem med en ATmega328P och en 16x2 LCD-skärm. Det visar annonser från flera kunder med olika krav och prioriteringar. Systemet stöder flera visningslägen (statisk, rullande, blinkande) och använder viktat slumpmässigt urval. Det säkerställer också att samma kund aldrig visas två gånger i rad och inkluderar tidsregeln för specifika annonser.

Implementeringen är skriven i ren C utan Arduino-bibliotek, med hjälp av  registermanipulation och en anpassad LCD-drivrutin.