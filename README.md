<h1 align="center">
  String Slayer
</h1>

<br>
<p align="center">
  <a href="#"> <img src="https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white" /> </a>
  <a href="https://www.raylib.com/"> <img src="https://img.shields.io/badge/Raylib-000000?style=for-the-badge&logoColor=white" /> </a>
</p>

---

<h1><strong>🎸 Descrição</strong></h1>

<h4>
String Slayer é um arcade digital em desenvolvimento, composto por uma coletânea de minijogos com diferentes estilos e desafios. A proposta é oferecer uma experiência dinâmica e acessível, onde cada jogo testa habilidades distintas do jogador, como reflexos, precisão e estratégia. Ideal para quem busca entretenimento variado em uma única plataforma.
</h4>

---

<h1><strong>👥 Vídeo Demonstrativo</strong></h1>

<a href="https://youtu.be/JYtRvtClJTQ?si=ELyZO96sPUvDlrxI">
    <img src="assets\images\String Slayer - Background.png" alt="Stringslayer">
</a>

---

<h1><strong>🛠️ Tecnologias Utilizadas</strong></h1>

<div align="left">
  <img src="https://cdn.jsdelivr.net/gh/devicons/devicon/icons/c/c-original.svg" height="40" alt="C logo" />
  <img width="12" />
  <img src="https://www.raylib.com/favicon.ico" height="40" alt="Raylib logo" />
</div>

---

**Estrutura do projeto**

```bash
String-Slayer/
├── .vscode
├── assets/             # Imagens e sons   
├── build               # Executáveis
├── include/            # Headers do projeto
├── src/                # Códigos-fonte dos minigames
├── Makefile            # Script de build
├── README.md           # README.md
└── scores.dat          # Ponto de entrada do jogo
``` 

<h1><strong>🚀 Instalação e Execução</strong></h1>

### ✅ Pré-requisitos

- Distribuição Linux (Ubuntu/Debian recomendada)
- Compilador `gcc`
- `make` e `cmake`
- Biblioteca gráfica `raylib`

---
### 🔧 Passo a passo

1. Atualize os pacotes do sistema
Execute os seguintes comandos para garantir que seu sistema esteja atualizados
```
sudo apt update && sudo apt upgrade -y
```

2. **Instale depêndencias básicas de compilação**
```bash
sudo apt install -y build-essential make cmake git
````

3. **Instale as bibliotecas gráficas necessárias**
```bash
sudo apt install -y libgl1-mesa-dev libglu1-mesa-dev libx11-dev libxcursor-dev libxrandr-dev libxi-dev libxinerama-dev xorg-dev xserver-xorg-dev libopenal-dev libasound2-dev
```

4. **Instale a raylib**
💡 Se preferir usar a versão mais recente da raylib, siga o método manual abaixo. 
```bash
cd ~
git clone https://github.com/raysan5/raylib.git
cd raylib
mkdir build && cd build
cmake -DPLATFORM=Desktop ..
make
sudo make install
```
✅ Caso já tenha a raylib instalada, você pode pular este passo

5. **Clone o Repositório**
   
```bash
git clone https://github.com/caioguimaraesr/String-Slayer.git
cd String-Slayer
```

6. **Execução do Jogo**
Para compilar e rodar o jogo
```bash
make run
```
Apenas compilar (sem executar)
```bash
make
```
Limpar arquivos objeto:
```bash
make clean
```

<h1><strong>🕹️ Minijogos Inclusos</strong></h1>

<p> 
🏓 Pong — clássico dos arcades, jogável com dois jogadores locais. <br>
🚀 Astro Dodge — desvie e se proteja dos asteroides e sobreviva o máximo possível.<br>
🐤 Avoid the Walls — estilo Flappy Bird, teste seus reflexos!
</p>


<h1> 🎉 Obrigado por instalar o String Slayer! </h1>
Esperamos que você se divirta jogando tanto quanto nós nos divertimos criando! ❤️ 
Bom jogo!

<br>
<h1><strong>👥 Equipe</strong></h1>

<p align="center">
<table>
  <tr>
    <td align="center">
      <a href="https://github.com/caioguimaraesr">
        <img src="https://avatars.githubusercontent.com/caioguimaraesr" width="100" style="border-radius: 50%; margin: 10px;" />
        <br><strong>🎮 Caio G. Rocha</strong>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/rodrigo603">
        <img src="https://avatars.githubusercontent.com/rodrigo603" width="100" style="border-radius: 50%; margin: 10px;" />
        <br><strong>🎮 Rodrigo Dantas</strong>
      </a>
    </td>
  </tr>
</table>
</p>
