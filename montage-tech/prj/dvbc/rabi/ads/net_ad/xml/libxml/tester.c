/*
 * tester.c : a small tester program for XML input.
 *
 * See Copyright for the status of this software.
 *
 * $Id: tester.c,v 1.7 1998/11/27 06:39:48 veillard Exp $
 */

#ifdef WIN32
#define HAVE_FCNTL_H
#include <io.h>
#else
#include "config.h"
#endif
#include <sys/types.h>
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"
#include "tree.h"
#include "debugXML.h"

static int debug = 0;
static int copy = 0;

#if 0
/*
 * Note: there is a couple of errors introduced on purpose.
 */
static CHAR buffer[] = 
"\n\
<?xml version=\"1.0\">\n\
<?xml:namespace ns = \"http://www.ietf.org/standards/dav/\" prefix = \"D\"?>\n\
<?xml:namespace ns = \"http://www.w3.com/standards/z39.50/\" prefix = \"Z\"?>\n\
<D:propertyupdate>\n\
<D:set a=\"'toto'\" b>\n\
       <D:prop>\n\
            <Z:authors>\n\
                 <Z:Author>Jim Whitehead</Z:Author>\n\
                 <Z:Author>Roy Fielding</Z:Author>\n\
            </Z:authors>\n\
       </D:prop>\n\
  </D:set>\n\
  <D:remove>\n\
       <D:prop><Z:Copyright-Owner/></D:prop>\n\
  </D:remove>\n\
</D:propertyupdate>\n\
\n\
";
#endif

/************************************************************************
 *									*
 *				Debug					*
 *									*
 ************************************************************************/

int treeTest(void) {
    xmlDocPtr doc, tmp;
    xmlNodePtr tree, subtree;

    /*
     * build a fake XML document
     */
    doc = xmlNewDoc((CHAR*)"1.0");
    doc->root = xmlNewDocNode(doc, NULL, (CHAR*)"EXAMPLE", NULL);
    xmlSetProp(doc->root, (CHAR*)"prop1", (CHAR*)"gnome is great");
    xmlSetProp(doc->root, (CHAR*)"prop2", (CHAR*)"&linux; too");
    tree = xmlNewChild(doc->root, NULL, (CHAR*)"head", NULL);
    subtree = xmlNewChild(tree, NULL,(CHAR*) "title", (CHAR*)"Welcome to Gnome");
    tree = xmlNewChild(doc->root, NULL, (CHAR*)"chapter", NULL);
    subtree = xmlNewChild(tree, NULL, (CHAR*)"title", (CHAR*)"The Linux adventure");
    subtree = xmlNewChild(tree, NULL, (CHAR*)"p", (CHAR*)"bla bla bla ...");
    subtree = xmlNewChild(tree, NULL, (CHAR*)"image", NULL);
    xmlSetProp(subtree, (CHAR*)"href", (CHAR*)"linus.gif");

    /*
     * test intermediate copy if needed.
     */
    if (copy) {
        tmp = doc;
	doc = xmlCopyDoc(doc, 1);
	xmlFreeDoc(tmp);
    }

    /*
     * print it.
     */
    xmlDocDump(stdout, doc);

    /*
     * free it.
     */
    xmlFreeDoc(doc);
    return(0);
}

void parseAndPrintFile(char *filename) {
    xmlDocPtr doc, tmp;

    /*
     * build an XML tree from a string;
     */
    doc = xmlParseFile(filename);

    /*
     * test intermediate copy if needed.
     */
    if (copy) {
        tmp = doc;
	doc = xmlCopyDoc(doc, 1);
	xmlFreeDoc(tmp);
    }

    /*
     * print it.
     */
    if (!debug)
	xmlDocDump(stdout, doc);
    else
        xmlDebugDumpDocument(stdout, doc);

    /*
     * free it.
     */
    xmlFreeDoc(doc);
}

void parseAndPrintBuffer(CHAR *buf) {
    xmlDocPtr doc, tmp;

    /*
     * build an XML tree from a string;
     */
    doc = xmlParseDoc(buf);

    /*
     * test intermediate copy if needed.
     */
    if (copy) {
        tmp = doc;
	doc = xmlCopyDoc(doc, 1);
	xmlFreeDoc(tmp);
    }

    /*
     * print it.
     */
    if (!debug)
	xmlDocDump(stdout, doc);
    else
        xmlDebugDumpDocument(stdout, doc);

    /*
     * free it.
     */
    xmlFreeDoc(doc);
}

#if 0
int main(int argc, char **argv) {
    int i;
    int files = 0;

    for (i = 1; i < argc ; i++) {
	if ((!strcmp(argv[i], "-debug")) || (!strcmp(argv[i], "--debug")))
	    debug++;
	else if ((!strcmp(argv[i], "-copy")) || (!strcmp(argv[i], "--copy")))
	    copy++;
    }
    for (i = 1; i < argc ; i++) {
	if (argv[i][0] != '-') {
	    parseAndPrintFile(argv[i]);
	    files ++;
	}
    }
    if (files == 0) {
	printf("\nFirst test for the parser, with errors\n");
        parseAndPrintBuffer(buffer);
	printf("\nBuilding a tree from scratch and printing it\n");
	treeTest();
    }

    return(0);
}
#endif

