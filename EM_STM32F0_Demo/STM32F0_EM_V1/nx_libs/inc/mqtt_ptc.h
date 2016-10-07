#ifndef MQTT_PTC_H
#define MQTT_PTC_H

#include "main.h"

typedef struct{
 uint8_t connected;
 uint16_t msg_id;
}mqtt_t;

typedef struct{
	uint16_t length;
	uint8_t value[512];
}mqtt_data_t;

typedef struct{
	mqtt_data_t topic;
	mqtt_data_t payload;
}mqtt_dat_t;


#define MQTT_VERSION_3_1      3
//#define MQTT_VERSION_3_1_1    4

// MQTT_VERSION : Pick the version
//#define MQTT_VERSION MQTT_VERSION_3_1
//#define MQTT_VERSION MQTT_VERSION_3_1_1

// MQTT_MAX_PACKET_SIZE : Maximum packet size
#define MQTT_MAX_PACKET_SIZE 128

// MQTT_KEEPALIVE : keepAlive interval in Seconds
#define MQTT_KEEPALIVE 600


// MQTT_VERSION : Pick the version
#define MQTT_VERSION MQTT_VERSION_3_1 //bongkot uncomment
#ifndef MQTT_VERSION
#define MQTT_VERSION MQTT_VERSION_3_1_1
#endif

// MQTT_MAX_PACKET_SIZE : Maximum packet size
#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 128
#endif

// MQTT_KEEPALIVE : keepAlive interval in Seconds
#ifndef MQTT_KEEPALIVE
#define MQTT_KEEPALIVE 300
#endif

// MQTT_SOCKET_TIMEOUT: socket timeout interval in Seconds
#ifndef MQTT_SOCKET_TIMEOUT
#define MQTT_SOCKET_TIMEOUT 15
#endif

// MQTT_MAX_TRANSFER_SIZE : limit how much data is passed to the network client
//  in each write call. Needed for the Arduino Wifi Shield. Leave undefined to
//  pass the entire MQTT packet in each write call.
//#define MQTT_MAX_TRANSFER_SIZE 80

// Possible values for client.state()
#define MQTT_CONNECTION_TIMEOUT     -4
#define MQTT_CONNECTION_LOST        -3
#define MQTT_CONNECT_FAILED         -2
#define MQTT_DISCONNECTED           -1
#define MQTT_CONNECTED               0
#define MQTT_CONNECT_BAD_PROTOCOL    1
#define MQTT_CONNECT_BAD_CLIENT_ID   2
#define MQTT_CONNECT_UNAVAILABLE     3
#define MQTT_CONNECT_BAD_CREDENTIALS 4
#define MQTT_CONNECT_UNAUTHORIZED    5

#define MQTT_CONNECT				(1<<4)	//0001 0000
#define MQTT_CONNACK				(2<<4)	//0010 0000
#define MQTT_PUBLISH				(3<<4)	//0011 0000
#define MQTT_PUBACK					(4<<4)	//0010 0000
#define MQTT_PUBREC					(5<<4)	//0101 0000
#define MQTT_PUBREL					(6<<4)	//0110 0000
#define MQTT_PUBCOMP				(7<<4)	//0111 0000
#define MQTT_SUBSCRIBE			(8<<4)	//1000 0000
#define MQTT_SUBACK					(9<<4)	//1001 0000
#define MQTT_UNSUBSCRIBE		(10<<4)	//1010 0000
#define MQTT_UNSUBACK				(11<<4)	//1011 0000
#define MQTT_PINGREQ				(12<<4)	//1100 0000
#define MQTT_PINGRESP				(13<<4)	//1101 0000
#define MQTT_DISCONNECT			(14<<4)	//1110 0000

#define MQTT_QOS0        		(0 << 1)
#define MQTT_QOS1        		(1 << 1)
#define MQTT_QOS2        		(2 << 1)

#define MQTT_RETAIN      		(0 << 1)


int8_t mqtt_connect(uint8_t *user_val, uint16_t user_len, uint8_t *pwd_val, uint16_t pwd_len, uint8_t *client_id_val, uint16_t client_id_len);
int8_t mqtt_publish(uint8_t *topic_value, uint16_t topic_len, uint8_t *payload_value, uint16_t payload_len, uint8_t qos, uint8_t retain);
int8_t mqtt_subscribe(uint8_t *topic_value, uint16_t topic_len, uint8_t qos);
int8_t mqtt_callback();
int8_t mqtt_parser(uint8_t * val, uint16_t len);
int8_t mqtt_ack_ping(uint8_t *val, uint16_t len);
int8_t mqtt_ping();
int8_t mqtt_ack_pinging();
int8_t mqtt_ack_pub(uint16_t pack_id);
extern mqtt_t mqtt;
#endif 