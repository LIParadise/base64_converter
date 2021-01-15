#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void dump( char**, size_t*, int);

char base64_code_to_binary( char c){
    char ret;
    switch(c){
        case 'A' ... 'Z':
            ret = c-'A';
            break;
        case 'a' ... 'z':
            ret = c-'a';
            ret += 26;
            break;
        case '0' ... '9':
            ret = c-'0';
            ret += 52;
            break;
        case '+':
            ret = 62;
            break;
        case '/':
            ret = 63;
            break;
        default:
            ret = -1;
            break;
    }
    return ret;
}

size_t base64_to_dec( char* buf, char** pp_output, ssize_t s ){
    char* p_tmp = buf;
    while( buf[s] == '=' || buf[s] == '\0' || buf[s] == '\r' || buf[s] == '\n' ){
        --s;
    }

    free(*pp_output);
    *pp_output = calloc( 3*(s>>2)+1, sizeof(**pp_output) );
    size_t output_bytes = 0;
    int tmp_buf;
    while( p_tmp + 3 <= buf + s ){
        tmp_buf =
            ( base64_code_to_binary(p_tmp[0]) <<18 ) |
            ( base64_code_to_binary(p_tmp[1]) <<12 ) |
            ( base64_code_to_binary(p_tmp[2]) <<6  ) |
            ( base64_code_to_binary(p_tmp[3])      ) ;
        dump( pp_output, &output_bytes, tmp_buf);
        p_tmp += 4;
    }
    tmp_buf = 0;
    while( p_tmp <= buf + s ){
        tmp_buf <<= 6;
        tmp_buf |= base64_code_to_binary(*p_tmp);
        ++p_tmp;
    }
    if( tmp_buf ){
        dump( pp_output, &output_bytes, tmp_buf );
    }

    while( (*pp_output)[output_bytes] == '\0' ){
        -- output_bytes;
    }

    return output_bytes;
}

void dump( char** pp_output, size_t* location, int source ){
    (*pp_output)[ (*location) ]     = (source & 0xff0000) >> 16;
    (*pp_output)[ (*location) + 1 ] = (source & 0xff00) >> 8;
    (*pp_output)[ (*location) + 2 ] = (source & 0xff);
    *location += 3;
}

int main( int argc, char** argv){
    if( argc != 2 ){
        printf( "usage: <%s> <filename>\n", argv[0] );
        printf( "where <filename> is a base64 file\n" );
        exit(1);
    }

    ssize_t bytes_read;
    size_t default_buf_size = 256;
    char* result = NULL;
    char* buf = NULL;
    FILE* fp = fopen( argv[1], "r" );
    bytes_read = getline( &buf, &default_buf_size, fp );
    base64_to_dec( buf, &result, bytes_read );

    free( buf );
    printf( "%s", result );
    free( result );

}
