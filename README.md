# Chess_UE5.2

## Bug
- [ ] Crash if quit game while during bot turn
- [ ] LegalMove bug if reset game during bot turn


## Code
- [ ] Commentare le funzioni


## LegalMove

- [ ] Se il King si sposta tipo A-B, B-A, A-B e gli altri pezzi non si sono spostati è pareggio



## PiecesRecap

- [ ] Uniformare le convenzioni delle mosse a quelle std

## Min-Max

- [ ] Studiare l'algoritmo e una possibile implementazione
- [ ] idea score -> Per calcore il valore della scacchiera ho bisogno di una funzione di valutazione. Il caso base è considerare il valore dei pezzi.
                    Ogni pezzo ha un suo valore e lo score è dato dalla somma dei valori dei miei pezzi meno quella dei pezzi nemici.
                    Se tipo posso fare una cattura questo mi dice il vantaggio in termini di materiale. Poi posso considerare il numero di
                    legal move disponibili per ogni giocata. Più legal move ho, migliore è la situazione perchè ho più giocate disponibili-> uso un contatore, intanto devo simulare ogni spostamento.
                    Poi posso calcorare quanto è difeso il re. Se ad esempio tutte le caselle adiacenti al re sono occupate da pezzi alleati allora il punteggio è alto perchè il re è safe.
                    Infine posso considerare con un valore se la mossa mette sotto scacco.
      Pt         -> pedone: 1, alfiere e torre: 3, regina: 9. Numero legal move: 1(poce mosse) - 5(tante mosse). Sicurezza del re: 1(poco protetto) - 5(ben protetto). Scacco: 6.
      
- [ ] idea minmax -> calcolo la legal move di un pezzo->se può andare in quella posizione me la salvo. A quel punto simulo lo spotamento del pezzo nella prima posizione trovata
                     e valuto il valore della scacchiera. Poi ripeto e simulo lo spostamento per ogni posizione trovata. A quel punto vado a prendere un'altro pezzo e rifaccio la stessa cosa.
                     A questo punto bisogna solo scegliere la giocata col costo maggiore.
      
- [ ] Gestire all'inizio del gioco se usare random palyer o min max. Potrei spedire da blueprint una varibile boolean
      e se true nella game mode faccio lo spawn del minmax else random. In alternativa potrei fare un livello con minmax
      e uno con random da selezionare sempre da menu
