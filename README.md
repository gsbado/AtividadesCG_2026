# Computação Gráfica - Híbrido - 2026/1

Este repositório foi criado para a entrega das atividades da disciplina de **Computação Gráfica** do curso de Ciência da Computação (Unisinos - modalidade híbrida), com código em C++ utilizando OpenGL moderna (3.3+). Ele é estruturado para facilitar a organização dos arquivos e a compilação dos projetos utilizando CMake.

## 📌 Entregas de trabalho

1. [Hello3D](src/Desafios/M1/Hello3D.cpp): implementação principal de renderização básica
2. 

## 📂 Estrutura do Repositório

```plaintext
📂 ATIVIDADESCG_2026/
├── 📂 assets/                # Modelos 3D, texturas, fontes, etc.
├── 📂 build/                 # Arquivos gerados pelo CMake (não versionar)
├── 📂 Code snippets/         # Trechos de código auxiliares
├── 📂 Common/                # Código reutilizável (ex: GLAD)
├── 📂 include/               # Bibliotecas e headers
├── 📂 src/
│   ├── 📂 Desafios/
│   │   └── 📂 M1/
│   │       ├── Hello3D.cpp
│   │       └── RESULT.md
│   ├── SpherePhong.cpp
│   └── TriangleTex.cpp
├── 📄 CMakeLists.txt         # Configuração de build
├── 📄 GettingStarted.md      # Guia detalhado de configuração
├── 📄 README.md              # Documentação principal
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
