#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // impostazioni grafiche
    //ofSetVerticalSync(true);
    ofSetBackgroundAuto(false); // non voglio che il bg si aggiorni automaticamente
    ofBackground(255); // imposto il bg a bianco
    //ofSetLogLevel(OF_LOG_VERBOSE);
    ofFill();
    ofSetFullscreen(true); // metto la finestra in fullScreen
    w = ofGetWidth (); // mi salvo la larghezza della finestra
    h = ofGetHeight (); // mi salvo l'altezza della finestra
    center = ofPoint (w/2, h/2); // salvo il centro della mia finestra
    last_millis_update = 0; // salvo il tempo d'inizio del programma
    
    // per ottenere i dati dalla porta seriale
    //serial.listDevices();
    //vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    int baud = 9600; // frequenza di aggiornamento (9600 simboli al secondo)
    serial.setup(0, baud); // apro la prima porta disponibile
    
    // setto il mio pannello e i relativi parametri
    panel.setup ("Parameters");
    panel.setPosition (50, 50);
    instrumentSlider.setup ("instrument: ", 4, 0, 9);
    volumeSlider.setup ("volume: ", 1, 0, 10);
    angleSlider.setup ("angle: ", 30, MIN_ANGLE, MAX_ANGLE); // default 30°, min 15°, max 180°
    dimensionSlider.setup ("dimension: ", 10, 5, 40);
    sparsitySlider.setup ("sparsity: ", 5, 0, 10);
    newDrawingSlider.setup ("time for new drawing: ", 10, 2, 60);
    millisSlider.setup ("millis before update: ", 0, 0, 90);
    trailSlider.setup ("trail: ", 100, 0, 255);
    plantThresholdSlider.setup ("plant threshold: ", 120, 5, 1024);
    
    // aggiungo i parametri al pannello
    panel.add (&instrumentSlider);
    panel.add (&volumeSlider);
    panel.add (&angleSlider);
    panel.add (&dimensionSlider);
    panel.add (&sparsitySlider);
    panel.add (&newDrawingSlider);
    panel.add (&millisSlider);
    panel.add (&trailSlider);
    panel.add (&plantThresholdSlider);
    
    
    // inizializzo sample_collection con i file contenuti nelle cartelle in data
    string absolute_path = "/Users/martamurtas/Desktop/LAB_proj/2_mandala_moderno_generativo/bin/data/samples.txt"; // salvo il percorso in cui si trova il file che elenca le cartelle contenenti i samples
    std::unordered_map<string, vector <string>> files_and_samples = getSamples (absolute_path); // ottengo i nomi dei samples e le loro posizioni nelle diverse cartelle
    init_my_sample_collection (files_and_samples); // creo la mia collezione di samples
    
    
    // creo la mia palette di colori
    // https:\//www.shutterstock.com/it/image-photo/river-over-forest-grove-palette-colors-308928497
    color_palette [0] = ofColor (143, 204, 39);
    color_palette [1] = ofColor (62, 150, 24);
    color_palette [2] = ofColor (194, 181, 190);
    color_palette [3] = ofColor (149, 131, 143);
    color_palette [4] = ofColor (119, 117, 134);
    color_palette [5] = ofColor (27, 11, 2);
    
    // definisco le diverse forme possibili per i vari livelli di disegno
    drawing_types[0] = "line";
    drawing_types[1] = "particle";
    drawing_types[2] = "circle";
    drawing_types[3] = "segment";
    drawing_types[4] = "rectangle";
    drawing_types[5] = "triangle";
    drawing_types[6] = "rev_triangle";
    
    // creo il primo disegno
    addDrawing ();
}

//--------------------------------------------------------------
void ofApp::update(){
    // aggiorno l'attuale valore della pianta
    updatePlantValue ();
    
    // cambio solo la dimensione e il numero di angili dell'ultimo livello
    drawings[drawings.size()-1].updateRepresentation (center, angleSlider, dimensionSlider);
    // aggiorno le primitive di tutti i livelli in base al valore di frequenza e di sparsità (scelto dall'utente tramite la gui)
    for (int i = 0; i < drawings.size(); i++) {
        drawings[i].updatePoints (center, plantValue, MAX_SPARSITY - sparsitySlider);
    }
    
    // verifico a che intervallo appartiene l'attuale plantValue e riproduco i suoni dello strumento scelto dall'utente tramite gui (instrumentSlider)
    sample_collection[instrumentSlider].play (plantValue, volumeSlider/10);
    
    // verifico se è il caso di aggiungere un nuovo fiore
    int now = ofGetElapsedTimef();
    if (now - last_drawing_time >= newDrawingSlider) addDrawing();
}

//--------------------------------------------------------------
void ofApp::draw(){
    // disegno un rettangolo bianco con un'opacità pari a 255 - trailSlider
    ofSetColor (255, 255 - trailSlider);
    ofDrawRectangle(0, 0, w, h);
    
    // mostro a video i livelli di disegno
    for (int i = 0; i < drawings.size(); i++) {
        drawings[i].draw (center, angleSlider);
    }
    
    // mostro a video il pannello della gui
    panel.draw();
}

//--------------------------------------------------------------
void ofApp::updatePlantValue () {
    /* procedura che aggiorna il valore di plantValue */
    
    // salvo il tempo corrente
    int now = ofGetElapsedTimeMillis();
    // verifico che siano passati almeno millisSlider ms dall'ultimo rilevamento.
    // in caso contrario non aggiorno plantValue e ciò che dipende da lei.
    if (now - last_millis_update >= millisSlider) {
        // ottengo il valore dalla porta seriale
        string serialString = ofxGetSerialString(serial, '\n');
        // se sono in debug oppure la stringa letta ha lunghezza > 0...
        if (serialString.length() > 0 || DEBUG) {
            serial.flush (); // svuoto la porta
            // se sono in debug genero un valore
            if (DEBUG) {
                plantValue = ofRandom (1024);
            }
            else {
                try {
                    // trasformo il valore da string a int
                    plantValue = std::stoi( serialString );
                    if (plantValue > 1023) { // errore in lettura/scrittura
                        plantValue = ofRandom (90);
                    }
                }
                catch (std::invalid_argument e) {
                    // se per qualche ragione non ricevo il valore da Arduino, lo setto io a 0
                    plantValue = ofRandom (90);
                }
            }
            
            // aggiorno la variabile che tiene conto di quando è stato fatto l'ultimo aggiornamento di plantValue
            last_millis_update = ofGetElapsedTimeMillis ();
            
        }
    }
}

//--------------------------------------------------------------
void ofApp::addDrawing () {
    /* procedura che aggiunge un livello di disegno */
    
    // controllo se ho già fatto 6 disegni..
    if (drawings.size () == 7) {
        // cancello lo schermo dai disegni precedenti
        deleteDrawings ();
    }
    // creo un nuovo livello di disegno
    DrawLevel drawLevel;
    // aggiorno l'ultima distanza dal centro
    last_dist_from_center += 50;
    // estraggo un indice a caso per il tipo di disegno
    int type_idx = ofRandom(7);
    // pesco il tipo di disegno con l'indice appena calcolato
    string type = drawing_types[type_idx];
    // calcolo random l'indice per il colore di questo nuovo livello di disegno
    int color_idx = ofRandom (6);
    // prendo il colore dalla palette di colori
    ofColor color = color_palette[color_idx];
    // inizializzo il nuovo livello di disegno
    drawLevel.initialize (type, last_dist_from_center, color);
    // inserisco questo livello nel vettore di disegni
    drawings.push_back (drawLevel);
    // aggiorno la variabile che tiene conto di quando ho aggiornato l'ultimo livello
    last_drawing_time = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void ofApp::deleteDrawings () {
    /* procedura che pulisce lo schermo dai disegni precedenti */
    
    // se sì, cancello tutto e riparto da capo.
    // svuoto il vettore drawings
    drawings.clear();
    // il raggio iniziale è di 10
    last_dist_from_center = 10;
    // disegno il rettangolo bianco per cancellare la tela
    ofSetColor (255, 0);
    ofDrawRectangle(0, 0, w, h);
}

//--------------------------------------------------------------
std::unordered_map<string, vector <string>> ofApp::getSamples (string file_name) {
    /* metodo che mi restituisce i nomi delle cartelle e il loro contenuto */
    
    // creo un  hashmap non ordinata che conterrà string->vector<string>
    // ovvero nome cartella -> vettore di nomi di file musicali in quella cartella
    std::unordered_map<string, vector <string>> folders_and_samples = {};
    
    // per ottenere i nomi dei samples nelle cartelle devo andare a leggere il file
    // samples.txt che sta nella cartella data  (generato con bin/list_files.py)
    ifstream myfile (file_name);
    // creo la variabile line di tipo string che conterrà le linee lette
    string line;
    // apro il file
    if (myfile.is_open()) {
        // creo la variabile che manterrà il nome dell'ultima cartella letta
        string folder;
        // leggo riga per riga: memorizzo l'attuale riga nella variabile line
        while (getline (myfile, line)) {
            // se la linea inizia col carattere '-' allora sono in una nuova cartella
            if (line[0] == '-') {
                // il nome della cartella va dal char 1 al char finale
                folder = line.substr (1, line.length() - 1);
                // inserisco la nuova cartella nell'hashmap non ordinata
                folders_and_samples[folder] = vector <string> ();
            }
            else {
                // se già ho inserito folder nell'hashmap metto line nel corrispondente vettore
                folders_and_samples[folder].push_back(line);
            }
        }
        // chiudo il mio file
        myfile.close();
    }
    
    // restituisco l'hashmap non ordinata
    return folders_and_samples;
}

//--------------------------------------------------------------
void ofApp::init_my_sample_collection (std::unordered_map<string, vector <string>> files_and_samples) {
    /* procedura che inizializza sample_collection */
    
    // obiettivo: ottenere le chiavi dell'hashmap
    // creo un vettore di stringhe keys
    vector <string> keys;
    // scorro tutta l'hashmap con un iteratore dall'inizio alla fine
    // https:\//openframeworks.cc/ofBook/chapters/stl_vector.html#iterators
    for(std::unordered_map<string, vector <string>>::iterator it = files_and_samples.begin(); it != files_and_samples.end(); ++it) {
        // it + una coppia chiave -> valore
        // chiave è il primo elemento della coppia: per ottenerla accedo a first
        // metto la chiave nel vector keys
        keys.push_back(it->first);
    }
    // ora ho tutte le chiavi di files_and_samples in keys
    
    
    // scorro tutte le chiavi di files_and_samples
    for (int i = 0; i < keys.size(); i++) {
        // mi salvo l'iesima chiave
        string folder_name = keys[i];
        // creo un'istanza di SampleCollection
        SampleCollection s_c;
        // inizializzo la collezione di samples con il nome della cartella e i nomi dei file al suo interno
        s_c.initialize(folder_name, files_and_samples[folder_name], plantThresholdSlider);
        // inserisco questa collezione in sample_collection
        sample_collection.push_back(s_c);
    }
}

//--------------------------------------------------------------
string ofApp::ofxGetSerialString (ofSerial &serial, char until) {
    /* metodo per leggere dalla porta seriale fino al carattere until */
    
    // https:\//sureskumar.wordpress.com/2012/12/17/sending-strings-from-arduino-to-openframeworks/
    
    // elimino tutto ciò che non ho fatto in tempo a leggere mentre elaboravo
    //serial.flush ();
    // aspetto finché non mi arriva qualcosa da leggere (questione di millisecondi)
    //while (serial.available() == 0) {}
    
    // https:\//www.geeksforgeeks.org/static-keyword-cpp/
    // creo la stringa static str in modo che lo spazio per lei rimanga allocato
    static string str;
    // creo una sequenza di caratteri ss
    stringstream ss;
    // creo un carattere ch
    char ch;
    // finché il leggo un char di dimensione > 0 e questo char è diverso dal parametro until...
    while ((ch = serial.readByte())>0 && ch != until) {
        // metti ch in coda alla sequenza ss
        ss << ch;
    }
    // accedo al valore stringa di ss tramite il il metodo .str() e lo aggiungo a str
    str += ss.str();
    // devo capire perché sono uscita dal while...
    // se è perché ch è = ad until..
    if (ch == until) {
        // creo una variabile temporanea che prende il valore di str
        string tmp = str;
        // aggiorno il valore di str
        str = "";
        // taglio la stringa eliminando i caratteri "speciali"
        return ofxTrimString(tmp);
    }
    // se è perché non ho letto niente, restituisco una stringa vuota
    else {
        return "";
    }
}

//--------------------------------------------------------------
string ofApp::ofxTrimStringRight (string str) {
    /* metodo per tagliare tutto ciò che sta alla destra di str */
    
    // ottengo la posizione dell'ultimo carattere in str che non sia
    // spazio, tab, newline e o \r (carriage return)
    size_t endpos = str.find_last_not_of(" \t\r\n");
    
    // se endpos non indica la fine della stringa tolgo tutto ciò che è alla sua destra
    if (string::npos != endpos) return str.substr(0, endpos + 1);
    // altrimenti vuol dire che non ho caratteri speciali e posso restituire str così com'è
    else return str;
    // else return ""; // secondo me
}

//--------------------------------------------------------------
string ofApp::ofxTrimStringLeft(string str) {
    /* metodo per tagliare tutto ciò che sta alla sinsitra di str */
    
    // ottengo la posizione del primo carattere in str che non sia
    // spazio, tab, newline e o \r (carriage return)
    size_t startpos = str.find_first_not_of(" \t\r\n");
    
    // se startpos non indica la fine della stringa
    // (e quindi se al suo interno è presente almeno un carattere diverso da quelli speciali)
    // tolgo tutto ciò che è alla sua sinistra
    if (string::npos != startpos) return str.substr(startpos);
    // altrimenti vuol dire che non ho caratteri speciali e posso restituire str così com'è
    else return str;
    // else return ""; // secondo me
}

//--------------------------------------------------------------
string ofApp::ofxTrimString (string str) {
    // taglio i caratteri speciali da dx e da sx
    return ofxTrimStringLeft(ofxTrimStringRight(str));
    
    // versione ridotta mia
    /*
     std::string delimiters [] {" ", "\n", "\t", "\r"};
     string my_string = "";
     for (std::string delimiter : delimiters) {
     my_string = my_string.substr(0, my_string.find(delimiter));
     }
     //std::string token = s.substr(0, s.find(delimiter)); // token is "scott"
     return my_string;
     */
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        // se premo la barra spaziatrice salvo lo schermo
        ofImage img;
        img.grabScreen(0, 0 , w, h);
        img.save(std::to_string(ofGetElapsedTimef()) + ".png");
    }
    else if (key == 'c') {
        // se premo c cancello tutti
        deleteDrawings ();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    addDrawing();
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    addDrawing();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
