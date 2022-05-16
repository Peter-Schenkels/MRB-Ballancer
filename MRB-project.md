#MRB Project: Ball Balancer

### Todo
* componenten vinden
* componenten keuze beragumenteren
* Schema maken
* componenten kopen





### Componenten
* #### distance sensors
  * **Sonar**
    De sonar is makkelijke en goedkope oplossing voor dit probleem. De sonar werkt door een hoog frequency geluids signaal te sturen en wachten op de terugkaatsing van het signaal via het object wat je wilt meten. Daardoor ben je wel afhankelijk van de snelheid van geluid, ook kan het zo zijn dat het geluids signaal verkeerd botsts en op een ander later tijdstip dan verwacht aankomt, waardoor de meting inacurraat wordt. 
  * **Infrared distance sensor**
    Net zoals de sonar werkt de infrared sensor ook door een signaal te sturen en met de tijd van wanneer het signaal terugkaatst te bepalen wat de afstand is. Alleen doet de infrared sensor dit via licht wat immens veel sneller is dan geluid. Ook heeft de infrared distance sensor een hoek detectie zodat de hoek gelijk meegenomen wordt in de bereking waardoor de berekening accurater wordt. Wel is een Infrared sensor duurder. Ook blijkt dat meeste infrared sensoren een minimum range van 10CM hebben waardoor het zetten van een setpoint minder flexibel wordt.
  * **Lidar**
    Een lidar werkt ook op dezelfde mannier maar dit keer door een laser. De laser kan schadelijk zijn voor je ogen en een lidar is heel duur dus deze laat ik buiten beschouwing.

  * **Time of Flight Distance Sensor**
    Time of flight sensor werkt ook via licht en kan snelle readings maken. Het is eigenlijk het beste van een infrared sensor alleen heeft deze een lagere minimum range. 


    |   Type                                    | Sonar | Lidar | IR     | ToF  |
    |---------------------------------------|-------|-------|--------|------|
    | prijs                                 | 1.50  | 50.0  | 4.50   | 3.00 |
    | Hoge Sample Rate                      | No    | Yes   | Yes    | Yes  |
    | Voor korte afstand                    | Yes   | Yes   | No | Yes/No   |
    | Gevoelig voor  externe omstandigheden | Yes   | No    | No     | No   |
    | Geschikt voor complexe voormen        | No    | Yes   | Yes    | Yes  |

    Uit het grafiek hierboven heb ik besloten dat ik voor een Time of Flight sensor ga. Hieronder ga ik 2 verschillende sensoren vergelijken die in het TI-lab te vinden zijn. 

    |    Type               | GY530
    |-------------------|-------------|-|
    | Operating Voltage (lower better) | 2.6v - 3.5v | |
    | Supply-current (lower better) | 10mA |
    | Min-range (lower better) | 3cm | |
    | Max-range | 200cm |
    | Output protocol | $I^2C$
    | Output format | Millimeters |
    | Resolution (lower better) | 0.03 mm/pb|




* Servo (welke?)
* Microcontroller (welke?)
* Draadloos?


### Requirements
Waar moet de eind opdracht aan voldoen?

##### Requirements Systeem
* Er moet iets bewegen in het regel systeem
* Je moet het zelfstandig geprogrammeerd hebben.
* Goed werkende PID systeem
* Variable setpoint
* Tijdens het "runnen" van het systeem moet je real time  het systeem een ander setpoint kunnen geven. 
* Er moet minimaal 1 digitale filtertechniek toegepast zijn in je ontwerp.
* Een TDP
* Een filmpje


##### Requirements TDP
* Argumentatie welke sensoren wel of niet zijn gebruikt.
* Argumentatie welke actuatoren wel of niet zijn gebruikt
* Argumentatie hoe ik interface met de electronische componenten die gebruikt zijn
* Uitleggen wat het gedrag is van het filter(s) wat je toegepast hebt? 
* Argumentatie voor de PID waardes
* Performance uitleggen 
* HW schema's
* SW schemas's
* foto's van je product. 
    
