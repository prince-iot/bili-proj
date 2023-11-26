#ifndef _SERVE_H_
#define _SERVE_H_

_Bool AliNet_DevLink(void);
void AliNet_Subscribe(const char *topics[], unsigned char topic_cnt);
void AliNet_Publish(const char *topic, const char *msg);
void AliNet_RevPro(unsigned char *cmd);
void AliNet_Ping(void);

#endif
