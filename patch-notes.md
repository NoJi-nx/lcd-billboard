# Patch- Notes

## Part 3

#### 3.1 Enum för visning & regel
```C
//display läge enum, hantera olika visningslägen
typedef enum {
    MODE_STATIC,
    MODE_SCROLL,
    MODE_BLINK
} DisplayMode;

//regel enum, växla mellan olika lägen
typedef enum {
    RULE_NONE,
    RULE_EVEN_MINUTE,
    RULE_ODD_MINUTE
} RuleTy
```

## Part 2

#### 2.5 Testar + mixar display funktioner

```C
  //loop som visar olika annonser vid olika lägen
    while (1) {
       show_static("Static Demo", 2000);
       show_scroll("This is scrolling text on LCD", 250);
       show_blink("Blink Demo", 3000, 400)

    }
```

#### 2.4 Implementerar blink läge
```C
//visar blinkande text, implementera blinkande läge
void show_blink(const char* text, uint16_t total_duration_ms, uint16_t blink_interval_ms) {
    uint16_t elapsed = 0;

    while (elapsed < total_duration_ms) {
        lcd_clear();
        lcd_set_cursor(0,0);
        lcd_print(text);
        delay_ms_safe(blink_interval_ms);
        elapsed += blink_interval_ms;

        if (elapsed >= total_duration_ms) break;

        lcd_clear();
        delay_ms_safe(blink_interval_ms);
        elapsed += blink_interval_ms;
    }
}
```

Testar blinkande läge
```C
//loop som visar olika annonser vid olika lägen
    while (1) {
       show_blink("Harry Cars", 3000, 500); //blinkande
    }
```


#### 2.3 Implementera scroll-läge + helper finktion

```C
//helper funktion för att räkna längden på en string
uint8_t string_length(const char* str) {
    uint8_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

//visar scrollande text, implementera scrollande läge
void show_scroll(const char* text, uint16_t step_delay_ms) {
    uint8_t len = string_length(text);

    if (len <= 16) {
        show_static(text, 2000);
        return;
    }

    for (uint8_t start = 0; start <= len - 16; start++) {
        lcd_clear();
        lcd_set_cursor(0,0);

        for (uint8_t i = 0; i < 16; i++) {
            lcd_data(text[start + i]);
        }

        delay_ms_safe(step_delay_ms);
    }
}
```

Testar scroll lägen
```C
 //loop som visar olika annonser vid olika lägen
    while (1) {
       show_scroll("Buy a car from Harry", 300); //scrollande
    }

```

#### 2.2 Säker delay funktion + Testa statisk läge

```C
//säker delay funktion
void delay_ms_safe(uint16_t ms) {
    while (ms--) {
        _delay_ms(1);
    }
}


//visar statisk text, implementera statisk läge
void show_static(const char* text, uint16_t duration_ms){
    lcd_clear();
    lcd_set_cursor(0,0);
    lcd_print(text);
    delay_ms_safe(duration_ms); //använder säkra funktionen
}


```

Testar Statisk läge

```C
    //loop som visar olika annonser
    while (1) {
       show_static("Static Test", 2000);
       show_static("It Works!", 2000);
    }
``` 


#### 2.1 Display funktioner + statisk läge

```C
//display funktioner
void show_static(const char* text, uint16_t duration_ms);
void show_scroll(const char* text, uint16_t step_delay_ms);
void show_blink(const char* text, uint16_t total_duration_ms, uint16_t blink_interval_ms);


//visar statisk text, implementera statisk läge
void show_static(const char* text, uint16_t duration_ms){
    lcd_clear();
    lcd_set_cursor(0,0);
    lcd_print(text);
    _delay_ms(duration_ms);
}


```
#### 1.1 Setup

```C
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// mappar LCD pins till MCU pins
#define LCD_RS PD2
#define LCD_E PD3
#define LCD_D4 PD4
#define LCD_D5 PD5
#define LCD_D6 PD6
#define LCD_D7 PD7


//sätter LCD pins som outputs
void lcd_pins_init() {
    DDRD |= (1 << LCD_RS) |
            (1 << LCD_E) |
            (1 << LCD_D4) |
            (1 << LCD_D5) |
            (1 << LCD_D6) |
            (1 << LCD_D7) ;


}

//tillsätterr E pin
void lcd_enable_pulse() {
    PORTD |= (1 << LCD_E);
    _delay_us(1);
    PORTD &= ~(1 << LCD_E);
    _delay_us(100);
}


void lcd_send_nibble(uint8_t data){
    if (data & 0x01) PORTD |= (1 << LCD_D4);
    else PORTD &= ~(1 << LCD_D4);

    if (data & 0x02) PORTD |= (1 << LCD_D5);
    else PORTD &= ~(1 << LCD_D5);

    if (data & 0x04) PORTD |= (1 << LCD_D6);
    else PORTD &= ~(1 << LCD_D6);

    if (data & 0x08) PORTD |= (1 << LCD_D7);
    else PORTD &= ~(1 << LCD_D7);

    lcd_enable_pulse();
}

void lcd_send_byte(uint8_t data, uint8_t rs){
    if (rs)
        PORTD |= (1 << LCD_RS); //Data

        else
        PORTD &= ~(1 << LCD_RS); //Command

        lcd_send_nibble(data >> 4);
        lcd_send_nibble(data & 0x0f); 

        _delay_us(50);

}

void lcd_command(uint8_t cmd) {
    lcd_send_byte(cmd, 0);
}

void lcd_data(uint8_t data) {
    lcd_send_byte(data, 1);
}

//lcd initiliaziton
void lcd_init() {
    _delay_ms(50); 

    lcd_send_nibble(0x03); //funktion 
    _delay_ms(5);
    lcd_send_nibble(0x03); //funktion 
    _delay_us(150);
    lcd_send_nibble(0x03); //funktion
    _delay_us(150);
    lcd_send_nibble(0x02); //skickar 4 bit mode
    _delay_us(150);

    lcd_command(0x28); // 4-bit, 2 line, 5x8 dots
    lcd_command(0x0C); 
    lcd_command(0x06); 
    lcd_command(0x01); 
    _delay_ms(2);
}

//printar string
void lcd_print(const char* str) {
    while (*str) {
        lcd_data(*str++);
    }
}

//sättter cursor position
void lcd_set_cursor(uint8_t col, uint8_t row) {
    uint8_t address = (row == 0) ? 0x00 : 0x40;
    lcd_command(0x80 | (address + col));
}

//visar skärmen på LCD
void lcd_clear() {
    lcd_command(0x01);
    _delay_ms(2);
}

int main(void) {
    //setup
    lcd_pins_init();
    lcd_init();

    //text som visar på LCD
    lcd_print("Billboard");
    lcd_set_cursor(0,1);
    lcd_print("Online");
    _delay_ms(2000);


    //loop som visar olika annonser
    while (1) {
        lcd_clear();
        lcd_print("Ad 1");
        _delay_ms(2000);

        lcd_clear();
        lcd_print("Ad 2");
        _delay_ms(2000);
    }
}

```