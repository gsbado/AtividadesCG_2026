# First Person Camera

Este diretório contém o projeto Atividade Vivencial 02, desenvolvido para a disciplina de Computação Gráfica da Unisinos. O projeto demonstra a aplicação de iluminação tridimensional utilizando o modelo de Phong combinado com a técnica de iluminação de três pontos (Three-Point Lighting), aplicada a modelos 3D carregados a partir de arquivos `.OBJ`, com materiais definidos em arquivos `.MTL` e texturização.

Este diretório contém o projeto First Person Camera, desenvolvido para a disciplina de Computação Gráfica da Unisinos. O projeto demonstra a implementação de uma câmera em primeira pessoa utilizando OpenGL, combinada com carregamento de modelos 3D, texturização, materiais definidos em arquivos `.MTL` e iluminação baseada no modelo de Phong com a técnica de Three-Point Lighting.

## 💡 Descrição

O projeto carrega o modelo 3D Suzanne a partir de um arquivo `.OBJ`, utilizando o loader `LoadSimpleOBJ.cpp`, responsável por interpretar:

- vértices (`v`)
- coordenadas de textura (`vt`)
- vetores normais (`vn`)
- índices das faces (`f`)

Além da geometria, o programa realiza a leitura do arquivo de materiais `.MTL`, recuperando:

- coeficiente ambiente (`Ka`)
- coeficiente difuso (`Kd`)
- coeficiente especular (`Ks`)
- textura difusa (`map_Kd`)

A cena é composta por duas instâncias do modelo Suzanne renderizadas simultaneamente.

A iluminação é calculada através do modelo de Phong, utilizando três fontes de luz:

- Key Light (luz principal)
- Fill Light (luz de preenchimento)
- Back Light (luz de recorte)

As luzes são posicionadas dinamicamente em torno do objeto selecionado, permitindo visualizar os efeitos produzidos por cada componente da iluminação.

O principal diferencial desta versão é a implementação de uma câmera em primeira pessoa, permitindo que o usuário explore livremente a cena utilizando teclado e mouse.

## 📁 Estrutura

- `src/Desafios/M5/FirstPersonCamera.cpp`
  - implementação principal da aplicação, incluindo inicialização de GLFW/GLAD, carregamento de modelos, texturas, materiais, câmera em primeira pessoa e sistema de iluminação.
- `Code snippets/LoadSimpleOBJ.cpp`
  - contém a função `loadSimpleOBJ(...)` que converte o arquivo `.OBJ` em um VAO para OpenGL.
- `assets/Modelos3D/Suzanne.obj`
  - modelo 3D utilizado na cena.
- `assets/Modelos3D/Suzanne/Suzanne.mtl`
  - arquivo de materiais associado ao modelo.

## ⚙️ Como Executar

Para compilar e rodar este projeto, certifique-se de ter um compilador C++ e as bibliotecas necessárias instaladas (GLFW, GLAD, GLM). Você pode usar o Visual Studio Code, CLion, ou outro editor/IDE de sua preferência.

1. Abra o terminal e entre na pasta `build` do projeto: `cd build`
2. Gere os arquivos de build com o CMake (ou configure seu projeto na IDE).
3. Compile o projeto (pode utilizar `cmake --build .` no terminal).
4. Execute o programa gerado (`./FirstPersonCamera`).

Certifique-se de que as DLLs das bibliotecas estejam acessíveis no PATH do sistema, se necessário.

## 🎮 Controles

### Câmera
- `W` : move a câmera para frente
- `A` : move a câmera para esquerda
- `S` : move a câmera para trás
- `D` : move a câmera para direita
- `Mouse` : Controla a direção da câmera
- `Clique Esquerdo` : Captura o cursor
- `Clique Direito` : Libera o cursor
- `ESC`: fecha o programa.

### Objetos
- `TAB`: alterna entre os objetos instanciados.
- `←` / `→`: move o objeto selecionado no eixo X (esquerda/direita).
- `↑` / `↓`: move o objeto selecionado no eixo Y (cima/baixo).
- `X` / `Y` / `Z`: rotação nos eixos X, Y ou Z do objeto selecionado.
- `[` / `]`: diminui / aumenta a escala uniforme do objeto selecionado.

### Iluminação
- `1`: ativa/desativa a Key Light.
- `2`: ativa/desativa a Fill Light.
- `3`: ativa/desativa a Back Light.

## 🖥️ Preview

[▶ Ver demonstração em vídeo](../../../assets/demos/DEMOFirstPersonCamera.mp4)

![FirstPersonCamera](../../../assets/demos/FirstPersonCamera.png)

> Exemplo de navegação em primeira pessoa

## 📌 Observações Finais
- Implementação de câmera em primeira pessoa utilizando matrizes de visualização (`glm::lookAt`).
- Controle de orientação através do movimento do mouse (Yaw/Pitch).
- Movimentação livre na cena utilizando o teclado.
- Carregamento de modelos `.OBJ` e materiais `.MTL.`
- Aplicação de texturas utilizando `stb_image.`
- Implementação do modelo de iluminação de Phong.
- Configuração de iluminação de três pontos (Key, Fill e Back Light).
- Possibilidade de transformar os objetos em tempo real através do teclado.
- Ativação e desativação individual das fontes de luz durante a execução.