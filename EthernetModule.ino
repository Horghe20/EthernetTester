
bool allowSearch = false;    //per iniziare la ricerca con DHCP
bool allowSetIP = false;     //per iniziare la ricerca con Statico
bool receivePacket = false;  //attivare la ricezione di pacchetti
bool haveIP = false;         //per verificare se si possiede correttametne un ip
byte pingOnCount = 0;        //contare il numero di ping che hanno ricevuto una risposta
bool blockInterface = false; //per bloccare l'interfaccia durante un operazione(ping)
static uint32_t timer;
static uint32_t timer2;
unsigned long ini; //per controllare l'inizio del ping

byte Ethernet::buffer[700];
static byte mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x32};
static byte hisip[] = {74, 125, 79, 99};

MCUFRIEND_kbv *dis;

//si assume che tutte le variabili d'istanza siano a false prima dell'utilizzo dei metodi;

void inizializeModule(MCUFRIEND_kbv *tft)
{
  dis = tft;
}



void setAllowSearch(bool v)
{
  allowSearch = v;
}

bool getAllowSerch()
{
  return allowSearch;
}

void setAllowSetIP(bool v)
{
  allowSetIP = v;
}
bool getAllowSetIP()
{
  return allowSetIP;
}

void setReceivePacket(bool v)
{
  receivePacket = v;
}
bool getReceivePacket()
{
  return receivePacket;
}

void setHaveIP(bool v)
{
  haveIP = v;
}
bool getHaveIP()
{
  return haveIP;
}

void setPingOnCount(byte n)
{
  if (pingOnCount == 0 && n == 1)
    ini = millis();
  pingOnCount = n;
} //n il numero da assegnare a pingOnCount
byte getPingOnCount()
{
  return pingOnCount;
}

void setBlockInterface(bool v)
{
  blockInterface = v;
}
bool getBlockInterface()
{
  return blockInterface;
}

bool setUpDHCP()
{
  if (ether.begin(sizeof Ethernet::buffer, mymac, SS) == 0)
    return false;

  if (!ether.dhcpSetup())
    return false;

  haveIP = true;
  return true;
}

bool setUpStaticIP(byte _ip[], byte _gt[], byte _nm[])
{
  bool success = true;

  unsigned long init, fina; //variabili per controllo attesa
  if (ether.begin(sizeof(Ethernet::buffer), mymac, SS) == 0)
    return false;

  ether.staticSetup(_ip, _gt, NULL, _nm);

  ether.copyIp(ether.hisip, hisip);
  unsigned long ini = millis();
  while (ether.clientWaitingGw())
  {
    ether.packetLoop(ether.packetReceive());
    if ((millis() - ini) == 30000)
    {
      success = false;
      break;
    }
  }

  if (success)
    return true;
  else
    return false;
}

void setPingInformation(bool g, byte _pIp[])
{

  if (g)
  {
    if (!ether.dnsLookup(PSTR("www.google.com")))
      ether.parseIp(ether.hisip, "74.125.77.99");
  }
  else
    ether.copyIp(ether.hisip, _pIp);

  receivePacket = false;
  tft.setCursor(10,400);
  tft.setTextSize(1);
  tft.setTextColor(BLUE);
  tft.print("Inizio Ping");
}

//ogni volta che si richiama ping passare ini il valore di inizio del ping cosi da poter confrontare e evitare trempi infiniti
void ping()
{

  while (pingOnCount != 5 && (millis() - ini) < 15000)
  {
    word len = ether.packetReceive(); // go receive new packets
    word pos = ether.packetLoop(len); // respond to incoming pings

    // report whenever a reply to our outgoing ping comes back
    if (len > 0 && ether.packetLoopIcmpCheckReply(ether.hisip))
    {
      timer2 = (micros() - timer) * 0.001;
      blockInterface = true;
      dis->setTextColor(WHITE);
      dis->setTextSize(1);
      dis->setCursor(20, (400 + (pingOnCount * 10)));
      dis->print("Pinging: ");
      printIPMH(ether.hisip);
      dis->print(" ");
      dis->print(timer2);
      dis->print("ms");
      pingOnCount++;
    }

    // ping a remote server once every few seconds
    if (micros() - timer >= 5000000)
    {
      timer = micros();
      ether.clientIcmpRequest(ether.hisip);
    }
  }

  if (pingOnCount == 1)
  {
    pingOnCount = -1;
  }
}

static void gotPinged(byte *ptr)
{
  dis->setCursor(10, 425);
  dis->setTextColor(WHITE);
  dis->setTextSize(1);
  printIPMH(ptr);
  dis->print("  Ti sta pingando");
  delay(1000);
  dis->fillRect(10, 400, 320, 40, BLACK);
}

void printIPMH(uint8_t ip[4])
{
  for (size_t i = 0; i < 4; i++)
  {
    dis->print(ip[i]);
    if (i != 3)
      dis->print(".");
  }
}
