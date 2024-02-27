# Chess_UE5.2

## Bug
- [x] Key not found in TMap -> where?
- [ ] Sometimes bot doesn't make any plays -> Find the reason
- [x] Key non found is back -> where?


## Code
- [ ] Commentare le funzioni
- [ ] ChessPieces e ChessPawn verificare se si possono togliere un po di variabili e mettere come attributi della classe
- [ ] Controllare le variabile MarkedForword etc nelle varie classi -> non sempre utili, ma essendo un riferimento devo passare un bool
- [x] Spostare movePiece, capturePiece e win in una calsse "ManagerPieces"


## TODO

- [x] Change color huamn
- [x] uniformare la chessPawn e sitemare la game field
- [x] King -> ogni giocata devo verificare se una o più pedine avversarie possono catturare il re. Se vero metto le tile in uno stato BLOCK_KING 
            e devo spostare il re in una tile consentita oppure spostare una pedina che blocchi il movimento. Per Fare questo metto tutte le tile 
            che portano al re nello stato DANGER. A quel punto potrò spostare la pedina solo nelle caselle con stato danger.

            Chiamo una funzione che verifica se il re è sotto scacco.Metterò un nuovo attriibuto status alle Tile.
            Per fare questo prima controllo le giocate del re dell'attuale player e le metto nello stato MARK_BY_KING. 
            Poi chiamo la LegalMove di ogni pezzo avversario e faccio due cose:
            1) if (SelectedPiece->GetClass()->GetName() == (IsHumanPlayer ? "BP_b_King_C" : "BP_w_King_C"))
               metti vera una variabile Check e alloca il pezzo in un array CheckArray.
            2) Se il pezzo può andare in una tile con stato MARK_BY_KING cambia lo stato in BLOCK_KING e alloca il pezzo nell'array CheckArray.
            A questo punto se la variabile Check è falsa si rimuovono i pezzi dall'array e si impostato lo stato della tile in NEUTRAL. 
            Se vera chiamo una funzione sui pezzi dell'array CheckArray che metta lo staus delle Tile in MARK_TO_AVOID_CHECKMATE.
            A questo punto controllo se il giocatore può spostare una sua pedina in una tile MARK_TO_AVOID_CHECKMATE -> chiamo la LegalMove 
            modificata per fare controlli sul nuovo attributo staus delle tile e faccio il solito (se status == MARK_TO_AVOID_CHECKMATE) imposta
            lo stato in MARKED e allocalo nell'TileMarkedArray. Se alla fine l'array è vuoto c'è stato scacco matto.
            Se non ho fatto scacco matto ripulisco TileMarkedArray, reimposto gli stati aNEUTRAL e a questo punto iniza la solita routine 
            di chiamate alla LegalMove, ma se Check == true faccio controlli sul nuovo attributo status (nella LegalMove) e 
            marco solo le Tile che hanno stato MARK_TO_AVOID_CHECKMATE

- [x] Nota: devo marcare solo le tile in cui la pedina può effettivamente raggiungere il king
- [x] Sistemare il mark delle tile -> l'algoritmo per trovare tutte li tile tra due punti non funziona a dovere
- [x] rilevato movimento errato del pedone quando il re è sotto scacco da un cavallo -> le tile in mezzo non devono esser marcate
- [x] Se il king può muoversi solo a destra e io lo sto minaccaindo con una queen/rook lui si sposta a destra, ma teoricamente non
      scappa dalla minaccia -> dovrebbe esser sconfitta.
- [x] Il re non può fare una giocata che lo espone alla minaccia
- [ ] Se più pedine minacciano il re dovrei verificare che esiste una mossa univoca per scappare da entrambe le minaccie
- [ ] Una pedina non può muoversi se questa giocata espone il re -> gestire
- [x] Il king può mangiare la pedina che la sta minacciando a patto che la mossa non lo esponga a un nuovo scacco -> verificare questa situazione
- [ ] Se il King si sposta tipo A-B, B-A, A-B e gli altri pezzi non si sono spostati è pareggio
- [ ] Problema nella gestione dello scacco con il knight -> principalmente problemi nella possibilià di cattura durante lo scacco 
                                                            che non dovrebbe esser legit


## PawnPromotion 
- [x] Creare una classe che crei il widget prendendo un layout UMG e poi gestisca il click del bottone.
      Potrei distuggere il pawn appena arriva all'ottava riga e far aprire il menu con i bottoni. 
      Il cambio della pedina devo farlo dopo aver mosso il pawn ed esso è arrivato all'8 riga -> quindi nella game mode



## PiecesRecap

- [ ] Nell'array ArrayOfPlay mi salvo tutte le pedine che si spostano. Se viene catturata la rendo non visibile, ma non distruggo l'oggetto. 
      Quando voglio tornare alla posizione i-esima ripercorro la mappa dall'ultimo elemnto a i -> per farlo ottengo la posizione dal pezzo, e sposto
      la pedina con piece->SetActorLocation(NewLocation); 
      Come ottengo dall'interfaccia grafica il numero del turno?
