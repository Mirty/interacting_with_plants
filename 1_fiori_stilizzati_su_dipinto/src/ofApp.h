/**
 visualizzazione 1 - fiori stilizzati su dipinto
 @author Marta Murtas
 CMTI
 LAMC
 */

#pragma once

#include "ofMain.h"
#include "Flower.hpp"
#include "ofxInkSim.h"
#include "SampleBrush.h"
#include "SampleCollection.hpp"
#include "ofxGui.h"

#define MAX_WAITING_TIME 450 // 30 fps * 15 secondi
#define BORDER 70 // il bordo rispetto alla finestra
#define MIN_ANGLE 15 // il minimo angolo tra un petalo e l'altro
#define MAX_ANGLE 180 // il massimo angolo tra un petalo e l'altro
#define DEBUG false // è true quando non posso collegarmi ad Arduino ma devo testare
#define NUM_BGS 8 // il numero di img di bg scelte

class ofApp : public ofBaseApp{
    
private:
    // ------------------------------ proprietà ------------------------------
    ofSerial serial; // istanza di ofSerial per leggere i valori dati in output da Arduino
    int plantValue; // il valore del segnale attuale
    int last_frequency_time; // ultima volta che è stato registrato un segnale != 0
    int last_millis_update; // l'ultimo aggiornamento (in millisecondi)
    int last_flower_creation_time; // tengo traccia dell'ultima volta che ho creato un fiore
    int last_frequency ; // tengo conto dell'ultimo valore di frequenza percepito
    int last_bg_update; // tengo conto dell'ultima volta che ho aggiornato il bg
    int last_posX; // posizione in x dell'ultimo fiore
    int w, h; // larghezza e altezza della finestra
    bool bEvaporToDisapper; // per capire se voglio che il colore evapori o meno
    string bgs [NUM_BGS]; // array di stringhe coi nomi delle imgs di bg
    int bg_idx; // indice dell'attuale bg in uso
    ofImage bg_img; // immagine di bg attuale
    ofxInkSim inkSim; // istanza dell'addon ofxInkSim
    SampleBrush brush; // istanza di SampleBrush => il pennello che disegna sulla tela
    
    //https:\//soundpacks.com/free-sound-packs/ethnic-instrument-samples/
    vector <SampleCollection> sample_collection; // vettore di sample collection
    vector <Flower> flowers; // mio vettore che conterrà tutti i fiori creati
    
    ofxPanel panel; // pannello
    ofxIntSlider millisSlider; // slider per stabilire ogni quanto aggiornare plantValue
    ofxIntSlider angleSlider; // slider per stabilire l'angolo dei petali
    ofxIntSlider instrumentSlider; // slider per scegliere lo sturmento che genererà la musica
    ofxFloatSlider volumeSlider; // slider per il volume della melodia
    ofxIntSlider secondsFlowerSlider; // slider per stabilire dopo quanti secondi creare un nuovo fiore
    ofxIntSlider bgUpdateSlider; // slider per stabilire dopo quanti secondi cambiare il bg
    ofxIntSlider speedDesappearSlider; // slider per stabilire la grandezza del pennello
    ofxIntSlider maxFrequencySlider; // slider per la massima frequenza (1024 non viene mai raggiunta)
    ofxIntSlider differenceInBrightnessSlider; // slider per stabilire la  differenza in luminosità dei fiori rispetto allo sfondo
    ofxIntSlider plantThresholdSlider; // slider per la sogliatura del segnale della pianta
    
    // ------------------------------ metodi e funzioni ----------------------
    
    string ofxGetSerialString(ofSerial &serial, char until); // metodo per leggere dalla porta seriale fino al carattere until
    string ofxTrimStringRight(string str); // metodo per tagliare tutto ciò che sta alla destra di str
    string ofxTrimStringLeft(string str);  // metodo per tagliare tutto ciò che sta alla sx di str
    string ofxTrimString(string str); // metodo che taglia la stringa str
    
    void updatePlantValue (); // procedura che aggiorna plantValue a ogni chiamata
    void addFlower (int x); // procedura che aggiunge un fiore in x, rand
    void addFlower (int x, int y); // procedura che aggiunge un fiore in x,y
    void updateFlowers (); // procedura che aggiorna i fiori
    void updateWatercolor (); // procedura che aggiorna il disegno
    void updateBg (); // procedura che aggiorna l'img di bg
    std::unordered_map<string, vector <string>> getSamples (string file_name);  // metodo che mi restituisce i nomi delle cartelle e il loro contenuto
    void init_my_sample_collection (std::unordered_map<string, vector <string>> files_and_samples);  // procedura che inizializza sample_collection

public:
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    
};

