#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // impostazioni grafiche
    //ofSetVerticalSync(true);
    //ofBackground(BG_COLOR);
    //ofSetLogLevel(OF_LOG_VERBOSE);
    //ofFill();
    ofSetFullscreen(true);
    ofSetFrameRate (60);

    
    // popolo l'array di stringhe che conterrà i nomi delle immagini di background
    for (int i = 0; i < NUM_BGS; i++) {
        bgs [i] = "bgs/" + std::to_string(i+1) + ".jpg";
    }
    last_bg_update = 0; // ultima volta che ho aggiornato il bg
    bg_idx = ofRandom (NUM_BGS); // pesco a caso un indice per l'immagine di bg
    updateBg (); // aggiorno il bg
    
    // inizializzo gli elementi del mio pannello
    millisSlider.setup ("millis before update: ", 0, 0, 90);
    angleSlider.setup ("angle: ", 30, MIN_ANGLE, MAX_ANGLE);
    instrumentSlider.setup ("instrument: ", 4, 0, 9);
    volumeSlider.setup ("volume: ", 1, 0, 10);
    secondsFlowerSlider.setup ("add flower: ", 5, 2, 10);
    bgUpdateSlider.setup ("bg update", 60, 25, 120);
    speedDesappearSlider.setup ("desappear speed", 2, 1, 10);
    differenceInBrightnessSlider.setup ("difference in brightness", 50, 10, 255);
    plantThresholdSlider.setup ("plant threshold: ", 120, 5, 1024);
    
    // setto il mio pannello e vi aggiungo tutti gli elementi settati sopra
    panel.setup ("Parameters");
    panel.setPosition (50, 50);
    panel.add (&millisSlider);
    panel.add (&angleSlider);
    panel.add (&instrumentSlider);
    panel.add (&volumeSlider);
    panel.add (&secondsFlowerSlider);
    panel.add (&bgUpdateSlider);
    panel.add (&speedDesappearSlider);
    panel.add (&differenceInBrightnessSlider);
    panel.add (&plantThresholdSlider);
    
    
    // inizializzo sample_collection con i file contenuti nelle cartelle in data
    string absolute_path = "/Users/martamurtas/Desktop/LAB_proj/1_fiori_stilizzati_su_dipinto/bin/data/samples.txt"; // salvo il percorso in cui si trova il file che elenca le cartelle contenenti i samples
    std::unordered_map<string, vector <string>> files_and_samples = getSamples (absolute_path); // ottengo i nomi dei samples e le loro posizioni nelle diverse cartelle
    init_my_sample_collection (files_and_samples); // creo la mia collezione di samples
    
    
    // inizializzo variabili che mi saranno utili in futuro
    last_posX = ofRandom (MAX_PETAL_LENGTH + BORDER, w - MAX_PETAL_LENGTH - BORDER); // salvo la pos in x del primo albero
    plantValue = -1; // inizializzo a -1 plantValue
    w = ofGetWidth (); // mi salvo la larghezza della finestra
    h = ofGetHeight (); // mi salvo l'altezza della finestra
    
    // definisco la x del mio primo fiore
    int x = BORDER + (last_posX + int(ofRandom (180, 340))) % (w - BORDER*2);
    // creo il primo fiore specificando l'ascissa in cui verrà centrato
    addFlower(x);
    
    
    // per ottenere i dati dalla porta seriale
    // serial.listDevices(); // stampa la lista dei dispositivi disponibili
    //vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList(); // memorizzo la lista dei dispositivi dispo
    int baud = 9600; // la frequenza di aggiornamento
    serial.setup(0, baud); // apri il primo dispositivo disponibile con baud aggiornamenti al secondo
    serial.flush (); // svuota la porta in lettura
    
    
    // richiamo la funzione per il settaggio di inkSim (addon)
    inkSim.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // aggiorno il valore della pianta
    updatePlantValue ();
    
    // aggiorno eventualmente la soglia per la suddivisione dei player in intervalli
    if (sample_collection[instrumentSlider].th != plantThresholdSlider) sample_collection[instrumentSlider].update (plantThresholdSlider);
    // verifico a che intervallo appartiene l'attuale plantValue e suono
    sample_collection[instrumentSlider].play (plantValue, volumeSlider/10);

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // disegno le pennellate
    inkSim.draw();
    
    // disegno i fiori
    for (int i = 0; i < flowers.size() - 1; i++) {
        flowers[i].draw();
    }
    flowers[flowers.size() - 1].draw(plantValue >= plantThresholdSlider);
    
    // disegno il pannello in cima al resto
    ofSetColor (255);
    panel.draw();
    
}

//--------------------------------------------------------------
void ofApp::addFlower (int x) {
    // stabilisco l'ordinata del nuovo fiore
    int y = ofRandom (MAX_PETAL_LENGTH + BORDER, h - (MAX_PETAL_LENGTH + BORDER));
    // richiamo la funzione per generare il nuovo fiore in x, y
    addFlower(x, y);
}

//--------------------------------------------------------------
void ofApp::addFlower (int x, int y) {
    // genero un nuovo fiore
    Flower newFlower;
    // aggiorno la posizione in x dell'ultimo fiore creato
    last_posX = x;
    // genero la posizione del nuovo fiore, facendo sì che si distanzi dall'ultimo.
    // per fare questo devo tenere traccia dell'ultima posizione in x dell'ultimo fiore.
    ofPoint startingPoint = ofPoint (x, y);
    // definisco il colroe del fiore in base al colore assunto dall'img di sfondo in x,y
    ofColor colore = bg_img.getColor (x, y);
    // decremento la luminostià del colore in base a quanto scelto dall'utente
    colore.setBrightness(colore.getBrightness() - differenceInBrightnessSlider);
    // inizializzo il nuovo fiore
    newFlower.initialize(startingPoint, colore);
    // inserisco il nuovo fiore del vettore di fiori
    flowers.push_back(newFlower);
    // aggiorno la variabile che tiene conto del tempo di creazione dell'ultimo fiore
    last_flower_creation_time = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void ofApp::updatePlantValue () {
    /* procedura che aggiorna plantValue a ogni chiamata */
    
    int now = ofGetElapsedTimeMillis(); // salvo il tempo attuale
    // verifico che siano passati almeno millisSlider ms dall'ultimo rilevamento.
    // in caso contrario non aggiorno plantValue e ciò che dipende da lei.
    if (now - last_millis_update >= millisSlider) {
        // ottengo la stringa contenente il valore appena letto dalla porta seriale
        string serialString = ofxGetSerialString(serial, '\n');
        if (serialString.length() > 0 || DEBUG) {
            // se ho letto un valore corretto or DEBUG...

            // salvo l'ultimo valore letto
            last_frequency = plantValue;
        
            // se sono in modalità DEBUG assegno a plantValue un valore random da 0 a plantThresholdSlider
            if (DEBUG) plantValue = ofRandom (plantThresholdSlider);
            else { // se non sono in debug...
                try {
                    // trasformo il valore da string a int
                    plantValue = std::stoi( serialString );
                    if (plantValue > 1023) { // errore in lettura/scrittura
                        plantValue = -1;
                    }
                }
                catch (std::invalid_argument e) {
                    // se per qualche ragione non ricevo il valore da Arduino, lo setto io a 0
                    plantValue = -1;
                }
            }
            
            // aggiorno il disegno dello sfondo
            updateWatercolor ();

            // aggiorno i fiori
            updateFlowers();
            
            serial.flush (); // svuota la porta in lettura
            
            // aggiorno la variabile che tiene conto dell'ultima volta che ho aggiornato plantValue
            last_millis_update = ofGetElapsedTimeMillis ();
        }
    }
}

//--------------------------------------------------------------
void ofApp::updateFlowers () {
    /* procedura che aggiorna i fiori */
    
    // aggiorno la posizione di tutti i fiori tranne l'ultimo (nel caso in cui size != 1, altrimenti non entra nel ciclo)
    for (int i = 0; i < flowers.size() - 1; i++) {
        flowers[i].update(ofPoint (w/2, h/2), speedDesappearSlider);
        // sel la y del fiore raggiunge un certo punto fuori dallo schermo allora elimino il fiore, liberando spazio
        if (flowers[i].startingPoint.x <= - MAX_PETAL_LENGTH || flowers[i].startingPoint.x >= MAX_PETAL_LENGTH + w) flowers.erase (flowers.begin() + i);
    }
    // aggiorno la variabile che tiene traccia dell'ultimo aggiornamento di fr != 0
    int now = ofGetElapsedTimef ();
    if (plantValue != -1) last_frequency_time = now;
    
    // se non è passato troppo tempo dall'ultimo aggiornamento oppure se non è passato troppo tempo dall'ultima volta che ho creato un fiore, aggiorno l'ultimo fiore
    if (now - last_frequency_time <= secondsFlowerSlider && now - last_flower_creation_time <= secondsFlowerSlider) {
        flowers[flowers.size()-1].update (plantValue, angleSlider, plantThresholdSlider);
    }
    // altrimenti creo un nuovo fiore
    else {
        int x = BORDER + (last_posX + int(ofRandom (180, 340))) % (w - BORDER*2);
        addFlower(x);
    }
}

//--------------------------------------------------------------
void ofApp::updateWatercolor () {
    /* procedura che aggiorna il disegno */
    
    // controllo se è il caso di aggiornare l'img di bg
    if (ofGetElapsedTimef() >= last_bg_update + bgUpdateSlider) {
        // aggiorno il bg
        updateBg();
        cout << "aggiorno il bg" << endl;
        // aggiorno la var che tiene conto dell'ultimo aggiornamento
        last_bg_update = ofGetElapsedTimef();
    }
    
    // calcolo il punto da cui prendere il colore...
    // calcolo il numero totale di pixels che compongono l'img
    int bg_w = bg_img.getWidth();
    int bg_h = bg_img.getHeight();
    int total_pixels = bg_w * bg_h;
    // faccio un mapping del valore di plantValue per capire a che pixel corrisponde
    int value = ofMap (plantValue, 0, plantThresholdSlider, 0, total_pixels - 1);
    // calcolo ascissa e ordinata rispettivamente alle dimensioni dell'img di bg
    int x = value % bg_w;
    int y = value / bg_w;
    // ottengo il colore dell'immagine nelle coordinate x, y
    ofColor color = ofColor (0);
    // salvo il colore dell'img di bg in pos x,y
    if (x >= 0 && y >= 0 && x < bg_w && y < bg_h) color = bg_img.getColor(x, y);
    
    
    // calcolo il punto in cui dare una pennellata, ascissa e ordinata
    value = ofMap (plantValue, 0, plantThresholdSlider, 0, w * h - 1);
    x = value % w;
    y = value / w;
    
    // aggiorno la pennellata
    inkSim.update(bEvaporToDisapper);
    // dalla documentazione: The color is converted from the default RGB to an HSB color space and the resulting hue angle is returned.
    inkSim.stroke(&brush, x, y, getInkColor(color.getHueAngle(), 255, 50));
}

//--------------------------------------------------------------
void ofApp::updateBg () {
    /* procedura che aggiorna l'img di bg */
    
    // incremento l'indice dell'attuale bg
    bg_idx += 1;
    // se sono giunta al massimo indice torno indietro
    if (bg_idx == NUM_BGS) bg_idx = 0;
    // carico la nuova img di bg
    bg_img.load(bgs[bg_idx]);
    
    // pulisco la tela dall'acquarello
    inkSim.clear();
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
    /*
    if (plantValue != -1) {
        // elimino tutto ciò che non ho fatto in tempo a leggere mentre elaboravo
        serial.flush ();
        // aspetto finché non mi arriva qualcosa da leggere (questione di millisecondi)
        while (serial.available() == 0) {}
    }*/
    
    
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
    // faccio questo confronto perché se find_last_not_of() non trova niente, mi restituisce string::npos (ovvero -1)
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
    //return ofxTrimStringLeft(ofxTrimStringRight(str));
    
    // versione ridotta
    std::string delimiters [] {" ", "\n", "\t", "\r"};
    string my_string = str;
    for (std::string delimiter : delimiters) {
        my_string = str.substr(0, str.find(delimiter));
    }
    //cout << ofxTrimStringLeft(ofxTrimStringRight(str)) << " vs " << my_string << endl;
    return my_string;
     
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
    if (key == ' ') {
        // se premo la barra spaziatrice salvo lo schermo
        ofImage img;
        img.grabScreen(0, 0 , w, h);
        img.save(std::to_string(ofGetElapsedTimef()) + ".png");
    }
    else if (key == 'c') {
        flowers.clear();
        addFlower(last_posX);
        updateBg ();
        
    }
    else if (key == '1')
    {
        // se premo il tasto 1 vedo lo sfondo colorato
        inkSim.setDrawMode(ofxInkSim::INKFIX);
    }
    else if (key == '2')
    {
        // se premo il tasto 2 vedo lo sfondo in bianco e nero (sono le macchie dell'acqua)
        inkSim.setDrawMode(ofxInkSim::WATERFLOW);
    }
    else if (key == 'e') {
        bEvaporToDisapper = !bEvaporToDisapper;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    addFlower(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    addFlower(x, y);
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

