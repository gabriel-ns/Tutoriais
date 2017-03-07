/**
 * SGN Robótica Educacional - 2017
 * 
 * Sketch I2C_intro
 * 
 * Descrição: 
 * Este arquivo tem como objetivo ilustrar as funcionalidades básicas da biblioteca Wire
 * para a comunicação I2C.
 * O código presente não tem aplicação, servindo apenas para explicar
 * os recursos da biblioteca
 */

 /**
  * Para utilizar a comunicação I2C precisa-se importar a biblioteca Wire.h.
  * Note que como esta biblioteca faz parte do compilador, é chamada entre os caracteres <>.
  */
#include <Wire.h>

void setup() {
  /**
   * Para que seu microcontrolador inicialize os periféricos da comunicação I2C, é necessário utilizar a função
   * Wire.begin. Esta função deve ser chamada no void setup, pois só precisa ser executada uma vez.
   */
   Wire.begin();

   /**
    * É possível definir um endereço de 7 bits para o seu microcontroador. Para isso, o endereço deve ser um
    * argumento da função Wire.begin. É importante notar que não se faz duas chamadas da função como foi feito aqui,
    * deve-se optar por uma das formas de chamar a função.
    */
    Wire.begin(7_bit_address); /** 7_bit_address pode ser uma constante ou uma variável contendo o valor correspondente ao endereço desejado */

}

void loop() {

  /**
   *  Para iniciar a comunicação com um dispositivo escravo, usa-se a função Wire.beginTransmission, utilizando como argumento
   *  o endereço de 7 bits do escravo.
   */
   Wire.beginTransmission(slave_address);

   /**
    * O envio de dados para o dispositivo escravo é feito através da função Wire.write, sendo que os dados 
    * são os argumentos da função.
    * Com esta função, os dados podem ser enviados de 3 formas: 1 byte por vez, uma string ou um vetor.
    * Abaixo estão exemplos de como se utiliza cada uma destas formas:
    */
    Wire.write(0x50);       /** Envia o byte 0x50 para o escravo*/ 
    Wire.write("Mensagem"); /** Envia os bytes 4d 65 6e 73 61 67 65 6d (conversão para ascii de Mensagem) para o escravo*/

    byte data[4] = {0x10, 0x20, 0x30, 0x40);
    Wire.write(data, 4);     /** Envia o conteúdo do vetor data para o escravo. Nota-se que para este uso deve-se informar o tamanho do vetor. */


    /**
     * Para ler dados do dispositivo escravo utiliza-se a função Wire.requestFrom, que recebe dois argumentos:
     * o endereço do escravo e o número de bytes que este deve enviar.
     * Quando este comando é executado, os bytes recebidos ficam armazenados num buffer.
     */
     Wire.requestFrom(slave_address, number_of_bytes);

     /**
      * Para recuperar os dados recebidos por um escravo que está armazenado num buffer, utiliza-se a função 
      * Wire.read. Esta função irá retornar o primeiro byte recebido.
      */
      byte received_byte = Wire.read();

      /**
       *  Para verificar quantos bytes ainda estão disponíveis para a leitura utiliza-se a função
       *  Wire.available(). Esta função retorna o número de bytes disponíveis no buffer.
       *  São os usos:
       */
       int available_bytes = Wire.available();  /** Armazena em available_bytes o número de bytes disponíveis para a leitura*/
       if(Wire.available() > 0) {}              /** Condicional que executa o bloco de comandos se o buffer possuir algum valor para leitura */
       while(Wire.availabel > 0) { }            /** Executa o bloco de comandos enquanto houver dados no buffer */


       /**
        * Se não for necessário utilizar todos os valores recebidos é necessário limpar os dados
        * do buffer, desta forma o próximo valor retornado pela função Wire.read será referente a
        * informação mais recente.
        * Esta limpeza de buffer é feita através da função Wire.flush.
        */
        Wire.flush();

        /**
         * Se seu programa não precisa mais utilizar a comunicação I2C, é possível encerrá-la usando a função
         * Wire.end()
         */
         Wire.end();
       

}
