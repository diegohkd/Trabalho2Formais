<Gramatica Livre de Contexto>
P -> D L | L
D -> d D | &
L -> L ; C | C
C -> V = exp | id ( E )
V -> id [ E ] | id
E -> exp , E | exp
<Sentenca>
&
