# Report: Building a search engine like Google 
**Grup:** <br> Martí Gallego --> u251445,<br> Marc Bermudo --> u251337,<br> Marçal Bosch --> u251740  

## C4 Component Diagram

> Volàtil (Memòria): Fa servir el color blau<br>
> Persistent (arxiu en el disc): Fa servir el color verd

![DIAGRAMA C4](https://drive.google.com/uc?export=view&id=1yOC9r5ETO1kTWDllP1ObMLN0YtWuveRM)


## Runtime Complexity Analysis (taula)
| **Description**                                                                 | **Big-O**       | **Justification**                                                                                                                                                                                                                                                                                                                                                                                                                                    |
|---------------------------------------------------------------------------------|-----------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Parsing a document into the struct (including adding links)                     | O(N + L^2)       | *N* = número total de caràcters del fitxer (ID + títol + cos). *L* = nombre total d’enllaços (links) <br> • Llegir tot el fitxer (fgets, strcat, etc.) --> O(N).<br> • Cada cop que trobes un enllaç `[text](id)`, crides `afegeixEnllac()`, que recorre la llista d’enllaços des del cap fins al final (cost O(k) si ja hi ha k enllaços). Si hi ha L enllaços en total dins el document, la suma de k=0 a L-1 de O(k) = O(L^2). Per tant, O(N + L^2).   |
| Parsing a query into the struct                                                 | O(m)            | *m* = longitud (en caràcters) de la cadena de consulta.<br> • `strtok(copiaEntrada, " ")` recorre la cadena una sola vegada --> O(m).<br> • Creació de cada node `Consulta` amb inserció en “tail” és O(1) per node (mantens punter `actual`). Si hi ha K tokens, és O(K), però K ≤ m/log m (aprox.), queda O(m).                                                                                                           |
| Counting neighbours in the graph (total edges)                                   | O(V + E)         | *V* = nombre total de nodes (documents). *E* = nombre total d’arestes (links) del graf.<br> • No hi ha cap camp a 0 pre-emmagatzemat que sumi “totes” les arestes entrant o sortint. Per calcular la suma global d’arestes cal recórrer tots els nodes i sumar tots els graus sortints (o entrants) --> per cada node recorres la seva informació, sumar totes les llistes d’adjacència --> O(V + E).                                                                                           |
| Counting neighbours of a specific document                                       | O(1) avg         | Per obtenir el grau d’un document concret, es fa una crida a `cercarNodeEnGraf(graf, idDocument)`: <br> • S’aplica la funció de hash sobre `idDocument` i s’accedeix al bucket corresponent de `graf->nodes` en temps O(1) amortitzat. En cas de col·lisió, cal recórrer una llista curta, de mida O(1) en mitjana. <br> • Un cop trobat el node, es retorna el valor de `n->grauEntrant` o `n->grauSortint`, ja emmagatzemat.                                                               |
| Finding docs containing a keyword (reverse-index)                                | O(W + C)        | *W* = longitud de la paraula a cercar.<br> • Normalització (tolower, treure accents) en O(W).<br> • Càlcul de hash de la paraula en O(W).<br> • Accés a `mapa->entrades[index]` en O(1).<br> • S’exploren les entrades emmagatzemades en el mateix slot (longitud de la cadena de col·lisions = C) fins a trobar la paraula   <br> --> O(C).<br> • Retorna punter a la llista d’IDs de documents, sense recórrer tota la llista de documents.                            |
| Finding docs matching all keywords in query                                      | O(K · D^2)       | *K* = nombre de keywords a la consulta. *D* = mida màxima, per a qualsevol keyword de la llista de documents que contenen aquella paraula.<br>  1. Per a la primera paraula, recuperes la llista `L_1` (mida ≤ D). 2. Per a cada paraula següent `L_i` (també mida ≤ D), fas la **intersecció** de la llista acumulada amb `L_i` fent un doble “for” (cada element de la primera llista contra cada element de la segona --> O(D^2) en el pitjor cas). 3. Ho repeteixes per a cada una de les (K−1) paraules restants. Això dóna O(K*D^2).                        |





## Search Time Analysis: With/Without Reverse Index (gràfica)

![Amb o sense Reverse-Index](https://drive.google.com/uc?export=view&id=1eBtaytErL1SZZaMR6AoPD7Qg7-CZAoln)

S'ha implementat una funció ('comparaMetodesBusqueda') que mesura el temps d'execució (en mil·lisegons) de dues estratègies utilitzant la funció clock() de <time.h>:

- **Cerca sense índex invertit**: es fa una cerca lineal per tots els documents, comprovant si cada paraula clau apareix en el títol o el cos del document.
- **Cerca amb índex invertit**: es consulta un 'hashmap' que relaciona cada paraula amb els documents que la contenen, millorant notablement el temps de resposta.


## Initialization Time vs Hashmap Slot Count (gràfica)

![Temps d'inicialització](https://drive.google.com/uc?export=view&id=1vZCaWLYxyqnYkT3x_VUijFaNVhLS21aJ)


El temps d’inicialització disminueix significativament entre 100 i 2000 slots perquè augmentar els slots redueix les col·lisions i accelera la inserció d’entrades al hashmap (per exemple, en wiki540 baixa de 39.175 ms a 3.042 ms). Tot i això, en wiki5400 (amb slots per sobre de 100 000), el cost addicional de gestionar tants slots fa que els temps es mantinguin al voltant de 174 000–203 000 ms o fins i tot augmentin lleugerament (rendiments decreixents).


## Search Time vs Hashmap Slot Count (gràfica)

![Rendiment cerca per mida de taula hash](https://drive.google.com/uc?export=view&id=1smPI4NPfcvXgU-w-QiTwLoo9_hbyzteR)


El temps de cerca disminueix clarament en passar de 100 a 1000 slots, ja que es redueixen les col·lisions en el hashmap i es millora l’accés a la llista de documents per paraula.
Tanmateix, a partir de 1000 slots no s’observen millores importants i fins i tot hi ha petites oscil·lacions, indicant que existeix una mida òptima per al hashmap a partir de la qual afegir més espai no aporta beneficis clars.

## Reverse Index Improvement Proposal

Una possible millora del reverse-index podria consistir en **fer servir una estructura de "trie" o arbre de prefixes** en lloc del hashmap. En aquesta estructura, cada node representa un caràcter d'una paraula, i les fulles emmagatzemen les llistes de documents on apareix la paraula completa. Així, les cerces de paraules es poden optimitzar al compartir parts de la paraula (prefixes). Per exemple: "cat", "cats", "category".

### Avantatges:

- **Menor temps de cerca**: Aquest nou mètode evita la dispersió del hashing i redueix comparacions inútils entre paraules amb prefixes comuns.
- **Inicialització més ràpida**: L'Inicialització és més ràpida si s’implementa conjuntament amb una estratègia de construcció incremental i s’evita crear múltiples entrades per a paraules repetides dins d’un mateix document
- **Reducció de col·lisions**: Al no fer servir funcions hash, s’eliminen els costos relacionats a resoldre col·lisions.

### Desavantatges:

- **Us de memòria**: En general, un "trie" consumeix **més memòria** que un hashmap normal a causa de la sobrecàrrega de nodes intermedis, especialment si el vocabulari és limitat i les paraules no comparteixen cap prefix.
- **Més complexitat en la implementació**: És més difícil implementar-ho en el teu programa.

### Comparativa:

| Criteri                     | Hashmap normal                   | Trie optimitzat                          |
|-----------------------------|----------------------------------|------------------------------------------|
| Complexitat de cerca        | O(1) mitjana, O(n) en el pitjor cas (col·lisions) | O(k), on *k* es la longitud de la paraula |
| Complexitat de l'inicialització | O(N·k)                          | O(N·k), pero més constant si hi han prefixos compartits|
| Memòria utilitzada           | Mitjana                            | Major, per estructura de nodes intermedis |
| Temps d'execució         | Bo                                 | **Millor en cerces intensives**        |


En resum, aquesta millora **augmentaria l'us de memòria**, però també **reduiria considerablement el temps de cerca** i evitaria col·lisions, cosa que pot ser útil per datasets molt grans.