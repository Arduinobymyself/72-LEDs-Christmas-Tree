// Iluminação da árvore de Natal 72LED 20211112_72LED_XmasTreeV120
// Adicionar messageBoard etc.
// Conectar A1 a GND: Escurecimento com resistor variável, Aberto A1: Brilho especificado pelo valor de configuração do programa (BRIGHT)
// 2021/11/12 alicate de ponta fina

#define BRIGHT 50 // Especifique o brilho do LED com um valor de 0-100 (efetivo quando não for escurecimento manual)
#define TIMER_RESETPIN 11 // pino de reset do timer
#define MODE_PIN 12 // Pino de troca do modo de configuração de brilho
#define OFF_TIMER 21600 // off timer (definir o tempo para parar em segundos. 21600=6Hr)

volatile unsigned int Ly[8]; // exibe a memória padrão
volatile byte bSet = 0; // Valor de configuração de brilho (0-240 com valor do temporizador de interrupção, 0 desliga)
volatile long timeElapsed; // tempo decorrido

unsigned int Ly_a[8], Ly_b[8], Ly_c[8]; // exibe a memória de operação padrão
int L7[16];
unsigned long bitMapH[5]; // Buffer de exibição de bitmap superior
unsigned long bitMapL[5]; // inferior

float bMag = 1.0; // Ampliação de correção de brilho α (brilho final = αx + β) durante o escurecimento dentro do programa
float bOffset = 0.0; // Correção de brilho offset β durante escurecimento no programa

void setup() {
  Serial.begin(115200);
  pinMode(TIMER_RESETPIN, INPUT_PULLUP); // botão de reset do timer
  pinMode(MODE_PIN, INPUT_PULLUP); // Presença ou ausência de configuração de brilho por resistor variável (HIGH: não, LOW: sim)
  pinMode(13, OUTPUT);
  for (int n = 0; n <= 15; n++) {
    L7[n] = 0;
  }
  timeElapsed = 0; // timer de tempo decorrido
  tc2Setup(); // Define o timer 2 (interrompe a cada 2ms)
  show(10); // Execução fictícia uma vez para inicialização (configuração de brilho)
  // ledTest(); // Se não for comentado, teste de conexão do LED
}

void loop() { // Demonstração da exibição Especifique o número de execuções etc. com argumentos
  
  lampTest(5); // teste de lampada
  propeller(5); // hélice
  starSpiralUp(5); // Luzes espirais de baixo para cima
  fallingRing(5); // anel caindo
  dimming(3); // escurecimento
  
  stackUpDown(5); // CIMA BAIXO
  swing(5); // balanço
  verticalCut(5); // corte vertical
  rotateAndClimb(5); // Subir durante a rotação
  dimming(3); // escurecimento
  
  randomStar(5); // Iluminação aleatória Argumento 1 por 3 segundos
  fallingSnow(2); // queda de neve, acúmulo de neve
  verticalSeparate(5); // divisão vertical do centro
  dimming(3); // escurecimento
  

  //ledTest(); // teste de lampada
  //dialLock(1); // bloqueio de discagem (adicionado no V160)
  //messageBoard(2); // rola Feliz NATAL!

  show(2000);
  Serial.println(timeElapsed);
  if (timeElapsed > OFF_TIMER) { // Se o tempo de exibição exceder o valor definido
    while (digitalRead(TIMER_RESETPIN) == HIGH) { // pare até que o botão reiniciar seja pressionado (o reset é mais rápido)
    }
    timeElapsed = 0; // redefine o tempo decorrido e reinicia desde que o reinício foi pressionado
  }
}

void ledTest() { // Teste de conexão do LED (Ligue os LEDs um a um na parte inferior)
  for (;;) { // Em um loop infinito (mesmo conteúdo de starSpiralUp)
    Ly[7] = 0x0001; // planta sementes
    show(300); // acende
    for (int j = 0; j <= 72; j++) { // para cima
      spiralUp(); // espiral para cima
      show(300); // acende
    }
    clearArray();
    delay(200);
  }
}

void lampTest(int n) { // teste de lâmpada
  unsigned int p;
  for(int k = 0; k < n; k++){
    
    p = 0xFFFF; // Padrão de iluminação inicial
    for (int i = 0; i <= 16; i++) {
      for (int j = 0; j <= 7; j++) { // copia o padrão de iluminação para a memória
        Ly[j] = p;
      }
      show(200);
      p = p >> 1; // desloca o padrão de iluminação para a direita em 1 bit
    }
  
  }

}

void messageBoard(int n) { // Quadro de avisos eletrônico (caracteres de rolagem)
  //Definição dos caracteres de exibição Especificados como um bitmap 5x64
  // M-- e-- r-- r-- y-- C-- H-- R--
  bitMapH[0] = 0b10101110110011001010111010101100; // MERRYCH
  bitMapH[1] = 0b11101000101010101010101010101010;
  bitMapH[2] = 0b111011101100110001001000111101100;
  bitMapH[3] = 0b10101000101010100100101010101010;
  bitMapH[4] = 0b10101110101010100100111010101010;
  // I-- S-- T-- M-- A-- S-- (hart)
  bitMapL[0] = 0b01001110111010100100111000110110; // NATAL!
  bitMapL[1] = 0b01001000010011101010100000111110;
  bitMapL[2] = 0b01001110010011101110111000111110;
  bitMapL[3] = 0b01000010010010101010001000011100;
  bitMapL[4] = 0b01001110010010101010111000001000;

  for (int i = 0; i < n; i++) { // repete o número de vezes especificado pelos argumentos
    clearArray(); // Limpa buffer de exibição
    Ly[0] = 0x0180; // os dois primeiros estágios estão acesos
    Ly[1] = 0x03c0;

    for (int j = 31; j >= 0; j--) { // exibir bitMapH
      Ly[3] = Ly[3] | ((bitMapH[0] >> j) & 0x001); // Move o bit de destino para LSB e copia para a memória do display
      Ly[4] = Ly[4] | ((bitMapH[1] >> j) & 0x001);
      Ly[5] = Ly[5] | ((bitMapH[2] >> j) & 0x001);
      Ly[6] = Ly[6] | ((bitMapH[3] >> j) & 0x001);
      Ly[7] = Ly[7] | ((bitMapH[4] >> j) & 0x001);
      show(300); // significa
      Ly[3] = Ly[3] << 1; // rola para a esquerda para a próxima visualização
      Ly[4] = Ly[4] << 1;
      Ly[5] = Ly[5] << 1;
      Ly[6] = Ly[6] << 1;
      Ly[7] = Ly[7] << 1;
    }

    for (int j = 31; j >= 0; j--) { // exibir bitMapL
      Ly[3] = Ly[3] | ((bitMapL[0] >> j) & 0x001); // Move o bit de destino para LSB e copia para a memória do display
      Ly[4] = Ly[4] | ((bitMapL[1] >> j) & 0x001);
      Ly[5] = Ly[5] | ((bitMapL[2] >> j) & 0x001);
      Ly[6] = Ly[6] | ((bitMapL[3] >> j) & 0x001);
      Ly[7] = Ly[7] | ((bitMapL[4] >> j) & 0x001);
      show(300); // significa
      Ly[3] = Ly[3] << 1; // rola para a esquerda para a próxima visualização
      Ly[4] = Ly[4] << 1;
      Ly[5] = Ly[5] << 1;
      Ly[6] = Ly[6] << 1;
      Ly[7] = Ly[7] << 1;
    }

    for (int j = 15; j >= 0; j--) { // rola a tela restante
      Ly[3] = Ly[3] << 1;
      Ly[4] = Ly[4] << 1;
      Ly[5] = Ly[5] << 1;
      Ly[6] = Ly[6] << 1;
      Ly[7] = Ly[7] << 1;
      show(300); // significa
    }
    clearArray();
    show(200);
  }
}

void propeller(int n) { // exibição da hélice
  int a;
  for (int i = 0; i < n; i++) { // Repete o número de vezes especificado pelos argumentos
    for (a = 0; a < 1680 * 2; a += 10) { // rotação positiva (1680 é o mínimo múltiplo comum de 16,14,12,10)
      invertV(a); // Exibição de V reverso no ângulo especificado
      show(5);
    }
    for (a = 1680 * 2; a > 0; a -= 10) { // rotação reversa
      invertV(a);
      show(5);
    }
  }
}

void invertV(int k) { // Luzes em forma de V invertido no ângulo especificado (meia rotação com argumento 840)
  Ly[0] = 0b0000000110000000; // os dois primeiros estão sempre acesos
  Ly[1] = 0b0000001010000000 >> ((k / 420) % 2); // move o padrão V para o ângulo especificado
  Ly[2] = 0b0000010010000000 >> ((k / 280) % 3); // onde 840 é o mínimo múltiplo comum de 8,7,6,5,4,3,2
  Ly[3] = 0b0000100010000000 >> ((k / 210) % 4);
  Ly[4] = 0b0001000010000000 >> ((k / 168) % 5);
  Ly[5] = 0b0010000010000000 >> ((k / 140) % 6);
  Ly[6] = 0b0100000010000000 >> ((k / 120) % 7);
  Ly[7] = 0b1000000010000000 >> ((k / 105) % 8);
}

void starSpiralUp(int n) { // Acende os LEDs um a um de baixo para cima durante a rotação
  for (int i = 0; i < n; i++) { // tempos especificados
    Ly[7] = 0x0001; // planta sementes
    show(20); //
    for (int j = 0; j <= 72; j++) { // para cima
      spiralUp(); // espiral para cima
      show(20); // iluminação
    }
    clearArray();
    delay(200);
  }
}

void fallingRing(int c) { // solta o anel de cima
  for (int n = 0; n < c; n++) { // tempos especificados
    clearArray();
    Ly[0] = 0xFFFF;
    show(100); // Espera um momento para mostrar
    for (int i = 0; i < 7; i++) {
      Ly[i + 1] = Ly[i]; // desloca um passo para trás (para baixo)
      Ly[i] = 0;
      show(70); // espera
    }
  }
  clearArray(); // desliga no final
  delay(200);
}

void stackUpDown(int n) { // Acende quando empilhado, desliga quando empilhado para baixo
  for (int i = 0; i < n; i++) {
    for (int z = 7; z >= 0; z--) { // Luzes empilhadas de baixo para cima
      Ly[z] = 0xFFFF;
      show(30);
    }
    show(200); // Espera um pouco com todas as luzes acesas

    for (int z = 7; z >= 0; z--) { // de baixo para cima
      Ly[z] = 0x0000;
      show(30);
    }
    show(200); // Espera um pouco com todas as luzes apagadas

    for (int z = 0; z <= 7; z++) { // acende de cima para baixo
      Ly[z] = 0xFFFF;
      show(30);
    }
    show(200); // Espera um pouco com todas as luzes acesas

    for (int z = 0; z <= 7; z++) { // de cima para baixo
      Ly[z] = 0x0000;
      show(30);
    }
    show(300); // Espera um pouco com todas as luzes apagadas
  } // repete n vezes
}

void verticalSeparate(int n) { // Separa verticalmente do centro
  for(int i = 0; i < n; i++){
    
    allOn(); // tudo ligado
    show(500);
    Ly[0] &= ~0b0000000110000000;
    Ly[1] &= ~0b0000001111000000;
    Ly[2] &= ~0b0000010110100000;
    Ly[3] &= ~0b0000100110010000;
    Ly[4] &= ~0b0001000110001000;
    Ly[5] &= ~0b0010000110000100;
    Ly[6] &= ~0b0100000110000010;
    Ly[7] &= ~0b1000000110000001;
    show(200);
    Ly[2] &= ~0b0000001001000000;
    Ly[3] &= ~0b0000011001100000;
    Ly[4] &= ~0b0000101001010000;
    Ly[5] &= ~0b0001001001001000;
    Ly[6] &= ~0b0010001001000100;
    Ly[7] &= ~0b0100001001000010;
    show(200);
    Ly[4] &= ~0b0000010000100000;
    Ly[6] &= ~0b0001010000101000;
    Ly[5] &= ~0b0000110000110000;
    Ly[7] &= ~0b0010010000100100;
    show(200);
    Ly[6] &= ~0b0000100000010000;
    Ly[7] &= ~0b0001100000011000; // tudo desligado
    show(300);
  }
}

void swing(int n) { // desloca a linha diagonal para a direita
  for (int i = 0; i < n; i++) { // tempos especificados
    for (int j = 0; j < 2; j++) { // gira para a esquerda duas vezes
      Ly[0] = 0x0100; // Adiciona uma linha diagonal à esquerda
      Ly[1] = 0x0200;
      Ly[2] = 0x0400;
      Ly[3] = 0x0800;
      Ly[4] = 0x1000;
      Ly[5] = 0x2000;
      Ly[6] = 0x4000;
      Ly[7] = 0x8000;

      for (int k = 0; k < 16; k++) { // 16回
        for (int m = 0; m < 8; m++) { // para cada camada
          Ly[m] = Ly[m] >> 1; // desloca a imagem para a direita
        }
        show(50); // espera pela exibição
      }
    }
    show(200);
    for (int j = 0; j < 2; j++) { // gira para a direita duas vezes
      Ly[0] = 0x0080; // adiciona uma linha diagonal à direita
      Ly[1] = 0x0040;
      Ly[2] = 0x0020;
      Ly[3] = 0x0010;
      Ly[4] = 0x0008;
      Ly[5] = 0x0004;
      Ly[6] = 0x0002;
      Ly[7] = 0x0001;

      for (int k = 0; k < 16; k++) { // 16回
        for (int m = 0; m < 8; m++) {
          Ly[m] = Ly[m] << 1; // desloca a imagem para a direita
        }
        show(50); // espera pela exibição
      }
    }
    show(200);
    for (int j = 0; j < 2; j++) { // 2 balanços cruzados
      Ly_a[0] = 0x0100; Ly_b[0] = 0x0080; // Adiciona linhas diagonais em ambos os lados
      Ly_a[1] = 0x0200; Ly_b[1] = 0x0040;
      Ly_a[2] = 0x0400; Ly_b[2] = 0x0020;
      Ly_a[3] = 0x0800; Ly_b[3] = 0x0010;
      Ly_a[4] = 0x1000; Ly_b[4] = 0x0008;
      Ly_a[5] = 0x2000; Ly_b[5] = 0x0004;
      Ly_a[6] = 0x4000; Ly_b[6] = 0x0002;
      Ly_a[7] = 0x8000; Ly_b[7] = 0x0001;

      for (int k = 0; k < 16; k++) { // 16回
        for (int m = 0; m < 8; m++) {
          Ly_a[m] = Ly_a[m] >> 1; // desloca a imagem para a direita em um buffer
        }
        for (int m = 0; m < 8; m++) {
          Ly_b[m] = Ly_b[m] << 1; // deslocamento à esquerda b imagem do buffer
        }
        for (int m = 0; m <= 7; m++) {
          Ly[m] = Ly_a[m] | Ly_b[m]; // Composto para exibir a memória
        }
        show(50); // espera pela exibição
      }
    }
    show(500);
  }
}

void verticalCut(int n) { // liga e desliga horizontalmente com corte vertical
  clearArray();
  for (int i = 0; i < n; i++) { // tempos especificados
    // iluminação da esquerda para a direita
    Ly[6] |= 0b0000100000000000;
    Ly[7] |= 0b0001100000000000; // Define 1 bit para ligar
    show(50);
    Ly[4] |= 0b0000010000000000;
    Ly[5] |= 0b0000110000000000;
    Ly[6] |= 0b0001010000000000;
    Ly[7] |= 0b0010010000000000; // 2
    show(50);
    Ly[2] |= 0b0000001000000000;
    Ly[3] |= 0b0000011000000000;
    Ly[4] |= 0b0000101000000000;
    Ly[5] |= 0b0001001000000000;
    Ly[6] |= 0b0010001000000000;
    Ly[7] |= 0b0100001000000000; // 3
    show(50);
    Ly[0] |= 0b0000000100000000;
    Ly[1] |= 0b0000001100000000;
    Ly[2] |= 0b0000010100000000;
    Ly[3] |= 0b0000100100000000;
    Ly[4] |= 0b0001000100000000;
    Ly[5] |= 0b0010000100000000;
    Ly[6] |= 0b0100000100000000;
    Ly[7] |= 0b1000000100000000; // 4
    show(50);
    Ly[0] |= 0b0000000010000000;
    Ly[1] |= 0b0000000011000000;
    Ly[2] |= 0b0000000010100000;
    Ly[3] |= 0b0000000010010000;
    Ly[4] |= 0b0000000010001000;
    Ly[5] |= 0b0000000010000100;
    Ly[6] |= 0b0000000010000010;
    Ly[7] |= 0b0000000010000001; // 5
    show(50);
    Ly[2] |= 0b0000000001000000;
    Ly[3] |= 0b000000001100000;
    Ly[4] |= 0b0000000001010000;
    Ly[5] |= 0b0000000001001000;
    Ly[6] |= 0b0000000001000100;
    Ly[7] |= 0b0000000001000010; // 6
    show(50);
    Ly[4] |= 0b0000000000100000;
    Ly[5] |= 0b0000000000110000;
    Ly[6] |= 0b0000000000101000;
    Ly[7] |= 0b0000000000100100; // 7
    show(50);
    Ly[6] |= 0b0000000000010000;
    Ly[7] |= 0b0000000000011000; // 8
    show(300);
    // desliga da esquerda para a direita
    Ly[6] &= ~0b0000100000000000;
    Ly[7] &= ~0b0001100000000000; // desliga 1 bit
    show(50); //
    Ly[4] &= ~0b0000010000000000;
    Ly[5] &= ~0b0000110000000000;
    Ly[6] &= ~0b0001010000000000;
    Ly[7] &= ~0b0010010000000000; // 2
    show(50);
    Ly[2] &= ~0b0000001000000000;
    Ly[3] &= ~0b0000011000000000;
    Ly[4] &= ~0b0000101000000000;
    Ly[5] &= ~0b0001001000000000;
    Ly[6] &= ~0b0010001000000000;
    Ly[7] &= ~0b0100001000000000; // 3
    show(50);
    Ly[0] &= ~0b0000000100000000;
    Ly[1] &= ~0b0000001100000000;
    Ly[2] &= ~0b0000010100000000;
    Ly[3] &= ~0b0000100100000000;
    Ly[4] &= ~0b0001000100000000;
    Ly[5] &= ~0b0010000100000000;
    Ly[6] &= ~0b0100000100000000;
    Ly[7] &= ~0b1000000100000000; // 4
    show(50);
    Ly[0] &= ~0b0000000010000000;
    Ly[1] &= ~0b0000000011000000;
    Ly[2] &= ~0b0000000010100000;
    Ly[3] &= ~0b0000000010010000;
    Ly[4] &= ~0b0000000010001000;
    Ly[5] &= ~0b0000000010000100;
    Ly[6] &= ~0b0000000010000010;
    Ly[7] &= ~0b0000000010000001; // 5
    show(50);
    Ly[2] &= ~0b0000000001000000;
    Ly[3] &= ~0b0000000001100000;
    Ly[4] &= ~0b0000000001010000;
    Ly[5] &= ~0b0000000001001000;
    Ly[6] &= ~0b0000000001000100;
    Ly[7] &= ~0b0000000001000010; // 6
    show(50);
    Ly[4] &= ~0b0000000000100000;
    Ly[5] &= ~0b0000000000110000;
    Ly[6] &= ~0b0000000000101000;
    Ly[7] &= ~0b0000000000100100; // 7
    show(50);
    Ly[6] &= ~0b0000000000010000;
    Ly[7] &= ~0b0000000000011000; // 8 Luzes apagadas na extremidade direita
    show(300);
    // iluminação da direita para a esquerda
    Ly[6] |= 0b0000000000010000; //
    Ly[7] |= 0b0000000000011000;
    show(50);
    Ly[4] |= 0b0000000000100000;
    Ly[5] |= 0b0000000000110000;
    Ly[6] |= 0b0000000000101000;
    Ly[7] |= 0b0000000000100100; // 7
    show(50);
    Ly[2] |= 0b0000000001000000;
    Ly[3] |= 0b000000001100000;
    Ly[4] |= 0b0000000001010000;
    Ly[5] |= 0b0000000001001000;
    Ly[6] |= 0b0000000001000100;
    Ly[7] |= 0b0000000001000010; // 6
    show(50);
    Ly[0] |= 0b0000000010000000;
    Ly[1] |= 0b0000000011000000;
    Ly[2] |= 0b0000000010100000;
    Ly[3] |= 0b0000000010010000;
    Ly[4] |= 0b0000000010001000;
    Ly[5] |= 0b0000000010000100;
    Ly[6] |= 0b0000000010000010;
    Ly[7] |= 0b0000000010000001; // 5
    show(50);
    Ly[0] |= 0b0000000100000000;
    Ly[1] |= 0b0000001100000000;
    Ly[2] |= 0b0000010100000000;
    Ly[3] |= 0b0000100100000000;
    Ly[4] |= 0b0001000100000000;
    Ly[5] |= 0b0010000100000000;
    Ly[6] |= 0b0100000100000000;
    Ly[7] |= 0b1000000100000000; // 4
    show(50);
    Ly[2] |= 0b0000001000000000;
    Ly[3] |= 0b0000011000000000;
    Ly[4] |= 0b0000101000000000;
    Ly[5] |= 0b0001001000000000;
    Ly[6] |= 0b0010001000000000;
    Ly[7] |= 0b0100001000000000; // 3
    show(50);
    Ly[4] |= 0b0000010000000000;
    Ly[5] |= 0b0000110000000000;
    Ly[6] |= 0b0001010000000000;
    Ly[7] |= 0b0010010000000000; // 2
    show(50);
    Ly[6] |= 0b0000100000000000;
    Ly[7] |= 0b00011000000000000; // primeiro
    show(300);
    // desliga da direita para a esquerda
    Ly[6] &= ~0b0000000000010000;
    Ly[7] &= ~0b0000000000011000; // desliga 8 bits
    show(50);
    Ly[4] &= ~0b0000000000100000;
    Ly[5] &= ~0b0000000000110000;
    Ly[6] &= ~0b0000000000101000;
    Ly[7] &= ~0b0000000000100100; // 7
    show(50);
    Ly[2] &= ~0b0000000001000000;
    Ly[3] &= ~0b0000000001100000;
    Ly[4] &= ~0b0000000001010000;
    Ly[5] &= ~0b0000000001001000;
    Ly[6] &= ~0b0000000001000100;
    Ly[7] &= ~0b0000000001000010; // 6
    show(50);
    Ly[0] &= ~0b0000000010000000;
    Ly[1] &= ~0b0000000011000000;
    Ly[2] &= ~0b0000000010100000;
    Ly[3] &= ~0b0000000010010000;
    Ly[4] &= ~0b0000000010001000;
    Ly[5] &= ~0b0000000010000100;
    Ly[6] &= ~0b0000000010000010;
    Ly[7] &= ~0b0000000010000001; // 5
    show(50);
    Ly[0] &= ~0b0000000100000000;
    Ly[1] &= ~0b0000001100000000;
    Ly[2] &= ~0b0000010100000000;
    Ly[3] &= ~0b0000100100000000;
    Ly[4] &= ~0b0001000100000000;
    Ly[5] &= ~0b0010000100000000;
    Ly[6] &= ~0b0100000100000000;
    Ly[7] &= ~0b1000000100000000; // 4
    show(50);
    Ly[2] &= ~0b0000001000000000;
    Ly[3] &= ~0b0000011000000000;
    Ly[4] &= ~0b0000101000000000;
    Ly[5] &= ~0b0001001000000000;
    Ly[6] &= ~0b0010001000000000;
    Ly[7] &= ~0b0100001000000000; // 3
    show(50);
    Ly[4] &= ~0b0000010000000000;
    Ly[5] &= ~0b0000110000000000;
    Ly[6] &= ~0b0001010000000000;
    Ly[7] &= ~0b0010010000000000; // 2
    show(50);
    Ly[6] &= ~0b0000100000000000;
    Ly[7] &= ~0b0001100000000000; // 1
    show(500);
  }
}

void rotateAndClimb(int n) { // Subir durante a rotação
  clearArray();
  for (int i = 0; i < n; i++) { // tempos especificados
    for (int j = 0; j < 80; j++) {
      if ( (j % 18) > 8) { // a cada 8 vezes,
        Ly[7] |= 0x0001; // incorpora a estrela na parte inferior
      } else {
        Ly[7] &= ~0x0001; // remove a estrela
      }
      show(40);
      spiralUp();
    }
    for (int j = 0; j < 55; j++) {
      Ly[7] &= ~0x0001;
      show(40);
      spiralUp();
    }
  }
  clearArray();
  delay(500);
}

void dialLock(int n) { // chave de rotação
  int x;
  int maxSpeed = 15; // maxSpeed de alimentação (ms)
  int stdSpeed = 100; // Avanço inicial (ms)
  int accZone = 100; // Número de zonas de aceleração/desaceleração (número de passos)
  int t;
  float ratio; // aumento/diminuição da taxa
  x = OCR2B; // registra o brilho no início
  ratio = pow(1.0 * x, 1.0 / 40.0);
  for (int i = 0; i < n; i++) { // tempos especificados
    bMag = 0.0;
    Ly[0] = 0b0000000110000000; // configuração inicial do padrão de exibição
    Ly[1] = 0b0000000110000000;
    Ly[2] = 0b0000000110000000;
    Ly[3] = 0b0000000110000000;
    Ly[4] = 0b0000000110000000;
    Ly[5] = 0b0000001111000000;
    Ly[6] = 0b0000011111100000;
    Ly[7] = 0b1111111111111111; // Linha inferior está toda acesa

    bMag = 1.0;
    changeBrightness(ratio, 20);
    changeBrightness(1.0 / ratio, 40);
    changeBrightness(ratio, 20);
    bMag = 1.0; // corrige erro de cálculo
    delay(1000);

    // tudo CW
    for (int j = 0; j < 840; j++) { // 840 passos todos na mesma direção (return 7*6*5*4 = 840)
      if (j < accZone) {
        t = interval(j, stdSpeed, maxSpeed, accZone);
      }
      else if (j < (840 - accZone)) {
        t = maxSpeed;
      }
      else {
        t = interval(j - (840 - accZone), maxSpeed, stdSpeed, accZone);
      }
      show(t);
      rotateCW();
    }
    changeBrightness(ratio, 20);
    changeBrightness(1.0 / ratio, 40);
    changeBrightness(ratio, 20);
    bMag = 1.0; // corrige erro de cálculo
    delay(2000);
    ////
        //CW+CCW misturado
        for (int j = 0; j < 840; j++) { // avança 840 passos com rotação reversa para cada camada (return)
          if (j < 200) {
            t = interval(j, stdSpeed, maxSpeed, 200);
          }
          else if (j < 640) {
            t = maxSpeed;
          }
          else {
            t = interval(j - 640, maxSpeed, stdSpeed, 200);
          }
          show(t);
          rotateCCW();
        }
        changeBrightness(ratio, 20);
        changeBrightness(1.0 / ratio, 40);
        changeBrightness(ratio, 20);
        bMag = 1.0; // corrige erro de cálculo
        delay(2000);
    ////
  }
  clearArray(); // desliga
  delay(1000); // espera um minuto
}

void changeBrightness(float r, int n) { // Altere o coeficiente de correção de brilho (bMag) para o número especificado de vezes (n) e a ratio especificada (r)
  for (int i = 0; i <= n; i++) { // tempos especificados
    bMag *= r; // altera ligeiramente o brilho
    show(30); // espera
  }
}

int interval(int x, int p1, int p2, int nnn) { // Encontre o valor x ao mudar de p1 para p2 nnn vezes com uma curva COS d vezes
  float fx, fp1, fp2, fnnn;
  int y;
  fx = x; // ponto flutuante para cálculo
  fp1 = p1;
  fp2 = p2;
  fnnn = nnn;
  y = fp2 + (fp1 - fp2) * 0.5 * (1,0 + cos(3.1416 * fx / fnnn));
  return y;
}

void rotateCW() { // Gira cada camada no sentido horário em um passo (a camada superior gira mais rápido porque se move a uma velocidade constante)
  Ly[0] = Ly[0] << 1; if ( Ly[0] & 0x0400) Ly[0] |= 0x0040;
  Ly[1] = Ly[1] << 1; if (Ly[1] & 0x0400) Ly[1] |= 0x0040;
  Ly[2] = Ly[2] << 1; if (Ly[2] & 0x0800) Ly[2] |= 0x0020;
  Ly[3] = Ly[3] << 1; if (Ly[3] & 0x1000) Ly[3] |= 0x0010;
  Ly[4] = Ly[4] << 1; if (Ly[4] & 0x2000) Ly[4] |= 0x0008;
  Ly[5] = Ly[5] << 1; if (Ly[5] & 0x4000) Ly[5] |= 0x0004;
  Ly[6] = Ly[6] << 1; if (Ly[6] & 0x8000) Ly[6] |= 0x0002;
  Ly[0] &= 0x03C0; // apagador de pó
  Ly[1] &= 0x03C0;
  Ly[2] &= 0x07E0;
  Ly[3] &= 0x0FF0;
  Ly[4] &= 0x1FF8;
  Ly[5] &= 0x3FFC;
  Ly[6] &= 0x7FFE;
}

void rotateCCW() { // Gira cada camada no sentido horário e anti-horário uma etapa (a superior gira mais rápido porque se move a uma velocidade constante)
  Ly[0] = Ly[0] << 1; if ( Ly[0] & 0x0400) Ly[0] |= 0x0040;
  Ly[1] = Ly[1] >> 1; if (Ly[1] & 0x0020) Ly[1] |= 0x0200;
  Ly[2] = Ly[2] << 1; if (Ly[2] & 0x0800) Ly[2] |= 0x0020;
  Ly[3] = Ly[3] >> 1; if (Ly[3] & 0x0008) Ly[3] |= 0x0800;
  Ly[4] = Ly[4] << 1; if (Ly[4] & 0x2000) Ly[4] |= 0x0008;
  Ly[5] = Ly[5] >> 1; if (Ly[5] & 0x0002) Ly[5] |= 0x2000;
  Ly[6] = Ly[6] << 1; if (Ly[6] & 0x8000) Ly[6] |= 0x0002;
  Ly[0] &= 0x03C0; // apagador de pó
  Ly[1] &= 0x03C0;
  Ly[2] &= 0x07E0;
  Ly[3] &= 0x0FF0;
  Ly[4] &= 0x1FF8;
  Ly[5] &= 0x3FFC;
  Ly[6] &= 0x7FFE;
}

void randomStar(int n) { // aceso aleatoriamente
  for (int i = 0; i < n; i++) { // tempos especificados por argumentos
    for (int j = 0; j < 40; j++) { // 6 segundos (0,15 segundos x 40 vezes)
      Ly[0] |= 0x0180; // os dois primeiros estão sempre acesos
      Ly[1] = random(0, 0xFFFF) & random(0, 0xFFFF); // acende com probabilidade 1/4
      Ly[2] = random(0, 0xFFFF) & random(0, 0xFFFF);
      Ly[3] = random(0, 0xFFFF) & random(0, 0xFFFF);
      Ly[4] = random(0, 0xFFFF) & random(0, 0xFFFF);
      Ly[5] = random(0, 0xFFFF) & random(0, 0xFFFF);
      Ly[6] = random(0, 0xFFFF) & random(0, 0xFFFF);
      Ly[7] = random(0, 0xFFFF) & random(0, 0xFFFF);
      show(150);
    }
  } // loop n
  clearArray();
  show(1000);
}

void fallingSnow(int m) { // A queda de neve m é o fator de escala de tempo de exibição
  int p = 8; // probabilidade de aparecimento de neve (probabilidade = 1/p)
  for (int k = 0; k < 8; k++) { // limpa a quantidade de neve
    Ly_c[k] = 0;
  }
  for (int k = 0; k < (220 * m); k++) {
    if (random(0, p) == 0) Ly_a[3] = 0x0800; // Semente da linha A1
    if (random(0, p) == 0) Ly_a[2] = 0x0400; // Semente da linha A2
    if (random(0, p) == 0) Ly_a[1] = 0x0200; // Semente da linha A3
    if (random(0, p) == 0) Ly_a[1] = 0x0100; // Semente da linha A4
    if (random(0, p) == 0) Ly_a[1] = 0x0080; // Semente da linha A5
    if (random(0, p) == 0) Ly_a[1] = 0x0040; // Semente da linha A6
    if (random(0, p) == 0) Ly_a[2] = 0x0020; // Semente da linha A7
    if (random(0, p) == 0) Ly_a[3] = 0x0010; // Semente da linha A8
    if (random(0, p) == 0) Ly_b[3] = 0x0100; // Semente da linha B1
    if (random(0, p) == 0) Ly_b[2] = 0x0100; // Semente da linha B2
    if (random(0, p) == 0) Ly_b[1] = 0x0100; // semente da linha B3
    if (random(0, p) == 0) Ly_b[1] = 0x0200; // Semente da linha B4
    if (random(0, p) == 0) Ly_b[1] = 0x0040; // Semente da linha B5
    if (random(0, p) == 0) Ly_b[1] = 0x0080; // Semente da linha B6
    if (random(0, p) == 0) Ly_b[2] = 0x0080; // Semente da linha B7
    if (random(0, p) == 0) Ly_b[3] = 0x0080; // Semente da linha B8
    if (k > (100 * m)) Ly_c[7] = 0xFFFF; // Acumule gradualmente
    if (k > (120 * m)) Ly_c[6] = 0xFFFF;
    if (k > (140 * m)) Ly_c[5] = 0xFFFF;
    if (k > (160 * m)) Ly_c[4] = 0xFFFF;
    if (k > (170 * m)) Ly_c[3] = 0xFFFF;
    if (k > (185 * m)) Ly_c[2] = 0xFFFF;
    if (k > (200 * m)) Ly_c[1] = 0xFFFF;

    for (int j = 0; j < 8; j++) {
      Ly[j] = Ly_a[j] | Ly_b[j] | Ly_c[j]; // transfere para a memória do display
    }
    Ly[0] |= 0x0180; // acende a estrela principal
    show(80);
    stepDown(); // move a estrela para baixo
  } // próximo k
  clearArray();
  show(1000);
}

void stepDown() { // solte a estrela na rota especificada
  if (Ly_a[6] & 0x0800) L7[11] = 4; else L7[11]--; // mantém o fundo A1 piscando 4 vezes
  if (L7[11] < 0) L7[11] = 0;
  if (L7[11] > 0 ) Ly_a[7] |= 0x0800; else Ly_a[7] &= ~0x0800;
  if (Ly_a[5] & 0x0800) Ly_a[6] |= 0x0800; else Ly_a[6] &= ~0x0800;
  if (Ly_a[4] & 0x0800) Ly_a[5] |= 0x0800; else Ly_a[5] &= ~0x0800;
  if (Ly_a[3] & 0x0800) Ly_a[4] |= 0x0800; else Ly_a[4] &= ~0x0800;
  Ly_a[3] &= ~0x0800;

  if (Ly_a[6] & 0x0400) L7[10] = 4; else L7[10]--; // mantém o fundo A2 piscando 4 vezes
  if (L7[10] < 0) L7[10] = 0;
  if (L7[10] > 0 ) Ly_a[7] |= 0x0400; else Ly_a[7] &= ~0x0400;
  if (Ly_a[5] & 0x0400) Ly_a[6] |= 0x0400; else Ly_a[6] &= ~0x0400;
  if (Ly_a[4] & 0x0400) Ly_a[5] |= 0x0400; else Ly_a[5] &= ~0x0400;
  if (Ly_a[3] & 0x0400) Ly_a[4] |= 0x0400; else Ly_a[4] &= ~0x0400;
  if (Ly_a[2] & 0x0400) Ly_a[3] |= 0x0400; else Ly_a[3] &= ~0x0400;
  Ly_a[2] &= ~0x0400;

  if (Ly_a[6] & 0x0200) L7[9] = 4; else L7[9]--; // mantém o fundo A3 piscando 4 vezes
  if (L7[9] < 0) L7[9] = 0;
  if (L7[9] > 0 ) Ly_a[7] |= 0x0200; else Ly_a[7] &= ~0x0200;
  if (Ly_a[5] & 0x0200) Ly_a[6] |= 0x0200; else Ly_a[6] &= ~0x0200;
  if (Ly_a[4] & 0x0200) Ly_a[5] |= 0x0200; else Ly_a[5] &= ~0x0200;
  if (Ly_a[3] & 0x0200) Ly_a[4] |= 0x0200; else Ly_a[4] &= ~0x0200;
  if (Ly_a[2] & 0x0200) Ly_a[3] |= 0x0200; else Ly_a[3] &= ~0x0200;
  if (Ly_a[1] & 0x0200) Ly_a[2] |= 0x0200; else Ly_a[2] &= ~0x0200;
  Ly_a[1] &= ~0x0200;

  if (Ly_a[6] & 0x0100) L7[8] = 4; else L7[8]--; // mantém o fundo A4 piscando 4 vezes
  if (L7[8] < 0) L7[8] = 0;
  if (L7[8] > 0 ) Ly_a[7] |= 0x0100; else Ly_a[7] &= ~0x0100;
  if (Ly_a[5] & 0x0100) Ly_a[6] |= 0x0100; else Ly_a[6] &= ~0x0100;
  if (Ly_a[4] & 0x0100) Ly_a[5] |= 0x0100; else Ly_a[5] &= ~0x0100;
  if (Ly_a[3] & 0x0100) Ly_a[4] |= 0x0100; else Ly_a[4] &= ~0x0100;
  if (Ly_a[2] & 0x0100) Ly_a[3] |= 0x0100; else Ly_a[3] &= ~0x0100;
  if (Ly_a[1] & 0x0100) Ly_a[2] |= 0x0100; else Ly_a[2] &= ~0x0100;
  Ly_a[1] &= ~0x0100;

  if (Ly_a[6] & 0x0080) L7[7] = 4; else L7[7]--; // mantém o fundo A5 piscando 4 vezes
  if (L7[7] < 0) L7[7] = 0;
  if (L7[7] > 0 ) Ly_a[7] |= 0x0080; else Ly_a[7] &= ~0x0080;
  if (Ly_a[5] & 0x0080) Ly_a[6] |= 0x0080; else Ly_a[6] &= ~0x0080;
  if (Ly_a[4] & 0x0080) Ly_a[5] |= 0x0080; else Ly_a[5] &= ~0x0080;
  if (Ly_a[3] & 0x0080) Ly_a[4] |= 0x0080; else Ly_a[4] &= ~0x0080;
  if (Ly_a[2] & 0x0080) Ly_a[3] |= 0x0080; else Ly_a[3] &= ~0x0080;
  if (Ly_a[1] & 0x0080) Ly_a[2] |= 0x0080; else Ly_a[2] &= ~0x0080;
  Ly_a[1] &= ~0x0080;

  if (Ly_a[6] & 0x0040) L7[6] = 4; else L7[6]--; // mantém o fundo A6 piscando 4 vezes
  if (L7[6] < 0) L7[6] = 0;
  if (L7[6] > 0 ) Ly_a[7] |= 0x0040; else Ly_a[7] &= ~0x0040;
  if (Ly_a[5] & 0x0040) Ly_a[6] |= 0x0040; else Ly_a[6] &= ~0x0040;
  if (Ly_a[4] & 0x0040) Ly_a[5] |= 0x0040; else Ly_a[5] &= ~0x0040;
  if (Ly_a[3] & 0x0040) Ly_a[4] |= 0x0040; else Ly_a[4] &= ~0x0040;
  if (Ly_a[2] & 0x0040) Ly_a[3] |= 0x0040; else Ly_a[3] &= ~0x0040;
  if (Ly_a[1] & 0x0040) Ly_a[2] |= 0x0040; else Ly_a[2] &= ~0x0040;
  Ly_a[1] &= ~0x0040;

  if (Ly_a[6] & 0x0020) L7[5] = 4; else L7[5]--; // mantém o fundo A7 piscando 4 vezes
  if (L7[5] < 0) L7[5] = 0;
  if (L7[5] > 0 ) Ly_a[7] |= 0x0020; else Ly_a[7] &= ~0x0020;
  if (Ly_a[5] & 0x0020) Ly_a[6] |= 0x0020; else Ly_a[6] &= ~0x0020;
  if (Ly_a[4] & 0x0020) Ly_a[5] |= 0x0020; else Ly_a[5] &= ~0x0020;
  if (Ly_a[3] & 0x0020) Ly_a[4] |= 0x0020; else Ly_a[4] &= ~0x0020;
  if (Ly_a[2] & 0x0020) Ly_a[3] |= 0x0020; else Ly_a[3] &= ~0x0020;
  Ly_a[2] &= ~0x0020;

  if (Ly_a[6] & 0x0010) L7[4] = 4; else L7[4]--; // mantém o fundo A8 piscando 4 vezes
  if (L7[4] < 0) L7[4] = 0;
  if (L7[4] > 0 ) Ly_a[7] |= 0x0010; else Ly_a[7] &= ~0x0010;
  if (Ly_a[5] & 0x0010) Ly_a[6] |= 0x0010; else Ly_a[6] &= ~0x0010;
  if (Ly_a[4] & 0x0010) Ly_a[5] |= 0x0010; else Ly_a[5] &= ~0x0010;
  if (Ly_a[3] & 0x0010) Ly_a[4] |= 0x0010; else Ly_a[4] &= ~0x0010;
  Ly_a[3] &= ~0x0010;

  // Transmite as estrelas na linha B1-B8
  if (Ly_b[6] & 0x0800) L7[12] = 4; else L7[12]--;
  if (L7[12] < 0) L7[12] = 0;
  if (L7[12] > 0 ) Ly_b[7] |= 0x1000; else Ly_b[7] &= ~0x1000;
  if (Ly_b[5] & 0x0400) Ly_b[6] |= 0x0800; else Ly_b[6] &= ~0x0800;
  if (Ly_b[4] & 0x0200) Ly_b[5] |= 0x0400; else Ly_b[5] &= ~0x0400;
  if (Ly_b[3] & 0x0100) Ly_b[4] |= 0x0200; else Ly_b[4] &= ~0x0200;
  Ly_b[3] &= ~0x0100;

  if (Ly_b[6] & 0x1000) L7[13] = 4; else L7[13]--;
  if (L7[13] < 0) L7[13] = 0;
  if (L7[13] > 0 ) Ly_b[7] |= 0x2000; else Ly_b[7] &= ~0x2000;
  if (Ly_b[5] & 0x0800) Ly_b[6] |= 0x1000; else Ly_b[6] &= ~0x1000;
  if (Ly_b[4] & 0x0400) Ly_b[5] |= 0x0800; else Ly_b[5] &= ~0x0800;
  if (Ly_b[3] & 0x0200) Ly_b[4] |= 0x0400; else Ly_b[4] &= ~0x0400;
  if (Ly_b[2] & 0x0100) Ly_b[3] |= 0x0200; else Ly_b[3] &= ~0x0200;
  Ly_b[2] &= ~0x0100;

  if (Ly_b[6] & 0x2000) L7[14] = 4; else L7[14]--;
  if (L7[14] < 0) L7[14] = 0;
  if (L7[14] > 0 ) Ly_b[7] |= 0x4000; else Ly_b[7] &= ~0x4000;
  if (Ly_b[5] & 0x1000) Ly_b[6] |= 0x2000; else Ly_b[6] &= ~0x2000;
  if (Ly_b[4] & 0x0800) Ly_b[5] |= 0x1000; else Ly_b[5] &= ~0x1000;
  if (Ly_b[3] & 0x0400) Ly_b[4] |= 0x0800; else Ly_b[4] &= ~0x0800;
  if (Ly_b[2] & 0x0200) Ly_b[3] |= 0x0400; else Ly_b[3] &= ~0x0400;
  if (Ly_b[1] & 0x0100) Ly_b[2] |= 0x0200; else Ly_b[2] &= ~0x0200;
  Ly_b[1] &= ~0x0100;

  if (Ly_b[6] & 0x4000) L7[15] = 4; else L7[15]--; // B4 mantém o fundo piscando 4 vezes
  if (L7[15] < 0) L7[15] = 0;
  if (L7[15] > 0 ) Ly_b[7] |= 0x8000; else Ly_b[7] &= ~0x8000;
  if (Ly_b[5] & 0x2000) Ly_b[6] |= 0x4000; else Ly_b[6] &= ~0x4000;
  if (Ly_b[4] & 0x1000) Ly_b[5] |= 0x2000; else Ly_b[5] &= ~0x2000;
  if (Ly_b[3] & 0x0800) Ly_b[4] |= 0x1000; else Ly_b[4] &= ~0x1000;
  if (Ly_b[2] & 0x0400) Ly_b[3] |= 0x0800; else Ly_b[3] &= ~0x0800;
  if (Ly_b[1] & 0x0200) Ly_b[2] |= 0x0400; else Ly_b[2] &= ~0x0400;
  Ly_b[1] &= ~0x0200;

  if (Ly_b[6] & 0x0002) L7[0] = 4; else L7[0]--; // B5 mantém o fundo piscando 4 vezes
  if (L7[0] < 0) L7[0] = 0;
  if (L7[0] > 0 ) Ly_b[7] |= 0x0001; else Ly_b[7] &= ~0x0001;
  if (Ly_b[5] & 0x0004) Ly_b[6] |= 0x0002; else Ly_b[6] &= ~0x0002;
  if (Ly_b[4] & 0x0008) Ly_b[5] |= 0x0004; else Ly_b[5] &= ~0x0004;
  if (Ly_b[3] & 0x0010) Ly_b[4] |= 0x0008; else Ly_b[4] &= ~0x0008;
  if (Ly_b[2] & 0x0020) Ly_b[3] |= 0x0010; else Ly_b[3] &= ~0x0010;
  if (Ly_b[1] & 0x0040) Ly_b[2] |= 0x0020; else Ly_b[2] &= ~0x0020;
  Ly_b[1] &= ~0x0040;

  if (Ly_b[6] & 0x0004) L7[1] = 4; else L7[1]--; // B6 mantém o fundo piscando 4 vezes
  if (L7[1] < 0) L7[1] = 0;
  if (L7[1] > 0 ) Ly_b[7] |= 0x0002; else Ly_b[7] &= ~0x0002;
  if (Ly_b[5] & 0x0008) Ly_b[6] |= 0x0004; else Ly_b[6] &= ~0x0004;
  if (Ly_b[4] & 0x0010) Ly_b[5] |= 0x0008; else Ly_b[5] &= ~0x0008;
  if (Ly_b[3] & 0x0020) Ly_b[4] |= 0x0010; else Ly_b[4] &= ~0x0010;
  if (Ly_b[2] & 0x0040) Ly_b[3] |= 0x0020; else Ly_b[3] &= ~0x0020;
  if (Ly_b[1] & 0x0080) Ly_b[2] |= 0x0040; else Ly_b[2] &= ~0x0040;
  Ly_b[1] &= ~0x0080;

  if (Ly_b[6] & 0x0008) L7[2] = 4; else L7[2]--; // B7 mantém a parte inferior piscando 4 vezes
  if (L7[2] < 0) L7[2] = 0;
  if (L7[2] > 0 ) Ly_b[7] |= 0x0004; else Ly_b[7] &= ~0x0004;
  if (Ly_b[5] & 0x0010) Ly_b[6] |= 0x0008; else Ly_b[6] &= ~0x0008;
  if (Ly_b[4] & 0x0020) Ly_b[5] |= 0x0010; else Ly_b[5] &= ~0x0010;
  if (Ly_b[3] & 0x0040) Ly_b[4] |= 0x0020; else Ly_b[4] &= ~0x0020;
  if (Ly_b[2] & 0x0080) Ly_b[3] |= 0x0040; else Ly_b[3] &= ~0x0040;
  Ly_b[2] &= ~0x0080;

  if (Ly_b[6] & 0x0010) L7[3] = 4; else L7[3]--; // B8 mantém o fundo piscando 4 vezes
  if (L7[3] < 0) L7[3] = 0;
  if (L7[3] > 0 ) Ly_b[7] |= 0x0008; else Ly_b[7] &= ~0x0008;
  if (Ly_b[5] & 0x0020) Ly_b[6] |= 0x0010; else Ly_b[6] &= ~0x0010;
  if (Ly_b[4] & 0x0040) Ly_b[5] |= 0x0020; else Ly_b[5] &= ~0x0020;
  if (Ly_b[3] & 0x0080) Ly_b[4] |= 0x0040; else Ly_b[4] &= ~0x0040;
  Ly_b[3] &= ~0x0080;
}

void dimming(int c) { // escurecimento lento, escurecimento
  float x, ratio;
  bMag = 0.0; // Define temporariamente a intensidade da luz para zero
  show(10); // reflete o valor de configuração de brilho
  allOn(); // tudo ligado
  x = OCR2B; // registra o brilho no início
  ratio = pow(2.0 * x, 1.0 / 40.0); // Calcula a taxa de brilho para dobrar o brilho
  for (int j = 0; j < c; j++) { // tempos especificados
    bMag = 1,0/x;
    for (int i = 0; i <= 40; i++) { // iluminar com série geométrica
      bMag *= ratio; // valor correto
      show(30);
    }
    for (int i = 40; i >= 0; i--) { 
      bMag/= ratio;
      show(30);
    }
    show(100);
  }
  clearArray(); // Limpa a memória do padrão
  bMag = 1.0; // Restaura o fator de correção de brilho padrão
  show(500); // desliga no final
}

void ledOff() { // Desliga todos os LEDs pela operação da porta (Define todos os pinos do LED para baixo com a entrada)
  PORTD &= B00000011; // D2-7 BAIXO
  PORTB &= B11111000; // D8-10 BAIXO
  DDRD &= B00000010; // Atribui D2-7 às entradas
  DDRB &= B11111000; // atribui D8-10 à entrada
}

void clearArray() { // limpa a memória padrão
  for (int i = 0; i < 8; i++) {
    Ly[i] = 0;
    Ly_a[i] = 0;
    Ly_b[i] = 0;
  }
}

void allOn() { // liga todos os LEDs na memória padrão)
  Ly[0] = 0x0180;
  Ly[1] = 0x03C0;
  Ly[2] = 0x07E0;
  Ly[3] = 0x0FF0;
  Ly[4] = 0x1FF8;
  Ly[5] = 0x3FFC;
  Ly[6] = 0x7FFE;
  Ly[7] = 0xFFFF;
}

void spiralUp() { // espiral para cima
  Ly[0] &= 0x0180; // Limpa a área de exibição externa
  Ly[1] &= 0x03C0;
  Ly[2] &= 0x07E0;
  Ly[3] &= 0x0FF0;
  Ly[4] &= 0x1FF8;
  Ly[5] &= 0x3FFC;
  Ly[6] &= 0x7FFE;

  Ly[0] = Ly[0] << 1;
  if (Ly[1] & 0x0200) Ly[0] |= 0x0080; // Se Ly[1] mais à esquerda é 1, então Ly[0] mais à direita é 1
  Ly[1] = Ly[1] << 1;
  if (Ly[2] & 0x0400) Ly[1] |= 0x0040; // Se Ly[2] mais à esquerda é 1, então Ly[1] mais à direita é 1
  Ly[2] = Ly[2] << 1;
  if (Ly[3] & 0x0800) Ly[2] |= 0x0020; // Se Ly[3] mais à esquerda é 1, então Ly[2] mais à direita é 1
  Ly[3] = Ly[3] << 1;
  if (Ly[4] & 0x1000) Ly[3] |= 0x0010; // Se Ly[4] mais à esquerda é 1, então Ly[3] mais à direita é 1
  Ly[4] = Ly[4] << 1;
  if (Ly[5] & 0x2000) Ly[4] |= 0x0008; // Se Ly[5] mais à esquerda é 1, então Ly[4] mais à direita é 1
  Ly[5] = Ly[5] << 1;
  if (Ly[6] & 0x4000) Ly[5] |= 0x0004; // Se Ly[6] mais à esquerda é 1, então Ly[5] mais à direita é 1
  Ly[6] = Ly[6] << 1;
  if (Ly[7] & 0x8000) Ly[6] |= 0x0002; // Se Ly[7] mais à esquerda é 1, então Ly[6] mais à direita é 1
  Ly[7] = Ly[7] << 1;
}

void show(unsigned long t) { // Espera o tempo especificado para mostrar. Execute o cálculo de escurecimento ao mesmo tempo
  int x;
  delay(t);
  if (digitalRead(MODE_PIN) == LOW) { // se o ajuste de brilho por resistor variável estiver ativado
    x = analogRead(0); // lê o valor do sensor de luz
    x = map(x, 0, 1023, 10, 240); // Escala valores ADC para intervalo 10-240
  } else { // se você não usar um resistor variável
    x = map(BRIGHT, 0, 100, 10, 240); // usa o valor BRIGHT definido no programa
  }
  x = x * bMag + bOffset; // cálculo de correção de brilho
  bSet = constrain(x, 0, 240); // brilho final (usado como configuração de PWM na rotina de interrupção)
}

void tc2Setup() { // Configuração do TC2 para iluminação dinâmica
  // O método de configuração foi ensinado por Izakaya Garage Nikki no artigo abaixo. Obrigado!
  // http://igarage.cocolog-nifty.com/blog/2020/12/post-4c28b8.html
  cli(); // desabilita as interrupções
  TCCR2B = 0x00; // para o timer 2
  TCCR2A = 0b00000010; // define o modo novamente
  // |||| ++--- WGM: claro no modo CTC OCR2A
  // ||++------- Operação da porta padrão COM2B D3 PD3 (OC2A desconectado)
  // ++--------- COM2A D11 PB3 operação de porta padrão (OC2B desconectado)
  TCCR2B = 0b00000101; // Acerto do relógio
  // |+++--- CS:1/128 : 125kHz (passos de 8μs)
  // +------ WGM02
  OCR2A = 250 - 1; // 250 ciclos de contagem (interrupção no ciclo de 2ms)
  OCR2B = 10 - 1; // Interrompe em 10 contagens (80us) (o valor é provisório)
  TIMSK2 = 0b00000110;
  // ||+---- sem interrupção de estouro TOIE2
  // |+----- OCIE2A compara correspondência A interrupção ativa
  // +------ OCIE2B comparar match B interrupção habilitar
  sei(); // permissão de corte
}

ISR(TIMER2_COMPA_vect) { // Timer 2 A interrupção (acende o LED especificado)
  static int scanN = 0; // atualiza o contador de posição
  static int ttt = 0; // contador de tempo decorrido
  ttt++;
  if (ttt >= 500) { // após 1 segundo
    ttt = 0;
    timeElapsed++; // Incrementa o timer de exibição em 1 segundo (o valor é a especificação do timer de desligamento automático)
  }
  if (bSet > 0) { // Se o valor de configuração de brilho for 0 ou mais (bSet é de 8 bits, portanto, as interrupções não precisam ser desativadas)
    ledOff(); // Desliga todos os LEDs (desliga para se esconder nos bastidores)
    switch (scanN) { // Acende o LED correspondente ao número do scan (acende na divisão de tempo)
      case 0:
        d2High(); break;
      case 1:
        d3High(); break;
      case 2:
        d4High(); break;
      case 3:
        d5High(); break;
      case 4:
        d6High(); break;
      case 5:
        d7High(); break;
      case 6:
        d8High(); break;
      case 7:
        d9High(); break;
      case 8:
        d10High(); break;
      default:
        break;
    }
  }
  scanN++; // atualiza o número do escaneamento
  if (scanN > 8) { // zera se o limite for excedido
    scanN = 0;
  }
}

ISR(TIMER2_COMPB_vect) { // Interrupção do Timer 2 B (desliga e ajusta o brilho)
  int bSetBuff;
  ledOff(); // todos os leds desligados
  bSetBuff = bSet; // Copia porque seria problemático se o valor mudasse durante o processamento (bSet é de 8 bits, então as interrupções não precisam ser desabilitadas)
  if (bSetBuff <= 0) { // se 0 ou negativo
    OCR2B = 0; // Define OCR2B para 0 (Mesmo se 0 for definido, ele acenderá pelo tempo mínimo, então o COMPA forçará o desligamento)
  }
  else { // caso contrário (se for maior ou igual a 1)
    OCR2B = bSetBuff - 1; // define o valor menos 1
  }
}

// Ligue o LED correspondente com Charlie Plexing
void d2High() { // Saída alta de D2 e ​​liga o LED especificado
  if ( Ly[6] & 0b0010000000000000 ) DDRB |= B00000010; // D9
  if ( Ly[6] & 0b0000100000000000 ) DDRB |= B00000001; // D8
  if ( Ly[6] & 0b0000001000000000 ) DDRD |= B10000000; // D7
  if ( Ly[6] & 0b0000000010000000 ) DDRD |= B01000000; // D6
  if ( Ly[6] & 0b0000000000100000 ) DDRD |= B00100000; // D5
  if ( Ly[6] & 0b0000000000001000 ) DDRD |= B00010000; // D4
  if ( Ly[6] & 0b0000000000000010 ) DDRD |= B00001000; // D3
  if ( Ly[7] & 0b0000000000000010 ) DDRB |= B00000100; // D10
  DDRD |= B00000100; // Atribui D2 à saída
  PORTD |= B00000100; // D2 alto. Isso ligará o LED (faça isso por último para minimizar a quantidade de luz)
}

void d3High() { // Saída alta de D3 e liga o LED especificado
  if ( Ly[4] & 0b0000100000000000 ) DDRB |= B00000001; // D8
  if ( Ly[4] & 0b0000001000000000 ) DDRD |= B10000000; // D7
  if ( Ly[4] & 0b0000000010000000 ) DDRD |= B01000000; // D6
  if ( Ly[4] & 0b0000000000100000 ) DDRD |= B00100000; // D5
  if ( Ly[4] & 0b0000000000001000 ) DDRD |= B00010000; // D4
  if ( Ly[5] & 0b0000000000001000 ) DDRB |= B00000010; // D9
  if ( Ly[6] & 0b0000000000000100 ) DDRD |= B00000100; // D2
  if ( Ly[7] & 0b0000000000001000 ) DDRB |= B00000100; // D10
  DDRD |= B00001000; // Atribui D3 à saída
  PORTD |= B00001000; // Configura D3 para Alto (isso acenderá o LED)
}

void d4High() { // Saída alta de D4 e liga o LED especificado
  if ( Ly[2] & 0b0000001000000000 ) DDRD |= B10000000; // D7
  if ( Ly[2] & 0b0000000010000000 ) DDRD |= B01000000; // D6
  if ( Ly[2] & 0b0000000000100000 ) DDRD |= B00100000; // D5
  if ( Ly[3] & 0b0000000000100000 ) DDRB |= B00000001; // D8
  if ( Ly[4] & 0b0000000000010000 ) DDRD |= B00001000; // D3
  if ( Ly[5] & 0b0000000000100000 ) DDRB |= B00000010; // D9
  if ( Ly[6] & 0b0000000000010000 ) DDRD |= B00000100; // D2
  if ( Ly[7] & 0b0000000000100000 ) DDRB |= B00000100; // D10
  DDRD |= B00010000; // Atribui D4 à saída
  PORTD |= B00010000; // Configura D4 para Alto (isso acenderá o LED)
}

void d5High() { // Saída alta de D5 e liga o LED especificado
  if ( Ly[0] & 0b0000000010000000 ) DDRD |= B01000000; // D6
  if ( Ly[1] & 0b0000000010000000 ) DDRD |= B10000000; // D7
  if ( Ly[2] & 0b0000000001000000 ) DDRD |= B00010000; // D4
  if ( Ly[3] & 0b0000000010000000 ) DDRB |= B00000001; // D8
  if ( Ly[4] & 0b0000000001000000 ) DDRD |= B00001000; // D3
  if ( Ly[5] & 0b0000000010000000 ) DDRB |= B00000010; // D9
  if ( Ly[6] & 0b0000000001000000 ) DDRD |= B00000100; // D2
  if ( Ly[7] & 0b0000000010000000 ) DDRB |= B00000100; // D10
  DDRD |= B00100000; // Atribui D5 à saída
  PORTD |= B00100000; // define D5 como alto (isso acenderá o LED)
}

void d6High() { // Saída alta de D6 e liga o LED especificado
  if ( Ly[0] & 0b0000000100000000 ) DDRD |= B00100000; // D5
  if ( Ly[1] & 0b0000001000000000 ) DDRD |= B10000000; // D7
  if ( Ly[2] & 0b0000000100000000 ) DDRD |= B00010000; // D4
  if ( Ly[3] & 0b0000001000000000 ) DDRB |= B00000001; // D8
  if ( Ly[4] & 0b0000000100000000 ) DDRD |= B00001000; // D3
  if ( Ly[5] & 0b0000001000000000 ) DDRB |= B00000010; // D9
  if ( Ly[6] & 0b0000000100000000 ) DDRD |= B00000100; // D2
  if ( Ly[7] & 0b0000001000000000 ) DDRB |= B00000100; // D10
  DDRD |= B01000000; // Atribui D6 à saída
  PORTD |= B01000000; // Coloca D6 em High (isso acenderá o LED)
}

void d7High() { // Saída alta de D7 e liga o LED especificado
  if ( Ly[1] & 0b0000000100000000 ) DDRD |= B01000000; // D6
  if ( Ly[1] & 0b0000000001000000 ) DDRD |= B00100000; // D5
  if ( Ly[2] & 0b0000010000000000 ) DDRD |= B00010000; // D4
  if ( Ly[3] & 0b0000100000000000 ) DDRB |= B00000001; // D8
  if ( Ly[4] & 0b0000010000000000 ) DDRD |= B00001000; // D3
  if ( Ly[5] & 0b0000100000000000 ) DDRB |= B00000010; // D9
  if ( Ly[6] & 0b0000010000000000 ) DDRD |= B00000100; // D2
  if ( Ly[7] & 0b0000100000000000 ) DDRB |= B00000100; // D10
  DDRD |= B10000000; // Atribui D7 à saída
  PORTD |= B10000000; // Define D7 para alto (isso acenderá o LED)
}

void d8High() { // Saída alta de D8 e liga o LED especificado
  if ( Ly[3] & 0b0000010000000000 ) DDRD |= B10000000; // D7
  if ( Ly[3] & 0b0000000100000000 ) DDRD |= B01000000; // D6
  if ( Ly[3] & 0b0000000001000000 ) DDRD |= B00100000; // D5
  if ( Ly[3] & 0b0000000000010000 ) DDRD |= B00010000; // D4
  if ( Ly[4] & 0b0001000000000000 ) DDRD |= B00001000; // D3
  if ( Ly[5] & 0b0010000000000000 ) DDRB |= B00000010; // D9
  if ( Ly[6] & 0b0001000000000000 ) DDRD |= B00000100; // D2
  if ( Ly[7] & 0b0010000000000000 ) DDRB |= B00000100; // D10
  DDRB |= B00000001; // Atribui D8 à saída
  PORTB |= B00000001; // Configura D8 para Alto (isso acenderá o LED)
}

void d9High() { // Saída alta de D9 e liga o LED especificado
  if ( Ly[5] & 0b0001000000000000 ) DDRB |= B00000001; // atribui D8 à saída
  if ( Ly[5] & 0b0000010000000000 ) DDRD |= B10000000; // D7
  if ( Ly[5] & 0b0000000100000000 ) DDRD |= B01000000; // D6
  if ( Ly[5] & 0b0000000001000000 ) DDRD |= B00100000; // D5
  if ( Ly[5] & 0b0000000000010000 ) DDRD |= B00010000; // D4
  if ( Ly[5] & 0b0000000000000100 ) DDRD |= B00001000; // D3
  if ( Ly[6] & 0b0100000000000000 ) DDRD |= B00000100; // D2
  if ( Ly[7] & 0b1000000000000000 ) DDRB |= B00000100; // D10
  DDRB |= B00000010; // Atribui D9 à saída
  PORTB |= B00000010; // Coloca D9 em High (isso acenderá o LED)
}

void d10High() { // Saída alta de D10 e liga o LED especificado
  if ( Ly[7] & 0b0100000000000000 ) DDRB |= B00000010; // atribui D9 à saída
  if ( Ly[7] & 0b0001000000000000 ) DDRB |= B00000001; // D8
  if ( Ly[7] & 0b0000010000000000 ) DDRD |= B10000000; // D7
  if ( Ly[7] & 0b0000000100000000 ) DDRD |= B01000000; // D6
  if ( Ly[7] & 0b0000000001000000 ) DDRD |= B00100000; // D5
  if ( Ly[7] & 0b0000000000010000 ) DDRD |= B00010000; // D4
  if ( Ly[7] & 0b0000000000000100 ) DDRD |= B00001000; // D3
  if ( Ly[7] & 0b0000000000000001 ) DDRD |= B00000100; // D2
  DDRB |= B00000100; // Atribui D10 à saída
  PORTB |= B00000100; // Coloca D10 em alto (isso acenderá o LED)
}
