
//VARIABILI PER SCHERMO
MCUFRIEND_kbv tft;

char *name = "My LCD";
const int XP = 6, XM = A2, YP = A1, YM = 7; //320x480 ID=0x9486
const int TS_LEFT = 939, TS_RT = 159, TS_TOP = 948, TS_BOT = 154;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

int pixelX, pixelY;

//indirizzi ip
byte ip[SIZE] = {0, 0, 0, 0};
byte nm[SIZE] = {0, 0, 0, 0};
byte gt[SIZE] = {0, 0, 0, 0};
byte pIp[SIZE] = {0, 0, 0, 0};
bool havePIp = false;

unsigned int _ip[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned int _ipPos = 0;

// 0 menuIniziale 1 interfaccia dinamica 2 interfacia statica 3 tastierino
byte activeMenu = 0;
bool createInterface[4] = {false, false, false, false}; // per determinare se l'interfaccia è gia stata creata --- true devi crearlo false non devi crearlo

MCUFRIEND_kbv *getDisplay()
{
    return &tft;
}

byte *getIp()
{
    return ip;
}
byte *getNm()
{
    return nm;
}
byte *getGt()
{
    return gt;
}
byte *getPip()
{
    return pIp;
}

bool getHavePIp()
{
    return havePIp;
}

void setHavePIp(bool v)
{
    havePIp = v;
}

void inizialize()
{
    int id = tft.readID();
    tft.begin(id);
    tft.setRotation(0);
    tft.fillScreen(BLACK);
}

bool getPoint()
{
    TSPoint tp; //dove vengono passate le costanti
    bool pressed = false;
    tp = ts.getPoint();

    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);
    digitalWrite(XM, HIGH);

    if ((tp.z > MINPRESSURE) && (tp.z < MAXPRESSURE))
    {
        pressed = true;
        pixelX = map(tp.x, TS_LEFT, TS_RT, 0, tft.width()); //converto le coordinate da 0 a 1024 in pixel
        pixelY = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}

byte getActiveMenu()
{
    return activeMenu;
}

bool getCreateInterface(byte pos)
{
    return createInterface[pos];
}

void setCreateInterface(byte pos, bool val)
{
    createInterface[pos] = val;
}

void printIP(unsigned int a[12])
{

    for (int i = 0; i < 12; i++)
    {
        if (i % 3 == 0 && i > 1)
            tft.print(".");
        tft.print(a[i]);
    }
}

void printIP1(uint8_t ip[4])
{
    for (size_t i = 0; i < 4; i++)
    {
        tft.print(ip[i]);
        if (i != 3)
            tft.print(".");
    }
}

bool check3IP(unsigned int a[12], unsigned int initPos, unsigned int finalPos)
{
    unsigned int numb = a[initPos];
    for (int i = (initPos + 1); i <= finalPos; i++)
    {
        numb *= 10;
        numb += a[i];
    }
    if (numb > 255)
    {
        for (int i = initPos; i <= finalPos; i++)
            a[i] = 0;
        return false;
    }
    else
        return true;
}

void messagePrint(char *message, int c1, char *des, int c2, bool disappear)
{
    tft.fillRect(10, 400, 320, 40, BLACK);
    tft.setCursor(10, 400);
    tft.setTextColor(c1);
    tft.setTextSize(3);
    tft.print(message);
    tft.setCursor(10, 425);
    tft.setTextColor(c2);
    tft.setTextSize(1);
    tft.print(des);
    if (disappear)
    {
        delay(2000);
        tft.fillRect(10, 400, 320, 40, BLACK);
    }
}

//funzione ricorsiva che accetta inizio e fine ed effettua un for  per unificare i valori
bool setMaskCheck = true; //per abilitare la riscrittura automatica della netmask
void trasformAddress(unsigned int a[], byte b[])
{
    int j = 0;
    for (size_t i = 0; i < SIZE; i++)
    {
        b[i] = ((a[j] * 100) + (a[(j + 1)] * 10) + (a[(j + 2)]));
        j += 3;
    }
    setMaskCheck = true;
}

//LOGICA E SETTAGGIO BOTTONE DI RITORNO
Adafruit_GFX_Button returnBtn;

void setReturnBtn()
{
    returnBtn.initButton(&tft, 20, 20, 30, 40, WHITE, CYAN, BLACK, "<-|", 2);
    returnBtn.drawButton(false);
}

void returnBtnFunc(int window)
{

    //questa funzione ha solo la logica bisogna posizionarlo per usarlo
    bool down = getPoint();

    returnBtn.press(down && returnBtn.contains(pixelX, pixelY));

    if (returnBtn.justPressed())
        returnBtn.drawButton(true);

    if (returnBtn.justReleased())
    {
        returnBtn.drawButton(false);
        activeMenu = window;
        createInterface[window] = true;
    }
}
void readVolt()
{
  
    float vout = 0.0, vin = 0.0, R1 = 10000, R2 = 10000; // inserire valore esatto resistenza R1 (10K)
    int value = 0;

    /*for (size_t i = 0; i < 1; i++)
    {
        value += analogRead(analogInput);
        delay(50);
    }*/

    //value /= 6;

    value = analogRead(analogInput);
    vout = map (value, 0, 1023, 0, 500);
    //vin =  ((vout -0.25) * 2) / 100;
    setBattery((vout - 30));
}

void setBattery(float perCent)
{
    
    tft.fillRect(292, 5, 21, 8, BLACK);
    perCent -= 412;
    perCent = perCent < 0 ? 0 : perCent;
    perCent = ((perCent / 58) * 100); //max 8
    perCent = perCent > 100 ? 100 : perCent;
    tft.setTextSize(1);
    tft.setCursor(267, 6);
    tft.print((int)perCent);
    tft.print("%"); 
    tft.fillRect(313, 6, 3, 6, WHITE);
    tft.drawRect(290, 3, 24, 12, WHITE);
    //logica per calclare la batteria effettiva
    tft.fillRect(292, 5, (0.20 * perCent), 8, WHITE);


}

//LOGICA E GRAFICA PAGINA 0 -- MENU -- LIVELLO 0
Adafruit_GFX_Button btn[7]; //bottoni menu iniziale
//LOGICA E GRAFICA BATTEIRA


void setInitMenu()
{
    tft.fillScreen(BLACK);
    readVolt();
    tft.fillCircle(8, 8, 6, getHaveIP() ? GREEN : RED);
    btn[0].initButton(&tft, 160, 100, 200, 40, WHITE, CYAN, BLACK, "DHCP", 2);
    btn[1].initButton(&tft, 160, 160, 200, 40, WHITE, CYAN, BLACK, "STATIC", 2);
    btn[2].initButton(&tft, 105, 220, 90, 40, WHITE, CYAN, BLACK, "GOOGLE", 2);
    btn[6].initButton(&tft, 210, 220, 90, 40, WHITE, CYAN, BLACK, "GW", 2);
    btn[5].initButton(&tft, 105, 280, 90, 40, WHITE, CYAN, BLACK, "SET IP", 2);
    btn[3].initButton(&tft, 210, 280, 90, 40, WHITE, CYAN, BLACK, "PING", 2);
    btn[4].initButton(&tft, 160, 340, 200, 40, WHITE, CYAN, BLACK, "RST", 2);
    tft.setCursor(20, 25);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("Tipologia di indirizzo:");
    for (size_t i = 0; i < 7; i++)
        btn[i].drawButton(false);

    tft.setCursor(10, 460);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.print("Assicurati che il cavo sia connesso e i Led");
    tft.setCursor(10, 470);
    tft.print("lampeggianti");
}

void controlInitMenu()
{
    bool down = getPoint();

    for (int i = 0; i < 7; i++)
    {
        btn[i].press(down && btn[i].contains(pixelX, pixelY));

        if (btn[i].justPressed())
            btn[i].drawButton(true);

        if (btn[i].justReleased())
        {
            btn[i].drawButton(false);
            switch (i)
            {
            case 0:
                activeMenu = 1;
                createInterface[1] = true;
                break;

            case 1:
                activeMenu = 2;
                createInterface[2] = true;
                break;
            case 2:
                if (getHaveIP())
                {
                    setPingInformation(true, NULL);
                    setPingOnCount(1);
                }
                else
                {
                    messagePrint("No IP", RED, "Assegna un ip", WHITE, true);
                    activeMenu = 0;
                    createInterface[0] = true;
                }
                break;
            case 3:
                if (getHaveIP())
                {
                    if (getHavePIp())
                    {
                        setPingInformation(false, pIp);
                        setPingOnCount(1);
                    }
                    else
                         messagePrint("No IP da Ping", RED, "Assegna un ip da pingare", WHITE, true);                    
                }
                else
                {
                    messagePrint("No IP", RED, "Assegna un ip", WHITE, true);
                    activeMenu = 0;
                    createInterface[0] = true;
                }
                break;
            case 4:
                resetFunc();
                break;
            case 5:
                if (getHaveIP())
                {
                    activeMenu = 3;
                    createInterface[3] = true;
                    selectN = 3;
                }
                else
                {
                    messagePrint("No IP", RED, "Assegna un ip", WHITE, true);
                    activeMenu = 0;
                    createInterface[0] = true;
                }
                break;

            case 6:
                if (getHaveIP())
                {
                  setPingInformation(false, ether.gwip);
                  setPingOnCount(1);                  
                }
                else
                {
                    messagePrint("No IP", RED, "Assegna un ip", WHITE, true);
                    activeMenu = 0;
                    createInterface[0] = true;
                }
            default:
                break;
            }
            delay(150);
        }
    }
}

//LOGICA E GRAFICA PAGINA 1 -- DHCP -- LIVELLO 1
Adafruit_GFX_Button runBtn; //per avviare la ricerca dhcp

void setDinamicInterface()
{
    tft.fillScreen(BLACK);
    setReturnBtn();
    runBtn.initButton(&tft, 250, 20, 30, 40, WHITE, CYAN, BLACK, "V", 2);
    runBtn.drawButton(false);
    tft.setCursor(10, 55);
    tft.setTextColor(CYAN);
    tft.setTextSize(2);
    tft.print("DHCP:");
    tft.setTextColor(WHITE);
    tft.setCursor(20, 90);
    tft.print("Indirizzo IP:");
    tft.setCursor(25, 115);
    printIP1(ether.myip);

    tft.setCursor(20, 150);
    tft.print("NetMask:");
    tft.setCursor(25, 175);
    printIP1(ether.netmask);

    tft.setCursor(20, 210);
    tft.print("Gateway:");
    tft.setCursor(25, 235);
    printIP1(ether.gwip);
}

void controlDynamicInterface()
{
    returnBtnFunc(0); //pagina 1 to 0

    bool down = getPoint();

    runBtn.press(down && runBtn.contains(pixelX, pixelY));

    if (runBtn.justPressed())
        runBtn.drawButton(true);

    if (runBtn.justReleased())
    {
        runBtn.drawButton(false);
        setAllowSearch(true);
        delay(150);
    }
}

//LOGICA E GRAFICA PAGINA 2 -- VISUALIZZAZIONE DATI IP STATICO -- LIVELLO 1
Adafruit_GFX_Button ipBtn, gtBtn, nmBtn, run2Btn;

void setNetMask()
{
    byte i = 0;
    for (i = 0; i < 4; i++)
        nm[i] = 0;

    for (i = 0; i < 12; i++)
        _ip[i] = 0;
    _ipPos = 0;

    byte n = ip[0];
    byte p = 0;

    if (n >= 0 && n <= 127)
        p = 3;
    if (n >= 128 && n <= 191)
        p = 6;
    if (n >= 192 && n <= 223)
        p = 9;

    _ipPos = p;
    for (i = 0; i < p; i++)
    {
        if (i == 0 || i % 3 == 0)
            _ip[i] = 2;
        else
            _ip[i] = 5;
    }
    trasformAddress(_ip, nm);
    for (i = 0; i < 12; i++)
        _ip[i] = 0;
    _ipPos = 0;
}

void setStaticInterface()
{
    tft.fillScreen(BLACK);
    setReturnBtn();
    run2Btn.initButton(&tft, 250, 20, 30, 40, WHITE, CYAN, BLACK, "V", 2);
    run2Btn.drawButton(false);
    tft.setCursor(10, 55);
    tft.setTextColor(CYAN);
    tft.setTextSize(2);
    tft.print("STATIC:");

    ipBtn.initButton(&tft, 26, 90, 110, 30, BLACK, BLACK, CYAN, "IP", 2); //cambiare misure
    ipBtn.drawButton(false);
    tft.setCursor(25, 115);
    tft.setTextColor(WHITE);
    printIP1(ip);

    nmBtn.initButton(&tft, 60, 150, 110, 30, BLACK, BLACK, CYAN, "NetMask", 2);
    nmBtn.drawButton(false);
    tft.setCursor(25, 175);
    tft.setTextColor(WHITE);
    if (setMaskCheck)
    {
        setNetMask();
        setMaskCheck = false;
    }
    printIP1(nm);

    gtBtn.initButton(&tft, 60, 210, 110, 30, BLACK, BLACK, CYAN, "Gateway", 2);
    gtBtn.drawButton(false);
    tft.setCursor(25, 235);
    tft.setTextColor(WHITE);
    printIP1(gt);
}

void controlStaticInterface()
{
    returnBtnFunc(0);
    bool down = getPoint();

    ipBtn.press(down && ipBtn.contains(pixelX, pixelY));

    if (ipBtn.justPressed())
        ipBtn.drawButton(true);

    if (ipBtn.justReleased())
    {
        ipBtn.drawButton(false);
        activeMenu = 3;
        createInterface[3] = true;
        selectN = 0;
        delay(150);
    }

    nmBtn.press(down && nmBtn.contains(pixelX, pixelY));

    if (nmBtn.justPressed())
        nmBtn.drawButton(true);

    if (nmBtn.justReleased())
    {
        nmBtn.drawButton(false);
        activeMenu = 3;
        createInterface[3] = true;
        selectN = 1;
        delay(150);
    }

    gtBtn.press(down && gtBtn.contains(pixelX, pixelY));

    if (gtBtn.justPressed())
        gtBtn.drawButton(true);

    if (gtBtn.justReleased())
    {
        gtBtn.drawButton(false);
        activeMenu = 3;
        createInterface[3] = true;
        selectN = 2;
        delay(150);
    }

    run2Btn.press(down && run2Btn.contains(pixelX, pixelY));

    if (run2Btn.justPressed())
        run2Btn.drawButton(true);

    if (run2Btn.justReleased())
    {
        run2Btn.drawButton(false);
        setAllowSetIP(true);
        delay(150);
    }
}

//LOGICA E GRAFICA PAGINA 3 -- TASTIERINO -- LIVELLO 2
Adafruit_GFX_Button tBtn[12], setIp; //bottoni tastierino, bottone per settare L'ip di competenza

void keyInit(char title[])
{
    tft.fillScreen(BLACK);
    setReturnBtn();

    setIp.initButton(&tft, 250, 20, 30, 40, WHITE, CYAN, BLACK, "V", 2);
    setIp.drawButton(false);

    tft.setCursor(20, 60);
    tft.setTextColor(CYAN);
    tft.setTextSize(2);
    tft.print(title);

    tBtn[1].initButton(&tft, 40, 190, 60, 40, WHITE, CYAN, BLACK, "1", 2);
    tBtn[2].initButton(&tft, 120, 190, 60, 40, WHITE, CYAN, BLACK, "2", 2);
    tBtn[3].initButton(&tft, 200, 190, 60, 40, WHITE, CYAN, BLACK, "3", 2);

    tBtn[4].initButton(&tft, 40, 250, 60, 40, WHITE, CYAN, BLACK, "4", 2);
    tBtn[5].initButton(&tft, 120, 250, 60, 40, WHITE, CYAN, BLACK, "5", 2);
    tBtn[6].initButton(&tft, 200, 250, 60, 40, WHITE, CYAN, BLACK, "6", 2);

    tBtn[7].initButton(&tft, 40, 310, 60, 40, WHITE, CYAN, BLACK, "7", 2);
    tBtn[8].initButton(&tft, 120, 310, 60, 40, WHITE, CYAN, BLACK, "8", 2);
    tBtn[9].initButton(&tft, 200, 310, 60, 40, WHITE, CYAN, BLACK, "9", 2);

    tBtn[10].initButton(&tft, 40, 370, 60, 40, WHITE, CYAN, BLACK, "C", 2);
    tBtn[0].initButton(&tft, 120, 370, 60, 40, WHITE, CYAN, BLACK, "0", 2);
    tBtn[11].initButton(&tft, 200, 370, 60, 40, WHITE, CYAN, BLACK, "<-", 2);

    //ogni volta che avvio la grafica per l'inserimento dell'ip lo resetto
    for (int i = 0; i < 12; i++)
    {
        tBtn[i].drawButton(false);
        _ip[i] = 0;
    }
    _ipPos = 0;
    tft.fillRect(13, 100, 217, 40, WHITE);
    tft.setCursor(20, 115);
    tft.setTextColor(BLACK);
    switch (selectN)
    {
    case 0:
        printIP1(ip);
        break;
    case 1:
        printIP1(nm);
        break;
    case 2:
        printIP1(gt);
        break;
    case 3:
        printIP1(pIp);
        
        break;
    }
}

void compose(int n)
{
    if (n < 10 && _ipPos < 12)
    {
        _ip[_ipPos] = n;
        _ipPos++;
    }

    if (n == 11 && _ipPos > 0)
    {
        _ipPos--;
        _ip[_ipPos] = 0;
    }

    if (n == 10)
    {
        for (int i = 0; i < 12; i++)
            _ip[i] = 0;
        _ipPos = 0;
    }

    if ((_ipPos % 3 == 0 && _ipPos != 0) || _ipPos == 11)
    {
        bool result;
        unsigned int v = _ipPos - 1;

        if (v >= 0 && v <= 2)
        {
            result = check3IP(_ip, 0, 2);
            if (!result)
                _ipPos = 0;
        }
        if (v >= 3 && v <= 5)
        {
            result = check3IP(_ip, 3, 5);
            if (!result)
                _ipPos = 3;
        }
        if (v >= 6 && v <= 8)
        {
            result = check3IP(_ip, 6, 8);
            if (!result)
                _ipPos = 6;
        }
        if (v >= 9 && v <= 11)
        {
            result = check3IP(_ip, 9, 11);
            if (!result)
                _ipPos = 9;
        }
        if (!result)
            messagePrint("IP FUORI RANGE", RED, "Max 255", WHITE, true);
    }
}

void pressKey(int n)
{
    tft.fillRect(13, 100, 217, 40, WHITE);
    tft.setCursor(20, 115);
    tft.setTextSize(2);
    tft.setTextColor(BLACK);
    compose(n);
    tft.setCursor(20, 115);
    tft.setTextSize(2);
    tft.setTextColor(BLACK);
    printIP(_ip);
}

void controlKeyInterface()
{
    if (selectN <= 2)
        returnBtnFunc(2);
    else
        returnBtnFunc(0);

    bool down = getPoint();
    for (int i = 0; i < 12; i++)
    {
        tBtn[i].press(down && tBtn[i].contains(pixelX, pixelY));

        if (tBtn[i].justPressed())
            tBtn[i].drawButton(true);

        if (tBtn[i].justReleased())
        {
            tBtn[i].drawButton(false);
            pressKey(i);
            delay(150);
        }
    }

    setIp.press(down && setIp.contains(pixelX, pixelY));

    if (setIp.justPressed())
        setIp.drawButton(true);

    if (setIp.justReleased())
    {
        setIp.drawButton(false);
        trasformAddress(_ip, address);

        switch (selectN)
        {
        case 0:
            ether.copyIp(ip, address);
            break;
        case 1:
            ether.copyIp(nm, address);
            break;
        case 2:
            ether.copyIp(gt, address);
            break;
        case 3:
            ether.copyIp(pIp, address);
            setHavePIp(true);
            break;
        }
        // returnBtnFunc(2);
        activeMenu = selectN < 3 ? 2 : 0;
        createInterface[activeMenu] = true;
        delay(150);
    }
}
