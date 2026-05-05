# Hello Squares 3D

Este diretório contém o projeto **HelloSquares3D**, desenvolvido para o Módulo 02 da disciplina de Computação Gráfica da Unisinos. O projeto consiste na implementação da transformação da geometria original de uma pirâmide em um cubo 3D, além da adição de controles interativos via teclado para aplicar transformações de translação, rotação e escala, demonstrando na prática a representação de objetos em cenas 3D por meio de malhas trianguladas.

## 💡 Sobre o Programa

O programa renderiza cubos 3D compostos por 6 faces, sendo cada face formada por 2 triângulos. Cada face do cubo recebe uma cor distinta para facilitar a visualização, uma vez que ainda não há iluminação ou texturas avançadas.

O programa instancia múltiplos cubos na cena e permite controlar o cubo selecionado com teclas de movimento, rotação e escala.

## 📁 Estrutura

- `struct Cube`  
  Representa cada cubo da cena, armazenando posição, escala, estado de rotação e ângulos de transformação.

- `vector<Cube> cubes`  
  Armazena todas as instâncias de cubos renderizadas na cena.

- `selectedCubeIndex`  
  Controla qual cubo está atualmente selecionado para receber transformações via teclado.

- `main()`  
  Inicializa o contexto OpenGL, configura shaders e geometria, executa o loop principal de atualização e renderização.

- `key_callback()`  
  Captura as entradas do teclado e delega os comandos para movimentação, rotação, escala, seleção de cubos e encerramento do programa.

- `handleMovementKeys()`  
  Controla a translação do cubo selecionado nos eixos X, Y e Z.

- `handleRotationKeys()`  
  Ativa ou desativa a rotação contínua nos eixos X, Y e Z.

- `handleScaleKeys()`  
  Controla o aumento e redução uniforme da escala do cubo selecionado.

- `updateCubesRotation()`  
  Atualiza continuamente os ângulos de rotação dos cubos ativos a cada frame.

- `buildCubeModelMatrix()`  
  Monta a matriz de transformação de cada cubo, aplicando translação, escala e rotação.

- `renderCube()`  
  Envia as transformações para a GPU e desenha cada cubo na cena.

- `setupGeometry()`  
  Define a malha poligonal do cubo utilizando 12 triângulos (36 vértices), com cores distintas em cada face.

- `setupShader()`  
  Compila e conecta os shaders responsáveis pelo processamento gráfico.

- `showControlsGuide()`  
  Exibe no terminal uma legenda com os controles disponíveis ao iniciar o programa.

## ⚙️ Como Executar

Para compilar e rodar este projeto, certifique-se de ter um compilador C++ e as bibliotecas necessárias instaladas (GLFW, GLAD, GLM). Você pode usar o Visual Studio Code, CLion, ou outro editor/IDE de sua preferência.

1. Abra o terminal e entre na pasta `build` do projeto: `cd build`
2. Gere os arquivos de build com o CMake (ou configure seu projeto na IDE).
3. Compile o projeto (pode utilizar `cmake --build .` no terminal).
4. Execute o programa gerado (`./HelloSquares3D`).

Certifique-se de que as DLLs das bibliotecas estejam acessíveis no PATH do sistema, se necessário.

## 🎮 Controles

- `X` / `Y` / `Z`: alterna rotação nos eixos X, Y ou Z.
- `A` / `D`: move o cubo no eixo X (esquerda/direita).
- `W` / `S`: move o cubo no eixo Z (aproxima/afasta).
- `I` / `J`: move o cubo no eixo Y (cima/baixo).
- `[` / `]`: diminui / aumenta a escala uniforme do cubo.
- `TAB`: alterna entre os cubos instanciados.
- `ESC`: fecha o programa.

## 🖥️ Preview

[▶ Ver demonstração em vídeo](assets/demos/DEMOHelloSquares3D.mp4)

![Hello Squares 3D](assets/demos/HelloSquares3D.png)
> Exemplo da interface do projeto

## 📌 Observações Finais

- A arquitetura do código está organizada para separar a lógica de entrada, transformação e renderização.
- A estrutura atual pode ser expandida facilmente para incluir novos objetos 3D, câmeras, projeção perspectiva, iluminação e aplicação de texturas.
