#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    printf("==== LSB IMAGE STEGNOGRAPHY TOOL ====\n");
    printf("Usage:\n");
    printf("  %s -e <source.bmp> <secretfile.txt> <stego.bmp> : Encode\n", argv[0]);
    printf("  %s -d <stego.bmp> <output_basename>             : Decode\n", argv[0]);
    printf("-------------------------------------------------\n");

    if (argc < 2) 
    {
        printf("[ERROR] Not enough arguments provided!\n");
        printf("Please run the program with either encoding (-e) or decoding (-d) options.\n");
        return e_failure;
    }

    OperationType op = check_operation_type(argv);

    if (op == e_encode)
    {
        printf("[INFO] Mode: ENCODING\n");
        EncodeInfo encInfo;
        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("[INFO] All input files validated. Starting encoding process...\n");
            int res = do_encoding(&encInfo);
            if (res == e_success)
            {
                printf("[SUCCESS] Secret file has been encoded into the BMP image successfully!\n");
                printf("          Output (stego image): %s\n", encInfo.stego_image_fname);
            }
            else
            {
                printf("[FAILURE] Encoding process failed. Please check logs above for details.\n");
            }
        }
        else
        {
            printf("[ERROR] Input file validation failed. Please check your argument order and file types.\n");
            return e_failure;
        }
        return e_success;
    }
    else if (op == e_decode)
    {
        printf("[INFO] Mode: DECODING\n");
        DecodeInfo decInfo;
        if (read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("[INFO] Input stego image validated. Starting decoding process...\n");
            int res = do_decoding(&decInfo);
            if (res == e_success)
            {
                printf("[SUCCESS] Secret file extraction complete!\n");
                printf("          Output file prefix : %s\n", decInfo.output_fname);
                printf("          Decoded file created and written from hidden BMP data.\n");
            }
            else
            {
                printf("[FAILURE] Decoding process failed. Could not extract the secret file.\n");
            }
        }
        else
        {
            printf("[ERROR] Input file validation failed. Please ensure you pass a valid stego BMP file and output name.\n");
            return e_failure;
        }
        return e_success;
    }
    else
    {
        printf("[ERROR] Unsupported operation. Use -e for encoding or -d for decoding.\n");
        return e_failure;
    }
}


OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
        
}
    
