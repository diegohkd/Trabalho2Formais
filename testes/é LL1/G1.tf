<Gramatica Livre de Contexto>
E -> T E1
E1 -> + T E1 | - T E1 | &
T -> F T1
T1 -> * F T1 | / F T1 | &
F -> P F1
F1 -> ^ P F1 | &
P -> - P | + P | ( E ) | id
<Sentenca>
&
