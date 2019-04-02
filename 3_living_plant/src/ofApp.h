/**
 visualizzazione 3 - living plant
 @author Marta Murtas
 CMTI
 LAMC
 */

#pragma once

//#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxTriangle.h"
#include "SampleCollection.hpp"
#include "ofxBox2d.h"
#include "Leaf.hpp"

#define DEBUG false // è a true quando devo fare esperimenti ma ho Arduino con me
#define BANDS 256 // ogni istante ho in uscita 256 valori => 2 ^ 8

class ofApp : public ofBaseApp{
    private:
        // ---------------------------- PROPRIETA' ----------------------------------
        int w, h; // larghezza e altezza della finestra
        ofVideoGrabber grabber; // istanza di ofVideoGrabber per accedere alla webcam
        ofImage bgImage; // istanza di ofImage per salvare l'immagine di sfondo
        ofxTriangle triangle; // istanza di ofxTraingle => per triangolarizzare
    
        // https:\//openframeworks.cc/ofBook/chapters/image_processing_computer_vision.html
        ofxCvColorImage colorImg; // l'immagine attuale a colori
        ofxCvGrayscaleImage grayImage; // l'immagine attuale in scala di grigi
        ofxCvGrayscaleImage grayBg; // immagine di bg in scala di grigi
        ofxCvGrayscaleImage grayDiff; // per l'immagine delle differenze -> grayBg, grayImage
        ofxCvContourFinder contourFinder; // per trovare i contorni dei blob nell'img diff (le instanze di ofxCvContourFinder trovano elementi nell'immagine cercando contrasti tra pixel adiacenti: se c'è contrasto => sono oggetti diversi)
        bool bLearnBackground; // booleano per capire quando salvare grayBg (solo alla prima esecuzione)
    
        // pannello e relativi componenti
        ofxPanel panel; // pannello
        ofxIntSlider thresholdSlider; // slider per stabilire la sogliatura per l'img delle differenze
        ofxIntSlider instrumentSlider; // slider per scegliere lo sturmento che genererà la musica
        ofxFloatSlider volumeSlider; // slider per il volume della musica
        ofxIntSlider minAreaBlobSlider; // slider per stabilire l'area minima dei blob
        ofxIntSlider maxAreaBlobSlider; // slider per stabilire l'area massima dei blob
        ofxIntSlider interiorContoursSlider; // slider per stabilire se voglio vedere i buchi interni al blob trovato
        ofxIntSlider useApproximationSlider; // slider per stabilire se posso approssimare il contorno degli oggetti trovari
        ofxIntSlider maxBlosbsSlider; // slider per stabilire il massimo numero di blob che voglio trovare
        ofxIntSlider leavesAtATimeSlider; // slider per stabilire quante foglie generare ogni volta che tocco la pianta
        ofxIntSlider millisSlider; // slider per stabilire ogni quanto aggiornare plantValue
        ofxFloatSlider densitySlider; // slider per la fisica delle foglie -> densità
        ofxFloatSlider bounceSlider; // slider per la fisica delle foglie -> rimbalzo
        ofxFloatSlider frictionSlider; // slider per la fisica delle foglie -> scivolamento
        ofxIntSlider gravitySlider; // slider per la fisica delle foglie -> gravità
        ofxIntSlider windSlider; // slider per la fisica delle foglie -> potenza del vento
        ofxIntSlider triangularizationSlider; // slider per stabilire se triangolarizzare
        ofxColorSlider colorSlider; // slider per il colore delle figure trovate dal contourFinder
        ofxIntSlider plantThresholdSlider; // slider per la sogliatura del segnale della pianta
    
        // l'interazione con la pianta tramite Arduino
        ofSerial serial; // per leggere i valori dati in output da Arduino
        int plantValue; // il valore di frequenza attuale
        int last_millis_update; // l'ultimo aggiornamento in millisecondi
        vector <SampleCollection> sample_collection; // insieme di player musicali
        float spectrumDisplay [3]; // array per lo spettro sonoro per i livelli sonori in funzione della frequenza
    
        // per l'addon ofxBox2d
        ofxBox2d box2d; // scatolotto all'interno del quale decido è generato il mio mondo
        vector <Leaf*> leaves_vector; // ho una serie di puntatori a oggetti di classe Leaf : ofxBox2dCircle
        ofPolyline groundLine; // insieme dei vertici che formano il contorno delle figure
        ofxBox2dEdge ground; // bordo su cui rimbalzano le foglie
        ofPoint windGenerator; // punto da cui genero il vento che spinge le foglie
        int wy; // variabile per il rumore di Perlin (che sposta il windGenerator)
        ofPoint initialRepulsion; // punto di vento iniziale che spinge le foglie dentro lo schermo
        vector <ofTexture*> textures_vector; // vettore di puntatori a texture
    
        // ---------------------------- PROCEDURE E METODI ------------------------------
        void updatePlantValue (); // procedura che aggiorna plantValue a ogni chiamata
        void updateBg (); // procedura che aggiorna l'immagine delle differenze
        void updateWorld (); // procedura che aggiorna il mio mondo
        void updateLeaves (); // procedura che aggiorna le foglie e la loro fisica
        void updateSpectrum (); // procedura che fa l'update dello spettro sonoro
        void drawFigures (); // procedura che disegna a video le sagome delle figure trovate
        void clearLeaves (); // procedura che elimina i riferimenti alle foglie dalla memoria
        void clearTextures (); // procedura che elimina i riferimenti alle texture dalla mem
        void addElementToMyWorld (); // procedura che aggiunge una foglia al mondo
    
        string ofxGetSerialString(ofSerial &serial, char until); // metodo per leggere dalla porta seriale fino al carattere until
        string ofxTrimStringRight(string str); // metodo per tagliare tutto ciò che sta alla destra di str
        string ofxTrimStringLeft(string str); // metodo per tagliare tutto ciò che sta alla sx di str
        string ofxTrimString(string str); // metodo che taglia la stringa str
        std::unordered_map<string, vector <string>> getSamples (string file_name); // metodo che mi restituisce i nomi delle cartelle e il loro contenuto
        void init_my_sample_collection (std::unordered_map<string, vector <string>> files_and_samples); // procedura che inizializza sample_collection

	public:
        // ---------------------------- PROCEDURE E METODI ------------------------------
		void setup();
		void update();
		void draw();
        void exit (); // procedura che libera la memoria dai puntatori

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
