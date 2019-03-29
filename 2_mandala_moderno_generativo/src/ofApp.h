/**
 visualizzazione 2 - mandala moderno generativo
 @author Marta Murtas
 CMTI
 LAMC
 */

#pragma once

#include "ofMain.h"
#include "SampleCollection.hpp"
#include "DrawLevel.hpp"
#include "ofxGui.h"
#include "ofPoint.h"
#include <string>

#define MIN_ANGLE 15 // l'angolo minimo tra un elemento e l'altro
#define MAX_ANGLE 180 // l'angolo massimo tra un elemento e l'altro (linea)
#define MAX_SPARSITY 10 // livello di sparsità dell'intero disegno
#define DEBUG false // quando devo testare ma non ho Arduino con me

class ofApp : public ofBaseApp{
private:
    // ------------------------------ proprietà ------------------------------
    ofSerial serial; // istanza di ofSerial per leggere i valori dati in output da Arduino
    int plantValue; // il valore di frequenza attuale
    int w, h; // larghezza e altezza della finestra
    ofPoint center; // il centro dello scherml
    vector <SampleCollection> sample_collection; // la mia collezione di samples
    vector <DrawLevel> drawings; // il vettore dei disegni mostrati a video
    string drawing_types [7]; // tipi di disegni possibili
    int last_dist_from_center; // l'ultimo raggio (distanza dal centro)
    int last_drawing_time; // l'ultima volta che ho aggiunto un livello di disegno
    int last_millis_update; // l'ultimo aggiornamento del valore della pianta in millisecondi
    ofColor color_palette [6]; // vettore di 6 colori da me scelti per i disegni
    
    ofxPanel panel; // pannello
    ofxIntSlider angleSlider; // slider per stabilire l'angolo dei petali
    ofxIntSlider instrumentSlider; // slider per scegliere lo sturmento che genererà la musica
    ofxFloatSlider volumeSlider; // slider per il volume della melodia
    ofxFloatSlider dimensionSlider; // slider per la dimensione della primitiva
    ofxIntSlider sparsitySlider; // slider per stabilire il livello di sparsità delle primitive
    ofxIntSlider newDrawingSlider; // slider per stabilire ogni quanti secondi creare un nuovo disegno
    ofxIntSlider millisSlider; // slider per stabilire ogni quanto aggiornare plantValue
    ofxIntSlider trailSlider; // slider per stabilire quanta scia i disegni devono lasciare
    ofxIntSlider plantThresholdSlider; // slider per la sogliatura del segnale della pianta
    
    // ------------------------------ metodi e funzioni ----------------------
    string ofxGetSerialString(ofSerial &serial, char until); // metodo per leggere dalla porta seriale fino al carattere until
    string ofxTrimStringRight(string str); // metodo per tagliare tutto ciò che sta alla destra di str
    string ofxTrimStringLeft(string str); // metodo per tagliare tutto ciò che sta alla sx di str
    string ofxTrimString(string str); // metodo che taglia la stringa str
    std::unordered_map<string, vector <string>> getSamples (string file_name); // metodo che mi restituisce i nomi delle cartelle e il loro contenuto
    void init_my_sample_collection (std::unordered_map<string, vector <string>> files_and_samples); // procedura che inizializza sample_collection
    void updatePlantValue (); // procedura che aggiorna il valore di plantValue
    void addDrawing (); // procedura che aggiunge un livello di disegno
    void deleteDrawings (); // procedura che pulisce lo schermo dai disegni precedenti
    

public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
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
