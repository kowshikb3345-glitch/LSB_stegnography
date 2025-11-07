#include <stdio.h>
#include <string.h>
#include "encode.h"

/*Status pointer_pos(FILE *fptr_src, FILE *fptr_dest)
{
    if(ftell(fptr_src) == ftell(fptr_dest))
    {
        printf("Both offset are pointed at : %ld\n", ftell(fptr_src));
        return e_success;
    }
    printf("Position of offset in src file is : %ld\n", ftell(fptr_src));
    printf("Position of offset in stego file is : %ld\n", ftell(fptr_dest));
    return e_failure;
}*/

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

/*function to encode byte to lsb */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
   int i, n = 7;
   for(i = 0; i <= 7; i++)
   {
        int mask = 1 << n;
        int get = data & mask;
        image_buffer[i] = image_buffer[i] & ~1;
        get = (get >> n);
        image_buffer[i] = image_buffer[i] | get;
        n--;
   }
   return e_success;
}

/*function to encode size to lsb */
Status encode_size_to_lsb(int data, char *image_buffer)
{
   int i, n = 32;
   for(i = 0; i < 32; i++)
   {
        int mask = 1 << n;
        int get = data & mask;
        image_buffer[i] = image_buffer[i] & ~1;
        get = (get >> n);
        image_buffer[i] = image_buffer[i] | get;
        n--;
   }
   return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    printf("Enter the Magic string : ");
    scanf("%s", encInfo->magic);
    encInfo->magic_str_len = strlen(encInfo->magic);
    printf("Length is : %d\n", encInfo->magic_str_len);
    int sizeof_magic_len = sizeof(encInfo->magic_str_len);
    printf("sizeof Magic string is : %d\n", sizeof_magic_len );
    
    char *secret_ext = strstr(encInfo->secret_fname, ".");
    for(int i = 0; i < MAX_FILE_SUFFIX; i++)
    {
        encInfo->extn_secret_file[i] = secret_ext[i];
    }
    printf("Extension of Secret file: %s\n", encInfo->extn_secret_file);
    encInfo->ext_size = strlen(secret_ext);
    printf("Length of secret extension is: %d\n", encInfo->ext_size);
    
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    printf("Size of Secret data : %d\n", encInfo->size_secret_file);
    printf("Size of Secret file : %ld\n", sizeof(encInfo->size_secret_file));

    int file_size = get_image_size_for_bmp(encInfo->fptr_src_image);
    printf("Size Of Source image : %d\n", file_size);

    int bytes_encode;
    bytes_encode= sizeof_magic_len + encInfo->magic_str_len + encInfo->ext_size +
        encInfo->size_secret_file  + sizeof(encInfo->size_secret_file) + sizeof(encInfo->ext_size);
    printf("Capacity checking : %d\n", bytes_encode * 8);

    if((bytes_encode*8) <= file_size - 54)
    {
        fseek(encInfo->fptr_src_image, 0, SEEK_SET);
        return e_success;
    }
    else
    {
        return e_failure;
    } 
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    fseek(fptr_image, 18, SEEK_SET);
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);
    return width * height * 3;
}

Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
        return e_failure;
    }
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
        return e_failure;
    }
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
        return e_failure;
    }
    //pointer_pos(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_magic_string_len(int len, EncodeInfo *encInfo)
{
    //pointer_pos(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    char buffer[32];
    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(len, buffer);
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    //pointer_pos(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    for(int i = 0; i < encInfo->magic_str_len; i++)
    {
        char buffer[8];
        fread(buffer, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], buffer);
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_extn_len(int len, EncodeInfo *encInfo)
{
    //pointer_pos(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    char buffer[32];  
    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(len, buffer);
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    //pointer_pos(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    for(int i = 0; i < encInfo->ext_size; i++)
    {
        char buffer[8];
        fread(buffer, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], buffer);
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    //pointer_pos(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    char buffer[32];
    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, buffer);
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);
    return e_success;
} 

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //pointer_pos(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    char ch;
    while(fread(&ch, 1, 1, encInfo->fptr_secret) == 1)
    {
        char buffer[8];
        if(fread(buffer, 1, 8, encInfo->fptr_src_image) < 8)
        {
            return e_failure;
        }
        encode_byte_to_lsb(ch, buffer);
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    //pointer_pos(fptr_src, fptr_dest);
    int ch;
    while ((ch = fgetc(fptr_src)) != EOF)
    {
        fputc(ch, fptr_dest);
    }
    return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strstr(argv[2], ".bmp") != NULL)
    {
        printf(".bmp file is present\n");
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        printf(".bmp is not present\n");
        return e_failure;
    }
   
    if(strstr(argv[3], ".txt") != NULL || strstr(argv[3], ".c") != NULL ||
       strstr(argv[3], ".bmp") != NULL || strstr(argv[3], ".csv") != NULL)
    {
        encInfo->secret_fname = argv[3];
        printf("%s file is present\n", argv[3]);
        
    }
    else
    {
        printf("%s is not present\n", argv[3]);
        return e_failure;
    }
    if(argv[4]==NULL){
           encInfo->stego_image_fname="stego.bmp";
           printf("Using default file name stego.bmp\n");
    }
    else if (strstr(argv[4], ".bmp") != NULL)
    {
        printf("%s is present\n", argv[4]);
        encInfo->stego_image_fname = argv[4];
    }
   
    return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];
    fread(buffer, 1, 54, fptr_src_image);
    fwrite(buffer, 1, 54, fptr_dest_image);
    int res = ftell(fptr_dest_image);
    //pointer_pos(fptr_src_image, fptr_dest_image);
    if(res == 54)
        return e_success;
    else
        return e_failure;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_failure)
    {
        printf("File opening failed!\n");
        return e_failure;
    }

    if (check_capacity(encInfo) == e_failure)
    {
        printf("Check capacity is failed!\n");
        return e_failure;
    }
    printf("Check capacity is success!\n");

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("BMP header not copied successfully\n");
        return e_failure;
    }
    printf("BMP header is copied successfully\n");

    if (encode_magic_string_len(encInfo->magic_str_len, encInfo) == e_failure)
    {
        printf("Magic string length is not encoded\n");
        return e_failure;
    }
    printf("Magic string length is copied successfully\n");

    if (encode_magic_string(encInfo->magic, encInfo) == e_failure)
    {
        printf("Magic string is not encoded\n");
        return e_failure;
    }
    printf("Magic string data is copied successfully\n");

    if (encode_secret_file_extn_len(encInfo->ext_size, encInfo) == e_failure)
    {
        printf("Secret file Extension length is not encoded\n");
        return e_failure;
    }
    printf("Secret file extension length is copied successfully\n");

    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        printf("Secret file Extension is not encoded\n");
        return e_failure;
    }
    printf("Secret file extension is copied successfully\n");

    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        printf("Secret file size is not encoded\n");
        return e_failure;
    }
    printf("Secret file size is copied successfully\n");

    if (encode_secret_file_data(encInfo) == e_failure)
    {
        printf("Secret file data is not encoded\n");
        return e_failure;
    }
    printf("Secret file data is copied successfully\n");

    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("Remaining image data is not encoded\n");
        return e_failure;
    }
    printf("Remaining image data is copied successfully\n");

    return e_success;
}
