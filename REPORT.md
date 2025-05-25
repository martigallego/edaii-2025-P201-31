# Report: Building a search engine like Google 
**Grup:** Martí Gallego, Marc Bermudo, Marçal Bosch  

## Diagrama C4

A continuació es mostren els continguts del diagrama de components (C4):

- **Components volàtils (memòria RAM)**  
    - **CLI Interface**: gestiona la interacció per línia de comandes.  
    - **Document Loader**: parseja els fitxers i construeix les llistes de documents i enllaços.  
    - **Query Processor**: inicialitza i processa les consultes de l’usuari.  
    - **Reverse Index Manager**: construeix i manté en memòria l’índex invers (hashmap).  
    - **Graph Analyzer**: construeix en memòria el grafs de documents i calcula l’índex de rellevància.



- **Components persistents (disc)**  



---