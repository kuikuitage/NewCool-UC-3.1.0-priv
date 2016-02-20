/*
 * entities.c : implementation for the XML entities handking
 *
 * See Copyright for the status of this software.
 *
 * $Id: entities.c,v 1.10 1998/11/27 06:39:46 veillard Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entities.h"

/*
 * The XML predefined entities.
 */

struct xmlPredefinedEntityValue {
    const char *name;
    const char *value;
};
struct xmlPredefinedEntityValue xmlPredefinedEntityValues[] = {
    { "lt", "<" },
    { "gt", ">" },
    { "apos", "'" },
    { "quot", "\"" },
    { "amp", "&" }
};

xmlEntitiesTablePtr xmlPredefinedEntities = NULL;

/*
 * A buffer used for converting entities to their equivalent and back.
 *
 * TODO: remove this, this helps performances but forbid reentrancy in a 
 *       stupid way.
 */
static int buffer_size = 0;
static CHAR *buffer = NULL;

void growBuffer(void) {
    buffer_size *= 2;
    buffer = (CHAR *) realloc(buffer, buffer_size * sizeof(CHAR));
    if (buffer == NULL) {
	perror("realloc failed");
	exit(1);
    }
}

/*
 * xmlFreeEntity : clean-up an entity record.
 */
void xmlFreeEntity(xmlEntityPtr entity) {
    if (entity == NULL) return;

    if (entity->name != NULL)
	free((char *) entity->name);
    if (entity->ExternalID != NULL)
        free((char *) entity->ExternalID);
    if (entity->SystemID != NULL)
        free((char *) entity->SystemID);
    if (entity->content != NULL)
        free((char *) entity->content);
    memset(entity, -1, sizeof(xmlEntity));
}

/*
 * xmlAddEntity : register a new entity for an entities table.
 *
 * TODO !!! We should check here that the combination of type
 *          ExternalID and SystemID is valid.
 */
static void
xmlAddEntity(xmlEntitiesTablePtr table, const CHAR *name, int type,
              const CHAR *ExternalID, const CHAR *SystemID, CHAR *content) {
    int i;
    xmlEntityPtr cur;
    int len;

    for (i = 0;i < table->nb_entities;i++) {
        cur = &table->table[i];
	if (!xmlStrcmp(cur->name, name)) {
	    /*
	     * The entity is already defined in this Dtd, the spec says to NOT
	     * override it ... Is it worth a Warning ??? !!!
	     */
	    return;
	}
    }
    if (table->nb_entities >= table->max_entities) {
        /*
	 * need more elements.
	 */
	table->max_entities *= 2;
	table->table = (xmlEntityPtr) 
	    realloc(table->table, table->max_entities * sizeof(xmlEntity));
	if (table->table) {
	    perror("realloc failed");
	    exit(1);
	}
    }
    cur = &table->table[table->nb_entities];
    cur->name = xmlStrdup(name);
    for (len = 0;name[0] != 0;name++)len++;
    cur->len = len;
    cur->type = type;
    if (ExternalID != NULL)
	cur->ExternalID = xmlStrdup(ExternalID);
    else
        cur->ExternalID = NULL;
    if (SystemID != NULL)
	cur->SystemID = xmlStrdup(SystemID);
    else
        cur->SystemID = NULL;
    if (content != NULL)
	cur->content = xmlStrdup(content);
    else
        cur->content = NULL;
    table->nb_entities++;
}

/**
 * xmlInitializePredefinedEntities:
 *
 * Set up the predefined entities.
 */
void xmlInitializePredefinedEntities(void) {
    int i;
    CHAR name[50];
    CHAR value[50];
    const char *in;
    CHAR *out;

    if (xmlPredefinedEntities != NULL) return;

    xmlPredefinedEntities = xmlCreateEntitiesTable();
    for (i = 0;i < sizeof(xmlPredefinedEntityValues) / 
                   sizeof(xmlPredefinedEntityValues[0]);i++) {
        in = xmlPredefinedEntityValues[i].name;
	out = &name[0];
	for (;(*out++ = (CHAR) *in);)in++;
        in = xmlPredefinedEntityValues[i].value;
	out = &value[0];
	for (;(*out++ = (CHAR) *in);)in++;
        xmlAddEntity(xmlPredefinedEntities, (const CHAR *) &name[0],
	             XML_INTERNAL_PREDEFINED_ENTITY, NULL, NULL,
		     &value[0]);
    }
}

/**
 * xmlGetPredefinedEntity:
 * @name:  the entity name
 *
 * Check whether this name is an predefined entity.
 *
 * return values: NULL if not, othervise the entity
 */
xmlEntityPtr
xmlGetPredefinedEntity(const CHAR *name) {
    int i;
    xmlEntityPtr cur;

    if (xmlPredefinedEntities == NULL)
        xmlInitializePredefinedEntities();
    for (i = 0;i < xmlPredefinedEntities->nb_entities;i++) {
	cur = &xmlPredefinedEntities->table[i];
	if (!xmlStrcmp(cur->name, name)) return(cur);
    }
    return(NULL);
}

/**
 * xmlAddDtdEntity:
 * @doc:  the document
 * @name:  the entity name
 * @type:  the entity type XML_xxx_yyy_ENTITY
 * @ExternalID:  the entity external ID if available
 * @SystemID:  the entity system ID if available
 * @content:  the entity content
 *
 * Register a new entity for this document DTD.
 */
void
xmlAddDtdEntity(xmlDocPtr doc, const CHAR *name, int type,
              const CHAR *ExternalID, const CHAR *SystemID, CHAR *content) {
    xmlEntitiesTablePtr table;

    if (doc->dtd == NULL) {
        fprintf(stderr, "xmlAddDtdEntity: document without Dtd !\n");
	return;
    }
    table = (xmlEntitiesTablePtr) doc->dtd->entities;
    if (table == NULL) {
        table = xmlCreateEntitiesTable();
	doc->dtd->entities = table;
    }
    xmlAddEntity(table, name, type, ExternalID, SystemID, content);
}

/**
 * xmlAddDocEntity:
 * @doc:  the document
 * @name:  the entity name
 * @type:  the entity type XML_xxx_yyy_ENTITY
 * @ExternalID:  the entity external ID if available
 * @SystemID:  the entity system ID if available
 * @content:  the entity content
 *
 * Register a new entity for this document.
 */
void
xmlAddDocEntity(xmlDocPtr doc, const CHAR *name, int type,
              const CHAR *ExternalID, const CHAR *SystemID, CHAR *content) {
    xmlEntitiesTablePtr table;

    table = (xmlEntitiesTablePtr) doc->entities;
    if (table == NULL) {
        table = xmlCreateEntitiesTable();
	doc->entities = table;
    }
    xmlAddEntity(doc->entities, name, type, ExternalID, SystemID, content);
}

/**
 * xmlGetDtdEntity:
 * @doc:  the document referencing the entity
 * @name:  the entity name
 *
 * Do an entity lookup in the Dtd entity hash table and
 * returns the corresponding entity, if found.
 * 
 * return values: A pointer to the entity structure or NULL if not found.
 */
xmlEntityPtr
xmlGetDtdEntity(xmlDocPtr doc, const CHAR *name) {
    int i;
    xmlEntityPtr cur;
    xmlEntitiesTablePtr table;

    if ((doc->dtd != NULL) && (doc->dtd->entities != NULL)) {
	table = (xmlEntitiesTablePtr) doc->dtd->entities;
	for (i = 0;i < table->nb_entities;i++) {
	    cur = &table->table[i];
	    if (!xmlStrcmp(cur->name, name)) return(cur);
	}
    }
    return(NULL);
}

/**
 * xmlGetDocEntity:
 * @doc:  the document referencing the entity
 * @name:  the entity name
 *
 * Do an entity lookup in the document entity hash table and
 * returns the corrsponding entity, otherwise a lookup is done
 * in the predefined entities too.
 * 
 * return values: A pointer to the entity structure or NULL if not found.
 */
xmlEntityPtr
xmlGetDocEntity(xmlDocPtr doc, const CHAR *name) {
    int i;
    xmlEntityPtr cur;
    xmlEntitiesTablePtr table;

    if (doc->entities != NULL) {
	table = (xmlEntitiesTablePtr) doc->entities;
	for (i = 0;i < table->nb_entities;i++) {
	    cur = &table->table[i];
	    if (!xmlStrcmp(cur->name, name)) return(cur);
	}
    }
    if (xmlPredefinedEntities == NULL)
        xmlInitializePredefinedEntities();
    table = xmlPredefinedEntities;
    for (i = 0;i < table->nb_entities;i++) {
	cur = &table->table[i];
	if (!xmlStrcmp(cur->name, name)) return(cur);
    }

    return(NULL);
}

/*
 * [2] Char ::= #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD]
 *                  | [#x10000-#x10FFFF]
 * any Unicode character, excluding the surrogate blocks, FFFE, and FFFF.
 */
#define IS_CHAR(c)							\
    (((c) == 0x09) || ((c) == 0x0a) || ((c) == 0x0d) ||			\
     (((c) >= 0x20) && ((c) != 0xFFFE) && ((c) != 0xFFFF)))

/**
 * xmlEncodeEntities:
 * @doc:  the document containing the string
 * @input:  A string to convert to XML.
 *
 * Do a global encoding of a string, replacing the predefined entities
 * and non ASCII values with their entities and CharRef counterparts.
 *
 * TODO !!!! Once moved to UTF-8 internal encoding, the encoding of non-ascii
 *           get erroneous.
 *
 * TODO This routine is not reentrant and this will be changed, the interface
 *      should not be modified though.
 * 
 * return values: A newly allocated string with the substitution done.
 */
CHAR *
xmlEncodeEntities(xmlDocPtr doc, const CHAR *input) {
    const CHAR *cur = input;
    CHAR *out = buffer;

    if (input == NULL) return(NULL);
    if (buffer == NULL) {
        buffer_size = 1000;
        buffer = (CHAR *) malloc(buffer_size * sizeof(CHAR));
	if (buffer == NULL) {
	    perror("malloc failed");
            exit(1);
	}
	out = buffer;
    }
    while (*cur != '\0') {
        if (out - buffer > buffer_size - 100) {
	    int index = out - buffer;

	    growBuffer();
	    out = &buffer[index];
	}

	/*
	 * By default one have to encode at least '<', '>', '"' and '&' !
	 */
	if (*cur == '<') {
	    *out++ = '&';
	    *out++ = 'l';
	    *out++ = 't';
	    *out++ = ';';
	} else if (*cur == '>') {
	    *out++ = '&';
	    *out++ = 'g';
	    *out++ = 't';
	    *out++ = ';';
	} else if (*cur == '&') {
	    *out++ = '&';
	    *out++ = 'a';
	    *out++ = 'm';
	    *out++ = 'p';
	    *out++ = ';';
	} else if (*cur == '"') {
	    *out++ = '&';
	    *out++ = 'q';
	    *out++ = 'u';
	    *out++ = 'o';
	    *out++ = 't';
	    *out++ = ';';
	} else if (*cur == '\'') {
	    *out++ = '&';
	    *out++ = 'a';
	    *out++ = 'p';
	    *out++ = 'o';
	    *out++ = 's';
	    *out++ = ';';
	} else if (((*cur >= 0x20) && (*cur < 0x80)) ||
	    (*cur == '\n') || (*cur == '\r') || (*cur == '\t')) {
	    /*
	     * default case, just copy !
	     */
	    *out++ = *cur;
#ifndef USE_UTF_8
	} else if ((sizeof(CHAR) == 1) && (*cur >= 0x80)) {
	    char buf[10], *ptr;
#ifdef HAVE_SNPRINTF
	    snprintf(buf, 9, "&#%d;", *cur);
#else
	    sprintf(buf, "&#%d;", *cur);
#endif
            ptr = buf;
	    while (*ptr != 0) *out++ = *ptr++;
#endif
	} else if (IS_CHAR(*cur)) {
	    char buf[10], *ptr;

#ifdef HAVE_SNPRINTF
	    snprintf(buf, 9, "&#%d;", *cur);
#else
	    sprintf(buf, "&#%d;", *cur);
#endif
            ptr = buf;
	    while (*ptr != 0) *out++ = *ptr++;
	}
#if 0
	else {
	    /*
	     * default case, this is not a valid char !
	     * Skip it...
	     */
	    fprintf(stderr, "xmlEncodeEntities: invalid char %d\n", (int) *cur);
	}
#endif
	cur++;
    }
    *out++ = 0;
    return(buffer);
}

/**
 * xmlCreateEntitiesTable:
 *
 * create and initialize an empty entities hash table.
 *
 * return values: the xmlEntitiesTablePtr just created or NULL in case of error.
 */
xmlEntitiesTablePtr
xmlCreateEntitiesTable(void) {
    xmlEntitiesTablePtr ret;

    ret = (xmlEntitiesTablePtr) 
         malloc(sizeof(xmlEntitiesTable));
    if (ret == NULL) {
        fprintf(stderr, "xmlCreateEntitiesTable : malloc(%d) failed\n",
	        sizeof(xmlEntitiesTable));
        return(NULL);
    }
    ret->max_entities = XML_MIN_ENTITIES_TABLE;
    ret->nb_entities = 0;
    ret->table = (xmlEntityPtr ) 
         malloc(ret->max_entities * sizeof(xmlEntity));
    if (ret == NULL) {
        fprintf(stderr, "xmlCreateEntitiesTable : malloc(%d) failed\n",
	        ret->max_entities * sizeof(xmlEntity));
	free(ret);
        return(NULL);
    }
    return(ret);
}

/**
 * xmlFreeEntitiesTable:
 * @table:  An entity table
 *
 * Deallocate the memory used by an entities hash table.
 */
void
xmlFreeEntitiesTable(xmlEntitiesTablePtr table) {
    int i;

    if (table == NULL) return;

    for (i = 0;i < table->nb_entities;i++) {
        xmlFreeEntity(&table->table[i]);
    }
    free(table->table);
    free(table);
}

/**
 * xmlCopyEntitiesTable:
 * @table:  An entity table
 *
 * Build a copy of an entity table.
 * 
 * return values: the new xmlEntitiesTablePtr or NULL in case of error.
 */
xmlEntitiesTablePtr
xmlCopyEntitiesTable(xmlEntitiesTablePtr table) {
    xmlEntitiesTablePtr ret;
    xmlEntityPtr cur, ent;
    int i;

    ret = (xmlEntitiesTablePtr) malloc(sizeof(xmlEntitiesTable));
    if (ret == NULL) {
        fprintf(stderr, "xmlCopyEntitiesTable: out of memory !\n");
	return(NULL);
    }
    ret->table = (xmlEntityPtr) malloc(table->max_entities *
                                         sizeof(xmlEntity));
    if (ret->table == NULL) {
        fprintf(stderr, "xmlCopyEntitiesTable: out of memory !\n");
	free(ret);
	return(NULL);
    }
    ret->max_entities = table->max_entities;
    ret->nb_entities = table->nb_entities;
    for (i = 0;i < ret->nb_entities;i++) {
	cur = &ret->table[i];
	ent = &table->table[i];
	cur->len = ent->len;
	cur->type = ent->type;
	if (ent->name != NULL)
	    cur->name = xmlStrdup(ent->name);
	else
	    cur->name = NULL;
	if (ent->ExternalID != NULL)
	    cur->ExternalID = xmlStrdup(ent->ExternalID);
	else
	    cur->ExternalID = NULL;
	if (ent->SystemID != NULL)
	    cur->SystemID = xmlStrdup(ent->SystemID);
	else
	    cur->SystemID = NULL;
	if (ent->content != NULL)
	    cur->content = xmlStrdup(ent->content);
	else
	    cur->content = NULL;
    }
    return(ret);
}

/**
 * xmlDumpEntitiesTable:
 * @table:  An entity table
 *
 * This will dump the content of the entity table as an XML DTD definition
 *
 * NOTE: TODO an extra parameter allowing a reentant implementation will
 *       be added.
 */
void
xmlDumpEntitiesTable(xmlEntitiesTablePtr table) {
    int i;
    xmlEntityPtr cur;

    if (table == NULL) return;

    for (i = 0;i < table->nb_entities;i++) {
        cur = &table->table[i];
        switch (cur->type) {
	    case XML_INTERNAL_GENERAL_ENTITY:
	        xmlBufferWriteChar("<!ENTITY ");
		xmlBufferWriteCHAR(cur->name);
		xmlBufferWriteChar(" \"");
		xmlBufferWriteCHAR(cur->content);
		xmlBufferWriteChar("\">\n");
	        break;
	    case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
	        xmlBufferWriteChar("<!ENTITY ");
		xmlBufferWriteCHAR(cur->name);
		if (cur->ExternalID != NULL) {
		     xmlBufferWriteChar(" PUBLIC \"");
		     xmlBufferWriteCHAR(cur->ExternalID);
		     xmlBufferWriteChar("\" \"");
		     xmlBufferWriteCHAR(cur->SystemID);
		     xmlBufferWriteChar("\"");
		} else {
		     xmlBufferWriteChar(" SYSTEM \"");
		     xmlBufferWriteCHAR(cur->SystemID);
		     xmlBufferWriteChar("\"");
		}
		xmlBufferWriteChar(">\n");
	        break;
	    case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
	        xmlBufferWriteChar("<!ENTITY ");
		xmlBufferWriteCHAR(cur->name);
		if (cur->ExternalID != NULL) {
		     xmlBufferWriteChar(" PUBLIC \"");
		     xmlBufferWriteCHAR(cur->ExternalID);
		     xmlBufferWriteChar("\" \"");
		     xmlBufferWriteCHAR(cur->SystemID);
		     xmlBufferWriteChar("\"");
		} else {
		     xmlBufferWriteChar(" SYSTEM \"");
		     xmlBufferWriteCHAR(cur->SystemID);
		     xmlBufferWriteChar("\"");
		}
		if (cur->content != NULL) { /* Should be true ! */
		    xmlBufferWriteChar(" NDATA ");
		    xmlBufferWriteCHAR(cur->content);
		}
		xmlBufferWriteChar(">\n");
	        break;
	    case XML_INTERNAL_PARAMETER_ENTITY:
	        xmlBufferWriteChar("<!ENTITY % ");
		xmlBufferWriteCHAR(cur->name);
		xmlBufferWriteChar(" \"");
		xmlBufferWriteCHAR(cur->content);
		xmlBufferWriteChar("\">\n");
	        break;
	    case XML_EXTERNAL_PARAMETER_ENTITY:
	        xmlBufferWriteChar("<!ENTITY % ");
		xmlBufferWriteCHAR(cur->name);
		if (cur->ExternalID != NULL) {
		     xmlBufferWriteChar(" PUBLIC \"");
		     xmlBufferWriteCHAR(cur->ExternalID);
		     xmlBufferWriteChar("\" \"");
		     xmlBufferWriteCHAR(cur->SystemID);
		     xmlBufferWriteChar("\"");
		} else {
		     xmlBufferWriteChar(" SYSTEM \"");
		     xmlBufferWriteCHAR(cur->SystemID);
		     xmlBufferWriteChar("\"");
		}
		xmlBufferWriteChar(">\n");
	        break;
	    default:
	        fprintf(stderr,
		    "xmlDumpEntitiesTable: internal: unknown type %d\n",
		        cur->type);
	}
    }
}
