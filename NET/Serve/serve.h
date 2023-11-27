#ifndef _SERVE_H_
#define _SERVE_H_

_Bool Ali_DevLink(void);
void Ali_Subscribe(const char *topics[], unsigned char topic_cnt);
void Ali_Publish(const char *topic, const char *msg);
void Ali_RevPro(unsigned char *cmd);
void AliNet_Ping(void);

#endif
