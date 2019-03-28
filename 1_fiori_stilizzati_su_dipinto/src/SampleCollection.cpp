//
//  SampleCollection.cpp
//  plant_OF
//
//  Created by Marta Murtas on 06/03/2019.
//
//

#include "SampleCollection.hpp"


void SampleCollection::initialize (string folder_name, vector <string> sample_names) {
    /* procedura che inizializza l'oggetto di classe SampleCollection */
    
    // salvo il nome della cartella in cui ci sono dei file .waw
    this->folder_name = folder_name;
    // creo un oggetto player, istanza della classe ofSoundPlayer
    ofSoundPlayer player;
    // salvo il numero di file contenuti in folder_name (dimensione di sample_names)
    int num_samples = sample_names.size();
    // scorro tante volte quante sono i samples nella cartella...
    for (int i = 0; i < num_samples; i++) {
        // salvo il titolo dell'iesimo sample
        samples_names.push_back (sample_names[i]);
        // carico il sample
        player.load(sample_names[i]);
        // aggiungo il player nel vettore players
        players.push_back(player);
        // gli permetto di riprodurre + audio in contemporanea
        players[i].setMultiPlay(true);
        // setto l'intervallo di frequenza iesimo
        intervalli_frequenze.push_back((float(700) / float(num_samples)) * i);
    }
    // setto l'ultima frequenza
    intervalli_frequenze.push_back(1024.0);
}

void SampleCollection::play (int plantValue, float volume) {
    /* procedura per riprodurre un file musicale
     a seconda del valore di plantValue e del volume scelto */
    
    // scorro per tutta la grandezza del vettore players...
    for (int i = 0; i < players.size(); i++) {
        // setto il volume a tutti i player uno per volta
        players[i].setVolume(volume);
        // devo trovare il player da far suonare a seconda del valore assunto da plantValue,
        // quindi verifico a che intervallo di frequenza appartiene plantValue
        if (plantValue > intervalli_frequenze[i] && plantValue < intervalli_frequenze[i+1]) {
            // riproduco il sample desiderato richiamando la procedura play sull'iesimo oggetto
            players[i].play();
            // esco dal ciclo perché è inutile continuare a fare controlli negativi
            break;
        }
    }
}
