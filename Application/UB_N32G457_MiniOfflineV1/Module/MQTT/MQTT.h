#ifndef INCLUDED_MQTT_H
#define INCLUDED_MQTT_H

////////////////////////////////////////////////////////////////////////////////
////
void MQTT_Init(void);

int MQTT_Publish(void* data, int size);

#endif /*INCLUDED_MQTT_H*/
