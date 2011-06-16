// Copyright 2011 Juri Glass, Mathias Runge, Nadim El Sayed 
// DAI-Labor, TU-Berlin
// 
// This file is part of libSML.
// 
// libSML is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// libSML is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with libSML.  If not, see <http://www.gnu.org/licenses/>.


#include <sml/sml_octet_string.h>

#include <stdio.h>

#ifdef SML_WITH_UUID_LIB
#include <uuid/uuid.h>
#endif

octet_string *sml_octet_string_init(unsigned char *str, int length) {
    octet_string *s = (octet_string *)malloc(sizeof(octet_string));
    memset(s, 0, sizeof(octet_string));
    if (length > 0) {
        s->str = (unsigned char *)malloc(length);
        memcpy(s->str, str, length);
        s->len = length;
    }
	return s;
}

uint8_t ctoi(uint8_t c){
    uint8_t ret = 0;
    
    if((c >= '0') && (c <= '9')){
        ret = c - '0';
    } else if((c >= 'a') && (c <= 'f')){
        ret = c - 'a' + 10;
    } else if((c >= 'A') && (c <= 'F')){
        ret = c - 'A' + 10;
    }
    
    return ret;
}


inline uint8_t c2toi(uint8_t c1, uint8_t c2){
    return ctoi(c1) << 4 | ctoi(c2);
}

inline uint8_t c2ptoi(char* c){
    return ctoi((uint8_t)c[0]) << 4 | ctoi((uint8_t)c[1]);
}

octet_string *sml_octet_string_init_from_hex(char *str) {
    int i, len = strlen(str);
    if (len % 2 != 0) {
        return 0;
    }
    unsigned char bytes[len / 2];
    for (i = 0; i < (len / 2); i++) {
        bytes[i] = c2ptoi(&(str[i * 2]));
    }
    return sml_octet_string_init(bytes, len / 2);
}

void sml_octet_string_free(octet_string *str) {
    if (str) {
		if (str->str) {
			free(str->str);
		}
		free(str);
	}
}

octet_string *sml_octet_string_parse(sml_buffer *buf) {
    int l;
	if (mc_sml_buf_get_current_type(buf) != SML_TYPE_OCTET_STRING) {
		buf->error = 1;
		return 0;
	}
	
	l = mc_sml_get_length(buf);
	if (l < 0) {
		buf->error = 1;
		return 0;
	}
	
    octet_string *str = sml_octet_string_init(mc_sml_buf_get_current_buf(buf), l);
	mc_sml_buf_update_read_bytes(buf, l);
	return str;
}

void sml_octet_string_write(octet_string *str, sml_buffer *buf) {
    mc_sml_set_type_and_length(buf, SML_TYPE_OCTET_STRING, (unsigned int) str->len);
    memcpy(mc_sml_buf_get_current_buf(buf), str->str, str->len);
    buf->cursor += str->len;
}

octet_string *sml_octet_string_generate_uuid() {
#ifdef SML_WITH_UUID_LIB
    uuid_t uuid;
    uuid_generate(uuid);
    return sml_octet_string_init(uuid, 16);
#else
    printf("TODO %s\n", __FUNCTION__);
    return sml_octet_string_init((unsigned char*)"RANDOM", 6);
#endif
}

int sml_octet_string_cmp_with_hex(octet_string *str, char *hex) {
    octet_string *hstr = sml_octet_string_init_from_hex(hex);
    if (str->len != hstr->len) {
        sml_octet_string_free(hstr);
        return -1;
    }
    int result = memcmp(str->str, hstr->str, str->len);
    sml_octet_string_free(hstr);
    return result;
}




