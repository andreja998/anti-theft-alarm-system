sbit LCD_RS at LATB4_bit;
sbit LCD_EN at LATB5_bit;
sbit LCD_D4 at LATB0_bit;
sbit LCD_D5 at LATB1_bit;
sbit LCD_D6 at LATB2_bit;
sbit LCD_D7 at LATB3_bit;
sbit LCD_RS_Direction at TRISB4_bit;
sbit LCD_EN_Direction at TRISB5_bit;
sbit LCD_D4_Direction at TRISB0_bit;
sbit LCD_D5_Direction at TRISB1_bit;
sbit LCD_D6_Direction at TRISB2_bit;
sbit LCD_D7_Direction at TRISB3_bit;

void KontrolerRada();
void UnosSaTastature();
void CekajOtpustTastera();
void interrupt();
void IspisiLogo();
void PromeniUlaznoIzlaznoVreme(short);
void IzracunajVreme();
void IspisiVreme();
void UkljuciSigurnosniMod();
void Resetuj();

short trenutnoPritisnutiTaster, i, unosBrojac = 0;

short meniStanje       = 0, postojiLiSifra = 0, pokreniVreme        = 0, prvaZvezdica       = 0, programskiMod      = 0, detektovan         = 0, unesiteSifru = 0,
      sigurnosniMod    = 0, alarmUkljucen  = 0, promenaSifre        = 0, promenaUlIzVremena = 0, prvaCifra          = 0, postojiLiUlIzVreme = 0,
      ulazIzlazTajmer  = 0, ampm           = 1, daLiJeVremePodeseno = 0, podesitiVreme      = 0, mozePrikazatiVreme = 0, napoljuUnutra      = 0,
      uSigurnosnomModu = 0;

int   sifraUnos[4], sifraCitanje[4], sifraProcitana, sifraUneta, ulaznoIzlaznoVreme = 10, tajmer1Brojac = 0, tajmer0Brojac = 0, temp;
int   sat = 100, minut = 100, sekund = 100;
char  ispisiCifru, ispisiUlIzVreme[4], _sat[7], _minut[7], _sekund[7], ispisiTacnoVreme[7];


void main() {
     ANSELA = ANSELC = ANSELB = ANSELE = ANSELD = 0;
     TRISC        = 0b11100000;
     TRISB        = 0x00;
     TRISE        = 0x01;
     TRISD        = 0x00;
     TRISD.B7     = 1;
     LATD         = 0;
     LATD.B2      = 1;
     T1CON        = 0x30;
     TMR1IF_bit   = 0;
     TMR1H        = 0x00;
     TMR1L        = 0x00;
     TMR1IE_bit   = 1;
     INTCON.PEIE  = 1;
     INTCON.GIE   = 1;

     T0CON.T08BIT = 0;
     T0CON.PSA    = 0;
     T0CON.T0PS0  = 1;
     T0CON.T0PS1  = 0;
     T0CON.T0PS2  = 1;
     TMR0H        = 0x85;
     TMR0L        = 0xEF;
     T0CON.T0CS   = 0;
     TMR0IF_bit   = 0;
     TMR0ON_bit   = 0;
     TMR0IF_bit   = 1;
     TMR0IE_bit   = 1;

     ulazIzlazTajmer = 0;

     Sound_Init(&PORTE, 1);
     Lcd_Init();
     LATE.B2      = 0;

     Lcd_Cmd(_LCD_CLEAR);
     Delay_ms(30);
     Lcd_Out(1, 1, "   UCITAVANJE   ");
     Lcd_Out(2, 1, "    SISTEMA!    ");
     postojiLiSifra      = EEPROM_Read(0x00 + 4);
     Delay_ms(30);
     postojiLiUlIzVreme  = EEPROM_Read(0x00 + 5);
     Delay_ms(30);
     daLiJeVremePodeseno = EEPROM_Read(0x00 + 7);
     Delay_ms(30);
     uSigurnosnomModu    = EEPROM_Read(0x00 + 8);
     Delay_ms(30);

     if (uSigurnosnomModu == 1)
     {
          UkljuciSigurnosniMod();
     }
     if (postojiLiUlIzVreme != 1)
     {
          EEPROM_Write(0x00 + 6, 15);
          Delay_ms(30);

          EEPROM_Write(0x00 + 5, 1);
          Delay_ms(30);

          ulaznoIzlaznoVreme = 15;
     }
     else
     {
          ulaznoIzlaznoVreme = EEPROM_Read(0x00 + 6);
          Delay_ms(30);
     }
     if (postojiLiSifra != 1)
     {
          EEPROM_Write(0x00 + 0, 9);
          Delay_ms(30);
          EEPROM_Write(0x00 + 1, 8);
          Delay_ms(30);
          EEPROM_Write(0x00 + 2, 7);
          Delay_ms(30);
          EEPROM_Write(0x00 + 3, 6);
          Delay_ms(30);

          EEPROM_Write(0x00 + 4, 1);
          Delay_ms(30);
     }
     Delay_ms(2000);
     if (uSigurnosnomModu != 1)
          IspisiLogo();
     while (1)
     {
          if (PORTD.B7 == 1 && sigurnosniMod == 1 && alarmUkljucen == 0 && detektovan == 0)
          {

               detektovan      = 1;
               tajmer0Brojac   = 0;
               ulazIzlazTajmer = 1;
               TMR0ON_bit      = 1;
               Lcd_Cmd(_LCD_CLEAR);
               Delay_ms(50);
               Lcd_Out(1, 1, "  ULAZNO VREME  ");
               Lcd_Cmd(_LCD_SECOND_ROW);
               unesiteSifru    = 1;
               unosBrojac      = 0;
          }
          if (PORTE.B0 == 1 && alarmUkljucen == 0 && sigurnosniMod == 1)
          {
               LATD.B0       = 1;
               LATD.B4       = 1;
               alarmUkljucen = 1;
               Lcd_Cmd(_LCD_CLEAR);
               Delay_ms(50);
               Lcd_Out(1, 1, "ALARM ZONA 2");
               Lcd_Cmd(_LCD_SECOND_ROW);
               Sound_Play(1000, 5000);
               unesiteSifru    = 1;
               unosBrojac      = 0;
               TMR0ON_bit      = 0;
               tajmer0Brojac   = 0;
          }
          if (tajmer1Brojac >= 30)
          {
               if (sigurnosniMod == 1)
               {
                    Lcd_Cmd(_LCD_CLEAR);
                    Delay_ms(50);
                    Lcd_Out(1, 1, "Vasa sifra->");
                    Delay_ms(50);
                    unesiteSifru    = 1;
                    unosBrojac      = 0;
                    tajmer1Brojac   = 0;
                    TMR1ON_bit      = 0;
               }
               else
               {
                    IspisiLogo();
                    meniStanje      = 0;
                    unesiteSifru    = 0;
                    unosBrojac      = 0;
                    tajmer1Brojac   = 0;
                    TMR1ON_bit      = 0;
               }
          }
          KontrolerRada();
          if (tajmer0Brojac >= ulaznoIzlaznoVreme) // && ulazIzlazTajmer == 1)
          {
               if (sigurnosniMod == 1)
               {
                    Lcd_Cmd(_LCD_CLEAR);
                    Delay_ms(50);
                    Lcd_Out(1, 1, "ALARM ZONA 1");
                    Lcd_Cmd(_LCD_SECOND_ROW);
                    Sound_Play(1000, 5000);
                    unosBrojac      = 0;
                    alarmUkljucen   = 1;
                    LATD.B4         = 1;
                    TMR0ON_bit      = 0;
                    tajmer0Brojac   = 0;
                    LATD.B1         = 1;
               }
               else
               {
                    Lcd_Cmd(_LCD_CLEAR);
                    Delay_ms(50);
                    Lcd_Out(1, 1, " SIGURNOSNI MOD ");
                    Delay_ms(1000);
                    sigurnosniMod      = 1;
                    alarmUkljucen      = 0;
                    tajmer0Brojac      = 0;
                    unosBrojac         = 0;
                    napoljuUnutra      = 0;
                    //ulazIzlazTajmer    = 0;
                    TMR0ON_bit         = 0;
               }
          }
     }
}

void KontrolerRada()
{
     UnosSaTastature();
     if (trenutnoPritisnutiTaster != 13)
     {
          Sound_Play(1000, 60);
          if (detektovan == 0 && programskiMod == 0 && podesitiVreme == 0)
          {
               TMR1H      = 0x00;
               TMR1L      = 0x00;

               TMR1ON_bit = 1;
          }
          if (meniStanje == 0 && unesiteSifru == 0 && sigurnosniMod == 0 && programskiMod == 0 && promenaUlIzVremena == 0)//sigurnosniMod == 0 && unosBrojac == 0 && postaviSifru == 0 && unesiSifru == 0 && programskiMod == 0)
          {
               Lcd_Cmd(_LCD_CLEAR);
               Delay_ms(50);
               Lcd_Cmd(_LCD_CURSOR_OFF);
               Lcd_Out(1, 1, "1.Sigurnosni mod");

               meniStanje         = 1;
               unosBrojac         = 1;
               CekajOtpustTastera();
               return;
          }

          if (meniStanje == 1)
          {
               if (trenutnoPritisnutiTaster == 1)
               {
                    if (promenaUlIzVremena == 1)
                    {
                         PromeniUlaznoIzlaznoVreme(-1);
                         return;
                    }
                    if (programskiMod != 1)
                    {
                         Lcd_Cmd(_LCD_CLEAR);
                         Delay_ms(50);
                         Lcd_Out(1, 1, "Vasa sifra->");
                    }
                    else
                    {
                         Lcd_Cmd(_LCD_CLEAR);
                         Delay_ms(50);
                         Lcd_Out(1, 1, "Nova sifra->");
                         promenaSifre = 1;
                    }
                    unesiteSifru = 1;
                    unosBrojac   = 0;
                    meniStanje   = 0;
                    CekajOtpustTastera();
                    return;
               }
               if (trenutnoPritisnutiTaster == 2)
               {
                    if (promenaUlIzVremena == 1)
                    {
                         PromeniUlaznoIzlaznoVreme(1);
                         return;
                    }
                    meniStanje         = 0;
                    if (programskiMod == 1)
                    {
                         Lcd_Cmd(_LCD_CLEAR);
                         Lcd_Out(1, 1, "1-> -1s 2-> +1s");
                         Delay_ms(1500);
                         if (postojiLiUlIzVreme == 1)
                         {
                              ulaznoIzlaznoVreme = EEPROM_Read(0x00 + 6);
                              Delay_ms(30);
                         }
                         else
                              ulaznoIzlaznoVreme = 15;
                         temp = ulaznoIzlaznoVreme;
                         IntToStr(ulaznoIzlaznoVreme, ispisiTacnoVreme);
                         Lcd_Out(2, 1, "Novo vreme->");
                         Lcd_Out(2, 13, ispisiTacnoVreme + 2);
                         Delay_ms(50);
                         meniStanje    = 1;
                    }
                    unosBrojac         = 0;
                    promenaUlIzVremena = 1;
                    //ulazIzlazTajmer    = 0;
               }

               CekajOtpustTastera();
          }
          if (promenaUlIzVremena == 1)
          {
               if (trenutnoPritisnutiTaster == 10)
               {
                     EEPROM_Write(0x00 + 6, temp);
                     Delay_ms(30);
                     if (postojiLiUlIzVreme != 1)
                     {
                          EEPROM_Write(0x00 + 5, 1);
                          Delay_ms(30);
                     }
                     ulaznoIzlaznoVreme = temp;
                     promenaUlIzVremena = 0;
                     programskiMod      = 0;
                     unosBrojac         = 0;
                     meniStanje         = 0;
                     unesiteSifru       = 0;
                     //ulazIzlazTajmer    = 0;
                     CekajOtpustTastera();
                     Lcd_Cmd(_LCD_CLEAR);
                     Delay_ms(50);
                     Lcd_Out(1, 1, "  UL\IZ  Vreme  ");
                     Lcd_Out(2, 1, "   promenjeno   ");
                     Delay_ms(1500);
                     IspisiLogo();
               }
          }
          if (unesiteSifru == 1)
          {
               if (trenutnoPritisnutiTaster != 10 && trenutnoPritisnutiTaster != 11 && unosBrojac < 4)
               {
                    ShortToStr(trenutnoPritisnutiTaster, ispisiCifru);
                    Lcd_Out_CP(ispisiCifru + 3);
                    Delay_ms(50);
                    sifraUnos[unosBrojac] = trenutnoPritisnutiTaster;
                    unosBrojac++;
                    tajmer1Brojac = 0;
                    CekajOtpustTastera();
                    return;
               }
               else if (unosBrojac == 0 && trenutnoPritisnutiTaster == 11 && prvaZvezdica == 0 && programskiMod == 0)
               {
                    prvaZvezdica = 1;
                    unosBrojac   = 0;
                    CekajOtpustTastera();
                    return;
               }
               if ((trenutnoPritisnutiTaster == 10 || trenutnoPritisnutiTaster == 11) && unosBrojac >= 3 && napoljuUnutra == 0)
               {
                    if (promenaSifre == 1)
                    {
                         for (i = 0; i < 4; i++)
                         {
                              EEPROM_Write(0x00 + i, sifraUnos[i]);
                              Delay_ms(30);
                         }
                         Lcd_Cmd(_LCD_CLEAR);
                         Delay_ms(50);
                         Lcd_Out(1, 1, "     Sifra      ");
                         Lcd_Out(2, 1, "   promenjena   ");
                         Delay_ms(1500);
                         meniStanje      = 0;
                         promenaSifre    = 0;
                         programskiMod   = 0;
                         unosBrojac      = 0;
                         unesiteSifru    = 0;
                         //ulazIzlazTajmer = 0;
                         CekajOtpustTastera();
                         IspisiLogo();
                         return;
                    }
                    for (i = 0; i < 4; i++)
                    {
                         sifraCitanje[i] = EEPROM_Read(0x00 + i);
                         Delay_ms(30);
                    }
                    sifraProcitana = sifraCitanje[0] * 1000 + sifraCitanje[1] * 100 + sifraCitanje[2] * 10 + sifraCitanje[3];
                    sifraUneta     = sifraUnos[0]    * 1000 + sifraUnos[1]    * 100 + sifraUnos[2]    * 10 + sifraUnos[3];
                    if (sifraUneta == sifraProcitana)
                    {
                         promenaUlIzVremena = 0;
                         if (trenutnoPritisnutiTaster == 10)
                         {
                              if (sigurnosniMod == 1)
                              {
                                   alarmUkljucen      = 0;
                                   detektovan         = 0;
                                   napoljuUnutra      = 0;
                                   unosBrojac         = 0;
                                   unesiteSifru       = 0;
                                   sigurnosniMod      = 0;
                                   tajmer0Brojac      = tajmer1Brojac = 0;
                                   TMR0ON_bit         = 0;
                                   TMR1ON_bit         = 0;
                                   prvaZvezdica       = 0;
                                   LATD.B0 = LATD.B1  = LATD.B4 = 0;
                                   LATD.B2 = 1;
                                   EEPROM_Write(0x00 + 8, 0);
                                   Delay_ms(30);
                                   CekajOtpustTastera();
                                   IspisiLogo();
                                   return;
                              }
                              UkljuciSigurnosniMod();
                              //sigurnosniMod = 1;
                         }
                         else
                         {
                              if (sigurnosniMod == 0)
                              {
                                   for (i = 0; i < 3; i++)
                                   {
                                        Delay_ms(250);
                                        Lcd_Cmd(_LCD_CLEAR);
                                        Delay_ms(250);
                                        Lcd_Out(1, 1, " PROGRAMSKI MOD ");
                                        Delay_ms(250);
                                   }
                                   programskiMod = 1;
                                   Lcd_Cmd(_LCD_CLEAR);
                                   Lcd_Out(1, 1, "1.Nova sifra");
                                   Lcd_Out(2, 1, "2.UL/IZ vreme");
                                   Delay_ms(50);
                                   ulazIzlazTajmer = 0;
                                   prvaZvezdica    = 0;
                                   meniStanje      = 1;
                                   tajmer1Brojac   = 0;
                                   tajmer0Brojac   = 0;
                                   TMR1ON_bit      = 0;
                              }
                         }
                         CekajOtpustTastera();
                    }
                    else if (sifraUneta == 9753 && prvaZvezdica == 1 && trenutnoPritisnutiTaster == 10)
                    {

                         Lcd_Cmd(_LCD_CLEAR);
                         Lcd_Out(1, 1, "     SISTEM     ");
                         Lcd_Out(2, 1, "   RESETOVAN!   ");
                         Sound_Play(1000, 2000);
                         Delay_ms(1000);
                         Resetuj();
                         meniStanje      = 0;

                         unosBrojac      = 0;
                         unesiteSifru    = 0;
                         prvaZvezdica    = 0;
                         unosBrojac      = 0;
                         unesiteSifru    = 0;
                         tajmer0Brojac   = 0;
                         tajmer1Brojac   = 0;
                         ulazIzlazTajmer = 0;
                         CekajOtpustTastera();
                         IspisiLogo();
                    }
                    else
                    {
                         if (sigurnosniMod == 1)
                         {
                              Lcd_Cmd(_LCD_CLEAR);
                              Delay_ms(50);
                              Lcd_Out(1, 1, " Pogresna sifra ");
                              Delay_ms(1000);
                              Lcd_Cmd(_LCD_CLEAR);
                              Delay_ms(50);
                              Lcd_Out(1, 1, "Vasa sifra->");
                              unesiteSifru    = 1;
                              unosBrojac      = 0;
                              TMR0ON_bit      = 0;
                              tajmer0Brojac   = 0;
                              CekajOtpustTastera();
                              return;
                         }
                         else
                         {
                              Lcd_Cmd(_LCD_CLEAR);
                              Delay_ms(50);
                              Lcd_Out(1, 1, " Pogresna sifra ");
                              Delay_ms(1000);
                              unesiteSifru = 0;
                              unosBrojac   = 0;
                         }
                         tajmer0Brojac   = 0;
                         CekajOtpustTastera();
                         IspisiLogo();
                    }
               }
          }
     }
}

void UnosSaTastature()
{
     LATC = 0b11101110;
     if (!PORTC.B7)
     {
          Delay_ms(50);
          trenutnoPritisnutiTaster = 1;
          return;
     }
     if (!PORTC.B6)
     {
          Delay_ms(50);
          trenutnoPritisnutiTaster = 4;
          return;
     }
     if (!PORTC.B5)
     {
          Delay_ms(50);
          trenutnoPritisnutiTaster = 7;
          return;
     }
     LATC = 0b11101101;
     if (!PORTC.B7)
     {
          Delay_ms(50);
          trenutnoPritisnutiTaster = 2;
          return;
     }
     if (!PORTC.B6)
     {
          Delay_ms(50);
          trenutnoPritisnutiTaster = 5;
          return;
     }
     if (!PORTC.B5)
     {
          Delay_ms(50);
          trenutnoPritisnutiTaster = 8;
          return;
     }
     LATC = 0b11101011;
     if (!PORTC.B7)
     {
          Delay_ms(50);
          trenutnoPritisnutiTaster = 3;
          return;
     }
     if (!PORTC.B6)
     {
          Delay_ms(50);
          trenutnoPritisnutiTaster = 6;
          return;
     }
     if (!PORTC.B5)
     {
          Delay_ms(50);
          trenutnoPritisnutiTaster = 9;
          return;
     }
     LATC = 0b11100111;
     if (!PORTC.B7)
     {
          Delay_ms(50);
          trenutnoPritisnutiTaster = 10;   //Taraba
          return;
     }
     if (!PORTC.B6)
     {
          Delay_ms(50);
          trenutnoPritisnutiTaster = 11;   //Zvezda
          return;
     }
     if (!PORTC.B5)
     {
          Delay_ms(50);
          trenutnoPritisnutiTaster = 0;
          return;
     }
     trenutnoPritisnutiTaster = 13;
}

void CekajOtpustTastera()
{
     PORTC = 0b11100000;
     while(PORTC != 0b11100000)
          PORTC = 0b11100000;
     Delay_ms(30);
}

void IspisiLogo()
{
     Lcd_Cmd(_LCD_CLEAR);
     Delay_ms(50);
     Lcd_Cmd(_LCD_CURSOR_OFF);
     Lcd_Out(1, 1, " SREDNJA  SKOLA ");
     Lcd_Out(2, 1, "    LAJKOVAC    ");
     Delay_ms(50);
}

void interrupt()
{
     if (TMR1IF_bit == 1)
     {
          tajmer1Brojac++;
          TMR1IF_bit = 0;
     }
     if (TMR0IF_bit == 1)
     {
          TMR0H      = 0x85;
          TMR0L      = 0xEF;

          TMR0IF_bit = 0;
          tajmer0Brojac++;
     }
}

void PromeniUlaznoIzlaznoVreme(short plusMinus)
{
     if (temp > 0 || (temp == 0 && trenutnoPritisnutiTaster == 2))
     {
          short duzinaStringa;
          temp += plusMinus;
          //Lcd_Cmd(_LCD_CLEAR);
          //Delay_ms(50);
          Lcd_Out(1, 1, "1-> -1s 2-> +1s");
          Lcd_Cmd(_LCD_SECOND_ROW);
          Lcd_Out(2, 1, "Novo vreme->");
          IntToStr(temp, ispisiTacnoVreme);
          Lcd_Out(2, 13, ispisiTacnoVreme + 2);
          Delay_ms(20);
     }
     else
     {
          Lcd_Cmd(_LCD_CLEAR);
          Delay_ms(30);
          Lcd_Out(1, 1, "   MORA  BITI   ");
          Lcd_Out(2, 1, "   POZITIVNO    ");
          Delay_ms(1500);
          CekajOtpustTastera();
          Lcd_Out(1, 1, "1-> +1s 2-> -1s");
          Lcd_Cmd(_LCD_SECOND_ROW);
          Lcd_Out(2, 1, "Novo vreme->");
          Delay_ms(20);
     }
}

void UkljuciSigurnosniMod()
{
     TMR1ON_bit = 0;
     TMR0ON_bit = 1;
     tajmer1Brojac = 0;
     for (i = 0; i < 3; i++)
     {
          Delay_ms(250);
          Lcd_Cmd(_LCD_CLEAR);
          Delay_ms(250);
          Lcd_Out(1, 1, " IZLAZNO VREME ");
          Delay_ms(250);
     }
     tajmer0Brojac     = 0;
     pokreniVreme      = 1;
     prvaZvezdica      = 0;
     napoljuUnutra     = 1;
     uSigurnosnomModu  = 1;
     LATD.B2           = 0;
}

void Resetuj()
{
     EEPROM_Write(0x00 + 6, 15);
     Delay_ms(30);

     EEPROM_Write(0x00 + 5, 1);
     Delay_ms(30);


     EEPROM_Write(0x00 + 0, 9);
     Delay_ms(30);
     EEPROM_Write(0x00 + 1, 8);
     Delay_ms(30);
     EEPROM_Write(0x00 + 2, 7);
     Delay_ms(30);
     EEPROM_Write(0x00 + 3, 6);
     Delay_ms(30);

     EEPROM_Write(0x00 + 4, 1);
     Delay_ms(30);
}
