//
//  SampleCollection.hpp
//  vento
//
//  Created by Marta Murtas on 08/03/2019.
//
//


#ifndef SampleCollection_hpp
#define SampleCollection_hpp

#include <stdio.h>
#include <vector>
#include "ofMain.h"

#endif /* SampleCollection_hpp */

class SampleCollection {
    
private:
    //https:\//soundpacks.com/free-sound-packs/ethnic-instrument-samples/
    std::vector <ofSoundPlayer> players; // la SampleCollection è un vettore di ofSoundPlayer
    string folder_name; // è la cartella contenente i file musicali
    vector <string> samples_names; // sono i nomi dei file musicali
    vector <float> intervalli_frequenze; // gli intervalli di frequenza
    vector <ofSoundPlayer> currently_playing; // i player che stanno venendo riprodotti ora
    
public:
    int th; // variabile che tiene conto della soglia per rilevare l'interazione con la pianta
    void initialize (string folder_name, vector <string> sample_names, int th); // procedura che inizializza l'oggetto di classe SampleCollection
    void play (int plantValue, float volume); // procedura per riprodurre un file musicale a seconda del valore di plantValue e del volume scelto
    void update (int th); // procedura che aggiorna la soglia e conseguentemente la divisione delle frequenze per i vari samples
};