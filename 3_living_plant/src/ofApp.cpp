#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFullscreen (true); // imposto la modalità a schermo intero
    ofSeedRandom(0); // imposto il seed per la generazione dei numeri random
    ofSetVerticalSync (true);
    w = ofGetWidth (); // salvo la larghezza della finestra
    h = ofGetHeight (); // salvo l'altezza della finestra
    grabber.initGrabber (w, h); // voglio prendere il video di tutta la finestra
    bLearnBackground = true; // voglio salvare l'img di bg
    ofBackground(0); // setto il colore di bg a nero
    
    
    // per ottenere i dati dalla porta seriale
    //serial.listDevices(); // stampo a video la lista delle porte
    //vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList(); // ottengo la lista dei dispositivi
    int baud = 9600; // il numero di dati trasmessi ogni secondo
    serial.setup(0, baud); // apro il primo dispositivo e setto il baud rate
    plantValue = 0; // inizializzo plantValue a 0
    last_millis_update = 0; // inizializzo last_millis_update a 0
    
    // setto i parametri del pannello
    instrumentSlider.setup ("instrument: ", 4, 0, 9);
    volumeSlider.setup ("volume: ", 1, 0, 10);
    millisSlider.setup ("millis before update: ", 0, 0, 90);
    thresholdSlider.setup ("threshold: ", 40, 0, 200);
    minAreaBlobSlider.setup ("min blob area: ", 5000, 1000, 10000);
    maxAreaBlobSlider.setup ("max blob area: ", 700000, 200000, w * h);
    // "Contours can be explained simply as a curve joining all the continuous points (along the boundary), having same color or intensity." da https:\//docs.opencv.org/3.3.0/d4/d73/tutorial_py_contours_begin.html
    maxBlosbsSlider.setup ("max blobs: ", 2, 1, 5);
    interiorContoursSlider.setup ("interior: ", false, false, true);
    useApproximationSlider.setup ("aproximate contour: ", false, false, true);
    leavesAtATimeSlider.setup ("leaves at a time: ", 2, 1, 5);
    densitySlider.setup ("density: ", 2.5, 0, 5);
    bounceSlider.setup ("bounce: ", 0.0, 0.0, 2);
    frictionSlider.setup ("friction: ", 2, 0.0, 2);
    gravitySlider.setup ("gravity: ", 5, 1, 50);
    windSlider.setup ("wind: ", 7, 2, 30);
    triangularizationSlider.setup ("triangulate: ", false, false, true);
    colorSlider.setup ("color: ", ofColor (255), 0, 255);
    plantThresholdSlider.setup ("plant threshold: ", 200, 5, 1024);
    
    // setto il mio pannello
    int panel_border = 20;
    panel.setup ("Parameters");
    panel.setPosition (w - (panel.getWidth() + panel_border), panel_border);
    
    // aggiungo gli elementi al mio pannello
    panel.add (&instrumentSlider);
    panel.add (&volumeSlider);
    panel.add (&millisSlider);
    panel.add (&thresholdSlider);
    panel.add (&minAreaBlobSlider);
    panel.add (&maxAreaBlobSlider);
    panel.add (&maxBlosbsSlider);
    panel.add (&interiorContoursSlider);
    panel.add (&useApproximationSlider);
    panel.add (&leavesAtATimeSlider);
    panel.add (&densitySlider);
    panel.add (&bounceSlider);
    panel.add (&frictionSlider);
    panel.add (&gravitySlider);
    panel.add (&windSlider);
    panel.add (&colorSlider);
    panel.add (&plantThresholdSlider);
    panel.add (&triangularizationSlider);
    
    
    // inizializzo sample_collection con i file contenuti nelle cartelle in data
    string absolute_path = "/Users/martamurtas/Desktop/LAB_proj/3_living_plant/bin/data/samples.txt"; // salvo il percorso in cui si trova il file che elenca le cartelle contenenti i samples
    std::unordered_map<string, vector <string>> files_and_samples = getSamples (absolute_path); // ottengo i nomi dei samples e le loro posizioni nelle diverse cartelle
    init_my_sample_collection (files_and_samples); // creo la mia collezione di samples
    
    
    // inizializzo le immagini che userò per trovare le differenze tra bg e img attuale.
    // assegno loro la grandezza della finestra (fullscreen)
    colorImg.allocate(w, h);
    grayImage.allocate(w, h);
    grayBg.allocate(w, h);
    grayDiff.allocate(w, h);
    
    
    // carico le texture in un vettore di puntatori a texture
    string filename; // stringa che conterrà il nome del file
    for (int i = 0; i < 8; i++) {
        filename = "leaves/leaf_" + std::to_string(i) + ".png"; // creo il nome del file
        ofTexture * texture = new ofTexture (); // creo un puntatore a Texture
        ofLoadImage (*texture, filename); // carico l'immagine di nome filename in texture
        textures_vector.push_back(texture); // metto texture nel vettore delle texture
        //cout << "carico la texture " << filename << endl; // feedback del caricamento
    }
    
    
    windGenerator = ofPoint (0, h/3); // posiziono il mio punto vento in 0, h/3
    wy = 0; // inizializzo ty a 0 (è il parametro per il rumore di Perlin)
    
    box2d.init(); // inizializzo il mio mondo
    box2d.setGravity(0, gravitySlider); // assegno gravità al mondo
    box2d.setFPS(60.0); // l'fps del mondo è diverso da quello dell'applicazione
    box2d.registerGrabbing(); // accendo gli ascoltatori delle collisioni
}

//--------------------------------------------------------------
void ofApp::update(){
    // aggiorno il valore della pianta
    updatePlantValue ();

    
    // aggiorno eventualmente la soglia per la suddivisione dei player in intervalli
    if (sample_collection[instrumentSlider].th != plantThresholdSlider) sample_collection[instrumentSlider].update (plantThresholdSlider);
    // verifico a che intervallo appartiene l'attuale plantValue
    sample_collection[instrumentSlider].play (plantValue, volumeSlider/10);
    
    
    // aggiornamento dei valori dello spettro sonoro in base all'audio riprodotto
    updateSpectrum ();
    
    
    // aggiorno il punto windGenerator
    windGenerator.x = ofMap (plantValue, 0, plantThresholdSlider, 0, w/3);
    windGenerator.y += ofSignedNoise (wy) * 10;
    wy += plantValue * 3;
    
 
    // aggiorno il bg e il ground
    updateBg ();
 
    
    // aggiorno il mio mondo
    updateWorld ();
 
    
    // aggiorno le foglioline
    updateLeaves ();
     
}

//--------------------------------------------------------------
void ofApp::draw(){
    //ofSetColor (255);
    //bgImage.draw (0, 0); // mostro a video l'img di bg
    //ofSetColor (0, 200); // setto il colore nero con alpha 200
    //ofDrawRectangle (0, 0, w, h); // disegno un rettangolo sopra l'immagine di sfondo

    
    // disegno le figure
    drawFigures ();
    
    
    // disegno le foglioline
    for (Leaf * leaf : leaves_vector) {
        leaf->draw (); // richiamo il metodo draw dell'oggetto puntato
    }
    
    
    // mostro a video il pannello coi vari parametri della gui
    panel.draw ();
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
        if (serialString.length() > 0 || DEBUG) { // se ho letto un valore corretto or DEBUG...
            
            // se sono in modalità DEBUG assegno a plantValue un valore random da 0 a 1023
            if (DEBUG) plantValue = ofRandom (1024);
            else { // se non sono in debug...
                try {
                    // trasformo il valore da string a int
                    plantValue = std::stoi( serialString );
                    // errore in lettura/scrittura
                    if (plantValue > 1023)  plantValue = ofRandom (90);
                }
                // se per qualche ragione non ricevo il valore da Arduino, lo setto io a 0
                catch (std::invalid_argument e) {
                    plantValue = ofRandom (90);
                }
                
            }
            // se plantValue >= plantThresholdSlider creo leavesAtATimeSlider nuove foglie
            if (plantValue >= plantThresholdSlider) {
                for (int i = 0; i < leavesAtATimeSlider; i++) {
                    addElementToMyWorld ();
                }
            }
            
            // aggiorno la variabile che tiene conto dell'ultima volta che ho aggiornato plantValue
            last_millis_update = now;
            
        }
    }
    
    serial.flush();
    
}

//--------------------------------------------------------------
void ofApp::updateSpectrum () {
    /* procedura che fa l'update dello spettro sonoro */
    
    // la trasformata di fourier (ofSoundGetSpectrum) restituisce il valore delle frequenze (le ampiezze) per ogni istante
    // le basse frequenze hanno ampiezze elevate
    // le alte frequenze hanno basse ampiezze
    float * currentSpectrum = ofSoundGetSpectrum (BANDS); // divide lo spettro in n bande
    // puntatore a un elemento float.
    
    for (int i = 0; i < 3; i ++) {
        spectrumDisplay [i] *= 0.98;
        spectrumDisplay [i] = max (spectrumDisplay [i], currentSpectrum [i]); // prendo il valore più alto. quindi riprendo currentSpectrum solo quando è maggiore, ed è per questo che non vedo scatti, perché decrementa sempre del 98% fino a quando non trovo un valore + alto.
    }
}

//--------------------------------------------------------------
void ofApp::updateBg(){
    /* procedura che aggiorna l'immagine delle differenze */
    
    grabber.update (); // aggiorno il grabber
    if (grabber.isFrameNew ()) { // se è disponibile un nuovo frame...
        bgImage.setFromPixels (grabber.getPixels ()); // ... aggiorno l'img di sfondo
        bgImage.mirror(false, true); // specchio il frame attuale
        
        // copio il frame attuale specchiato in colorImg
        colorImg.setFromPixels(bgImage);
        // creo una versione in scala di grigi dell'attuale frame specchiato
        // = fa la conversione direttamente (= è un overloading)
        grayImage = colorImg;
        // se è la prima esecuzione salvo l'img di bg in grayBg
        if (bLearnBackground){ // lo eseguo solo la prima volta
            grayBg = grayImage; // overloading (=)
            bLearnBackground = false; // lo metto a false così non si entrerà più in questo if
        }
        // calcolo la differenza in valore assoluto tra l'immagine attuale e l'img di bg
        grayDiff.absDiff(grayBg, grayImage);
        // faccio la sogliatura dell'immagine delle differenze
        // lascio a 1 solo quei pixel il cui valore è > del valore di thresholdSlider
        grayDiff.threshold(thresholdSlider);
        
        // trovo i contorni in grayDiff. i parametri specificati sono:
        // - la minima area di un blob
        // - la massima area di un blob
        // - il massimo numero di blob che voglio trovare
        // - voglio trovare blob con buchi al loro interno?
        // - voglio poter approssimare i bordi?
        int numBlobs = contourFinder.findContours(grayDiff, minAreaBlobSlider, maxAreaBlobSlider, maxBlosbsSlider, interiorContoursSlider, useApproximationSlider);
        
        if (triangularizationSlider) {
            // svuolo triangle
            triangle.clear();
            // faccio la triangolarizzazione dei singoli blob
            for (int i = 0; i < numBlobs; i ++) {
                // passo alla funzione triangulate tutti i punti che compongono l'attuale blob
                triangle.triangulate(contourFinder.blobs[i].pts);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::updateWorld () {
    /* procedura che aggiorna il mio mondo */
    
    // assegno gravità verticale al mondo (0 in orizzontale)
    box2d.setGravity(0, gravitySlider);
    // aggiorno il mondo
    box2d.update ();
    
    // elimino i vertici già presenti in groundLine
    groundLine.clear();
    // ottengo il numero di blob in contourFinder
    int numBlobs = contourFinder.nBlobs;
    // aggiungo tutti i vertici dei tutti i blob a groundLine
    for (int i=0; i < numBlobs; i++) groundLine.addVertices(contourFinder.blobs[i].pts);

    // se c'è almeno un vertice in ground lo svuoto
    if (ground.getVertices().size() > 0 ) ground.clear();
    // se in groundLine ci sono almeno 3 vertici...
    if (groundLine.getVertices().size() > 2) {
        // aggiungo a ground i vertici di groundLine
        ground.addVertexes (groundLine);
        // genero il ground dentro il mondo
        ground.create(box2d.getWorld());
        // aggiorno la forma di ground
        ground.updateShape();
    }
}

//--------------------------------------------------------------
void ofApp::updateLeaves () {
    /* procedura che aggiorna le foglie e la loro fisica */
    
    // creo un oggetto di classe ofRectangle per contenere i contorni immaginari del mondo
    ofRectangle contorni;
    
    // creo un contatore i
    int i = 0;
    // scorro tutte le foglioline puntate...
    for (Leaf * leaf : leaves_vector) {
        // ottengo posizione della foglia attualmente puntata
        ofPoint pos = ofPoint (leaf->getPosition().x, leaf->getPosition().y);
        // ottengo il raggio della foglia e lo triplico
        int radius = leaf->getRadius () * 3;
        // creo i punti di inizio e fine del mio bordo (in alto a sx e in basso a dx)
        ofPoint start = ofPoint (- radius, - radius);
        ofPoint end = ofPoint (w + radius, h + radius);
        // inizializzo contorni chiamando il costruttore e passandogli i punti estremi
        contorni = ofRectangle (start, end);
        // controllo se devo eliminare la foglia perché al di fuori dei miei contorni
        if(! contorni.inside(pos)) { // se contorni non contiene il punto in posizione pos...
            // https:\//openframeworks.cc/ofBook/chapters/stl_vector.html
            leaf = NULL; // metto a NULL la foglia
            delete leaves_vector[i]; // elimino l'oggetto puntato da leaves_vector[i]

            leaves_vector.erase (leaves_vector.begin() + i); // rimuovo l'elemento dal vettore
            i -= 1; // devo tornare indietro di 1 con l'indice: così faccio il controllo dal successivo e non finisco fuori dai confini del vettore in memoria
        }
        else { // aggiorno la sua posizione a seconda della sua distanza dal punto ventoso
            //if(!leaf->isSleeping() && leaf->isBody ()) {
            // aggiorno la fisica della foglia attualmente puntata
            leaf->setPhysics(densitySlider, bounceSlider, frictionSlider);
            // calcolo la distanza tra il punto vento e la posizione della foglia
            float dis = windGenerator.distance(pos);
            // calcolo la forza basandomi sulla distanza tra i 2 punti:
            // se la distanza è tanta la forza è poca.
            float force = ofMap (dis, 0, w, windSlider, windSlider.getMin());
            // se la foglia sta alla destra del punto vento...
            if (pos.x > windGenerator.x && abs(pos.y - windGenerator.y) < 150) {
                // se la distanza in valore assoluto tra le ordinate è < 100... (perché voglio che la foglia sia dentro la finestra)
                    // aggiungo alla foglia il punto di repulsione (windGenerator) con forza force
                leaf->addRepulsionForce(windGenerator, force);
                
            }
            // altrimenti se la foglia non sta alla destra del punto vento...
            else {
                // aggiungo il punto di attrazione (windGenerator) con forza force...
                leaf->addAttractionPoint(windGenerator, force);
            }
        }
        // aggiorno il valore del contatore incrementandolo di 1
        i++;
    }
}

//--------------------------------------------------------------
void ofApp::drawFigures () {
    /* procedura che disegna a video le sagome delle figure trovate */
    
    // disegno la triangolarizzazione richiamando la procedura draw sull'oggetto triangle e passandogli il colore scelto dall'utente tramite il colorpicker (ho modificato il metodo)
    if (triangularizationSlider) triangle.draw (0, 0, colorSlider);
    
    // scelgo una fr per influenzare il raggio e una per la opacità
    //cout << spectrumDisplay [2] << " " << spectrumDisplay [1] << endl;
    float radius = ofMap (spectrumDisplay [2], 0, 0.05, 2, 15); // raggio delle ellissi
    float opacity = ofMap (spectrumDisplay [1], 0, 0.02, 30, 150); // l'opacità delle ellissi
     
    // voglio però ottenere un effetto un po' sfumato, per non mostrare a video una sagoma troppo definita, che può risultare fastidiosa. per fare questo disegno attorno ai bordi delle piccole ellissi che sfumano un minimo la sagoma dei contorni trovati.
    // ottengo il numero di blob trovati dal contourFinder
    int numBlobs = contourFinder.nBlobs;
    // creo c prendendo il colore di colorSlider e settando l'opacità a 50
    ofColor c = ofColor (colorSlider, opacity);
    // setto c come colore per disegnare
    ofSetColor (c);
    // scorro tutti i blob trovati
    for (int i = 0; i < numBlobs; i ++) {
        // scorro tutti i punti di ogni blob
        for (ofPoint point : contourFinder.blobs[i].pts) {
            // disegno un cerchio pieno con centro nel punto point e raggio 5
            ofDrawCircle (point.x, point.y, radius);
        }
    }

}

//--------------------------------------------------------------
void ofApp::addElementToMyWorld () {
    /* procedura che aggiunge una foglia al mondo */
    
    // creo un nuovo oggetto nell'heap e lo punto con newLeaf
    Leaf * newLeaf = new Leaf ();
    
    // genero un raggio random per la foglia
    float radius = ofRandom (20, 45);
    // calcolo la y della foglia come un numero random che va da -radius a +radius
    int y = ofRandom(-radius, radius);
    // setto le coordinate del punto di repulsione
    initialRepulsion.x = -radius*2;
    initialRepulsion.y = y-5;
    
    // faccio il setup, agigorno la fisica, setto la texture della foglia e aggiungo repulsione
    newLeaf->setPhysics (densitySlider, bounceSlider, frictionSlider);
    newLeaf->setup (box2d.getWorld(), -radius*1.5, y, radius);
    newLeaf->setTexture (textures_vector[int(ofRandom(8))]);
    newLeaf->addRepulsionForce(initialRepulsion, plantValue * 5);

    // aggiungo la foglia al vettore
    leaves_vector.push_back(newLeaf);
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
        cout << "inizializzo " << folder_name << endl;
    }
}

//--------------------------------------------------------------
string ofApp::ofxGetSerialString (ofSerial &serial, char until) {
    /* metodo per leggere dalla porta seriale fino al carattere until */
    
    // https:\//sureskumar.wordpress.com/2012/12/17/sending-strings-from-arduino-to-openframeworks/
    
    // elimino tutto ciò che non ho fatto in tempo a leggere mentre elaboravo
    //serial.flush ();
    // aspetto finché non mi arriva qualcosa da leggere (questione di millisecondi)
    //while (serial.available() == 0) {cout << "attendo" << endl;}
    
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
        // se preimo c cancella le foglie
        clearLeaves ();
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
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
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

//--------------------------------------------------------------
void ofApp::clearLeaves () {
    /* procedura che elimina i riferimenti alle foglie dalla memoria */
    
    // scorro tutte le foglie nel vettore di foglie
    for (Leaf * leaf : leaves_vector) {
        delete leaf; // elimino l'oggetto puntato da leaf e libero la RAM
        leaf = NULL; // per essere sicura che non continui a puntare a una zona della RAM appena liberata
    }
    
    // svuoto il vettore
    leaves_vector.clear ();
}

//--------------------------------------------------------------
void ofApp::clearTextures () {
    /* procedura che elimina i riferimenti alle texture dalla mem */
    
    // scorro tutte le textures nel vettore
    for (ofTexture * texture : textures_vector) {
        delete texture; // elimino l'oggetto puntato da texture e libero la RAM
        texture = NULL; // per essere sicura che non continui a puntare a una zona della RAM appena liberata
    }
    
    // svuoto il vettore
    textures_vector.clear ();
}

//--------------------------------------------------------------
void ofApp::exit(){
    /* procedura che libera la memoria dai puntatori */
    
    // prima di uscire mi accerto di aver liberato la memoria dai puntatori.
    // se non lo faccio rimane allocata ma non so più come accedervi.
    
    // elimino le foglie
    clearLeaves ();
    
    // elimino le texture
    clearTextures ();

    
    cout << "Ho liberato la RAM" << endl;
}
