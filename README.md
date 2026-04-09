# lcd-billboard
The Billboard (LCD AD Scheduler)

Github repo:
https://github.com/NoJi-nx/lcd-billboard

Wokwi link:
https://wokwi.com/projects/458899997912236033


## Översikt

Detta är ett projekt för ett digitalt reklamskyltsystem med en ATmega328P och en 16x2 LCD-skärm. Det visar annonser från flera kunder med olika krav och prioriteringar. Systemet stöder flera visningslägen (statisk, rullande, blinkande) och använder viktat slumpmässigt urval. Det säkerställer också att samma kund aldrig visas två gånger i rad och inkluderar tidsregeln för specifika annonser.

Implementeringen är skriven i ren C utan Arduino-bibliotek, med hjälp av  registermanipulation och en anpassad LCD-drivrutin.

## Systemarkitektur

Systemet är uppdelat i flera  moduler Var och en ansvarar för en specifik del av funktionaliteten:

### LCD-drivrutin (lågnivå)
Hanterar all kommunikation med LCD-skärmen med hjälp av direkt registermanipulation. Den implementerar ett anpassat 4-bitarsgränssnitt, inklusive funktioner för att skicka kommandon, skriva data, styra markören och rensa displayen.

### Displaymotor
Ansvarar för att visa text på LCD-skärmen. Den inkluderar tre visningslägen:

* Statisk
* Rullning
* Blinkande

Varje läge implementeras som en återvinnsfunktion och fungerar oberoende av schema logiken.
<br>

### Datamodell (kunder och annonser)
Systemet använder strukturerade datatyper (struct) för att representera kunder och annonser. 
Varje kund innehåller:
* Ett namn
* En vikt (baserat på betalning)
* En lista med annonser

<br>

Varje annons innehåller:
* Text
* Visningsläge
* Valfri regellogik
<br>

Denna design gör systemet skalbart och enkelt att utöka.

### Schemaläggare (Scheduler)
Styr systemets exekveringsflöde. Den:
* Väljer en kund
* Visar kundens namn
* Väljer en giltig annons
* Kör annonsen under en fast tidslucka


Schemaläggaren körs kontinuerligt i huvudloopen.

### Slumpmässigt urval (Random Selection)
Implementerar en  slumpmässig algoritm för att välja kunder baserat på deras betalning. Säkerställer vissa villkor:
* Högre betalande kunder visas oftare
* Samma kund aldrig väljs två gånger i rad
