<Gramatica Livre de Contexto>
P -> B P1 
P1 -> ; B P1 | &
B -> K V C
K -> c K | &
V -> v V | &
C -> b C2 | C1
C1 -> com C1 | &
C2 -> K V ; C e C1 | C e C1
<Sentenca>
&
