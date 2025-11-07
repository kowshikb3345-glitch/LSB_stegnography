#include <stdio.h>
#include <string.h>
#include "decode.h"

Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    int i, n = 7;
    char result = 0;

    for(i = 0; i <= 7; i++)
    {
        int bit = image_buffer[i] & 1;
        bit = (bit << n);
        result = result | bit;
        n--;
    }
    *data = result;
    return e_success;
}

Status decode_size_from_lsb(int *data, char *image_buffer)
{

    int i, n = 32;
    int result = 0;

    for(i = 0; i < 32; i++)
    {
        int bit = image_buffer[i] & 1;
        bit = (bit << n);
        result = result | bit;
        n--;
    }

    *data = result;
    return e_success;
}

Status decode_open_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if(decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
        return d_failure;
    }
    printf("Position of offset in stego file is : %ld\n", ftell(decInfo->fptr_stego_image));
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    printf("Position of offset in stego file is : %ld\n", ftell(decInfo->fptr_stego_image));
    return d_success;
}

Status decode_magic_string_len(int *len, DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_stego_image);
    decode_size_from_lsb(len, buffer);
    decInfo->magic_str_len = *len;
    printf("Magic string length is : %d\n", decInfo->magic_str_len);
    return e_success;
}

/* Store Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    for(int i = 0; i < (decInfo->magic_str_len); i++)
    {
        char buffer[8];
        fread(buffer, 1, 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&decInfo->decoded_magic[i], buffer);
    }
    decInfo->decoded_magic[decInfo->magic_str_len] = '\0';
    //printf("Magic_string is : %s\n", decInfo->decoded_magic);
    return d_success;
}
Status validate_magic_string(DecodeInfo *decInfo)
{
    printf("Enter the magic string :\n");
    scanf("%s",decInfo->Magic_string);
    if(strcmp(decInfo->Magic_string,decInfo->decoded_magic)!=0)
    {
        return d_failure;
    }
    return d_success;
}

Status decode_secret_file_extn_len(int *len, DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_stego_image);
    decode_size_from_lsb(len, buffer);
    decInfo->de_ext_size = *len;
    printf("Extension length is : %d\n", decInfo->de_ext_size);
    return e_success;
}

/* Decode secret file extension */
Status decode_secret_file_extn(const char *file_extn, DecodeInfo *decInfo)
{
    char output[50];
    for(int i = 0; i < (decInfo->de_ext_size); i++)
    {
        char buffer[8];
        fread(buffer, 1, 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&output[i], buffer);
    }
    output[decInfo->de_ext_size] = '\0';
    decInfo->secret_fname = output;
    printf("Secret file Extenxion is : %s\n", decInfo->secret_fname);
    strcat(decInfo->output_fname, decInfo->secret_fname);
    decInfo->fptr_output_file = fopen(decInfo->output_fname, "w");
    if(decInfo->fptr_output_file == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_fname);
        return d_failure;
    }
    printf("Output file created : %s\n", decInfo->output_fname);
    return d_success;
}

Status decode_secret_file_size(int file_size, DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_stego_image);
    decode_size_from_lsb(&file_size, buffer);
    decInfo->size_secret_file = file_size;
    printf("Secret file size is : %d\n", decInfo->size_secret_file);
    return e_success;
}

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8];
    char ch;
    for(int i = 0; i < decInfo->size_secret_file; i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&ch, buffer);
        fputc(ch, decInfo->fptr_output_file);
    }
   return d_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if (decode_open_files(decInfo) == d_failure)
    {
        printf("File opening failed!\n");
        return d_failure;
    }

    if (decode_magic_string_len(&decInfo->magic_str_len, decInfo) == d_failure)
    {
        printf("Magic string length is not decoded\n");
        return d_failure;
    }

    if (decode_magic_string(decInfo->decoded_magic, decInfo) == d_failure)
    {
        printf("Magic string is not decoded\n");
        return d_failure;
    }
    if(validate_magic_string(decInfo) == d_failure)
    {
        printf("Authentication failed.Try again?\n");
        return d_failure;
    }

    if (decode_secret_file_extn_len(&decInfo->de_ext_size, decInfo) == d_failure)
    {
        printf("Extension length is not decoded\n");
        return d_failure;
    }

    if (decode_secret_file_extn(decInfo->secret_fname, decInfo) == d_failure)
    {
        printf("Extension is not decoded\n");
        return d_failure;
    }

    if (decode_secret_file_size(decInfo->size_secret_file, decInfo) == d_failure)
    {
        printf("Secret file size is not decoded\n");
        return d_failure;
    }

    if (decode_secret_file_data(decInfo) == d_failure)
    {
        printf("Secret file data is not decoded\n");
        return d_failure;
    }
    printf("Secret file data is decoded successfully\n");

    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output_file);

    return d_success;
}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char str[20];
    char def_fname[20] = "Output";
    if(strstr(argv[2], ".bmp") != NULL)
    {
        printf(".bmp file is present\n");
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        printf(".bmp is not present\n");
        return d_failure;
    }
    if(argv[3] == NULL)
    {
        argv[3] = def_fname;
    }
    strcpy(str, argv[3]);
    for(int i= 0 ; str[i] != '\0'; i++)
    {
        if(str[i] == '.')
        {
            str[i] = '\0';
        }
    }
    strcpy(decInfo->output_fname, str);
    printf("File name is : %s\n", decInfo->output_fname);
    return d_success;
}