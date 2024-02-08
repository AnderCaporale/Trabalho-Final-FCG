# Trabalho Final para a cadeira de Fundamentos de Computação Gráfica (INF01047) da UFRGS.


## Sobre o jogo:
  Um jogo de labirinto, em que o objetivo é coletar até 10 coelhos espalhados aleatoriamente pelo mapa. Cada coelho abre um pouco uma parede secreta que dá acesso ao Boss do jogo.
Ao matar o Boss, o jogo é reiniciado após alguns segundos, com mais 10 coelhos em novos lugares.

<br>

## Usabilidade:
### Questões de Jogabilidade:
 - Movimentação com teclado WASD e mouse
 - Botão esquerdo do mouse para usar a espada
 - Botão TAB para trocar da espada para a lantena e vice-versa
 - Botão F para ligar/desligar a lanterna
 - Botão M para abrir o mini-mapa do labirinto

### Questões de Implementação:
 - Shift Q para mudar o modelo de interpolação
 - Shift E para abrir a porta secreta
 - Shift R para mostrar o caminho do início até o fim do labirinto
 - Shift L para mudar para a câmera Look-at
 - Shift C para mudar para a câmera livre

<br>

## Critérios Técnicos
- [x] Malhas poligonais complexas 
&nbsp;&nbsp; Modelos OBJ do minotauro, coelhos, esferas e paredes (cubos).

- [x] Transformações geométricas controladas pelo usuário 
&nbsp;&nbsp; Ataque da espada, abrir porta secreta.

- [x] Câmera livre e câmera look-at 
&nbsp;&nbsp; Livre: Permite movimentação sem colisões e voar. 
&nbsp;&nbsp; Look-At: Fixada no Boss no fim do labirinto.

- [x] Instâncias de objetos
&nbsp;&nbsp; Usado para gerar as paredes do labirinto e os coelhos.

- [x] Três tipos de testes de intersecção
&nbsp;&nbsp; Reta-Plano nas paredes; Ponto-Esfera para os coelhos; Ponto-BoundBox pro minotauro;

- [x] Modelos de Iluminação Difusa e Blinn-Phong 
&nbsp;&nbsp; Coelhos possuem iluminação difusa, e quando iluminada pela lanterna mudam para Blinn-Phong.

- [x] Modelos de Interpolação de Phong e Gouraud 
&nbsp;&nbsp; Possível alterar com o teclado

- [x] Mapeamento de texturas em todos os objetos 
&nbsp;&nbsp; Todos os objetos possuem texturas

- [x] Movimentação com curva Bézier cúbica 
&nbsp;&nbsp; Duas curvas de Bézier para simular o efeito circular do Sol e da Lua

- [x] Animações baseadas no tempo ($\Delta t$) 
&nbsp;&nbsp; Movimentação, Morte do Minotauro


<br>

## Contribuições

Seguem alguns pontos específicos que trabalhamos isoladamente, mas ambos trabalhamos em quase todos os pontos do trabalho.

### Anderson:
- [x] Codificação do Labirinto
- [x] Câmera look-at
- [x] Curvas de Bézier Cúbicas
- [x] SkyBox


### Luca: 
- [x] Colisões
- [x] Modelos de Intepolações
- [x] Movimentação

<br>

## Imagens

Ciclo de Dia e Noite.
  <img src="Imgs/LabirintoDia.png">
  <img src="Imgs/LabirintoPorDoSol.png">
  <img src="Imgs/LabirintoNoite.png">


Lanterna ilumina mais forte o que está perto do player.
  <img src="Imgs/Lanterna.png">


Olhando o Mapa para encontrar o caminho.
<img src="Imgs/OlhandoMapa.png">

Porta secreta que abre ao matar os coelhos pelo labirinto.
<img src="Imgs/PortaSecreta.png">

Boss no final do labirinto.
<img src="Imgs/Boss.png">
