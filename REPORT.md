# Report: Building a search engine like Google 
**Grup:** Martí Gallego, Marc Bermudo, Marçal Bosch  

## C4 Component Diagram

> Volàtil (Memòria): Fa servir el color blau<br>
> Persistent (arxiu en el disc): Fa servir el color verd

![DIAGRAMA C4](/img/C4diagrama.png)



## Runtime Complexity Analysis (taula)
| **Description**                                                                 | **Big-O**       | **Justification**                                                                                                                                                                                                                                                                                                                                                                                                                                    |
|---------------------------------------------------------------------------------|-----------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Parsing a document into the struct (including adding links)                     | O(N + L^2)       | *N* = número total de caràcters del fitxer (ID + títol + cos).<br> • Llegir tot el fitxer (fgets, strcat, etc.) --> O(N).<br> • Cada cop que trobes un enllaç `[text](id)`, crides `afegeixEnllaç()`, que recorre la llista d’enllaços des del cap fins al final (cost O(k) si ja hi ha k enllaços). Si hi ha L enllaços en total dins el document, la suma de k=0 a L-1 de O(k) = O(L^2). Per tant, O(N + L^2).   |
| Parsing a query into the struct                                                 | O(m)            | *m* = longitud (en caràcters) de la cadena de consulta.<br> • `strtok(copiaEntrada, " ")` recorre la cadena una sola vegada --> O(m).<br> • Creació de cada node `Consulta` amb inserció en “tail” és O(1) per node (mantens punter `actual`). Si hi ha K tokens, és O(K), però K ≤ m/log m (aprox.), i el dominant queda O(m).                                                                                                           |
| Counting neighbours in the graph (total edges)                                   | O(V + E)         | *V* = nombre total de nodes (documents). *E* = nombre total d’arestes (links) del grafo.<br> • No hi ha cap camp a 0 pre-emmagatzemat que sumi “totes” les arestes entrant o sortint. Per calcular la suma global d’arestes cal recórrer tots els nodes i sumar tots els graus sortints (o entrants) → per cada node recorres la seva informació; sumar totes les llistes d’adjacència --> O(V + E).                                                                                           |
| Counting neighbours of a specific document                                       | O(1) avg         | Donat `idDocument`, crides `cercarNodeEnGraf(graf, idDocument)`:<br> • Hash-lookup a `graf->nodes[index]` (calcular hash d’ID --> O(1), accés bucket --> O(1) amortitzat, possiblement repassa una llista de col·lisió molt curta O(1) mitjana).<br> • Retorna directament `n->grauEntrant` o `n->grauSortint` (camp pre-emmagatzemat). No cal recórrer llista d’adjacència.                                                               |
| Finding docs containing a keyword (reverse-index)                                | O(W + C)        | *W* = longitud de la paraula a cercar.<br> • Normalització (tolower, treure accents) en O(W).<br> • Càlcul de hash de la paraula en O(W).<br> • Accés a `mapa->entrades[index]` en O(1) amortitzat.<br> • Recorregut de les entrades dins el mateix bucket (cola = C) fins trobar coincidència --> O(C).<br> • Retorna punter a la llista d’IDs de documents, sense recórrer tota la llista de documents.                            |
| Finding docs matching all keywords in query                                      | O(K · D^2)       | *K* = nombre de keywords a la consulta. *D* = mida màxima, per a qualsevol keyword, de la llista de documents que contenen aquella paraula.<br>  1. Per a la primera paraula, recuperes la llista `L_1` (mida ≤ D). 2. Per a cada paraula següent `L_i` (també mida ≤ D), fas la **intersecció** de la llista acumulada amb `L_i` fent un doble “for” (cada element de la primera llista contra cada element de la segona --> O(D^2) en el pitjor cas). 3. Ho repeteixes per a cada una de les (K−1) paraules restants. Això dóna O(K*D^2).                        |





## Search Time Analysis: With/Without Reverse Index (gràfica)
![Amb o sense Reverse-Index](/img/Amb-sense.png)

S'ha implementat una funció ('comparaMetodesBusqueda') que mesura el temps d'execució (en mil·lisegons) de dues estratègies:

- **Cerca sense índex invertit**: es fa una cerca lineal per tots els documents, comprovant si cada paraula clau apareix en el títol o el cos del document.
- **Cerca amb índex invertit**: es consulta un 'hashmap' que relaciona cada paraula amb els documents que la contenen, millorant notablement el temps de resposta.


## Initialization Time vs Hashmap Slot Count (gràfica)
![Temps d'inicialització](/img/TempsHash.png)

El temps d'inicialització disminueix significativament entre 100 i 2000 slots, amb la millora més destacada en wiki540, on el temps baixa de 39.175 ms (a 100 slots) a 3.042 ms (a 2000 slots). Tot i això, en el dataset wiki5400 (únic amb dades per sobre de 100.000 slots), els temps es mantenen estables entre 174.471 - 203.398 ms o augmenten lleugerament, indicant un decreixement quan hi han molts slots.

## Search Time vs Hashmap Slot Count (gràfica)

![Rendiment cerca per mida de taula hash](/img/MidaHash1.png)

El temps de cerca disminueix clarament en passar de 100 a 1000 slots, ja que es redueixen les col·lisions en el hashmap i es millora l’accés a la llista de documents per paraula.
Tanmateix, a partir de 1000 slots no s’observen millores significatives i fins i tot hi ha petites oscil·lacions, indicant que existeix una mida òptima per al hashmap a partir de la qual afegir més espai no aporta beneficis clars.

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