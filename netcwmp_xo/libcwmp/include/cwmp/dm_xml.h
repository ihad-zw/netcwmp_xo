#ifndef __DM_XML_H__
#define __DM_XML_H__

#define CWMPD_DMXML_FILE "/tmp/dm.xml"

extern int dmxml_create(char * dataxmlfile);
extern int dmxml_set_value(char *paraname,char *paravalue);


#endif // __DM_XML_H__

