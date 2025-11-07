#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * decoding secret file from source image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    int size_secret_file;
    int de_ext_size;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Magic string */
    char decoded_magic[20];
    char Magic_string[20];
    int magic_str_len;

    /* Output file */
    char output_fname[50];
    FILE *fptr_output_file;

} DecodeInfo;


/* Decoding function prototypes */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get file pointers for i/p and o/p files */
Status decode_open_files(DecodeInfo *decInfo);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Get Magic String Length */
Status decode_magic_string_len(int *len, DecodeInfo *decInfo);

/* Decode secret file extension length */
Status decode_secret_file_extn_len(int *len, DecodeInfo *decInfo);

/* Decode secret file extension */
Status decode_secret_file_extn(const char *file_extn, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(int file_size, DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a byte from LSB of image buffer */
Status decode_byte_from_lsb(char *data, char *image_buffer);

/* Decode a size from LSB of image buffer */
Status decode_size_from_lsb(int *data, char *image_buffer);

/* Validating the user's and decode magic string */
Status validate_magic_string(DecodeInfo *decInfo);

#endif
