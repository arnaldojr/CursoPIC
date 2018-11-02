/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de Código para utilização com a McLab3             *
 *                               Exemplo 12                                *
 *                                                                         *
 *                               MOSAICO                                   *
 *                                                                         *
 *  TEL: (0XX11) 4992-8775               SITE: www.mosaico.com.br          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   VERSÃO : 1.0                                                          *
 *   DATA : 10/05/2011                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                             Descrição geral                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// ESTE EXEMPLO FOI ELABORADO PARA EXPLICAR O FUNCIONAMENTO DA LEITURA/ESCRITA
// TANTO NA MEMÓRIA DE DADOS QUANTO NA MEMÓRIA DE PROGRAMA.
// OS BOTÔES PARA A UTILIZAÇÂO DO EXEMPLO ESTÃO NA COLUNA 1 COMO SEGUE:
//
//     LINHA 1 (S1) -> ALTERNA O TIPO DE MEMÓRIA
//     LINHA 2 (S5) -> INCREMENTA O VALOR QUE SERÀ SALVO NA MEMÓRIA
//     LINHA 3 (S9) -> DECREMENTA O VALOR QUE SERÀ SALVO NA MEMÓRIA
//     LINHA 4 (S13)-> SALVA O VALOR INDICADO NA MEMÓRIA SELECIONADA
// 
// REINICIE O EQUIPAMENTO E OBSERVANDO O RESULTADO NO LCD.


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              DEFINIÇÃO PIC                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>           //Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUNÇÕES DE PERIFÉRICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <delays.h>             //Delay library functions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Configurações para gravação                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma config FOSC = HS
#pragma config CPUDIV = OSC1_PLL2
#pragma config WDT = ON
#pragma config WDTPS = 128
#pragma config LVP = OFF
#pragma config PWRT = ON
#pragma config BOR = OFF
#pragma config BORV = 0
#pragma config PBADEN = OFF
#pragma config DEBUG = OFF
#pragma config PLLDIV = 1
#pragma config USBDIV = 1
#pragma config FCMEN = OFF
#pragma config IESO = OFF
#pragma config VREGEN = OFF
#pragma config MCLRE = ON
#pragma config LPT1OSC = OFF
#pragma config CCP2MX = ON
#pragma config STVREN = OFF
#pragma config ICPRT = OFF
#pragma config XINST = OFF

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Definição e inicialização das variáveis globais          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//Neste bloco estão definidas as variáveis globais do programa.

unsigned char    FILTRO;        //FILTRO BOTÃO

char mem[8];

unsigned char    tempo_turbo = 30;
unsigned char    cont_mem_dados;
unsigned short   cont_mem_prog;
unsigned char    x;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de constantes facilita a programação e a manutenção.
//Este programa não utiliza nenhuma constante de usuário

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Declaração dos flags de software                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de flags ajuda na programação e economiza memória RAM.
struct
{
    unsigned BIT0:1;
    unsigned BIT1:1;
    unsigned BIT2:1;
    unsigned BIT3:1;
    unsigned BIT4:1;
    unsigned BIT5:1;
    unsigned BIT6:1;
    unsigned BIT7:1;
}FLAGSbits;            //ARMAZENA OS FLAGS DE CONTROLE

#define TIPO_MEM    FLAGSbits.BIT0

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                   Definição e inicialização dos port's                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       PROTOTIPAGEM DE FUNÇÕES                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void comando_lcd(unsigned char caracter);
void escreve_lcd(unsigned char caracter);
void limpa_lcd(void);
void inicializa_lcd(void);
void escreve_frase(const rom char *frase);
void atualiza_linha_1(void);
void atualiza_linha_2(void);
void write_EEPROM(unsigned char endereco, unsigned char dado);
unsigned char read_EEPROM(unsigned char endereco);
void write_FLASH(unsigned addr, char *buffer);
unsigned short read_FLASH(unsigned endereco);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programação e
//futuras alterações do hardware.

#define     BT1     PORTDbits.RD4   //PORTA DO BOTÃO S1
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     BT2     PORTDbits.RD5   //PORTA DO BOTÃO S5
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     BT3     PORTDbits.RD6   //PORTA DO BOTÃO S9
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     BT4     PORTDbits.RD7   //PORTA DO BOTÃO S13
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 SAÍDAS                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//As saídas devem ser associadas a nomes para facilitar a programação e
//futuras alterações do hardware.

#define     COLUNA_1    PORTDbits.RD0   //PINO PARA ATIVAR COLUNA 1 DO TECLADO
                                        //MATRICIAL
                                        //1 -> COLUNA ATIVADA
                                        //0 -> COLUNA DESATIVADA

#define     rs          PORTEbits.RE0   // via do lcd que sinaliza recepção de dados ou comando
#define     rw          PORTBbits.RB7   // via do lcd que configura leitura ou escrita no barramento
#define     enable      PORTEbits.RE1   // enable do lcd

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                Rotina que envia um COMANDO para o LCD                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void comando_lcd(unsigned char caracter)
{
    TRISD = 0b00000000; // configura portd como saída
    rs = 0;             // seleciona o envio de um comando
    PORTD = caracter;   // carrega o PORTD com o caracter
    enable = 1;         // gera pulso no enable
    Delay10TCYx(1);     // espera 10 microsegundos
    enable = 0;         // desce o pino de enable
    TRISD = 0b11110000; // CONFIGURA O PORTD PARA O TECLADO
    Delay10TCYx(4);     // espera mínimo 40 microsegundos
    COLUNA_1 = 1;       // ativa a coluna 1 do teclado
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *             Rotina que envia um DADO a ser escrito no LCD               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void escreve_lcd(unsigned char caracter)
{
    TRISD = 0b00000000; // configura portd como saída
    rs = 1;             // seleciona o envio de um dado
    PORTD = caracter;   // carrega o PORTD com o caracter
    enable = 1;         // gera pulso no enable
    Delay10TCYx(1);     // espera 10 microsegundos
    enable = 0;         // desce o pino de enable
    TRISD = 0b11110000; // CONFIGURA O PORTD PARA O TECLADO
    Delay10TCYx(4);     // espera mínimo 40 microsegundos
    COLUNA_1 = 1;       // ativa a coluna 1 do teclado
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              Função para limpar o LCD                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void limpa_lcd(void)
{
    comando_lcd(0x01);  // limpa lcd
    Delay1KTCYx(2);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                   Inicialização do Display de LCD                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void inicializa_lcd(void)
{
    rw = 0;             // pino rw em 0
    comando_lcd(0x30);  // envia comando para inicializar display
    Delay1KTCYx(4);     // espera 4 milisengundos

    comando_lcd(0x30);  // envia comando para inicializar display
    Delay10TCYx(10);    // espera 100 microsengundos

    comando_lcd(0x30);  // envia comando para inicializar display

    comando_lcd(0x38);  // liga o display, sem cursor e sem blink

    limpa_lcd();        // limpa lcd

    comando_lcd(0x0c);  // display sem cursor

    comando_lcd(0x06);  // desloca cursor para a direita
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *             Função para escrever uma frase no LCD                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void escreve_frase(const rom char *frase)
{
    do
    {
        escreve_lcd(*frase);
    }while(*++frase);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                FUNÇÃO PARA ATUALIZAR A LINHA 1 DO LCD                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void atualiza_linha_1(void)
{
    limpa_lcd();                // limpa lcd
    comando_lcd(0x80);          // posiciona o cursor na linha 0, coluna 0
                                // imprime mensagem no lcd
    escreve_frase("M.DADOS  M.PROG.");
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                FUNÇÃO PARA ATUALIZAR A LINHA 2 DO LCD                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void atualiza_linha_2(void)
{
    union
    {
        struct
        {
            unsigned short low:8;
            unsigned short high:8;
        } ;
        struct
        {
            unsigned short all;
        } ;
    }dado;

    unsigned char aux;

    comando_lcd(0xC1);                 //POSICIONA CURSOR NA LINHA 1, COLUNA 1

    if(TIPO_MEM) escreve_lcd('>');     //SE TIPO DE MEMÓRIA = 1, IMPRIME '>'
    else escreve_lcd(' ');             //SE TIPO DE MEMÓRIA = 0, IMPRIME ' '

    aux = cont_mem_dados >> 4;         //CONVERTE cont_mem_dados EM HEXADECIMAL
    if(aux < 10) aux = aux + 0x30;
    else aux = aux + 0x37;
    escreve_lcd(aux);

    aux = cont_mem_dados & 0b00001111;
    if(aux < 10) aux = aux + 0x30;
    else aux = aux + 0x37;
    escreve_lcd(aux);

    if(TIPO_MEM) escreve_lcd('<');     //SE TIPO DE MEMÓRIA = 1, IMPRIME '<'
    else escreve_lcd(' ');             //SE TIPO DE MEMÓRIA = 0, IMPRIME ' '

    dado.all = cont_mem_prog;

    escreve_lcd(' ');
    escreve_lcd(' ');
    escreve_lcd(' ');

    if(!TIPO_MEM) escreve_lcd('>');       //SE TIPO DE MEMÓRIA = 0, IMPRIME '>'
    else escreve_lcd(' ');                //SE TIPO DE MEMÓRIA = 1, IMPRIME ' '

    aux = dado.high >> 4;                 //CONVERTE cont_mem_prog EM HEXADECIMAL
    if(aux < 10) aux = aux + 0x30;
    else aux = aux + 0x37;
    escreve_lcd(aux);

    aux = dado.high & 0b00001111;
    if(aux < 10) aux = aux + 0x30;
    else aux = aux + 0x37;
    escreve_lcd(aux);

    aux = dado.low >> 4;
    if(aux < 10) aux = aux + 0x30;
    else aux = aux + 0x37;
    escreve_lcd(aux);

    aux = dado.low & 0b00001111;
    if(aux < 10) aux = aux + 0x30;
    else aux = aux + 0x37;
    escreve_lcd(aux);

    if(!TIPO_MEM) escreve_lcd('<');       //SE TIPO DE MEMÓRIA = 0, IMPRIME '<'
    else escreve_lcd(' ');                //SE TIPO DE MEMÓRIA = 1, IMPRIME ' '
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                   Função de escrita da EEPROM interna                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void write_EEPROM(unsigned char endereco, unsigned char dado)
{
    EEDATA = dado;               //carrega dado
    EEADR = endereco;            //carrega endereço

    EECON1bits.CFGS = 0;         //habilita acesso a EEPROM
    EECON1bits.EEPGD = 0;        //aponta para memória de dados
    EECON1bits.WREN = 1;         //habilita escrita
    INTCONbits.GIE = 0;          //desabilita todas as interrupções
    EECON2 = 0x55;               //escreve 0x55 em EECON2 (obrigatório)
    EECON2 = 0xAA;               //escreve 0xAA em EECON2 (obrigatório)
    EECON1bits.WR = 1;           //inicia a escrita
    INTCONbits.GIE = 1;          //habilita todas as interrupções
    while(EECON1bits.WR);        //aguarda bit WR de EECON1 = 0
    EECON1bits.WREN = 0;         //desabilita escrita
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                   Função de leitura da EEPROM interna                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

unsigned char read_EEPROM(unsigned char endereco)
{
    EEADR = endereco;            //carrega endereço

    EECON1bits.CFGS = 0;         //habilita acesso a EEPROM
    EECON1bits.EEPGD = 0;        //aponta para memória de dados
    EECON1bits.RD = 1;           //habilita leitura

    return(EEDATA);              //retorna dado disponível em EEDATA
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Função de escrita da FLASH interna                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// OBS: ESTA FUNÇÃO, O ENDEREÇO DEVE TER FINAL 0 OU 8
// EX.: 0x500, 0x508, 0x60, 0x68 ...

void write_FLASH(unsigned addr, char *buffer)
{
    unsigned char i;

    TBLPTR = addr;                //carrega endereço

    EECON1bits.EEPGD = 1;         //aponta para memória FLASH
    EECON1bits.CFGS = 0;          //habilita acesso a FLASH
    EECON1bits.WREN = 1;          //habilita escrita
    EECON1bits.FREE = 1;          //apaga FLASH a partir do endereço apontado no TBLPTR
    INTCONbits.GIE = 0;           //desabilita todas as interrupções
    EECON2 = 0x55;                //escreve 0x55 em EECON2 (obrigatório)
    EECON2 = 0xAA;                //escreve 0xAA em EECON2 (obrigatório)
    EECON1bits.WR = 1;            //inicializa escrita da FLASH
    INTCONbits.GIE = 1;           //habilita todas as interrupções
    while(EECON1bits.WR);
    EECON1bits.WREN = 0;          //desabilita escrita

    for(i = 0;i < 8;i++)
    {
        TABLAT = buffer[i];       //carrega dado em TABLAT

        _asm
        TBLWTPOSTINC              //escreve dado carregado em TABLAT e incrementa TBLPTR
        _endasm

        if(((i + 1) % 8) == 0)    //identifica os grupos de 8 bytes e habilita a escrita
        {
            TBLPTR -= 8;          //retorna 8 posições em TBLPTR

            EECON1bits.EEPGD = 1; //aponta para memória FLASH
            EECON1bits.CFGS = 0;  //habilita acesso a FLASH
            EECON1bits.WREN = 1;  //habilita escrita
            EECON1bits.FREE = 0;  //somente escrita da FLASH
            INTCONbits.GIE = 0;   //desabilita todas as interrupções
            EECON2 = 0x55;        //escreve 0x55 em EECON2 (obrigatório)
            EECON2 = 0xAA;        //escreve 0xAA em EECON2 (obrigatório)
            EECON1bits.WR = 1;    //inicializa escrita da FLASH
            INTCONbits.GIE = 1;   //habilita todas as interrupções
            while(EECON1bits.WR);
            EECON1bits.WREN = 0;  //desabilita escrita

            TBLPTR += 8;          //retorna à posição original em TBLPTR
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Função de leitura da FLASH interna                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

unsigned short read_FLASH(unsigned endereco)
{
    union
    {
        struct
        {
            unsigned short low:8;
            unsigned short high:8;
        } ;
        struct
        {
            unsigned short all;
        } ;
    }dado;

    TBLPTR = endereco;        //carrega endereço

    _asm                      //lê posição apontada pelo TBLPTR, envia
    TBLRDPOSTINC              //dado para TABLAT e incremanta TBLPTR
    _endasm

    dado.low = TABLAT;        //salva dado em dado.low

    _asm                      //lê posição apontada pelo TBLPTR, envia
    TBLRDPOSTINC              //dado para TABLAT e incremanta TBLPTR
    _endasm

    dado.high = TABLAT;       //salva dado em dado.high

    return(dado.all);         //retorna dado disponível em dado.all
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Função Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void main(void)
{
    PORTA = 0x00;               //Limpa PORTA
    PORTB = 0x00;               //Limpa PORTB
    PORTC = 0x00;               //Limpa PORTC
    PORTD = 0x00;               //Limpa PORTD
    PORTE = 0x00;               //Limpa PORTE

    LATA = 0x00;                //Limpa PORTA
    LATB = 0x00;                //Limpa PORTB
    LATC = 0x00;                //Limpa PORTC
    LATD = 0x00;                //Limpa PORTD
    LATE = 0x00;                //Limpa PORTE

    TRISA = 0b11000011;         //CONFIG DIREÇÃO DOS PINOS PORTA
    TRISB = 0b00000000;         //CONFIG DIREÇÃO DOS PINOS PORTB
    TRISC = 0b11111111;         //CONFIG DIREÇÃO DOS PINOS PORTC
    TRISD = 0b00000000;         //CONFIG DIREÇÃO DOS PINOS PORTD
    TRISE = 0b00000100;         //CONFIG DIREÇÃO DOS PINOS PORTE

    ADCON1 = 0b00001111;        //DESLIGA CONVERSORES A/D

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicialização do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    COLUNA_1 = 1;               //ATIVA COLUNA 1 DO TECLADO MATRICIAL
    
    inicializa_lcd();

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    cont_mem_dados = read_EEPROM(0x10); //LÊ MEMÓRIA EEPROM
    cont_mem_prog = read_FLASH(0x1000);  //LÊ MEMÓRIA FLASH

    for(x=0;x<8;x++) mem[x]=0;          //INICIA BUFFER PARA GRAVAÇÃO
                                        //DA MEMÓRIA FLASH

    atualiza_linha_1();                 //ATUALIZA LINHA 1
    atualiza_linha_2();                 //ATUALIZA LINHA 2

    while(1)
    {
        ClrWdt();                       //LIMPA O WDT

        if(BT1)                         //SE BOTÃO 1 PRESSIONADO
        {
            if(FILTRO)                  //AÇÃO DO BOTÃO FOI EXECUTADA?
            {
                FILTRO--;               //DECREMENTA FILTRO
                if(!FILTRO)             //TERMINOU FILTRO?
                {
                    if(TIPO_MEM) TIPO_MEM = 0;  //SE TIPO DE MEM = 1, TIPO DE MEM = 0
                    else TIPO_MEM = 1;          //SE NÃO, TIPO DE MEM = 1
                    atualiza_linha_2();         //ATUALIZA LINHA 2 DO LCD
                }
            }
            continue;
        }

        if(BT2)                         //SE BOTÃO 2 PRESSIONADO
        {
            FILTRO--;                   //DECREMENTA FILTRO
            if(!FILTRO)                 //TERMINOU FILTRO?
            {
                tempo_turbo--;
                if(!tempo_turbo)
                {
                    if(TIPO_MEM) cont_mem_dados++;  //SE TIPO DE MEM = 1, INCREMENTA cont_mem_dados
                    else cont_mem_prog++;           //SE NÃO, INCREMENTA cont_mem_prog
                    atualiza_linha_2();             //ATUALIZA LINHA 2 DO LCD
                    tempo_turbo = 30;               //REINICIA TURBO DE TECLA
                }
            }
            continue;
        }

        if(BT3)                         //SE BOTÃO 3 PRESSIONADO
        {
            FILTRO--;                   //DECREMENTA FILTRO
            if(!FILTRO)                 //TERMINOU FILTRO?
            {
                tempo_turbo--;
                if(!tempo_turbo)
                {
                    if(TIPO_MEM) cont_mem_dados--;  //SE TIPO DE MEM = 1, DECREMENTA cont_mem_dados
                    else cont_mem_prog--;           //SE NÃO, DECREMENTA cont_mem_prog
                    atualiza_linha_2();             //ATUALIZA LINHA 2 DO LCD
                    tempo_turbo = 30;               //REINICIA TURBO DE TECLA
                }
            }
            continue;
        }

        if(BT4)                         //SE BOTÃO 4 PRESSIONADO
        {
            if(FILTRO)                  //AÇÃO DO BOTÃO FOI EXECUTADA?
            {
                FILTRO--;               //DECREMENTA FILTRO
                if(!FILTRO)             //TERMINOU FILTRO?
                {
                    write_EEPROM(0x10, cont_mem_dados); //GRAVA NA EEPROM E FLASH
                    mem[0] = cont_mem_prog;
                    mem[1] = cont_mem_prog >> 8;
                    write_FLASH(0x1000, (char *)&mem);
                }
            }
            continue;
        }

        FILTRO = 255;                   //RECARREGA FILTRO DE BOTÕES

    }// FIM LAÇO PRINCIPAL
}
