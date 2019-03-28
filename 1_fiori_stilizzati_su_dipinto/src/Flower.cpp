//
//  Flower.cpp
//  plant_OF
//
//  Created by Marta Murtas on 04/03/2019.
//
//

#include "Flower.hpp"


void Flower::initialize (ofPoint startingPoint, ofColor colore) {
    /* procedura per l'inizializzazione dell'istanza */
    
    // inizializzo le proprietà starting point e colore
    this->startingPoint = startingPoint;
    this->colore = colore;
}

void Flower::update (ofPoint c, int value) {
    /* procedura per l'aggiornamento della posizione del fiore */
    
    // se il fiore si trova nella metà sx della finestra,
    // allora scompare verso sx con velocità value,
    if (startingPoint.x <=  c.x) startingPoint.x -= value;
    // altrimenti va verso dx, sempre con velocità value
    else startingPoint.x += value;
}

void Flower::update (int freq, int angle, int max_frequency) {
    /* procedura per l'aggiornamento del fiore */
    
    // variabile per capire se ho aggiornato un petalo oppure se sto ricevendo un valore di frequenza nuovo. la setto a false, perché ancora non conosco la risposta.
    bool isUpdated = false;
    
    
    // INTERVALLI
    // determino il numero di intervalli (o petali)
    int num_intervalli = 360 / angle;
    // creo un array che conterrà l'angolo di ogni intervallo
    int intervalli [num_intervalli + 1];
    // assegno all'array i valori degli intervalli
    for (int i = 0; i < num_intervalli; i++) {
        // se angle = 15 ho che:
        // intervalli [0] = 0
        // intervalli [1] = 15
        // ...
        intervalli [i] = angle * i;
    }
    intervalli[num_intervalli] = 360;
    
    
    // scorro tutti i petali per aggiornarne la lunghezza
    for (int i = 0; i < petals.size(); i ++) {
        if (petals[i].frequencyValue == freq) {
            petals[i].updateLength (4); // aumento di 3 unità la lunghezza del petalo
            isUpdated = true; // aggiorno la varaibile che mi dice se ho aggiornato un petalo
        }
        else {
            // se la frequenza attuale non è quella di questo petalo...
            petals[i].updateLength (-1); // decremento di 1 unità la lunghezza del petalo
        }
        
        // ogni petalo è rappresentato attorno al pistillo.
        // la sua angolatura dipende dal valore di frequenza del petalo stesso.
        // il valore attuale è alto, allora sarà uno degli ultimi petali,
        // se sarà basso (a 0 per esempio) sarà il primo petalo.
        // la rotazione avviene in senso orario a partire dalle ore 3.
        
        // calcolo l'angolo per rappresentare il petalo
        // in base al valore di angleSlider settato dall'utente =>
        // x : 360 = petals[i].frequencyValue / max_frequency
        float relative_angle = float(petals[i].frequencyValue * 360)/float(max_frequency);
        int intervallo = 0;
        bool found = false;
        for (int j = 0; j < num_intervalli; j++) {
            // verifico a che intervallo appartienen
            if (relative_angle > intervalli[j] && relative_angle < intervalli[j+1]) {
                intervallo = intervalli[j];
                found = true;
                break;
            }
        }
        //if (! found) intervallo = intervalli[num_intervalli-1];
        petals[i].setAngle(intervallo);
        
        
    }
    // se isUpdated == false vuol dire che ho una frequenza nuova, non ancora appartenente ai petali attualmente memorizzati
    if (! isUpdated) {
        if (freq != 0) {
            addPetal(freq); // aggiungo il petalo
        }
    }
}

void Flower::addPetal (int frequencyValue) {
    /* procedura per l'aggiunta di un nuovo petalo */
    
    // ogni fiore può avere da un minimo di 0 petali a un massimo di 1024.
    // un petalo corrisponde a ogni valore di frequenza.
    // i petali non sono rappresentati graficamente, mi servono come back-end.
    
    // creo un nuovo petalo
    Petal newPetal;
    // inizializzo il petalo
    newPetal.initialize(frequencyValue, startingPoint, 0);
    //aggiungo un nuovo petalo al fiore
    petals.push_back (newPetal);
}

void Flower::draw () {
    /* procedura per il disegno del fiore */
    
    // ora è il momento di rappresentare i petali.
    // la prima cosa che devo fare è capire quanto sarà lungo ogni petalo,
    // e ogni petalo ha un angolo diverso dall'altro. per fare questo,
    // creo un hashmap float->int ovvero angolo->lunghezza
    std::unordered_map<float, int> map = {}; // O (1) perché non è ordinata
    // e un vettore di angoli
    vector <float> angles;
    // ottengo le lunghezze per ogni angolo
    for (int i = 0; i < petals.size(); i ++) {
        float angle = petals[i].angle;
        int lunghezza = petals[i].lunghezza;
        // se già angolo non è contenuto...
        if (map.find(angle) == map.end()) {
            map [angle] = lunghezza; // inserisco l'angolo e la lunghezza
            angles.push_back(angle); // aggiorno il vettore degli angoli
        } else {
            // altrimenti aggiorno la lunghezza per quell'angolo
            map [angle] = map [angle] + lunghezza;
            // non faccio sforare la lunghezza per quell'angolo oltre un certo valore
            if (map[angle] > MAX_PETAL_LENGTH) map[angle] = MAX_PETAL_LENGTH;
        }
    }
    
    
    // setto il colore del fiore
    ofSetColor (colore);
    // disegno tutti i suoi petali (un petalo per angolo)
    for (int i = 0; i < angles.size(); i++) {
        // ottengo l'angolo
        float angle = angles[i];
        // ottengo la sua lunghezza
        int lunghezza = map[angle];
        // trasfondo l'angolo in radianti
        float radians = float (angle * PI) / float(180);
        // calcolo il punto finale del petalo
        ofPoint endPoint = ofPoint (startingPoint.x + cos(radians) * lunghezza, startingPoint.y + sin (radians) * lunghezza);
        // disegno la linea del petalo
        ofDrawLine (startingPoint.x,  startingPoint.y, endPoint.x,  endPoint.y);
        // disegno l'ellisse che simboleggia la fine del petalo
        ofDrawCircle (endPoint.x, endPoint.y, 5, 5);
    }
    // disegno il pistillo del fiore di grandezza in base alla presenza o meno di petali
    if (angles.size () == 0) ofDrawCircle (startingPoint.x, startingPoint.y, 10);
    else ofDrawCircle (startingPoint.x, startingPoint.y, 3);
    
    // disegno il gambo
    ofDrawLine (startingPoint.x, startingPoint.y, startingPoint.x, ofGetHeight());
    
}

void Flower::setColor (ofColor color) {
    /* procedura per il settaggio del colore del fiore */
    colore = color;
}