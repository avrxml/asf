#ifndef IP64_WEBSERVER_H
#define IP64_WEBSERVER_H

void ip64_webserver_init(void);

/* webserver log apis*/
void webserver_log(char *msg);
void webserver_log_file(uip_ipaddr_t *requester, char *file);

/* urlconv.h apis*/
void urlconv_init(void);
void urlconv_tofilename(char *dest, char *source, unsigned char maxlen);

#endif /* IP64_WEBSERVER_H */
