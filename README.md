# Computação Gráfica - Híbrido - 2026/1

Este repositório foi criado para a entrega das atividades da disciplina de **Computação Gráfica** do curso de Ciência da Computação (Unisinos - modalidade híbrida), com código em C++ utilizando OpenGL moderna (3.3+). Ele é estruturado para facilitar a organização dos arquivos e a compilação dos projetos utilizando CMake.

## 📌 Entregas de trabalho

1. Módulo 1: [Hello3D](src/Desafios/M1/Hello3D.cpp) - implementação principal de renderização básica
2. Módulo 2: [HelloSquares3D](src/Desafios/M2/HelloSquares3D.cpp) - modelagem de um cubo com aplicação de transformações geométricas (translação, escala, rotação) e suporte a múltiplas instâncias via teclado
3. Atividade Vivencial 01: [Vivencial01](src/Desafios/AV1/Vivencial01.cpp) - importação de geometria via .OBJ, renderização de múltiplos objetos 3D e aplicação de transformações independentes com seleção via teclado.
4. Módulo 3: [HelloTextures3D](src/Desafios/M3/HelloTextures3D.cpp) - importação de geometria via .OBJ, leitura de materiais .MTL e renderização de objetos 3D texturizados com mapeamento UV
5. Módulo 4: [HelloIllumination3D](src/Desafios/M4/HelloIllumination3D.cpp) - aplicação do modelo de iluminação de Phong em objetos 3D texturizados, utilizando normais do arquivo .OBJ e propriedades de material definidas em .MTL
6. Atividade Vivencial 02: [Vivencial02](src/Desafios/AV2/Vivencial02.cpp) - implementação de iluminação de três pontos (Key, Fill e Back Light)
7. Módulo 05: [FirstPersonCamera](src/Desafios/M5/FirstPersonCamera.cpp) - implementação de uma câmera em primeira pessoa com movimentação via teclado
8. Módulo 06: [ObjectTrajectories](src/Desafios/M6/ObjectTrajectories.cpp) - [loading]

## 📂 Estrutura do Repositório

```plaintext
📂 ATIVIDADESCG_2026/
├── 📂 assets/                <- Modelos 3D, texturas, fontes, etc.
├── 📂 build/                 <- Arquivos gerados pelo CMake
├── 📂 Code snippets/         <- Trechos de código auxiliares
├── 📂 Common/                <- Código reutilizável
├── 📂 include/               <- Bibliotecas e headers
├── 📂 src/
│   ├── 📂 Desafios/          <- Atividades práticas desenvolvidas ao longo do semestre 2026/1
│   │   ├── 📂 M1/
│   │   |   ├── Hello3D.cpp
│   │   |   └── RESULT.md
│   │   ├── 📂 M2/
│   │   |   ├── HelloSquares3D.cpp
│   │   |   └── README.md
|   |   ├── 📂 AV1/
|   |   |   ├── Vivencial01.cpp
|   |   |   └── README.md
|   |   ├── 📂 M3/
|   |   |   ├── HelloTextures3D.cpp
│   │   |   └── README.md
|   |   ├── 📂 M4/
|   |   |   ├── HelloIllumination3D.cpp
│   │   |   └── README.md
|   |   ├── 📂 AV2/
|   |   |   ├── Vivencial02.cpp
|   |   |   └── README.md
|   |   ├── 📂 M5/
|   |   |   ├── FirstPersonCamera.cpp
│   │   |   └── README.md
|   |   └── 📂 M6/
|   |       ├── ObjectTrajectories.cpp
│   │       └── README.md
│   ├── CuboIluminacao.cpp
│   ├── SpherePhong.cpp
|   └── TriangleTex.cpp
├── 📄 CMakeLists.txt         <- Configuração de build
├── 📄 GettingStarted.md      <- Guia detalhado de configuração
└── 📄 README.md              <- Documentação principal
```

---

## ⚙️ Configuração do projeto
Siga as instruções detalhadas em [GettingStarted.md](GettingStarted.md) para configurar e compilar o projeto.

### 1. Pré-requisitos

Antes de iniciar, certifique-se de ter instalado:

- CMake
- Compilador C++ (GCC, Clang ou MSVC)
- Git (opcional)

### 2. Dependências

As seguintes bibliotecas são gerenciadas automaticamente pelo projeto via CMake:
- `glfw`
- `glm`
- `stb_image`

✅ Não é necessário instalá-las manualmente.

### 3. ⚠️ Configuração obrigatória: GLAD

O **GLAD não é incluído automaticamente** e precisa ser configurado manualmente.

#### 🔗 Acesse o gerador:
👉 [GLAD Generator](https://glad.dav1d.de/)

#### ⚙️ Configure da seguinte forma:
- **API:** OpenGL  
- **Version:** 3.3+ (ou superior compatível com sua máquina)  
- **Profile:** Core  
- **Language:** C/C++  

### 📥 **Baixe e extraia os arquivos:**
Após a geração, extraia os arquivos baixados e coloque-os nos diretórios correspondentes:
- Copie **`glad.h`** para `include/glad/`
- Copie **`khrplatform.h`** para `include/glad/KHR/`
- Copie **`glad.c`** para `common/`

## 🚨 Observações importantes

- Sem esses arquivos de configuração, o projeto não irá compilar.
- Se você mover arquivos, atualize `CMakeLists.txt` para apontar o caminho correto.
- `build/` é gerado pelo CMake e normalmente não entra no Git.
