/*
*
*   terminal tedium / OSC client (UDP)
*
*
*   - TD: gate outputs (?); make nicer. etc (pigpio?)
*   - TD: make more userfriendly, pass IP and port as arguments, etc.
*
*
*   compile with: gcc *.c -Werror -lwiringPi -std=gnu99 -O2 -g -o tedium_osc
*/

#define DEFAULT_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <lo/lo.h>
// #include "tinyosc.h"

const uint16_t BUFLEN = 1024;
const uint64_t NOW = 1L;
const uint32_t TIMEOUT = 1000;    // sleep (in us)
const uint32_t TIMEOUT_T = 10;    // short time out (for OSC off messages) (triggers) // 10 * TIMEOUT (us)

#define ADC_SPI_CHANNEL 1
#define ADC_SPI_SPEED 4000000
#define ADC_NUM_CHANNELS 6
#define ADC_NUM ADC_NUM_CHANNELS-1
#define RESOLUTION 4095
#define DEADBAND 2
#define SCALE 4000
#define SCALE_INV 1.0f/(float)SCALE

// GPIO :

#define B1 23
#define B2 25
#define B3 24
#define LED 26
#define TR1 4
#define TR2 17
#define TR3 14
#define TR4 27
#define GATE1 16
#define GATE2 12

#define _MUTEX 0


#define IP_SEND_ADDRESS "127.0.0.1"
#define  SENDING_PORT "9000"
#define LISTENING_PORT "7770"



void die(char *s)
{
    perror(s);
    exit(1);
}

uint16_t adc[ADC_NUM_CHANNELS]; // adc values
uint16_t old_adc[ADC_NUM_CHANNELS]; // adc values


/*  interrupt flags etc:  */

static volatile uint8_t TR1_flag = 0;
static volatile uint8_t TR2_flag = 0;
static volatile uint8_t TR3_flag = 0;
static volatile uint8_t TR4_flag = 0;

static volatile uint8_t B1_flag  = 0;
static volatile uint8_t B1_status  = 0;
static volatile uint8_t B2_flag  = 0;
static volatile uint8_t B2_status  = 0;
static volatile uint8_t B3_flag  = 0;
static volatile uint8_t B3_status  = 0;



const char potnames[6][6]={"/pot1","/pot2","/pot3","/pot4","/pot5","/pot6"};

void Interrupt_TR1 (void) { TR1_flag = 1; }
void Interrupt_TR2 (void) { TR2_flag = 1; }
void Interrupt_TR3 (void) { TR3_flag = 1; }
void Interrupt_TR4 (void) { TR4_flag = 1; }

//const char LISTENING_PORT[5] =  "7770";
/*const char IP_SEND_ADDRESS[] =  "127.0.0.1"*/


/*const char IP_SEND_ ADDRESS[10], SENDING_PORT )
"127.0.0.1","9000");*/

// Interrup for Buttons

void Interrupt_B1  (void) {
    if (B1_status == 0)
        {
            B1_flag=1 ;
          //  printf("button1 pressed\n");
            B1_status=1;
        }
        else{
            B1_flag=1 ;
         //   printf("button1 released\n");
            B1_status=0;
        };
}


void Interrupt_B2  (void) {
    if (B2_status == 0)
        {
            B2_flag=1 ;
          //  printf("button1 pressed\n");
            B2_status=1;
        }
        else{
            B2_flag=1 ;
          //  printf("button1 released\n");
            B2_status=0;
        };
}

void Interrupt_B3  (void) {
    if (B3_status == 0)
        {
            B3_flag=1 ;
         //   printf("button1 pressed\n");
            B3_status=1;
        }
        else{
            B3_flag=1 ;
         //   printf("button1 released\n");
            B3_status=0;
        };
}


// flags and counters for on/off messages:

uint8_t TR1_OFF = 0;
uint8_t TR2_OFF = 0;
uint8_t TR3_OFF = 0;
uint8_t TR4_OFF = 0;
uint16_t _wait_TR1, _wait_TR2, _wait_TR3, _wait_TR4;

/*

mcp3208 : return 1 if we need to send OSC, 0 otherwise

*/


uint16_t readADC(int _channel, uint16_t *adc_val){

      uint8_t spi_data[3];
      uint16_t result, tmp = *(adc_val + _channel); // previous.

      spi_data[0] = 0x06 | (_channel>>2) & 0x01;    // single ended
      spi_data[1] = _channel<<6;
      spi_data[2] = 0x00;

      wiringPiSPIDataRW(ADC_SPI_CHANNEL, spi_data, 3);

      // invert + limit result:
      result = SCALE - (((spi_data[1] & 0x0f) << 8) | spi_data[2]);
      result = result > RESOLUTION ? 0 : result;

      if ( (result - tmp) > DEADBAND || (tmp - result) > DEADBAND ) {
        *(adc_val + _channel) = result ;
        return 1;
      }
      else {
        *(adc_val + _channel)  = tmp;
        return 0;
      }
}


///  Serveur functions
int done = 0;
void error(int num, const char *m, const char *path);


int gate1_handler(const char *path, const char *types, lo_arg ** argv,
                int argc, void *data, void *user_data);

int gate2_handler(const char *path, const char *types, lo_arg ** argv,
                int argc, void *data, void *user_data);


int led_handler(const char *path, const char *types, lo_arg ** argv,
                int argc, void *data, void *user_data);

int quit_handler(const char *path, const char *types, lo_arg ** argv,
                 int argc, void *data, void *user_data);

void read_stdin(void);
///////////////////



/* --------------------------------------------------------- */

int main(void)
{

    int k=0;

    int count=0;
    int flag_send =0;

// init liblo
    lo_address t=  lo_address_new(IP_SEND_ADDRESS,  SENDING_PORT ) ;
// init mem for receiving ADC values
    memset(adc, 0, ADC_NUM_CHANNELS);


    // setup SPI
    wiringPiSetupGpio();
    wiringPiSPISetup(ADC_SPI_CHANNEL, ADC_SPI_SPEED);

    // pull ups:
    pinMode(B1, INPUT);
    pullUpDnControl(B1, PUD_UP);
    pinMode(B2, INPUT);
    pullUpDnControl(B2, PUD_UP);
    pinMode(B3, INPUT);
    pullUpDnControl(B3, PUD_UP);

    pinMode(TR1, INPUT);
    pullUpDnControl(TR1, PUD_UP);
    pinMode(TR2, INPUT);
    pullUpDnControl(TR2, PUD_UP);
    pinMode(TR3, INPUT);
    pullUpDnControl(TR3, PUD_UP);
    pinMode(TR4, INPUT);
    pullUpDnControl(TR4, PUD_UP);

    // LED + gate outputs (??) (what should those do?)
    pinMode(LED, OUTPUT);
    pinMode(GATE1, OUTPUT);
    pinMode(GATE2, OUTPUT);

    // interrupt, trigger inputs:
    wiringPiISR (TR1, INT_EDGE_FALLING, &Interrupt_TR1) ;
    wiringPiISR (TR2, INT_EDGE_FALLING, &Interrupt_TR2) ;
    wiringPiISR (TR3, INT_EDGE_FALLING, &Interrupt_TR3) ;
    wiringPiISR (TR4, INT_EDGE_FALLING, &Interrupt_TR4) ;

    // interrupt, buttons:
	 wiringPiISR (B1,  INT_EDGE_BOTH, &Interrupt_B1);
	 wiringPiISR (B2, INT_EDGE_BOTH, &Interrupt_B2);
	 wiringPiISR (B3, INT_EDGE_BOTH, &Interrupt_B3);


    // flags
    int _cnt = 0, _send = 0, toggle = 0;

    /* Now set up the server  */
     lo_server_thread st = lo_server_thread_new(LISTENING_PORT, error);

    /* add method that will match any path and args */
  /*  lo_server_thread_add_method(st, NULL, NULL, generic_handler, NULL);*/

    /* add method that will match the path /foo/bar, with two numbers, coerced
     * to float and int */

    lo_server_thread_add_method(st, "/gate1", "i", gate1_handler, NULL);
    lo_server_thread_add_method(st, "/gate2", "i", gate2_handler, NULL);
    lo_server_thread_add_method(st, "/led", "i", led_handler, NULL);

     /* add method that will match the path /quit with no args */
    lo_server_thread_add_method(st, "/quit", "", quit_handler, NULL);
    lo_server_thread_start(st);




    // main action is happening here (..)
     while(1)
    {
            usleep(TIMEOUT);

           /* server */


            /* Now client call */
            // Is there something to send ?
            flag_send = readADC(count,adc);
            // if yes
            if (flag_send==1){
                // check if value has changed
                if (old_adc[count]!= adc[count]) {
                            // send the value
                            lo_send(t, potnames[count] ,"f",(float)adc[count] * SCALE_INV );
                            old_adc[count]= adc[count];
                            };
                // nothing to send anymore
                flag_send=0;
                };
            // Next loop we will check the next pot
            count++;
            //reset count to  when all pots have been read
            if (count >=6) {count = 0;};

            // buttons ?

            if (B1_flag){
                if ( B1_status){lo_send(t,"/button1","i",1); }
                else {lo_send(t,"/button1","i",0); };
                B1_flag=0;
            };
            if (B2_flag){
                if ( B2_status){lo_send(t,"/button2","i",1); }
                else {lo_send(t,"/button2","i",0); };
                B2_flag=0;
            };
            if (B3_flag){
                if ( B3_status){lo_send(t,"/button3","i",1); }
                else {lo_send(t,"/button3","i",0); };
                B3_flag=0;
            };



          // handle trig inputs:

            if (TR1_flag && !TR1_OFF) {

                    TR1_flag =  0;
                    lo_send(t,"/trigger1","i",1);
                 /*   int len = tosc_writeMessage(buf, sizeof(buf), "/trigger1", "i", 1);
                    // send message:
                    if (sendto(s, buf, len, 0, (struct sockaddr *) &_serv, slen)) TR1_OFF = 1;*/
                    _wait_TR1 = 0;
            }

            if (TR2_flag && !TR2_OFF) {

                    TR2_flag =  0;
                    lo_send(t,"/trigger2","i",1);
             /*       int len = tosc_writeMessage(buf, sizeof(buf), "/trigger2", "i", 1);
                    // send message:
                    if (sendto(s, buf, len, 0, (struct sockaddr *) &_serv, slen)) TR2_OFF = 1;*/
                    _wait_TR2 = 0;
            }

            if (TR3_flag && !TR3_OFF) {

                    TR3_flag =  0;
                    lo_send(t,"/trigger3","i",1);
                 /*   int len = tosc_writeMessage(buf, sizeof(buf), "/trigger3", "i", 1);
                    // send message:
                    if (sendto(s, buf, len, 0, (struct sockaddr *) &_serv, slen)) TR3_OFF = 1;*/
                    _wait_TR3 = 0;
            }

            if (TR4_flag && !TR4_OFF) {

                    TR4_flag =  0;
                    lo_send(t,"/trigger4","i",1);
             /*       int len = tosc_writeMessage(buf, sizeof(buf), "/trigger4", "i", 1);
                    // send message:
                    if (sendto(s, buf, len, 0, (struct sockaddr *) &_serv, slen)) TR4_OFF = 1;*/
                    _wait_TR4 = 0;
            }

            // turn off gate1
            if (TR1_OFF && _wait_TR1 < TIMEOUT_T) _wait_TR1++;
            else if (TR1_OFF && _wait_TR1 >= TIMEOUT_T) {
                    // reset
                   TR1_OFF = _wait_TR1 = 0;
                   lo_send(t,"/trigger1","i",0);
                 /*   int len = tosc_writeMessage(buf, sizeof(buf), "/trigger1", "i", 0);
                    sendto(s, buf, len, 0, (struct sockaddr *) &_serv, slen);*/
            }
            // turn off gate2
            if (TR2_OFF && _wait_TR2 < TIMEOUT_T) _wait_TR2++;
            else if (TR2_OFF && _wait_TR2 >= TIMEOUT_T) {
                    // reset
                    TR2_OFF = _wait_TR2 = 0;
                    lo_send(t,"/trigger2","i",0);
                  /*  int len = tosc_writeMessage(buf, sizeof(buf), "/trigger2", "i", 0);
                    sendto(s, buf, len, 0, (struct sockaddr *) &_serv, slen);*/
            }
            // turn off gate3
            if (TR3_OFF && _wait_TR3 < TIMEOUT_T) _wait_TR3++;
            else if (TR3_OFF && _wait_TR3 >= TIMEOUT_T) {
                    // reset
                    TR3_OFF = _wait_TR3 = 0;
                    lo_send(t,"/trigger3","i",0);
                  /*  int len = tosc_writeMessage(buf, sizeof(buf), "/trigger3", "i", 0);
                    sendto(s, buf, len, 0, (struct sockaddr *) &_serv, slen);*/
            }
            // turn off gate4
            if (TR4_OFF && _wait_TR4 < TIMEOUT_T) _wait_TR4++;
            else if (TR4_OFF && _wait_TR4 >= TIMEOUT_T) {
                    // reset
                    lo_send(t,"/trigger4","i",0);
                    TR4_OFF = _wait_TR4 = 0;
                 /*   int len = tosc_writeMessage(buf, sizeof(buf), "/trigger4", "i", 0);
                    sendto(s, buf, len, 0, (struct sockaddr *) &_serv, slen);*/
            }
    }
 //   close(s);
  lo_server_thread_free(st);
    return 0;
}
// end



void error(int num, const char *msg, const char *path)
{
    printf("liblo server error %d in path %s: %s\n", num, path, msg);
    fflush(stdout);
}




int gate1_handler(const char *path, const char *types, lo_arg ** argv,
                int argc, void *data, void *user_data)
{
    digitalWrite(GATE1,(int) argv[0]->i);
    return 0;
}



int gate2_handler(const char *path, const char *types, lo_arg ** argv,
                int argc, void *data, void *user_data)
{
    digitalWrite(GATE2,(int) argv[0]->i);
    return 0;
}


int led_handler(const char *path, const char *types, lo_arg ** argv,
                int argc, void *data, void *user_data)
{
    digitalWrite(LED,(int) argv[0]->i);
    return 0;
}

int quit_handler(const char *path, const char *types, lo_arg ** argv,
                 int argc, void *data, void *user_data)
{
    done = 1;
    printf("quiting\n\n");
    fflush(stdout);

    return 0;
}
