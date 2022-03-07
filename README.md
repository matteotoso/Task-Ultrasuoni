# TASK-RILEVAOGGETTO
# Progetto rullo trasportatore
DESCRIZIONE PROGETTO: Creare un nastro trasportatore che alla rilevazione dell'oggeto, individua se nero o bianco e infine lo smista se bianco a sinistra e se nero a destra.

Abbiamo ideato e crato la struttura del rullo su Tinkercad, creando un modello 3D così da avere un idea di come sarebbe venuto. Una volta finito la struttura base su Tinkercad abbimao costruito il vero moello e questo è il risultato:

PRIMA

![rullo](https://user-images.githubusercontent.com/97526713/157097849-27d7b626-d719-4f96-8065-893b30c24352.PNG) 

DOPO

![rullo vero](https://user-images.githubusercontent.com/97526713/157098375-03ab3acb-805c-488d-970a-a902c9b8dc26.PNG)

# Struttura del codice
Per quanto riguarda la struttura del codice da scrivere, il progetto è stato suddiviso in parti e in ogni parte una persona si occupava di un azione specifica del rullo trasportatore. Io mi sono occupato della gestione SENSORE ULTRASUONI SFR05.

# Specifiche tecniche SENSORE ULTRASUONI SFR05
L'SRF05 è un’evoluzione dell'SRF04 ed è stato progettato per aumentarne la flessibilità, aumentare la portata e ridurre ulteriormente i costi. In quanto tale, l'SRF05 è completamente compatibile con l'SRF04. La portata è aumentata da 3 metri a 4 metri. Una nuova modalità operativa (che collega il pin della modalità a massa) consente all'SRF05 di utilizzare un singolo pin sia per il trigger che per l'eco, risparmiando così preziosi pin sul controller. Quando il pin della modalità viene lasciato scollegato, l'SRF05 funziona con trigger separati e pin di eco, come l'SRF04. L'SRF05 include un piccolo ritardo prima dell'impulso di eco per dare ai controller più lenti come il Basic Stamp e il Picaxe il tempo di eseguire il loro impulso nei comandi.

Tensione Operativa		5V
Corrente Operativa Tipica	4mA
Frequenza				40 Khz
Portata				1cm - 4mt
Dimensioni			43 x 20 x H 17 mm

Impulso di ritorno: Segnale TTL positivo, di durata proporzionale alla distanza rilevata.
Trigger di Input: Impulso TTL di durata minima di 10 uS.

FUNZIONAMENTO:

![sfr05 grafico](https://user-images.githubusercontent.com/97526713/157099999-77e1cde5-0c30-4569-80c5-fc396e2491ec.PNG)

RILEVAMENTO DELL'OGGETTO:

![sfr05 dist](https://user-images.githubusercontent.com/97526713/157100733-91c6bde9-3538-4849-afd1-a636c388230e.PNG)

CALCOLO DELLA DISTANZA: 

È sufficiente fornire un breve impulso da 10uS all'ingresso trigger per avviare la misurazione. L'SRF05 invierà una serie di 8 cicli di ultrasuoni a 40 kHz e alzerà la sua linea di eco alta (o la linea di trigger in modalità 2). Quindi ascolta un eco e non appena ne rileva uno abbassa nuovamente la linea dell'eco. La linea dell'eco è quindi un impulso la cui larghezza è proporzionale alla distanza dall'oggetto. Misurando la durata dell'impulso è possibile calcolare la distanza.

Se l'ampiezza dell'impulso è misurata in uS, dividendo per 58 si otterrà la distanza in cm, oppure dividendo per 148 si otterrà la distanza in pollici. uS/58 = cm o uS/148 = pollici.

# Funzionamento e descrizione della task

Per il funzionamento del sensore abbiamo creato una task TaskRilevaOggetto(), nella quale inseriamo 2 timer per il funzionamento dei trigger e dell'echo. 

SPECIFICHE DELLA TASK: Il sensore è collegato tramite dei cavi ai pin della scheda stm32. Nello specifico il Trigger è collegato al pin PB6, mentre l’echo al pin PA5. Entrambi i pin sono settati come alternate function e sono assegnati ad un timer (PB6 ->TIM4_CH1, PA5 -> TIM2_CH1) che generano un segnale PWM, necessario alla regolazione del segnale di trigger, al calcolo dei secondi e alla generazione dell’interrupt (necessari per il calcolo della distanza). Questa task deve essere sempre in funzione, e appena viene rilevato un oggetto deve avvertire tramite un flag degli eventgroup i task del rullo,del buzzer e del rileva colore.

