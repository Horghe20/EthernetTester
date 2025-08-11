#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>
#include <EtherCard.h>

#define MINPRESSURE 200
#define MAXPRESSURE 1000

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define GREY 0x7BEF
#define SIZE 4

void (*resetFunc)(void) = 0;

int analogInput = 6; //per leggere i volt della batteria

byte address[SIZE] = {0, 0, 0, 0};

unsigned int selectN = 0; //variabile per scegliere quale indirizzo modificare

void setup()
{

    pinMode(analogInput, INPUT);
    Serial.begin(57600);
    inizialize();
    inizializeModule(getDisplay());
    /*tft.setCursor(100, 310);
  tft.setTextSize(4);
  tft.setTextColor(YELLOW);
  tft.print("HORGHE");
  delay(1500);
  tft.fillRect(100,310, 420, 40, BLACK);
  tft.setTextSize(2);
  for(int i = 477; i > 0; i= i-4)
    tft.drawTriangle(0,i,160,0,320,i, MAGENTA);*/
    setInitMenu();
}

void loop()
{

    if (getPingOnCount() != 0)
    {   
        ping();
        if (getPingOnCount() >0)
        {
            setPingOnCount(0);
            delay(2000);
            getDisplay()->fillRect(20, 400, 320, 60, BLACK);
            messagePrint("Fine PIGN", BLUE, "", BLACK, true);
            setReceivePacket(true);
            setBlockInterface(false);
        }
        else{
            messagePrint("Host non rag", RED, "", BLACK, true);
            setBlockInterface(false);
            setPingOnCount(0);
        }
        
        
    }
    else
    {
        if (getReceivePacket())
        {
            ether.packetLoop(ether.packetReceive());
            if (getActiveMenu() == 0)
                ether.registerPingCallback(gotPinged);
        }
    }

    if (!getBlockInterface())
    {
        switch (getActiveMenu())
        {

        case 0:
        {
            if (getCreateInterface(0))
            {
                setInitMenu();
                setCreateInterface(0, false);
            }
            controlInitMenu();
        }
        break;
        case 1:
        {
            if (getCreateInterface(1))
            {
                setDinamicInterface();
                setCreateInterface(1, false);
            }
            if (getAllowSerch())
            {
                messagePrint("Ricerca DHCP", BLUE, "Max 30s", WHITE, false);
                if (setUpDHCP())
                {
                    messagePrint("DHCP TROVATO", GREEN, "", BLACK, true);
                    setReceivePacket(true);
                }
                else
                    messagePrint("NO DHCP", RED, "", BLACK, true);
                setAllowSearch(false);
                setCreateInterface(1, true);
            }
            else
                controlDynamicInterface();
        }
        break;
        case 2:
        {
            if (getCreateInterface(2))
            {
                setStaticInterface();
                setCreateInterface(2, false);
            }
            if (getAllowSetIP())
            {
                messagePrint("Ricerca GT", BLUE, "Max 30s", WHITE, false);
                if (setUpStaticIP(getIp(), getGt(), getNm()))
                {
                    setReceivePacket(true);
                    setHaveIP(true);
                    messagePrint("Gateway Trovato", GREEN, "", BLACK, true);
                }
                else
                     messagePrint("GT Non Trovato", RED, "", BLACK, true);
                

                setAllowSetIP(false);
                setCreateInterface(2, true);
            }
            else
                controlStaticInterface();
        }
        break;
        case 3:
        {
            if (getCreateInterface(3))
            {
                switch (selectN)
                {
                case 0:
                    keyInit("IP:");
                    break;
                case 1:
                    keyInit("NetMask:");
                    break;
                case 2:
                    keyInit("GateWay:");
                    break;
                case 3:
                    keyInit("Ping IP:");
                    break;
                }
                setCreateInterface(3, false);
            }
            controlKeyInterface();
        }
        break;
        }
    }
}

//********************GESTIONE SCHERMO****************************
//FUNZIONI DI CONTROLLO
