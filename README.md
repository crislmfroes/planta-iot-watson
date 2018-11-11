Neste projeto pretendo demonstrar a criação de uma planta com diversos sensores conectada à plataforma IBM Watson.

Primeiramente, clone o repositório usando
#git clone https://github.com/crislmfroes/planta-iot-watson.git

Para contribuir com o projeto, primeiro é necessário instalar o visual studio code, além do plugin platformIO.

Após ter instalado o plugin, na tela que se abrirá "PIO Home", selecione a opção "Open Project" e navegue até a pasta onde você clonou o repositório.

Para configurar as chaves de API e senhas de acesso WiFi, utilize o script gen_secret.sh da seguinte maneira:
#bash gen_secret.sh <SSID da rede> <senha da rede> <id do canal no thingspeak> <chave de api de escrita no thingspeak> <tópico mqtt de publish> <tópico mqtt de subscribe> <id mqtt>

Se você não executar este script ocorrerá um erro na hora de fazer o upload do firmware para o ESP8266.